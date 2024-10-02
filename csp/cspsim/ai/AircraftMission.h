#pragma once
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

#include <csp/cspsim/ai/AircraftTask.h>
#include <csp/cspsim/ai/Waypoint.h>
#include <vector>

namespace csp {
namespace ai {

class AircraftMission: public AircraftTask {
public:
	AircraftMission();

	// TODO tighten up these accessors, make mission data a private struct
	std::vector<Ref<Waypoint> > &waypoints() { return m_Waypoints; }
	const std::vector<Ref<Waypoint> > &waypoints() const { return m_Waypoints; }
	int waypoint() const { return m_Waypoint; }
	void setWaypoint(int waypoint) { m_Waypoint = waypoint; }

private:
	enum { TAKEOFF, WAYPOINTS, LAND };
	int m_Waypoint;
	std::vector<Ref<Waypoint> > m_Waypoints;


	void onTakeoff();
	void onWaypoints();
	void onLand();
	void onLandingDone(Status status);
};

} // end namespace ai
} // end namespace csp
