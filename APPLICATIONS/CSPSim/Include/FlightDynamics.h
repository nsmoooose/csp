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
 * @file FlightDynamics.h
 *
 **/


#ifndef __FLIGHTDYNAMICS_H__
#define __FLIGHTDYNAMICS_H__

#include <SimData/InterfaceRegistry.h>

#include "BaseDynamics.h"
#include "DataRecorder.h"

class FlightModel;


/**
 * class FlightDynamics - aircraft primary flight model implementation.
 *
 */
class FlightDynamics: public BaseDynamics, public simdata::Referenced {
	friend class FlightModel;
	virtual ~FlightDynamics();
public:
	FlightDynamics();

	void computeForceAndMoment(double x);
	void initializeSimulationStep(double dt);
	void postSimulationStep(double dt);

	double getAngleOfAttack() const { return m_Alpha; }
	double getSideSlip() const { return m_Beta; }
	double getGForce() const { return m_GForce; }
	double getSpeed() const { return m_Airspeed; }

	void setControlSurfaces(double aileron, 
	                        double elevator, 
	                        double rudder, 
	                        double airbrake);

	void setMassInverse(double massInverse);

	virtual void initDataRecorder(DataRecorder *);

protected:

	void setFlightModel(FlightModel *flight_model);
	simdata::Ref<FlightModel> m_FlightModel;

	void updateAirflow(double dt);

	double controlIVbasis(double p_t) const;
	double controlInputValue(double p_gForce) const;
	
	double m_depsilon;       // G-force control feedback stall
	double m_ElevatorScale;  // elevator correction
	double m_ElevatorInput;  // desired elevator deflection

	// control surfaces
	double m_Aileron;
	double m_Elevator;
	double m_Rudder;
	double m_Airbrake;

	// derived quantities
	double m_Alpha;		// current angle of attack
	double m_Alpha0;	// discrete AOA
	double m_AlphaDot;	// AOA rate
	double m_Beta;		// side slip angle
	double m_GForce;	// current g acceleration
	
	double m_MassInverse;

	double m_Airspeed;
	simdata::Vector3 m_AirflowBody;         // air flow velocity in body coordinate

private:
	RecorderInterface m_Recorder;
	enum { 
		CH_ALPHA, 
		CH_BETA,
		CH_GFORCE,
		CH_AIRSPEED
	};

};


#endif  // __FLIGHTDYNAMICS_H__

