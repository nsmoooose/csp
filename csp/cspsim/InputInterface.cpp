// Combat Simulator Project
// Copyright (C) 2002, 2005 The Combat Simulator Project
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

#include <csp/cspsim/InputInterface.h>
#include <sigc++/adaptors/hide.h>

CSP_NAMESPACE


InputInterfaceDispatch::~InputInterfaceDispatch() {
	for (EventHandlerMap::iterator iter = m_EventHandlers.begin(); iter != m_EventHandlers.end(); ++iter) {
		delete iter->second;
	}
}

InputInterface::InputInterface(): m_RuntimeDispatch(0) { }

InputInterface::~InputInterface() {
	delete m_RuntimeDispatch;  // ok if null
}

bool InputInterface::onMapEvent(MapEvent const &event) {
	if (m_RuntimeDispatch && m_RuntimeDispatch->onMapEvent(event)) {
		return true;
	}
	InputInterfaceDispatch *map = _getInputInterfaceDispatch();
	return map ? map->callHandler(this, event) : false;
}

bool RuntimeDispatch::onMapEvent(MapEvent const &event) {
	CommandCallbacks::iterator command_iter;
	MotionCallbacks::iterator motion_iter;
	AxisCallbacks::iterator axis_iter;
	switch (event.type) {
		case MapEvent::ID_COMMAND_EVENT:
			command_iter = m_CommandCallbacks.find(event.id);
			if (command_iter == m_CommandCallbacks.end()) return false;
			command_iter->second.emit(event.id_command);
			return true;
		case MapEvent::ID_AXIS_EVENT:
			axis_iter = m_AxisCallbacks.find(event.id);
			if (axis_iter == m_AxisCallbacks.end()) return false;
			axis_iter->second.emit(event.id_axis);
			return true;
		case MapEvent::ID_MOTION_EVENT:
			motion_iter = m_MotionCallbacks.find(event.id);
			if (motion_iter == m_MotionCallbacks.end()) return false;
			motion_iter->second.emit(event.id_motion);
			return true;
		default:
			break;
	}
	return false;
}

void RuntimeDispatch::bindAction(std::string const &id, ActionEventSlot const &callback) {
	m_CommandCallbacks[id].connect(sigc::hide(callback));  // actions do not receive the event argument
}

void RuntimeDispatch::bindClick(std::string const &id, ClickEventSlot const &callback) {
	m_CommandCallbacks[id].connect(callback);
}

void RuntimeDispatch::bindMotion(std::string const &id, MotionEventSlot const &callback) {
	m_MotionCallbacks[id].connect(callback);
}

void RuntimeDispatch::bindAxis(std::string const &id, AxisEventSlot const &callback) {
	m_AxisCallbacks[id].connect(callback);
}


CSP_NAMESPACE_END
