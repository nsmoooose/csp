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

#include <csp/cspsim/FlightDynamics.h>
#include <csp/cspsim/FlightModel.h>
#include <csp/cspsim/ConditionsChannels.h>
#include <csp/cspsim/ControlSurfacesChannels.h>
#include <csp/cspsim/FlightDynamicsChannels.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/stores/DragProfile.h>
#include <csp/cspsim/stores/StoresDynamics.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/util/Conversions.h>

#include <sstream> // for getInfo
#include <iomanip> // for getInfo
#include <algorithm>

CSP_NAMESPACE

CSP_XML_BEGIN(FlightDynamics)
	CSP_DEF("flight_model", m_FlightModel, true)
	CSP_DEF("store_drag", m_DragProfile, false)
CSP_XML_END


FlightDynamics::FlightDynamics():
	m_Beta(0.0),
	m_Alpha(0.0),
	m_Alpha0(0.0),
	m_AlphaDot(0.0),
	m_Airspeed(1.0),
	m_QBar(0.0),
	m_FilterG(5.0),
	m_FilterLateralG(5.0)
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
	b_G = bus->registerLocalDataChannel<double>(bus::FlightDynamics::G, 0.0);
	b_LateralG = bus->registerLocalDataChannel<double>(bus::FlightDynamics::LateralG, 0.0);
}

void FlightDynamics::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Aileron = bus->getChannel(bus::ControlSurfaces::AileronDeflection, false);
	b_Elevator = bus->getChannel(bus::ControlSurfaces::ElevatorDeflection, false);
	b_Rudder = bus->getChannel(bus::ControlSurfaces::RudderDeflection, false);
	b_Airbrake = bus->getChannel(bus::ControlSurfaces::AirbrakeDeflection, false);
	b_LeadingEdgeFlap = bus->getChannel(bus::ControlSurfaces::LeadingEdgeFlapDeflection, false);
	b_TrailingEdgeFlap = bus->getChannel(bus::ControlSurfaces::TrailingEdgeFlapDeflection, false);
	b_Density = bus->getChannel(bus::Conditions::Density);
	b_Mach = bus->getChannel(bus::Conditions::Mach);
	b_WindVelocity = bus->getChannel(bus::Conditions::WindVelocity);
	b_GroundZ = bus->getChannel(bus::Kinetics::GroundZ);
	b_AccelerationBody = bus->getChannel(bus::Kinetics::AccelerationBody);
	b_StoresDynamics = bus->getChannel(bus::Kinetics::StoresDynamics, false);
	b_CenterOfMassOffset = bus->getChannel(bus::Kinetics::CenterOfMassOffset);
}

void FlightDynamics::initializeSimulationStep(double dt) {
	BaseDynamics::initializeSimulationStep(dt);
	const double da = !b_Aileron ? 0.0 : b_Aileron->value();
	const double de = !b_Elevator ? 0.0 : b_Elevator->value();
	const double dr = !b_Rudder ? 0.0 : b_Rudder->value();
	const double dsb = !b_Airbrake ? 0.0 : b_Airbrake->value();
	const double lef = !b_LeadingEdgeFlap ? 0.0 : b_LeadingEdgeFlap->value();
	const double tef = !b_TrailingEdgeFlap ? 0.0 : b_TrailingEdgeFlap->value();
	m_FlightModel->setControlSurfaces(da, de, dr, dsb, lef, tef);
	m_WindVelocityBody = m_Attitude->invrotate(b_WindVelocity->value());
}

void FlightDynamics::computeForceAndMoment(double x) {
	updateAirflow(x);
	double agl = m_Position->z() - b_GroundZ->value();
	m_FlightModel->setAirstream(m_Alpha, m_AlphaDot, m_Beta, b_Mach->value(), m_Airspeed, m_QBar);
	m_FlightModel->setKinetics(b_CenterOfMassOffset->value(), *m_AngularVelocityBody, agl);
	m_FlightModel->calculateForceAndMoment(m_Force, m_Moment);
	addExternalDrag();
}

