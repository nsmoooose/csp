// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file Atmosphere.cpp
 *
 **/

#include "Atmosphere.h"
#include "LogStream.h"

#include <SimData/InterfaceRegistry.h>
#include <SimData/Random.h>

//SIMDATA_REGISTER_INTERFACE(Atmosphere)


class Perlin1D {
	double m_persistence;
	int m_octaves;
	int m_offset;
	public:

	Perlin1D(double p=0.5, int n=1) {
		m_octaves = n;
		m_persistence = p;
		randomize();
	}

	void set(double p, int n) {
		m_persistence = p;
		m_octaves = n;
	} 

	void select(int idx) {
		m_offset = idx;
	}
	
	void randomize() {
		m_offset = int(simdata::g_Random.NewRand()*1.0e+9);
	}

	double Noise(int x) {
		x = (x << 13) ^ x;
		return (1.0 -
		        ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) /
		        1073741824.0);
	} 

	double SmoothedNoise(int x) {
		return Noise(x) / 2 + Noise(x - 1) / 4 + Noise(x + 1) / 4;
	} 

	double Interpolate(double a, double b, double x) {
		return  a*(1.0-x) + b*x;
	}

	double InterpolatedNoise(double x) {
		int integer_X = int(x);
		double fractional_X = x - integer_X;

		double v1 = SmoothedNoise(integer_X);
		double v2 = SmoothedNoise(integer_X + 1);

		return Interpolate(v1, v2, fractional_X);
	} 

	double get(double x) {
		double total = 0.0;
		double p = m_persistence;
		int n = m_octaves - 1;
		double amplitude = 1.0;
		for (int i = 0; i <= n; i++) {
			double frequency = 1<<i;
			amplitude *= p;
			total += InterpolatedNoise(x * frequency + m_offset) * amplitude;
		} 
		return total;
	}

	std::vector<float> generate(int n, bool periodic, double timescale=1.0, double scale=1.0, double offset=0.0) {
		std::vector<float> buffer(n);
		if (n > 0) {
			int i;
			double s = timescale/n;
			for (i=0; i<n; i++) {
				double f = i*s;
				buffer[i] = (float) (get(f)*scale + offset);
			}
			if (periodic) {
				s = 1.0 / n;
				for (i=0; i<n/2; i++) {
					float f = 0.5 + i*s;
					float a = buffer[i];
					float b = buffer[n-i-1];
					buffer[i] = f*a+(1-f)*b;
					buffer[n-i-1] = f*b+(1-f)*a;
				}
			}
		}
		return buffer;
	}

};


Atmosphere::Atmosphere() {
	generateWinds();
	m_Latitude = 0.0;
	m_UpdateTime = 0.0;
	m_UpdateIndex = 0;
	m_TargetTemperature = 288.15;
	m_TargetPressure = 101325.0;
	m_TimeScale = 0.00015; // about 2 hours
	m_WindIndex = 0.0;
	m_WindScale = 0.0;
	m_PrevailingWind = simdata::Vector3(1.0, 0.0, 0.0);
	m_TurbulenceBlend = 0.0;
	m_TurbulenceBlendUp = true;
	m_GustModulation = 1.0;
	reset();
}

void Atmosphere::generateWinds() {
	Perlin1D noise;

	// pressure variation (10 days)
	noise.set(0.3, 5);
	noise.randomize();
	m_PressureTime = noise.generate(1000, true, 30.0, 15000.0, 0.0);

	// turbulence (10000 m)
	noise.set(0.9, 6);
	noise.randomize();
	m_DensityTime = noise.generate(1000, true, 10.0, 0.02, 0.0);

	// turbulence altitude buffers (15000 m)
	noise.set(0.9, 6);
	noise.randomize();
	m_TurbulenceAltA = noise.generate(1000, false, 10.0, 2.0, -1.0);
	m_TurbulenceAltB = noise.generate(1000, false, 10.0, 2.0, -1.0);
	
	// wind direction variation (30000 m)
	noise.set(0.8, 4);
	noise.randomize();
	m_WindAltX = noise.generate(100, false, 8.0, 1.0, 0.0);
	noise.randomize();
	m_WindAltY = noise.generate(100, false, 8.0, 1.0, 0.0);

	// wind direction variation (10 days)
	noise.set(0.6, 5);
	noise.randomize();
	m_WindTimeX = noise.generate(1000, true, 10.0);
	noise.randomize();
	m_WindTimeY = noise.generate(1000, true, 10.0);

	// wind gust variation (300 s)
	noise.set(0.9, 5);
	noise.randomize();
	m_WindGust = noise.generate(1000, true, 1.0);
}

simdata::Vector3 Atmosphere::getWind(simdata::Vector3 const &p) const {
	return simdata::Vector3::ZERO;
/*  DISABLE FOR THE MOMENT
	simdata::Vector3 wind = m_AverageWind;
	double h = p.z * 0.0033;
	int idx = int(h);
	double f = 0.0;
	if (h < 0) {
		h = 0; 
	} else
	if (h > 98) {
		h = 98; 
	} else {
		f = h - idx;
	}
	wind.x += m_WindAltX[idx]*(1.0-f) + m_WindAltX[idx+1]*f;
	wind.y += m_WindAltY[idx]*(1.0-f) + m_WindAltY[idx+1]*f;
	return wind * m_WindScale * m_GustModulation;
*/
}

double Atmosphere::getTurbulence(simdata::Vector3 const &p, double dist) const {
	// XXX to write...
	return 0.0;
}

void Atmosphere::setPosition(double lat, double lon) { 
	m_Latitude = lat; 
	m_Longitude = lon;
}

void Atmosphere::setPrevailingWind(simdata::Vector3 const &wind) {
	m_PrevailingWind = wind;
}

