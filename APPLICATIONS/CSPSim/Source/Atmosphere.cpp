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
		m_offset = int(simdata::g_Random.newRand()*1.0e+9);
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
	m_FastUpdate = 0.0;
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
	m_GustIndex = 0;
	reset();
	tabulateCAS();
}

void Atmosphere::generateWinds() {
	Perlin1D noise;
	int i;

	// pressure variation (10 days)
	noise.set(0.3, 5);
	noise.randomize();
	m_PressureTime = noise.generate(1000, true, 30.0, 15000.0, 0.0);

	// turbulence (10000 m)
	noise.set(0.9, 6);
	noise.randomize();
	m_TurbulenceX = noise.generate(1000, true, 10.0, 1.0, 0.0);
	noise.randomize();
	m_TurbulenceY = noise.generate(1000, true, 10.0, 1.0, 0.0);
	noise.randomize();
	m_TurbulenceZ = noise.generate(1000, true, 10.0, 1.0, 0.0);
	for (i = 0; i < 1000; i++) {
		float a;
		a= m_TurbulenceX[i];
		m_TurbulenceX[i] *= 10.0 * a * fabs(a);
		a = m_TurbulenceY[i];
		m_TurbulenceY[i] *= 10.0 * a * fabs(a);
		a = m_TurbulenceZ[i];
		m_TurbulenceZ[i] *= 10.0 * a * fabs(a);
	}

	// turbulence altitude buffers (15000 m)
	noise.set(0.9, 6);
	noise.randomize();
	m_TurbulenceAltA = noise.generate(1000, false, 10.0, 3.0, -1.5);
	noise.randomize();
	m_TurbulenceAltB = noise.generate(1000, false, 10.0, 3.0, -1.5);
	for (i = 0; i < 1000; i++) {
		float f = i * 0.015; // f in km
		f = (12.5 * f + 7.6) / (f*f + 45.0);
		m_TurbulenceAltA[i] *= f;
		m_TurbulenceAltB[i] *= f;
	}
	
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
	m_GustTime = noise.generate(1000, true, 60.0, 3.0);
}

simdata::Vector3 Atmosphere::getWind(simdata::Vector3 const &p) const {
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
	double alt_scale = 1.0 + h*h*0.0025;
	if (alt_scale > 2.0) alt_scale = 2.0;
	wind.x += m_WindAltX[idx]*(1.0-f) + m_WindAltX[idx+1]*f;
	wind.y += m_WindAltY[idx]*(1.0-f) + m_WindAltY[idx+1]*f;
	return wind * m_WindScale * m_GustModulation * alt_scale;
}

simdata::Vector3 Atmosphere::getTurbulence(simdata::Vector3 const &p, double dist) const {
	int idx = int(p.z * 1000.0 / 15000.0);
	if (idx < 0) idx = 0;
	else if (idx > 999) idx = 999;
	float a = m_TurbulenceAltA[idx];
	float b = m_TurbulenceAltB[idx];
	if (a < 0.0) a = 0.0;
	if (b < 0.0) b = 0.0;
	a = a * (1.0 - m_TurbulenceBlend) + b * m_TurbulenceBlend;
	
	/*
	static int i = 0;
	if ((i++ % 40) == 0) {
		std::cout << a << "\n";
	}
	*/

	if (a <= 0.0) return simdata::Vector3::ZERO;

	if (dist < 0.0) dist = - dist;
	idx = int(dist * 0.1) % 1000;
	return simdata::Vector3(a*m_TurbulenceX[idx],a*m_TurbulenceY[idx],a*m_TurbulenceZ[idx]);
}

void Atmosphere::setPosition(double lat, double lon) { 
	m_Latitude = lat; 
	m_Longitude = lon;
}

void Atmosphere::setPrevailingWind(simdata::Vector3 const &wind) {
	m_PrevailingWind = wind;
}

/**
 * Air temperature versus altitude.
 *
 * From 1976 Standard Atmosphere, below 32000 m
 */
double Atmosphere::getTemperature(double h) const {
	double scale;
	if (h < 11000.0) {
		scale = 1.0 - h * 0.0000225586;
	} else
	if (h < 20000.0) {
		scale = 0.751865;
	} else {
		if (h > 32000.0) h = 32000.0;
  		scale = 0.682357 + h * .00000347058;
	}
	return m_GroundTemperature*scale;
}

