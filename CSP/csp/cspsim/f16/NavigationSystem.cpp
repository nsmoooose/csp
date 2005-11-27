// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file NavigationSystem.cpp
 *
 **/

#include "NavigationSystem.h"
#include <CSPSim.h>
#include <Theater.h>
#include <TerrainObject.h>
#include <Projection.h>

#include <csp/csplib/data/Date.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/Conversions.h>


NavigationSystem::NavigationSystem() {
	m_Projection = CSPSim::theSim->getTheater()->getTerrain()->getProjection();
	assert(m_Projection.valid());
	addTestSteerpoints();
}

void NavigationSystem::addTestSteerpoints() {
	CSP_LOG(APP, DEBUG, "add test steerpoints");
	m_Steerpoints.clear();
	Steerpoint *steerpoint;
	simdata::DateZulu sptime = CSPSim::theSim->getCurrentTime();
	steerpoint = new Steerpoint(m_Projection, 1, "takeoff", simdata::Vector3(-29495.0, -10830, 85.5));
	sptime.addTime(60.0);
	steerpoint->setTime(sptime);
	m_Steerpoints.push_back(steerpoint);
	steerpoint = new Steerpoint(m_Projection, 2, "climb", simdata::Vector3(-29495.0, -25000, 1000.0));
	sptime.addTime(120.0);
	steerpoint->setTime(sptime);
	m_Steerpoints.push_back(steerpoint);
	steerpoint = new Steerpoint(m_Projection, 3, "cruise", simdata::Vector3(30495.0, -35000, 6000.0));
	sptime.addTime(200.0);
	steerpoint->setTime(sptime);
	m_Steerpoints.push_back(steerpoint);
	steerpoint = new Steerpoint(m_Projection, 4, "simulated attack", simdata::Vector3(-29495.0, -10830, 85.5));
	sptime.addTime(240.0);
	steerpoint->setTime(sptime);
	steerpoint->setOffsetAimpoint0(Steerpoint::OffsetPoint(1000.0, 290.0, 90.0));
	// FIXME these are not steerpoint properties.  need to find a home for them and a
	// way to access them from the hud, ded, and mfd.
	//steerpoint->setVisualInitialPoint(Steerpoint::OffsetPoint(4000.0, 110.0, 90.0));
	//steerpoint->setVisualReferencePoint(Steerpoint::OffsetPoint(2000.0, 60.0, 90.0));
	m_Steerpoints.push_back(steerpoint);
	m_SteerpointIndex = m_Steerpoints.size() - 1;
	nextSteerpoint();
}

void NavigationSystem::nextSteerpoint() {
	CSP_LOG(APP, DEBUG, "next steerpoint " << m_SteerpointIndex << " " << m_Steerpoints.size());
	if (!m_Steerpoints.empty()) {
		m_SteerpointIndex = (m_SteerpointIndex + 1) % m_Steerpoints.size();
	}
}

void NavigationSystem::prevSteerpoint() {
	CSP_LOG(APP, DEBUG, "prev steerpoint " << m_SteerpointIndex << " " << m_Steerpoints.size());
	if (!m_Steerpoints.empty()) {
		m_SteerpointIndex = (m_SteerpointIndex + m_Steerpoints.size() - 1) % m_Steerpoints.size();
	}
}

void NavigationSystem::setActiveSteerpoint(unsigned index) {
	if (index > 0 && index <= m_Steerpoints.size()) {
		m_SteerpointIndex = index - 1;
	}
}

Steerpoint::Ref NavigationSystem::activeSteerpoint() {
	return m_Steerpoints.empty() ? 0 : m_Steerpoints[m_SteerpointIndex];
}

simdata::LLA NavigationSystem::fromWorld(simdata::Vector3 const &position) {
	return m_Projection->convert(position);
}

simdata::Vector3 NavigationSystem::toWorld(simdata::LLA const &position) {
	return m_Projection->convert(position);
}
