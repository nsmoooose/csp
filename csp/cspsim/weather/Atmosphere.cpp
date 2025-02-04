// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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

#include <csp/csplib/util/Noise.h>
#include <csp/csplib/util/Random.h>
#include <csp/cspsim/weather/Atmosphere.h>

// temporary, for testing.
#include <cstdio>
namespace {
	void DumpNoise(std::vector<float> const &noise, std::string const &filename) {
		FILE *fp = fopen(filename.c_str(), "wt");
		for (unsigned i = 0; i < noise.size(); ++i) {
			fprintf(fp, "%d\t%f\n", i, noise[i]);
		}
	}
}


namespace csp {

namespace weather {

Atmosphere::Atmosphere() {
	generateWinds();
	m_Latitude = 0.0;
	m_Longitude = 0.0;
	m_FastUpdate = 0.0;
	m_UpdateTime = 0.0;
	m_UpdateIndex = 0;
	m_TargetTemperature = 288.15;
	m_TargetPressure = 101325.0;
	m_TimeScale = 0.00015; // about 2 hours
	m_WindIndex = 0.0;
	m_WindScale = 0.0;
	m_PrevailingWind = Vector3(1.0, 0.0, 0.0);
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
	noise.setParameters(0.3, 5);
	noise.randomize();
	m_PressureTime = noise.generate(1000, true, 10.0, 15000.0, 0.0);
	DumpNoise(m_PressureTime, "atmosphere.pressure-time.dat");

	// turbulence (10000 m)
	noise.setParameters(0.9, 6);
	noise.randomize();
	m_TurbulenceX = noise.generate(1000, true, 10.0, 1.0, 0.0);
	noise.randomize();
	m_TurbulenceY = noise.generate(1000, true, 10.0, 1.0, 0.0);
	noise.randomize();
	m_TurbulenceZ = noise.generate(1000, true, 10.0, 1.0, 0.0);
	for (i = 0; i < 1000; i++) {
		float a;
		a = m_TurbulenceX[i];
		m_TurbulenceX[i] *= 10.0f * a * fabsf(a);
		a = m_TurbulenceY[i];
		m_TurbulenceY[i] *= 10.0f * a * fabsf(a);
		a = m_TurbulenceZ[i];
		m_TurbulenceZ[i] *= 10.0f * a * fabsf(a);
	}
	DumpNoise(m_TurbulenceX, "atmosphere.turbulence-x.dat");
	DumpNoise(m_TurbulenceY, "atmosphere.turbulence-y.dat");
	DumpNoise(m_TurbulenceZ, "atmosphere.turbulence-z.dat");

	// turbulence altitude buffers (15000 m)
	noise.setParameters(0.9, 6);
	noise.randomize();
	m_TurbulenceAltA = noise.generate(1000, false, 10.0, 3.0, -1.5);
	noise.randomize();
	m_TurbulenceAltB = noise.generate(1000, false, 10.0, 3.0, -1.5);
	for (i = 0; i < 1000; i++) {
		float f = i * 0.015f; // f in km
		f = (12.5f * f + 7.6f) / (f*f + 45.0f);
		m_TurbulenceAltA[i] *= f;
		m_TurbulenceAltB[i] *= f;
	}
	DumpNoise(m_TurbulenceAltA, "atmosphere.turbulence-alt-a.dat");
	DumpNoise(m_TurbulenceAltB, "atmosphere.turbulence-alt-b.dat");
	
	// wind direction variation (30000 m)
	noise.setParameters(0.8, 4);
	noise.randomize();
	m_WindAltX = noise.generate(100, false, 8.0, 1.0, 0.0);
	noise.randomize();
	m_WindAltY = noise.generate(100, false, 8.0, 1.0, 0.0);
	for (i = 0; i < 100; ++i) {
		const float alt_scale = 1.0f + i * i * 0.0025f;
		m_WindAltX[i] *= alt_scale;
		m_WindAltY[i] *= alt_scale;
	}
	DumpNoise(m_WindAltX, "atmosphere.wind-alt-x.dat");
	DumpNoise(m_WindAltY, "atmosphere.wind-alt-y.dat");

	// wind direction variation (10 days)
	noise.setParameters(0.6, 5);
	noise.randomize();
	m_WindTimeX = noise.generate(1000, true, 10.0);
	noise.randomize();
	m_WindTimeY = noise.generate(1000, true, 10.0);
	DumpNoise(m_WindTimeX, "atmosphere.wind-time-x.dat");
	DumpNoise(m_WindTimeY, "atmosphere.wind-time-y.dat");

	// wind gust variation (300 s)
	noise.setParameters(0.9, 5);
	noise.randomize();
	m_GustTime = noise.generate(1000, true, 60.0, 1.5);
	DumpNoise(m_GustTime, "atmosphere.gust-time.dat");
}

Vector3 Atmosphere::getWind(Vector3 const &p) const {
	Vector3 wind = m_AverageWind;
	const double h = std::min(98.0, std::max(0.0, p.z() * 0.0033));
	const int idx = static_cast<int>(h);
	const double f = h - idx;
	wind.x() += m_WindAltX[idx]*(1.0-f) + m_WindAltX[idx+1]*f;
	wind.y() += m_WindAltY[idx]*(1.0-f) + m_WindAltY[idx+1]*f;
	wind *= m_WindScale * m_GustModulation;
	if (wind.length() > 100.0) {  // XXX remove me
		CSPLOG(WARNING, PHYSICS) << "strong wind! " << m_WindScale << " " << m_GustModulation << " " << wind;
	}
	return wind;
}

Vector3 Atmosphere::getTurbulence(Vector3 const &p, double dist) const {
	assert(dist >= 0.0 && dist < 2e+9);
	int idx = clampTo(static_cast<int>(p.z() * (1000.0 / 15000.0)), 0, 999);
	double a = std::max(m_TurbulenceAltA[idx], 0.0f);
	float b = std::max(m_TurbulenceAltB[idx], 0.0f);
	assert(m_TurbulenceBlend >= 0.0 && m_TurbulenceBlend <= 1.0);
	a = a * (1.0 - m_TurbulenceBlend) + b * m_TurbulenceBlend;
	if (a <= 0.0) return Vector3::ZERO;
	idx = std::max(0, static_cast<int>(dist * 0.1)) % 1000;
	const Vector3 turbulence(a * m_TurbulenceX[idx], a * m_TurbulenceY[idx], a * m_TurbulenceZ[idx]);
	if (turbulence.length() > 100.0) {  // XXX remove me
		CSPLOG(WARNING, PHYSICS) << "strong turbulence! " << a << " " << turbulence << " " << m_TurbulenceX[idx] << " " << idx << " " << dist << " " << m_TurbulenceBlend;
	}
	return turbulence;
}

void Atmosphere::setPosition(double lat, double lon) {
	m_Latitude = lat;
	m_Longitude = lon;
}

void Atmosphere::setPrevailingWind(Vector3 const &wind) {
	m_PrevailingWind = wind;
}

float Atmosphere::getTemperature(double h) const {
	double scale;
	if (h < 11000.0) {
		scale = 1.0 - h * 0.0000225586;
	} else
	if (h < 20000.0) {
		scale = 0.751865f;
	} else {
		if (h > 32000.0) h = 32000.0;
  		scale = 0.682357 + h * .00000347058;
	}
	return static_cast<float>(m_GroundTemperature*scale);
}

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

bool Atmosphere::fastUpdate(double &dt) {
	m_FastUpdate += dt;
	m_GustIndex += dt * 3.0;
	const int index = static_cast<int>(m_GustIndex);
	const float f = static_cast<float>(m_GustIndex - index);
	const float gust = m_GustTime[index%1000] * (1.0f - f) + m_GustTime[(index+1)%1000] * f;
	m_GustModulation = std::max(1.0, 1.0 + gust);
	if (m_FastUpdate < 3.0) return false;
	dt = m_FastUpdate;
	m_FastUpdate = 0.0;
	return true;
}

void Atmosphere::update(double dt) {
	if (fastUpdate(dt)) return;
	if (m_UpdateIndex == 0) reset();
	m_UpdateTime += dt;
	m_Date.addTime(dt);
	m_WindIndex += dt;
	m_WindScale = 0.05 * fabs(m_GroundPressure - m_TargetPressure);
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
		slowUpdate();
		m_UpdateTime = 0.0;
	}
	if (m_TurbulenceBlendUp) {
		m_TurbulenceBlend += dt * 0.0003;
		if (m_TurbulenceBlend > 1.0) {
			Perlin1D noise;
			noise.setParameters(0.9, 6);
			noise.randomize();
			m_TurbulenceBlend = 1.0;
			m_TurbulenceBlendUp = false;
			m_TurbulenceAltA = noise.generate(1000, false, 10.0, 2.0, -1.0);
			for (int i = 0; i < 1000; i++) {
				float f = i * 0.015f; // f in km
				f = (12.5f * f + 7.6f) / (f*f + 45.0f);
				m_TurbulenceAltA[i] *= f;
			}
		}
	} else {
		m_TurbulenceBlend -= dt * 0.0003;
		if (m_TurbulenceBlend < 0.0) {
			Perlin1D noise;
			noise.setParameters(0.9, 6);
			noise.randomize();
			m_TurbulenceBlend = 0.0;
			m_TurbulenceBlendUp = true;
			m_TurbulenceAltB = noise.generate(1000, false, 10.0, 2.0, -1.0);
			for (int i = 0; i < 1000; i++) {
				float f = i * 0.015f; // f in km
				f = (12.5f * f + 7.6f) / (f*f + 45.0f);
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
void Atmosphere::slowUpdate() {
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
	m_TargetWind = m_PrevailingWind + Vector3(m_WindTimeX[idx], m_WindTimeY[idx], 0.0);
	/*
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std:: endl;
	std::cout << "CYCL: " << cycle << " " << m_Latitude << std:: endl;
	std::cout << "SUNH: " << sun << " " << mean_T << std::endl;
	std::cout << "PRES: " << m_TargetPressure << std:: endl;
	std::cout << "TEMP: " << m_TargetTemperature << std:: endl;
	std::cout << "WIND: " << m_TargetWind << std:: endl;
	*/
}

void Atmosphere::setDate(SimDate const &date) {
	m_Date = date;
}

void Atmosphere::reset() {
	slowUpdate();
	m_GroundTemperature = m_TargetTemperature;
	m_GroundPressure = m_TargetPressure;
	m_GroundDensity = m_GroundPressure / (286.9 * m_GroundTemperature);
	m_AverageWind = m_TargetWind;
	slowUpdate();
	slowUpdate();
	slowUpdate();
}


float Atmosphere::getSpeedOfSound(double altitude) const {
	float T = getTemperature(altitude);
	return 20.0324f * sqrtf(T); // m/s
}

float Atmosphere::getPreciseCAS(double mach, double altitude) const {
	double delta = getPressure(altitude) / m_GroundPressure;
	double gamma = pow(1.0 + 0.2 * mach * mach, 3.5) - 1.0;
	return 760.369f * static_cast<float>(sqrt( pow(gamma * delta + 1.0, 0.2857142857) - 1.0 ));
}


void Atmosphere::tabulateCAS() {
	std::vector<float> cas;
	std::vector<float>::iterator cas_i;
	std::vector< std::vector<float> > breaks(2);
	int i, j;
	breaks[0].resize(300);
	breaks[1].resize(300);
	for (i = 0; i < 300; i++) breaks[0][i] = 0.02f * i;
	for (i = 0; i < 300; i++) breaks[1][i] = 100.0f * i;
	cas.resize(300*300);
	cas_i = cas.begin();
	for (i = 0; i < 300; i++) {
		float mach = 0.02f * i;
		for (j = 0; j < 300; j++) {
			float altitude = 100.0f * j;
			*cas_i++ = getPreciseCAS(mach, altitude);
		}
	}
	m_CAS.load(cas, breaks);
	m_CAS.interpolate(Table2::Dim(300)(300), Interpolation::LINEAR);
}

}

} // namespace csp


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