/**
 * Air pressure versus altitude.
 *
 * From 1976 Standard Atmosphere, below 32000 m
 */
double Atmosphere::getPressure(double h) const {
	double scale;
	if (h < 11000.0) {
		scale = pow(1.0 - h * 0.0000225586, 5.255876);
	} else
	if (h < 20000.0) {
       		scale = 0.223361 * exp((10999.0-h)*0.000157694);
	} else {
		if (h > 32000.0) h = 32000.0;
		scale = pow(0.988626 + h * 0.00000502758, -34.16319);
	}
	return m_GroundPressure * scale;
}


/**
 * Air density versus altitude.
 *
 * From 1976 Standard Atmosphere, below 32000 m
 */
double Atmosphere::getDensity(double h) const {
	double scale;
	if (h < 11000.0) {
		scale = pow(1.0 - h * 0.0000225586, 4.255876);
	} else
	if (h < 20000.0) {
		scale = 0.297076 * exp( (10999.0 - h) * 0.000157694);
	} else {
		if (h > 32000.0) h = 32000.0;
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
	m_FastUpdate += dt;
	float gust = m_GustTime[m_GustIndex%1000];
	if (gust < 0.0) gust = 0.0;
	m_GustModulation = 1.0 + gust;
	m_GustIndex++;
	//std::cout << getWind(simdata::Vector3::ZERO) << m_GustModulation << std:: endl;
	if (m_FastUpdate < 3.0) return;
	if (m_UpdateIndex == 0) reset();
	dt = m_FastUpdate;
	m_FastUpdate = 0.0;
	m_UpdateTime += dt;
	m_Date.addTime(dt);
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
	if (m_TurbulenceBlendUp) {
		m_TurbulenceBlend += dt * 0.0003;
		if (m_TurbulenceBlend > 1.0) {
			Perlin1D noise;
			noise.set(0.9, 6);
			noise.randomize();
			m_TurbulenceBlend = 1.0;
			m_TurbulenceBlendUp = false;
			m_TurbulenceAltA = noise.generate(1000, false, 10.0, 2.0, -1.0);
			for (int i = 0; i < 1000; i++) {
				float f = i * 0.015; // f in km
				f = (12.5 * f + 7.6) / (f*f + 45.0);
				m_TurbulenceAltA[i] *= f;
			}
		}
	} else {
		m_TurbulenceBlend += dt * 0.0003;
		if (m_TurbulenceBlend < 0.0) {
			Perlin1D noise;
			noise.set(0.9, 6);
			noise.randomize();
			m_TurbulenceBlend = 0.0;
			m_TurbulenceBlendUp = true;
			m_TurbulenceAltB = noise.generate(1000, false, 10.0, 2.0, -1.0);
			for (int i = 0; i < 1000; i++) {
				float f = i * 0.015; // f in km
				f = (12.5 * f + 7.6) / (f*f + 45.0);
				m_TurbulenceAltB[i] *= f;
			}
		}
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
	_update();
	_update();
	_update();
}


double Atmosphere::getSpeedOfSound(double altitude) const {
	double T = getTemperature(altitude);
        return 20.0324 * sqrt(T); // m/s
}

double Atmosphere::getPreciseCAS(double mach, double altitude) const {
	double delta = getPressure(altitude) / m_GroundPressure;
	double gamma = pow(1.0 + 0.2 * mach * mach, 3.5) - 1.0;
	return 760.369 * sqrt( pow(gamma * delta + 1.0, 0.2857142857) - 1.0 );
}


void Atmosphere::tabulateCAS() {
	simdata::Table::vector_t breaks, cas;
	simdata::Table::vector_it cas_i;
	int i, j;
	breaks.resize(300);
	for (i = 0; i < 300; i++) breaks[i] = 0.02 * i;
	m_CAS.setXBreaks(breaks);
	m_CAS.setXSpacing(0.02);
	breaks.resize(300);
	for (i = 0; i < 300; i++) breaks[i] = 100.0 * i;
	m_CAS.setYBreaks(breaks);
	m_CAS.setYSpacing(100.0);
	cas.resize(300*300);
	cas_i = cas.begin();
	for (i = 0; i < 300; i++) {
		float altitude = 100.0 * i;
		for (j = 0; j < 300; j++) {
			float mach = 0.02 * j;
			*cas_i++ = getPreciseCAS(mach, altitude);
		}
	}
	m_CAS.setData(cas);
	m_CAS.interpolate();
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
       

