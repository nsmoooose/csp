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
#include "Engine.h"
#include "HID.h"
#include "LandingGear.h"
//#include "PrimaryAeroDynamics.h"
#include "FlightDynamics.h"


class FlightModel;
class AnimationValueChannel;


class AircraftDynamics: public simdata::Object {
	//simdata::Link<PrimaryAeroDynamics> m_PrimaryAeroDynamics;
	simdata::Ref<FlightDynamics> m_FlightDynamics;
	simdata::Link<FlightModel> m_FlightModel;
	simdata::Link<GearDynamics> m_GearDynamics;
	simdata::Link<EngineDynamics> m_EngineDynamics;
public:
	SIMDATA_OBJECT(AircraftDynamics, 4, 0)
	BEGIN_SIMDATA_XML_INTERFACE(AircraftDynamics)
		//SIMDATA_XML("primary_aero_dynamics", AircraftDynamics::m_PrimaryAeroDynamics, true)
		SIMDATA_XML("flight_model", AircraftDynamics::m_FlightModel, true)
		SIMDATA_XML("gear_dynamics", AircraftDynamics::m_GearDynamics, true) 
		SIMDATA_XML("engine_dynamics", AircraftDynamics::m_EngineDynamics, true)
	END_SIMDATA_XML_INTERFACE
	/*
	simdata::Ref<PrimaryAeroDynamics> getAeroDynamics() const {
		return m_PrimaryAeroDynamics;
	}
	*/
	simdata::Ref<FlightDynamics> getFlightDynamics() const {
		return m_FlightDynamics;
	}
	simdata::Ref<GearDynamics> getGearDynamics() const {
		return m_GearDynamics;
	}
	simdata::Ref<EngineDynamics> getEngineDynamics() const {
		return m_EngineDynamics;
	}
protected:
	void pack(simdata::Packer& p) const;
	void unpack(simdata::UnPacker& p);
	void postCreate();
};


class AircraftPhysicsModel;
class GroundCollisionDynamics;

class AircraftObject: public DynamicObject
{
public:
	SIMDATA_OBJECT(AircraftObject, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(AircraftObject, DynamicObject)
		SIMDATA_XML("complex_physics", AircraftObject::m_ComplexPhysics, true)
		SIMDATA_XML("aileron_min", AircraftObject::m_AileronMin, true)
		SIMDATA_XML("aileron_max", AircraftObject::m_AileronMax, true)
		SIMDATA_XML("elevator_min", AircraftObject::m_ElevatorMin, true)
		SIMDATA_XML("elevator_max", AircraftObject::m_ElevatorMax, true)
		SIMDATA_XML("rudder_min", AircraftObject::m_RudderMin, true)
		SIMDATA_XML("rudder_max", AircraftObject::m_RudderMax, true)
		SIMDATA_XML("airbrake_max", AircraftObject::m_AirbrakeMax, true)
		SIMDATA_XML("airbrake_rate", AircraftObject::m_AirbrakeRate, true)
		SIMDATA_XML("aircraft_dynamics", AircraftObject::m_AircraftDynamics, true)
	END_SIMDATA_XML_INTERFACE

	AXIS_INTERFACE(AircraftObject, setThrottle);
	AXIS_INTERFACE(AircraftObject, setRudder);
	AXIS_INTERFACE(AircraftObject, setAileron);
	AXIS_INTERFACE(AircraftObject, setElevator);
	AXIS_INTERFACE(AircraftObject, setAirbrake);
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
	ACTION_INTERFACE(AircraftObject, MarkersToggle);
	ACTION_INTERFACE(AircraftObject, OpenAirbrake);
	ACTION_INTERFACE(AircraftObject, CloseAirbrake);
	ACTION_INTERFACE(AircraftObject, IncAirbrake);
	ACTION_INTERFACE(AircraftObject, DecAirbrake);

	AircraftObject();
	virtual ~AircraftObject();
	virtual void dump();
	
	virtual double onUpdate(double);
	virtual unsigned int onRender();

	virtual void initialize();
	
	void updateControls(double);
	void doSimplePhysics(double dt);
	void doComplexPhysics(double dt);
	void doMovement(double dt);
	void doFCS(double dt);

	void setAttitude(double pitch, double roll, double heading);

	void setComplexPhysics(bool flag) { m_ComplexPhysics = flag; }

	inline double getAngleOfAttack() const {  
		//return m_PrimaryAeroDynamics->getAngleOfAttack();
		return m_FlightDynamics->getAngleOfAttack();
	}
	inline double getSideSlip() const {  
		//return m_PrimaryAeroDynamics->getSideSlip();
		return m_FlightDynamics->getSideSlip();
	}
	inline double getGForce() const { 
		//return m_PrimaryAeroDynamics->getGForce();
		return m_FlightDynamics->getGForce();
	}
	inline double getSpeed() const { 
		//return m_PrimaryAeroDynamics->getSpeed();
		return m_FlightDynamics->getSpeed();
	}

	void getStats(std::vector<std::string> &stats) const;

//	void initializeHud();
//	Hud * GetpHud() const { return m_phud;};
//	Hud * m_phud;

protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);
	virtual void postCreate();
	virtual void convertXML();

	virtual void bindAnimations();
	simdata::Ref<AnimationValueChannel> m_AnimateRudder;
	simdata::Ref<AnimationValueChannel> m_AnimateElevator;
	simdata::Ref<AnimationValueChannel> m_AnimateAileron;
	simdata::Ref<AnimationValueChannel> m_AnimateAirbrake;

	virtual void setGearStatus(bool on);
	bool isGearRetracted() const;

	// data recording
	virtual void initDataRecorder();

	// dynamic properties	
	double m_Roll;
	double m_Pitch;
	double m_Heading;
	
	// control inputs	
	double m_AileronInput;
	double m_ElevatorInput;
	double m_RudderInput;
	double m_ThrottleInput;
	double m_AirbrakeInput;

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
	double m_Airbrake;

	// control surface limits
	double m_AileronMin;
	double m_AileronMax;
	double m_ElevatorMin;
	double m_ElevatorMax;
	double m_RudderMin;
	double m_RudderMax;
	double m_AirbrakeMax;
	double m_AirbrakeRate;

	simdata::Ref<AircraftPhysicsModel> m_AircraftPhysicsModel;

	// various torques
	simdata::Link<AircraftDynamics> m_AircraftDynamics;
	//simdata::Ref<PrimaryAeroDynamics> m_PrimaryAeroDynamics;
	simdata::Ref<FlightDynamics> m_FlightDynamics;
	simdata::Ref<GearDynamics> m_GearDynamics;
	simdata::Ref<GroundCollisionDynamics> m_GroundCollisionDynamics;
	simdata::Ref<EngineDynamics> m_EngineDynamics;
	
	bool m_PhysicsInitialized;
	bool m_ComplexPhysics;

private:
	enum {
		CH_AILERON_DEFLECTION, 
		CH_ELEVATOR_DEFLECTION, 
		CH_RUDDER_DEFLECTION,
	};
	RecorderInterface m_Recorder;
	
};

	
#endif // __AIRCRAFTOBJECT_H__

