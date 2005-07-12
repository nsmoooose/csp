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
 * @file FlightDynamics.cpp
 *
 **/

#include <FlightDynamics.h>
#include <FlightModel.h>
#include <CSPSim.h>
#include <ConditionsChannels.h>
#include <ControlSurfacesChannels.h>
#include <FlightDynamicsChannels.h>
#include <KineticsChannels.h>

#include <SimCore/Util/Log.h>
#include <SimData/Math.h>
#include <SimData/Key.h>

#include <sstream>
#include <iomanip>
#include <algorithm>


using simdata::toDegrees;
using simdata::toRadians;


SIMDATA_XML_BEGIN(FlightDynamics)
	SIMDATA_DEF("flight_model", m_FlightModel, true)
SIMDATA_XML_END


FlightDynamics::FlightDynamics():
	m_Beta(0.0),
	m_Alpha(0.0),
	m_Alpha0(0.0),
	m_AlphaDot(0.0),
	m_Airspeed(1.0),
	m_QBar(0.0)
{
}

FlightDynamics::~FlightDynamics() {
}

void FlightDynamics::registerChannels(Bus *bus) {
	assert(bus!=0);
	b_Alpha = bus->registerLocalDataChannel<double>(bus::FlightDynamics::Alpha, 0.0);
	b_Beta = bus->registerLocalDataChannel<double>(bus::FlightDynamics::Beta, 0.0);
	b_Airspeed = bus->registerLocalDataChannel<double>(bus::FlightDynamics::Airspeed, 0.0);
	b_QBar = bus->registerLocalDataChannel<double>(bus::FlightDynamics::QBar, 0.0);
	b_G = bus->registerLocalDataChannel<double>(bus::FlightDynamics::GForce, 0.0);
}

void FlightDynamics::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Aileron = bus->getChannel(bus::ControlSurfaces::AileronDeflection);
	b_Elevator = bus->getChannel(bus::ControlSurfaces::ElevatorDeflection);
	b_Rudder = bus->getChannel(bus::ControlSurfaces::RudderDeflection);
	b_Airbrake = bus->getChannel(bus::ControlSurfaces::AirbrakeDeflection);
	b_Density = bus->getChannel(bus::Conditions::Density);
	b_WindVelocity = bus->getChannel(bus::Conditions::WindVelocity);
	b_GroundZ = bus->getChannel(bus::Kinetics::GroundZ);
	b_AccelerationBody = bus->getChannel(bus::Kinetics::AccelerationBody);
}

void FlightDynamics::initializeSimulationStep(double dt) {
	BaseDynamics::initializeSimulationStep(dt);
	m_FlightModel->setControlSurfaces(b_Aileron->value(), b_Elevator->value(), b_Rudder->value(), b_Airbrake->value());
	m_WindVelocityBody = m_Attitude->invrotate(b_WindVelocity->value());
}

void FlightDynamics::computeForceAndMoment(double x) {
	updateAirflow(x);
	double agl = m_PositionLocal->z() - b_GroundZ->value();
	m_FlightModel->setAirstream(m_Alpha, m_AlphaDot, m_Beta, m_Airspeed, m_QBar);
	m_FlightModel->setKinetics(*m_AngularVelocityBody, agl);
	m_Force = m_FlightModel->calculateForce();
	m_Moment = m_FlightModel->calculateMoment();
}

void FlightDynamics::postSimulationStep(double dt) {
	updateAirflow(dt);
	m_Alpha0 = m_Alpha;
	b_Alpha->value() = m_Alpha;
	b_Beta->value() = m_Beta;
	b_Airspeed->value() = m_Airspeed;
	b_QBar->value() = m_QBar;
	double G = m_Attitude->invrotate(simdata::Vector3::ZAXIS).z();
	b_G->value() = G + b_AccelerationBody->value().z() / 9.806;
}

void FlightDynamics::updateAirflow(double h) {
	simdata::Vector3 airflowBody = *m_VelocityBody - m_WindVelocityBody;
	m_Airspeed = airflowBody.length();
	m_Alpha = -atan2(airflowBody.z(), airflowBody.y());
	
	if (h > 0.0) {
		m_AlphaDot = ( m_Alpha - m_Alpha0 ) / h;
	} // else keep previous value

	// restrict m_alphaDot in vertical stalls
	m_AlphaDot = simdata::clampTo(m_AlphaDot,-1.0,1.0);
	
	// Calculate side angle
	// beta is 0 when v velocity (i.e. side velocity) is 0; note here v is m_AirflowBody.x()
	//m_Beta = atan2(airflowBody.x(), airflowBody.y());
	m_Beta = asin(airflowBody.x()/(1.0 + m_Airspeed));

	// Dynamic pressure = 1/2 p V^2 (N/m^2)
	m_QBar = 0.5 * m_Airspeed * m_Airspeed * b_Density->value();
}

void FlightDynamics::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line << "AOA: " << std::setprecision(1) << std::setw(5) << toDegrees(m_Alpha)
	     << ", Sideslip: " << std::setprecision(1) << std::setw(5) << toDegrees(m_Beta)
	     << ", Airspeed: " << std::setprecision(1) << std::setw(6) << m_Airspeed
	     << ", G: " << std::setprecision(1) << std::setw(4) << b_G->value();
	info.push_back(line.str());
}