double Atmosphere::getTemperature(double h) const {
	double scale;
	if (h < 11000.0) {
		scale = 1.0 - h * 0.0000225586;
	} else
	if (h < 20000.0) {
		scale = 0.751865;
	} else {
  		scale = 0.682357 + h * .00000347058;
	}
	return m_GroundTemperature*scale;
}

double Atmosphere::getPressure(double h) const {
	double scale;
	if (h < 11000.0) {
		scale = pow(1.0 - h * 0.0000225586, 5.255876);
	} else
	if (h < 20000.0) {
       		scale = 0.223361 * exp((10999.0-h)*0.000157694);
	} else {
		scale = pow(0.988626 + h * 0.00000502758, -34.16319);
	}
	return m_GroundPressure * scale;
}

double Atmosphere::getDensity(double h) const {
	double scale;
	if (h < 11000.0) {
		scale = pow(1.0 - h * 0.0000225586, 4.255876);
	} else
	if (h < 20000.0) {
		scale = 0.297076 * exp( (10999.0 - h) * 0.000157694);
	} else {
		scale = pow(0.978261 + h * 0.00000497488, -35.16319);
	}
	return m_GroundDensity * scale;
}

/**
 * Update the atmospheric conditions (pressure, winds, etc)
 *
 * Call approximately once per second.
 */
void Atmosphere::update(double dt) {
	if (m_UpdateIndex == 0) reset();
	m_Date.addTime(dt);
	m_UpdateTime += dt;
	m_WindIndex += dt;
	m_WindScale = 0.1 * fabs(m_GroundPressure - m_TargetPressure);
	double f = dt * m_TimeScale;
	if (f > 1.0) f = 1.0;
	m_GroundTemperature *= 1.0 - f;
	m_GroundTemperature += f * m_TargetTemperature;
	m_GroundPressure *= 1.0 - f;
	m_GroundPressure += f * m_TargetPressure;
	m_GroundDensity = m_GroundPressure / (286.9 * m_GroundTemperature);
	f = dt * 0.001; // 1000 second timescale for wind direction change
	m_AverageWind *= 1.0 - f;
	m_AverageWind += f * m_TargetWind;
	if (m_UpdateTime > 300.0) {
		_update();
		m_UpdateTime = 0.0;
	}
	/*
	std::cout << "==========================" << std:: endl;
	std::cout << "PRES: " << m_GroundPressure << std:: endl;
	std::cout << "TEMP: " << m_GroundTemperature << std:: endl;
	std::cout << "WIND: " << m_AverageWind << std:: endl;
	*/
}

/**
 * Internal atmospheric update to set targets.
 *
 * Called by update() approximately every 5 minutes.
 */
void Atmosphere::_update() {
	double sun;
	double cycle = m_Date.getDayOfYear() * 0.017202 - 1.3771;
	sun = fabs(m_Latitude - 0.40*sin(cycle)); // approx noontime sun theta
	double mean_T = cos(sun*0.35)*305.0;
	double t = m_Date.getHour()*0.2618+m_Longitude;
	double dT = cos(t)*10.0;
	m_UpdateIndex++;
	m_TargetTemperature = mean_T + dT;
	int idx = (m_UpdateIndex/3) % 1000; // 15 minutes
	m_TargetPressure = 101000.0 + m_PressureTime[idx];
	m_TargetWind = m_PrevailingWind + simdata::Vector3(m_WindTimeX[idx], m_WindTimeY[idx], 0.0);
	/*
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std:: endl;
	std::cout << "CYCL: " << cycle << " " << m_Latitude << std:: endl;
	std::cout << "SUNH: " << sun << " " << mean_T << std::endl;
	std::cout << "PRES: " << m_TargetPressure << std:: endl;
	std::cout << "TEMP: " << m_TargetTemperature << std:: endl;
	std::cout << "WIND: " << m_TargetWind << std:: endl;
	*/
}

void Atmosphere::setDate(simdata::SimDate const &date) {
	m_Date = date;
}

void Atmosphere::reset() {
	_update();
	m_GroundTemperature = m_TargetTemperature;	
	m_GroundPressure = m_TargetPressure;	
	m_GroundDensity = m_GroundPressure / (286.9 * m_GroundTemperature);
	m_AverageWind = m_TargetWind;
}

/*
http://www.grc.nasa.gov/WWW/K-12/airplane/atmosmet.html

h > 25000:
	T = -131.21 + 0.00299 h
	p = 2.488 * ((T+273.1)/216.6)**(-11.388)
h > 11000:
	T = -56.46
	p = 22.65 * exp(1.73 - 0.000157 h)
else:
	T = 15.04 - 0.00649 h
	p = 101.29 * ((T+273.1)/288.08)**(5.256)

r = p / (0.2869 * (T+273.1))

p in kPa
T in C
h in m
r in kg/m^3
*/


/*

http://scipp.ucsc.edu/outreach/balloon/atmos/1976%20Standard%20Atmosphere.htm
T0 = 288.15 (15 C)
r0 = 1.225 kg/m^3
P0 = 101325 N/m^2

11 km  T = T0 (1-h/44329)
       r = r0 (1-h/44329)**4.255876
       P = P0 (1-h/44329)**5.255876
20 km  T = T0 0.751865
       r = r0 (0.297076)*exp((10999-h)/6341.4)
       P = P0 (0.223361)*exp((10999-h)/6341.4)
32 km  T = T0 (0.682357 + h / 288136)
       r = r0 (0.978261 + h/201010)**(-35.16319)
       P = P0 (0.988626 + h/198903)**(-34.16319)
47 km  T = T0 (0.482561 + h/102906)
       r = r0 (0.857003 + h/57944)**(-13.20114)
       p = P0 (0.898309 + h/55280)**(-12.20114)
*/
       