void FlightDynamics::postSimulationStep(double dt) {
	updateAirflow(dt);
	m_Alpha0 = m_Alpha;
	b_Alpha->value() = m_Alpha;
	b_Beta->value() = m_Beta;
	b_Airspeed->value() = m_Airspeed;
	b_QBar->value() = m_QBar;
	Vector3 gravity_g_body = m_Attitude->invrotate(Vector3(0, 0, 9.806));
	Vector3 total_g_body = (gravity_g_body + b_AccelerationBody->value()) * (1.0 / 9.806);
	m_FilterG.update(total_g_body.z(), dt);
	m_FilterLateralG.update(total_g_body.x(), dt);
	b_G->value() = m_FilterG.value(); //total_g_body.z();
///	b_G->value() = total_g_body.z();
	b_LateralG->value() = m_FilterLateralG.value();  //total_g_body.x();
///	b_LateralG->value() = total_g_body.x();
}

void FlightDynamics::updateAirflow(double h) {
	Vector3 airflowBody = *m_VelocityBody - m_WindVelocityBody;
	m_Airspeed = airflowBody.length();
	m_Alpha = -atan2(airflowBody.z(), airflowBody.y());

	if (h > 0.0) {
		m_AlphaDot = ( m_Alpha - m_Alpha0 ) / h;
	} // else keep previous value

	// restrict m_alphaDot in vertical stalls
	m_AlphaDot = clampTo(m_AlphaDot, -1.0, 1.0);
	
	// Calculate side angle
	m_Beta = asin(airflowBody.x() / std::max(1.0, m_Airspeed));

	// Dynamic pressure = 1/2 p V^2 (N/m^2)
	m_QBar = 0.5 * m_Airspeed * m_Airspeed * b_Density->value();
}

// TODO use drag index values?
void FlightDynamics::addExternalDrag() {
	if (b_StoresDynamics.valid()) {
		double Cd = !m_DragProfile ? 1.0 : m_DragProfile->drag(b_Mach->value(), m_Alpha);
		Vector3 force, moment;
		b_StoresDynamics->value().computeDragForceAndMoment(m_QBar, Cd, m_Alpha, force, moment);
		m_Force += force;
		m_Moment += moment;
	}
}

void FlightDynamics::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line << "AOA: " << std::setprecision(1) << std::setw(5) << toDegrees(m_Alpha)
	     << ", Sideslip: " << std::setprecision(1) << std::setw(5) << toDegrees(m_Beta)
	     << ", Airspeed: " << std::setprecision(1) << std::setw(6) << m_Airspeed
	     << ", G: " << std::setprecision(2) << std::setw(4) << b_G->value();
	info.push_back(line.str());
	/* various other diagnostic info
	line.str("");
	line << "dE: " << std::setprecision(1) << toDegrees(b_Elevator->value())
	     << ", dA: " << std::setprecision(1) << toDegrees(b_Aileron->value())
	     << ", dR: " << std::setprecision(1) << toDegrees(b_Rudder->value())
	     << ", dLEF: " << std::setprecision(1) << toDegrees(b_LeadingEdgeFlap->value())
	     << ", dTEF: " << std::setprecision(1) << toDegrees(b_TrailingEdgeFlap->value());
	info.push_back(line.str());
	line.str("");
	double turn_rate = m_Attitude->rotate(*m_AngularVelocityBody).z();
	double turn_radius = m_VelocityBody->length() / turn_rate;
	line << "Turn rate: " << toDegrees(turn_rate) << " deg/s, sink rate: " << m_Attitude->rotate(*m_VelocityBody).z() * (100.0 / 2.54 / 12.0) << " ft/s, turn radius: " << (turn_radius * (100.0 / 2.54 / 12.0)) << " ft";
	info.push_back(line.str());
	*/
}

CSP_NAMESPACE_END

