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
 * @file AircraftObject.h
 *
 **/


#ifndef __AIRCRAFTOBJECT_H__
#define __AIRCRAFTOBJECT_H__

#include "DynamicObject.h"
#include "HID.h"
#include "AeroDynamics.h"
#include "LandingGear.h"



class AircraftObject: public DynamicObject
{
public:
	SIMDATA_OBJECT(AircraftObject, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(AircraftObject, DynamicObject)
		SIMDATA_XML("flight_model", AircraftObject::m_FlightModel, true)
		SIMDATA_XML("complex_physics", AircraftObject::m_ComplexPhysics, true)
		SIMDATA_XML("aileron_min", AircraftObject::m_AileronMin, true)
		SIMDATA_XML("aileron_max", AircraftObject::m_AileronMax, true)
		SIMDATA_XML("elevator_min", AircraftObject::m_ElevatorMin, true)
		SIMDATA_XML("elevator_max", AircraftObject::m_ElevatorMax, true)
		SIMDATA_XML("rudder_min", AircraftObject::m_RudderMin, true)
		SIMDATA_XML("rudder_max", AircraftObject::m_RudderMax, true)
		SIMDATA_XML("landing_gear", AircraftObject::m_Gear, true)
	END_SIMDATA_XML_INTERFACE

	AXIS_INTERFACE(AircraftObject, setThrottle);
	AXIS_INTERFACE(AircraftObject, setRudder);
	AXIS_INTERFACE(AircraftObject, setAileron);
	AXIS_INTERFACE(AircraftObject, setElevator);
	ACTION_INTERFACE(AircraftObject, IncElevator);
	ACTION_INTERFACE(AircraftObject, noIncElevator);
	ACTION_INTERFACE(AircraftObject, DecElevator);
	ACTION_INTERFACE(AircraftObject, noDecElevator);
	ACTION_INTERFACE(AircraftObject, IncAileron);
	ACTION_INTERFACE(AircraftObject, noIncAileron);
	ACTION_INTERFACE(AircraftObject, DecAileron);
	ACTION_INTERFACE(AircraftObject, noDecAileron);
	ACTION_INTERFACE(AircraftObject, IncThrottle);
	ACTION_INTERFACE(AircraftObject, noIncThrottle);
	ACTION_INTERFACE(AircraftObject, DecThrottle);
	ACTION_INTERFACE(AircraftObject, noDecThrottle);
	ACTION_INTERFACE(AircraftObject, SmokeOn);
	ACTION_INTERFACE(AircraftObject, SmokeOff);
	ACTION_INTERFACE(AircraftObject, SmokeToggle);
	AXIS_INTERFACE(AircraftObject, setWheelBrake);
	ACTION_INTERFACE(AircraftObject, WheelBrakePulse);
	ACTION_INTERFACE(AircraftObject, WheelBrakeOn);
	ACTION_INTERFACE(AircraftObject, WheelBrakeOff);
	ACTION_INTERFACE(AircraftObject, WheelBrakeToggle);
	ACTION_INTERFACE(AircraftObject, GearUp);
	ACTION_INTERFACE(AircraftObject, GearDown);
	ACTION_INTERFACE(AircraftObject, GearToggle);

	AircraftObject();
	virtual ~AircraftObject();
	virtual void dump();
	
	virtual unsigned int onRender();

	virtual void initialize();

	virtual void onUpdate(double);
	void updateControls(double);
	void doSimplePhysics(double dt);
	void doComplexPhysics(double dt);
	void doMovement(double dt);
	void doFCS(double dt);

	void setAttitude(double pitch, double roll, double heading);

	void setComplexPhysics(bool flag) { m_ComplexPhysics = flag; }

	double getAngleOfAttack() const { return m_FlightModel->getAngleOfAttack(); }
	double getSideSlip() const { return m_FlightModel->getSideSlip(); }
	virtual double getGForce() const { return m_FlightModel->getGForce();}
	virtual double getSpeed() const { return m_FlightModel->getSpeed();}

	void getStats(std::vector<std::string> &stats) const;

//	void initializeHud();
//	Hud * GetpHud() const { return m_phud;};
//	Hud * m_phud;

protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);
	virtual void postCreate();

	virtual void setGearStatus(bool on);
	bool isGearRetracted() const;

	simdata::Link<AeroDynamics> m_FlightModel;
	
	// dynamic properties
	
	double m_Roll;
	double m_Pitch;
	double m_Heading;
	
	// control inputs
	
	double m_AileronInput;
	double m_ElevatorInput;
	double m_RudderInput;
	double m_ThrottleInput;

	// keyboard inputs
	
	double m_dAileronInput;
	double m_dElevatorInput;
	double m_dRudderInput;
	double m_dThrottleInput;
	double m_BrakeInput;
	double m_BrakePulse;

	// keyboard relaxation

	int m_decayAileron;
	int m_decayElevator;
	int m_decayRudder;

	// control surfaces
	double m_Aileron;
	double m_Elevator;
	double m_Rudder;
	double m_Throttle;

	// control surface limits

	double m_AileronMin;
	double m_AileronMax;
	double m_ElevatorMin;
	double m_ElevatorMax;
	double m_RudderMin;
	double m_RudderMax;

	// landing gear
	simdata::Link<LandingGearSet> m_Gear;
	
	bool m_PhysicsInitialized;
	bool m_ComplexPhysics;

};


	

//////////////////////////////////////////////////////////////////////////

/*
class AircraftInterface: public VirtualHID
{
public:
	AircraftInterface() {
		BIND_AXIS(AircraftObject, "THROTTLE", setThrottle);
		BIND_AXIS(AircraftObject, "AILERON", setAileron);
		BIND_AXIS(AircraftObject, "ELEVATOR", setElevator);
		BIND_AXIS(AircraftObject, "RUDDER", setRudder);
	}
};
*/


#endif // __AIRCRAFTOBJECT_H__

