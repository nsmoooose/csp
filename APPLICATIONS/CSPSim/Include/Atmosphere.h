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


class Atmosphere { //: public simdata::Object {

public:
	Atmosphere();
	double getGravity(double alt) const { return 9.806; }
	double getTemperature(double alt) const;
	double getPressure(double alt) const;
	double getDensity(double alt) const;
	simdata::Vector3 getWind(simdata::Vector3 const &) const;
	double getTurbulence(simdata::Vector3 const &, double dist) const;
	void setPosition(double lat, double lon);
	void setPrevailingWind(simdata::Vector3 const &);
	void setDate(simdata::SimDate const &);
	void reset();
	void update(double dt);
	
protected:
	void _update();
	void generateWinds();

	double m_Latitude;
	double m_Longitude;
	double m_TargetTemperature;
	double m_GroundTemperature;
	double m_TargetPressure;
	double m_GroundPressure;
	double m_GroundDensity;
	double m_TimeScale;
	double m_UpdateTime;
	double m_WindIndex;
	double m_WindScale;
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
	std::vector<float> m_WindGust;
	std::vector<float> m_DensityTime;
	std::vector<float> m_TurbulenceAltA;
	std::vector<float> m_TurbulenceAltB;
	double m_TurbulenceBlend;
	bool m_TurbulenceBlendUp;
	double m_GustModulation;
};


#endif // __ATMOSPHERE_H__

