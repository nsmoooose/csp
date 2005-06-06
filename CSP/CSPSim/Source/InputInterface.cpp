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

InputInterface::InputInterface(): m_RuntimeDispatch(0) { }

InputInterface::~InputInterface() {
	delete m_RuntimeDispatch;  // ok if null
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
	if (m_RuntimeDispatch) {
		return m_RuntimeDispatch->onMapEvent(event);
	}
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

bool RuntimeDispatch::onMapEvent(MapEvent const &event) {
	ActionCallbacks::iterator action_iter;
	MotionCallbacks::iterator motion_iter;
	AxisCallbacks::iterator axis_iter;
	switch (event.type) {
		case MapEvent::ID_COMMAND_EVENT:
			action_iter = m_ActionCallbacks.find(event.id);
			if (action_iter == m_ActionCallbacks.end()) return false;
			action_iter->second.emit(event.id_command.x, event.id_command.y);
			return true;
		case MapEvent::ID_AXIS_EVENT:
			axis_iter = m_AxisCallbacks.find(event.id);
			if (axis_iter == m_AxisCallbacks.end()) return false;
			axis_iter->second.emit(event.id_axis.value);
			return true;
		case MapEvent::ID_MOTION_EVENT:
			motion_iter = m_MotionCallbacks.find(event.id);
			if (motion_iter == m_MotionCallbacks.end()) return false;
			motion_iter->second.emit(event.id_motion.x, event.id_motion.y, event.id_motion.dx, event.id_motion.dy);
			return true;
		default:
			break;
	}
	return false;
};

void RuntimeDispatch::bindAction(std::string const &id, ActionEventSlot const &callback) {
	m_ActionCallbacks[id].connect(callback);
}

void RuntimeDispatch::bindMotion(std::string const &id, MotionEventSlot const &callback) {
	m_MotionCallbacks[id].connect(callback);
}

void RuntimeDispatch::bindAxis(std::string const &id, AxisEventSlot const &callback) {
	m_AxisCallbacks[id].connect(callback);
}

