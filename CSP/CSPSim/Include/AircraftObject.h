// Combat Simulator Project - CSPSim
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
 * @file AircraftObject.h
 *
 **/


#ifndef __AIRCRAFTOBJECT_H__
#define __AIRCRAFTOBJECT_H__

#include "DynamicObject.h"
#include "HID.h"


/*
class AircraftPhysicsModel;
class GroundCollisionDynamics;
*/

class AircraftObject: public DynamicObject
{
public:
	SIMDATA_OBJECT(AircraftObject, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(AircraftObject, DynamicObject)
	END_SIMDATA_XML_INTERFACE
		//SIMDATA_XML("aircraft_dynamics", AircraftObject::m_AircraftDynamics, true)

	DECLARE_INPUT_INTERFACE(AircraftObject, DynamicObject)
		BIND_ACTION("SMOKE_ON", SmokeOn);
		BIND_ACTION("SMOKE_OFF", SmokeOff);
		BIND_ACTION("SMOKE_TOGGLE", SmokeToggle);
		BIND_ACTION("MARKS_TOGGLE", MarkersToggle);
	END_INPUT_INTERFACE

public:

	// input event handlers
	void SmokeOn();
	void SmokeOff();
	void SmokeToggle();
	void MarkersToggle();

	AircraftObject();
	virtual ~AircraftObject();
	virtual void dump();
	
	virtual void onRender();

	virtual void initialize();
	
	void setAttitude(double pitch, double roll, double heading);

	/**
	inline double getAngleOfAttack() const {  
		return m_FlightDynamics->getAngleOfAttack();
	}
	inline double getSideSlip() const {  
		return m_FlightDynamics->getSideSlip();
	}
	inline double getGForce() const { 
		return m_FlightDynamics->getGForce();
	}
	inline double getSpeed() const { 
		return m_FlightDynamics->getSpeed();
	}
	*/

	void getInfo(std::vector<std::string> &info) const;

	virtual void setDataRecorder(DataRecorder *recorder);

protected:

	virtual double onUpdate(double);
	void postUpdate(double dt);
	//void doPhysics(double dt);

	virtual void registerChannels(Bus::Ref bus);
	virtual void bindChannels(Bus::Ref bus);

	virtual void postCreate();
	virtual void convertXML();

	// dynamic properties	
	DataChannel<double>::Ref b_Roll;
	DataChannel<double>::Ref b_Pitch;
	DataChannel<double>::Ref b_Heading;
};

	
#endif // __AIRCRAFTOBJECT_H__

