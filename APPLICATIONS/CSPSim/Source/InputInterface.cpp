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
 * @file InputInterface.cpp
 *
 **/

# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning(disable : 4786)
# endif

#include "InputInterface.h"


bool InputInterface::onCommand(std::string const &id, int x, int y) {
	ActionAdapter adapter = m_Actions[id.c_str()];
	if (adapter) {
		adapter(this, x, y);
		return true;
	}
	return false;
}

bool InputInterface::onAxis(std::string const &id, double value) {
	AxisAdapter adapter = m_Axes[id.c_str()];
	if (adapter) {
		adapter(this, value);
		return true;
	}
	return false;
}

bool InputInterface::onMotion(std::string const &id, int x, int y, int dx, int dy) {
	if (id != m_LastMotionID) {
		m_LastMotionID = id;
		m_LastMotionAdapter = NULL;
		MotionMap::const_iterator map = m_Motions.find(id.c_str());
		if (map == m_Motions.end()) return false;
		m_LastMotionAdapter = map->second;
	}
	MotionAdapter adapter = m_LastMotionAdapter;
	if (!adapter) return false;
	adapter(this, x, y, dx, dy);
	return true;
}

