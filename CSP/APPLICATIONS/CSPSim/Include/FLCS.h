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
 * @file FLCS.h
 *
 **/


#ifndef __FLCS_H__
#define __FLCS_H__

#include "SimObject.h"
#include "HID.h"


class FlightControlSystem: public simdata::Object
{
public:
	SIMDATA_OBJECT(FLCS, 0, 0)
	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(FLCS)
	END_SIMDATA_XML_INTERFACE

	FLCS();
	virtual ~FLCS();
	virtual void dump();
	
	struct PilotInput {
		double stick_roll;
		double stick_pitch;
		double rudder;
	}

	struct ControlSurfaces {
		double rudder;
		double elevator;
		double flaps;
		double le_flaps;
		double ailerons;
	}

	virtual void bindAircraft(AircraftObject *a, PilotInput *i, ControlSurfaces *c) { 
		m_Input = i; 
		m_Control = c;
	}
	virtual void update(double dt);

protected:
	PilotInput *m_Input;
	ControlSurfaces *m_ControlSurfaces;

};

class FlightControlSystem_Mirage {

	virtual void OnUpdate(double);
	void doSimplePhysics(double dt);
	void doComplexPhysics(double dt);
	void doMovement(double dt);

	void setComplexPhysics(bool flag) { m_ComplexPhysics = flag; }

	double getElevator() const;
	double getAileron() const; 
	double getRudder() const; 
	double getThrottle() const;

	double getAngleOfAttack() { return m_AngleOfAttack; }
	void setGForce(double p_gForce) {m_gForce = p_gForce;};
	virtual double getGForce() {return m_gForce;};

//	Hud * GetpHud() const { return m_phud;};
//	Hud * m_phud;

protected:

	simdata::Ref<AeroDynamics> m_FlightModel;
	
	bool m_PhysicsInitialized;
	bool m_ComplexPhysics;

	//double m_ThrustMin;
	//double m_ThrustMax;
	/* move to AeroDynamics? */
	double m_AileronMin;
	double m_AileronMax;
	double m_ElevatorMin;
	double m_ElevatorMax;

	simdata::Vector3 m_AngularVelocity;
	simdata::Vector3 m_ForceBody;
	simdata::Vector3 m_CurrentForceTotal;
	simdata::Vector3 m_GravityForce;
	simdata::Vector3 m_ThrustForce;
	simdata::Vector3 m_LiftForce;
	simdata::Vector3 m_DragForce;

	double plusplus;
	double sensativity;
	double planesensx, planesensy;

	double m_gForce;
	double m_AngleOfAttack;
	double m_SideSlip;
	
	double m_ThrottleFactor;
	double m_Throttle;
	double m_Thrust;
	
	double m_Aileron;
	double m_Elevator;
	double m_Rudder;
};



	

//////////////////////////////////////////////////////////////////////////

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


#endif // __AIRCRAFTOBJECT_H__

