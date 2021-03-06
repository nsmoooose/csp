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
 * @file AircraftFlightSensors.cpp
 *
 **/

#include <csp/cspsim/systems/AircraftFlightSensors.h>

#include <csp/cspsim/weather/Atmosphere.h>

#include <csp/cspsim/ConditionsChannels.h>
#include <csp/cspsim/FlightDynamicsChannels.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/CSPSim.h>

#include <csp/csplib/util/Conversions.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <sstream>
#include <iomanip>

namespace csp {

CSP_XML_BEGIN(AircraftFlightSensors)
CSP_XML_END


AircraftFlightSensors::AircraftFlightSensors(): m_Distance(0) { }

double AircraftFlightSensors::onUpdate(double dt) {
	Vector3 pos = b_Position->value();
	double speed = b_Velocity->value().length();
	weather::Atmosphere const *atmosphere = CSPSim::theSim->getAtmosphere();
	if (atmosphere) {
		b_Density->value() = atmosphere->getDensity(pos.z());
		b_Temperature->value() = atmosphere->getTemperature(pos.z());
		b_Pressure->value() = atmosphere->getPressure(pos.z());
		Vector3 wind = atmosphere->getWind(pos);
		wind += atmosphere->getTurbulence(pos, m_Distance);
		b_WindVelocity->value() = wind;
		double mach = atmosphere->getMach(speed, pos.z());
		b_Mach->value() = mach;
		b_CAS->value() = atmosphere->getCAS(mach, pos.z());
		m_Distance += (wind - b_Velocity->value()).length() * dt;
	} else {
		b_Density->value() = 1.25; // nominal sea-level air density
		b_WindVelocity->value() = Vector3::ZERO;
	}
	b_VerticalVelocity->value() = b_Velocity->value().z();
	b_PressureAltitude->value() = pos.z(); // FIXME roll in atmospheric pressure variations
	return 0.0; //return 0.101;
}

void AircraftFlightSensors::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Position = bus->getChannel(bus::Kinetics::Position);
	b_Velocity = bus->getChannel(bus::Kinetics::Velocity);
}

void AircraftFlightSensors::registerChannels(Bus *bus) {
	assert(bus!=0);
	b_WindVelocity = bus->registerLocalDataChannel<Vector3>(bus::Conditions::WindVelocity, Vector3::ZERO);
	b_Pressure = bus->registerLocalDataChannel<double>(bus::Conditions::Pressure, 100000.0);
	b_Density = bus->registerLocalDataChannel<double>(bus::Conditions::Density, 1.25);
	b_Temperature = bus->registerLocalDataChannel<double>(bus::Conditions::Temperature, 300);
	b_Mach = bus->registerLocalDataChannel<double>(bus::Conditions::Mach, 0.0);
	b_CAS = bus->registerLocalDataChannel<double>(bus::Conditions::CAS, 0.0);
	b_PressureAltitude = bus->registerLocalDataChannel<double>(bus::FlightDynamics::PressureAltitude, 0.0);
	b_VerticalVelocity = bus->registerLocalDataChannel<double>(bus::FlightDynamics::VerticalVelocity, 0.0);
}

void AircraftFlightSensors::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line.precision(0);
	line << "P: " << std::setw(3) << b_Pressure->value()
	     << ", T: " << std::setw(3) <<  b_Temperature->value()
	     << ", Mach: " << std::setw(5) << std::setprecision(2) <<  b_Mach->value()
	     << ", CAS: " << std::setw(3) << std::setprecision(0) <<  convert::mps_kts(b_CAS->value()) << " kts";
	info.push_back(line.str());
}

} // namespace csp

