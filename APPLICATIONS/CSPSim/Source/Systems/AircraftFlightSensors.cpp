// Combat Simulator Project - CSPSim
// Copyright (C) 2002, 2003 The Combat Simulator Project
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


#include <Systems/AircraftFlightSensors.h>
#include <KineticsChannels.h>
#include <Atmosphere.h>
#include <CSPSim.h>

using bus::Kinetics;


SIMDATA_REGISTER_INTERFACE(AircraftFlightSensors)


double AircraftFlightSensors::onUpdate(double dt) {
	simdata::Vector3 pos = b_Position->value();
	Atmosphere const *atmosphere = CSPSim::theSim->getAtmosphere();
	if (atmosphere)	{
		b_Density->value() = atmosphere->getDensity(pos.z());
		b_Pressure->value() = atmosphere->getPressure(pos.z());
		simdata::Vector3 wind = atmosphere->getWind(pos);
		wind += atmosphere->getTurbulence(pos, m_Distance);
		b_WindVelocity->value() = wind;
		m_Distance += (wind - b_Velocity->value()).length() * dt;
	} else {
		b_Density->value() = 1.25; // nominal sea-level air density
		b_WindVelocity->value() = simdata::Vector3::ZERO;
	}
	return 0.101;
}

void AircraftFlightSensors::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Position = bus->getChannel(Kinetics::Position);
	b_Velocity = bus->getChannel(Kinetics::Velocity);
}

void AircraftFlightSensors::registerChannels(Bus *bus) {
	assert(bus!=0);
	b_WindVelocity = bus->registerLocalDataChannel<simdata::Vector3>("Conditions.WindVelocity", simdata::Vector3::ZERO);
	b_Pressure = bus->registerLocalDataChannel<double>("Conditions.Pressure", 100000.0);
	b_Density = bus->registerLocalDataChannel<double>("Conditions.Density", 1.25);
	b_Temperature = bus->registerLocalDataChannel<double>("Conditions.Temperature", 300);
}


