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
 * @file FlightDynamics.cpp
 *
 **/
# if defined(_MSC_VER) && (_MSC_VER <= 1300)
#define NOMINMAX
#endif

#include <algorithm>

#include "FlightDynamics.h"
#include "FlightModel.h"
#include "CSPSim.h"
#include "Log.h" 

#include <SimData/Math.h>
#include <SimData/Quat.h>


using simdata::RadiansToDegrees;
using simdata::DegreesToRadians;



FlightDynamics::FlightDynamics():
	m_depsilon(0.0),
	m_ElevatorScale(0.0),
	m_ElevatorInput(0.0),
	m_Aileron(0.0),
	m_Elevator(0.0),
	m_Rudder(0.0),
	m_Airbrake(0.0),
	m_Alpha(0.0),
	m_Alpha0(0.0),
	m_AlphaDot(0.0),
	m_Beta(0.0),
	m_GForce(1.0),
	m_Airspeed(0.0)
{
}

FlightDynamics::~FlightDynamics() {
}

void FlightDynamics::setFlightModel(FlightModel *flight_model) {
	assert(flight_model);
	m_FlightModel = flight_model;
}

void FlightDynamics::setControlSurfaces(double aileron, 
                                        double elevator, 
                                        double rudder, 
                                        double airbrake) 
{
	m_Aileron = aileron;
	m_ElevatorInput = elevator;
	m_Elevator = elevator;
	m_Rudder = rudder;
	m_Airbrake = airbrake;
	m_FlightModel->setControlSurfaces(aileron, elevator, rudder, airbrake);
}

void FlightDynamics::setMassInverse(double massInverse) {
	m_MassInverse = massInverse;
}

void FlightDynamics::initializeSimulationStep(double dt) {
	BaseDynamics::initializeSimulationStep(dt);
	double u = 0.05 + dt;
	m_ElevatorScale	= (1.0 - u) * m_ElevatorScale + u * controlInputValue(m_GForce);
	m_Elevator = m_ElevatorInput * m_ElevatorScale;
}

void FlightDynamics::computeForceAndMoment(double x) {
	updateAirflow(x);
	m_FlightModel->setAirstream(m_Alpha, m_AlphaDot, m_Beta, m_Airspeed, *m_qBar);
	m_FlightModel->setKinetics(*m_AngularVelocityBody, *m_Height);
	m_Force = m_FlightModel->calculateForce();
	m_Moment = m_FlightModel->calculateMoment();
	m_GForce = m_MassInverse * m_Force.z() / 9.81;
}

void FlightDynamics::postSimulationStep(double dt) {
	updateAirflow(dt);
	m_Alpha0 = m_Alpha;
	if (m_Recorder.isEnabled()) {
		m_Recorder.record(CH_ALPHA, m_Alpha);
		m_Recorder.record(CH_BETA, m_Beta);
		m_Recorder.record(CH_GFORCE, m_GForce);
		m_Recorder.record(CH_AIRSPEED, m_Airspeed);
	}
}

double FlightDynamics::controlIVbasis(double p_t) const {
	double cIV = p_t * p_t  * ( 3.0 - 2.0 * p_t);
	return cIV;
}

void FlightDynamics::initDataRecorder(DataRecorder *recorder) {
	m_Recorder.bind(recorder);
	m_Recorder.addChannel(CH_ALPHA, "alpha");
	m_Recorder.addChannel(CH_BETA, "beta");
	m_Recorder.addChannel(CH_GFORCE, "gforce");
	m_Recorder.addChannel(CH_AIRSPEED, "airspeed");
}

double FlightDynamics::controlInputValue(double p_gForce) const { 
	// to reduce G, decrease deflection control surface
	/* FIXME move to FCS class
	if (p_gForce > m_GMax && m_ElevatorInput > 0.0) return 0.0;
	if (p_gForce < m_GMin && m_ElevatorInput < 0.0) return 0.0;
	if (m_Alpha > m_stallAOA && m_ElevatorInput > 0.0) return 0.0;
	if (p_gForce > m_GMax - m_depsilon && m_ElevatorInput > 0.0) {
		return controlIVbasis((m_GMax - p_gForce) / m_depsilon);
	} 
	if ( p_gForce < m_GMin + m_depsilon && m_ElevatorInput < 0.0) {
		return controlIVbasis((p_gForce - m_GMin) / m_depsilon);
	}*/
	return 1.0;
}

void FlightDynamics::updateAirflow(double h) {
	m_AirflowBody =	*m_VelocityBody - *m_WindBody;
	m_Airspeed = m_AirflowBody.length();
	m_Alpha = -atan2(m_AirflowBody.z(), m_AirflowBody.y()); 
	if (h > 0.0) {
		m_AlphaDot = ( m_Alpha - m_Alpha0 ) / h;
	} // else keep previous value

	// restrict m_alphaDot in vertical stalls
	if (m_AlphaDot > 1.0) m_AlphaDot = 1.0;
	if (m_AlphaDot < -1.0) m_AlphaDot = -1.0;
	
	// Calculate side angle
	// beta is 0 when v velocity (i.e. side velocity) is 0; note here v is m_AirflowBody.x()
	m_Beta  = atan2(m_AirflowBody.x(), m_AirflowBody.y());
}




