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


#include <csp/cspsim/ai/AircraftMission.h>
#include <csp/cspsim/ai/AircraftControl.h>
#include <csp/cspsim/ai/LandingTask.h>
#include <csp/cspsim/ai/TakeoffTask.h>
#include <csp/cspsim/ai/Task.inl>
#include <csp/cspsim/ai/Waypoint.h>

namespace csp {
namespace ai {

AircraftMission::AircraftMission(): AircraftTask("Mission") {
	addHandler(TAKEOFF, &AircraftMission::onTakeoff, "TAKEOFF");
	addHandler(WAYPOINTS, &AircraftMission::onWaypoints, "WAYPOINTS");
	addHandler(LAND, &AircraftMission::onLand, "LAND");
	next(TAKEOFF);
}

void AircraftMission::onTakeoff() {
	AircraftTask *takeoff = new TakeoffTask;
	takeoff->bind(ai());
	takeoff->setRunway(runway());
	override(takeoff);
	next(WAYPOINTS);
}

void AircraftMission::onWaypoints() {
	if (initial()) m_Waypoint = 0;
	if (m_Waypoint >= static_cast<int>(m_Waypoints.size())) {
		next(LAND);
		return;
	}
	Ref<Waypoint> wp = m_Waypoints[m_Waypoint];
	if (ai()->flyToPositionSpeed(wp->position, wp->speed, 5.0, true, dt())) {
		m_Waypoint++;
	}
}

void AircraftMission::onLand() {
	if (done()) return;
	AircraftTask *landing = new LandingTask;
	landing->bind(ai());
	landing->setRunway(runway());
	override(landing, &AircraftMission::onLandingDone);
}

void AircraftMission::onLandingDone(Status status) {
	if (status == SUCCESS) {
		succeed();
	} else {
		m_Waypoint = 0;
		next(WAYPOINTS);
	}
}

} // end namespace ai
} // end namespace csp
