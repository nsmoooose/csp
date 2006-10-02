// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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
 * @file AircraftAgent.cpp
 *
 **/


// todos:
//
//   - wingmen have to be bound to the rest of their flight,
//     and there needs to be an easy way to obtain detailed
//     information about the other aircraft in the flight.
//     this could be done through a higher level structure
//     (e.g. FlightGroup).
//   - need an interface to get information about other
//     objects by id (via contacts list).
//   - many others...
//
// initial goals:
//   - take off, follow waypoints, then land.

#include <csp/cspsim/ai/AircraftAgent.h>
#include <csp/cspsim/ai/AircraftControl.h>
#include <csp/cspsim/ai/AircraftMission.h>
#include <csp/cspsim/ai/Runway.h>
#include <csp/cspsim/ConditionsChannels.h>
#include <csp/cspsim/ControlInputsChannels.h>
#include <csp/cspsim/FlightDynamicsChannels.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/LandingGearChannels.h>

#include <csp/csplib/data/ObjectInterface.h>

#include <iostream>

CSP_NAMESPACE

CSP_XML_BEGIN(AircraftAgent)
CSP_XML_END


AircraftAgent::AircraftAgent() {
	m_AircraftControl = new AircraftControl;

	Ref<Runway> runway = new Runway;
	runway->aimpoint = Vector3(-29510, -10400, 83.0);
	runway->direction = Vector3(0.0, -1.0, 0.0);
	runway->z = Vector3(0.0, 0.0, 1.0);
	runway->length = 2000.0;
	runway->width = 40.0;
	runway->glideslope = toRadians(3.0);
	runway->init();

	// hardcode a mission for testing and demonstration purposes
	AircraftMission *mission = new AircraftMission;
	mission->setRunway(runway.get());
	mission->bind(m_AircraftControl.get());
	mission->waypoints().push_back(new Waypoint(-29510, -30400, 1000.0, 180.0));
	mission->waypoints().push_back(new Waypoint(-39510, -40400, 1000.0, 200.0));
	mission->waypoints().push_back(new Waypoint(-39510, -10400, 500.0, 230.0));
	mission->waypoints().push_back(new Waypoint(-29510, -10400, 1500.0, 230.0));
	mission->waypoints().push_back(new Waypoint(-19510, -10400, 1500.0, 230.0));
	mission->waypoints().push_back(new Waypoint(-19510, -1400, 1500.0, 140.0));
	mission->waypoints().push_back(new Waypoint(-25510, -2400, 1500.0, 140.0));
	m_Task = mission;
}

AircraftAgent::~AircraftAgent() {
}

void AircraftAgent::registerChannels(Bus* bus) {
	// TODO should this be done by the AircraftControl class?
	b_PitchInput = bus->registerLocalDataChannel<double>(bus::ControlInputs::PitchInput, 0.0);
	b_RollInput = bus->registerLocalDataChannel<double>(bus::ControlInputs::RollInput, 0.0);
	b_RudderInput = bus->registerLocalDataChannel<double>(bus::ControlInputs::RudderInput, 0.0);
	b_LeftBrakeInput = bus->registerLocalDataChannel<double>(bus::ControlInputs::LeftBrakeInput, 0.0);
	b_RightBrakeInput = bus->registerLocalDataChannel<double>(bus::ControlInputs::RightBrakeInput, 0.0);
	b_ThrottleInput = bus->registerLocalDataChannel<double>(bus::ControlInputs::ThrottleInput, 0.0);
	b_AirbrakeInput = bus->registerLocalDataChannel<double>(bus::ControlInputs::AirbrakeInput, 0.0);
}

void AircraftAgent::importChannels(Bus* bus) {
	m_AircraftControl->importChannels(bus);
}

void AircraftAgent::getInfo(InfoList &info) const {
	info.push_back("AI: " + m_Task->stateName());
}

double AircraftAgent::onUpdate(double dt) {
	m_Task->update(dt);
	return 0.0;
}

CSP_NAMESPACE_END

