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
 * @file Atmosphere.h
 *
 **/

#ifndef __ATMOSPHERE_H__
#define __ATMOSPHERE_H__

#include <SimData/Object.h>
#include <SimData/Vector3.h>
#include <SimData/Date.h>
#include <SimData/Interpolate.h>

template <typename T>
T convert_kts_mps(T x) { return x * 0.514444; }

template <typename T>
T convert_mps_kts(T x) { return x * 1.94385; }

template <typename T>
T convert_ft_m(T x) { return x * 0.3048; }

template <typename T>
T convert_m_ft(T x) { return x * 3.2808398950; }

template <typename T>
T convert_kg_lb(T x) { return x * 2.2046; }

template <typename T>
T convert_lb_kg(T x) { return x * 0.45360; }

template <typename T>
T convert_nm_m(T x) { return x * 1852.0; }

template <typename T>
T convert_m_nm(T x) { return x * 0.0005399568; }

template <typename T>
T convert_pa_mmhg(T x) { return x * 0.00752; }

template <typename T>
T convert_mmhg_pa(T x) { return x * 133.0; }


/**
 * class Atmosphere
 *
 * This class maintains implements a primitive atmospheric model based on
 * the 1976 Standard Atmosphere.  Dynamic variables such as temperature and
 * pressure are modelled using a comibination of physical effects (e.g.
 * season and sun elevation) and hand-tailored noise functions.  The latter
 * is also used to generate winds, gusts, and turbulence.  A number of utility
 * functions such as mach number and calibrated airspeed are also implemented.
 *
 */
class Atmosphere { //: public simdata::Object {

public:
	Atmosphere();
	double getGravity(double alt) const { return 9.806; }
	float getTemperature(double alt) const;
	double getPressure(double alt) const;
	double getDensity(double alt) const;
	simdata::Vector3 getWind(simdata::Vector3 const &) const;
	simdata::Vector3 getTurbulence(simdata::Vector3 const &, double dist) const;
	void setPosition(double lat, double lon);
	void setPrevailingWind(simdata::Vector3 const &);
	void setDate(simdata::SimDate const &);
	void reset();
	void update(double dt);

	inline double getMach(double speed, double altitude) const;
	float getSpeedOfSound(double altitude) const;
	float getPreciseCAS(double mach, double altitude) const;
	float getCAS(double mach, double altitude) const;
	
protected:
	void _update();
	void generateWinds();
	void tabulateCAS();

	double m_Latitude;
	double m_Longitude;
	double m_TargetTemperature;
	double m_GroundTemperature;
	double m_TargetPressure;
	double m_GroundPressure;
	double m_GroundDensity;
	double m_TimeScale;
	double m_WindIndex;
	double m_WindScale;
	double m_FastUpdate;
	double m_UpdateTime;
	int m_UpdateIndex;
	simdata::SimDate m_Date;
	simdata::Vector3 m_PrevailingWind;
	simdata::Vector3 m_AverageWind;
	simdata::Vector3 m_TargetWind;
	std::vector<float> m_PressureTime;
	std::vector<float> m_WindTimeX;
	std::vector<float> m_WindTimeY;
	std::vector<float> m_WindAltX;
	std::vector<float> m_WindAltY;
	std::vector<float> m_GustTime;
	std::vector<float> m_TurbulenceX;
	std::vector<float> m_TurbulenceY;
	std::vector<float> m_TurbulenceZ;
	std::vector<float> m_TurbulenceAltA;
	std::vector<float> m_TurbulenceAltB;
	simdata::Table m_CAS;
	double m_TurbulenceBlend;
	bool m_TurbulenceBlendUp;
	double m_GustModulation;
	int m_GustIndex;
};

inline double Atmosphere::getMach(double speed, double alt) const {
	return speed / getSpeedOfSound(alt);
}

inline float Atmosphere::getCAS(double mach, double altitude) const {
	return m_CAS.getValue(float(mach), float(altitude));
}


#endif // __ATMOSPHERE_H__

