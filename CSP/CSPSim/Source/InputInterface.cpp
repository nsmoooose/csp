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

InputInterfaceDispatch::~InputInterfaceDispatch() {
	for (ActionMap::iterator iter = m_Actions.begin(); iter != m_Actions.end(); ++iter) {
		delete iter->second;
	}
	for (MotionMap::iterator iter = m_Motions.begin(); iter != m_Motions.end(); ++iter) {
		delete iter->second;
	}
	for (AxisMap::iterator iter = m_Axes.begin(); iter != m_Axes.end(); ++iter) {
		delete iter->second;
	}
}

bool InputInterface::onCommand(std::string const &id, int x, int y) {
	InputInterfaceDispatch *map = _getInputInterfaceDispatch();
	if (!map) return false;
	return map->callAction(id, this, x, y);
}

bool InputInterface::onAxis(std::string const &id, double value) {
	InputInterfaceDispatch *map = _getInputInterfaceDispatch();
	if (!map) return false;
	return map->callAxis(id, this, value);
}

bool InputInterface::onMotion(std::string const &id, int x, int y, int dx, int dy) {
	InputInterfaceDispatch *map = _getInputInterfaceDispatch();
	if (!map) return false;
	return map->callMotion(id, this, x, y, dx, dy);
}

bool InputInterface::onMapEvent(MapEvent const &event) {
	switch (event.type) {
		case MapEvent::ID_COMMAND_EVENT: 
			return onCommand(event.id, event.id_command.x, event.id_command.y);
		case MapEvent::ID_AXIS_EVENT:
			return onAxis(event.id, event.id_axis.value);
		case MapEvent::ID_MOTION_EVENT:
			return onMotion(event.id, event.id_motion.x, event.id_motion.y, event.id_motion.dx, event.id_motion.dy);
		default:
			break;
	}
	return false;
}

