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
 * @file GearDynamics.h
 *
 **/


#ifndef __GEARDYNAMICS_H__
#define __GEARDYNAMICS_H__

#include <vector>

#include <SimData/Vector3.h>
#include <SimData/Link.h>

#include <System.h>
#include <BaseDynamics.h>

class LandingGear;


class GearDynamics: public System, public BaseDynamics {

	typedef simdata::Link<LandingGear>::vector GearSet;
	
	void doComplexPhysics(double x);
	void doSimplePhysics(double x);

	//FIXME: just for some testing purpose
	void setStatus(bool on);
public:
	SIMDATA_OBJECT(GearDynamics, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(GearDynamics)
		SIMDATA_XML("gear_set", GearDynamics::m_Gear, true)
	END_SIMDATA_XML_INTERFACE

	GearDynamics();
	virtual void serialize(simdata::Archive&);
	void Retract();
	void Extend();
	bool getExtended() const;
	bool getWOW() const;
	void setBraking(double x);
	void setSteering(double x, double link_brakes=1.0);
	LandingGear const *getGear(unsigned i);
	size_t getGearNumber() const;
	std::vector<simdata::Vector3> getGearPosition() const;
	virtual void preSimulationStep(double dt);
	virtual void postSimulationStep(double dt);
	void computeForceAndMoment(double x);
protected:
	GearSet m_Gear;
	bool m_WOW;
	bool m_Extended;
};


#endif // __GEARDYNAMICS_H__

