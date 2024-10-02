// Combat Simulator Project
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
 * @file EventMapping.cpp
 *
 **/


# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning(disable : 4786)
# endif

#include <csp/cspsim/input/EventMapping.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <assert.h>


namespace csp {
namespace input {

EventMapping::EventMapping() {
}

int EventMapping::getKeyID(unsigned int key, unsigned int modifierMask, RawEvent::Keyboard::Type type) const {
	int press = (type == RawEvent::Keyboard::PRESSED) ? (1 << 31) : 0;
	return (key & 0xFFFF) + ((modifierMask & 0x3FFF) << 16) + press;
}

int EventMapping::getJoystickButtonID(unsigned int joystick, unsigned int button, RawEvent::JoystickButton::Type type, int jmod, int mode) const {
	int press = (type == RawEvent::JoystickButton::PRESSED) ? (1 << 31) : 0;
	return (joystick & 0xF) + ((button & 0xFFF) << 4) + ((jmod & 0xF) << 16) + ((mode & 0xF) << 20) + press;
}

int EventMapping::getJoystickAxisID(unsigned int joystick, unsigned int axis) const {
	return (joystick & 0xF) + ((axis & 0xFF) << 4);
}

int EventMapping::getMouseButtonID(unsigned int button, RawEvent::MouseButton::Type type, unsigned int modifierMask, int mode) const {
	unsigned int press = 0;
	switch ( type )
	{
		case RawEvent::MouseButton::PRESSED : press = (2 << 30); break;
		case RawEvent::MouseButton::DOUBLECLICK : press = (3 << 30); break;
		default: break;
	}
	return (button & 0xFF) + ((modifierMask & 0x3FFF) << 8) + ((mode & 0xF) << 22) + press;
}

int EventMapping::getMouseMotionID(unsigned int buttonMask, unsigned int modifierMask, int mode) const {
	return (buttonMask & 0xFF) + ((modifierMask & 0x3FFF) << 8) + ((mode & 0xF) << 22);
}


EventMapping::Script const *EventMapping::getKeyScript(unsigned int key, unsigned int modifierMask, RawEvent::Keyboard::Type type) const {
	int id = getKeyID(key, modifierMask, type);
	EventScript kScript = m_KeyMap.find(id);
	if (kScript == m_KeyMap.end()) return NULL;
	return &(kScript->second);
}

EventMapping::Script const *EventMapping::getJoystickButtonScript(unsigned int joystick, unsigned int button, RawEvent::JoystickButton::Type type, int jmod, int mode) const {
	int id = getJoystickButtonID(joystick, button, type, jmod, mode);
	EventScript bScript = m_JoystickButtonMap.find(id);
	if (bScript == m_JoystickButtonMap.end()) return NULL;
	return &(bScript->second);
}

EventMapping::Axis const *EventMapping::getJoystickAxis(unsigned int joystick, unsigned int axis) const {
	int id = getJoystickAxisID(joystick, axis);
	axis_map::const_iterator axisIt = m_JoystickAxisMap.find(id);
	if (axisIt == m_JoystickAxisMap.end()) return NULL;
	return &(axisIt->second);
}

void EventMapping::addJoystickAxisMap(unsigned int joystick, unsigned int axis, Axis const &a) {
	int id = getJoystickAxisID(joystick, axis);
	m_JoystickAxisMap[id] = a;
	m_usedJoysticks.insert(joystick);
}

void EventMapping::addKeyMap(unsigned int key, unsigned int modifierMask, RawEvent::Keyboard::Type type, Script const &s) {
	int id = getKeyID(key, modifierMask, type);
	m_KeyMap[id] = s;
}

void EventMapping::addJoystickButtonMap(unsigned int joystick, unsigned int button, RawEvent::JoystickButton::Type type, int jmod, int mode, Script const &s) {
	int id = getJoystickButtonID(joystick, button, type, jmod, mode);
	m_JoystickButtonMap[id] = s;
	m_usedJoysticks.insert(joystick);
}

void EventMapping::addMouseMotionMap(unsigned int buttonMask, unsigned int modifierMask, int mode, Motion const &motion) {
	int id = getMouseMotionID(buttonMask, modifierMask, mode);
	m_MouseMotionMap[id] = motion;
}
	
void EventMapping::addMouseButtonMap(unsigned int button, RawEvent::MouseButton::Type type, unsigned int modifierMask, int mode, Script const &s) {
	int id = getMouseButtonID(button, type, modifierMask, mode);
	m_MouseButtonMap[id] = s;
}

EventMapping::Script const *EventMapping::getMouseButtonScript(unsigned int button, RawEvent::MouseButton::Type type, unsigned int modifierMask, int mode) const {
	int id = getMouseButtonID(button, type, modifierMask, mode);
	EventScript mScript = m_MouseButtonMap.find(id);
	if (mScript == m_MouseButtonMap.end()) return NULL;
	return &(mScript->second);
}

EventMapping::Motion const *EventMapping::getMouseMotion(unsigned int buttonMask, unsigned int modifierMask, int mode) const {
	int id = getMouseMotionID(buttonMask, modifierMask, mode);
	motion_map::const_iterator map = m_MouseMotionMap.find(id);
	if (map == m_MouseMotionMap.end()) return NULL;
	return &(map->second);
}

void EventMapping::parseAction(const char *line, EventMapping::Script &script) {
	std::stringstream s(line+1);
	Action a;
	s >> a.time >> a.mode >> a.jmod >> a.stop >> a.loop >> a.id;
	script.push_back(a);
}

void EventMapping::parseMap(const char *line, EventMapping::Script &script) {
	std::stringstream s(line+1);
	Action a;
	char device_type, type;
	int device, kmod, jmod, mmod, id, mode;
	s >> device_type >> device >> type >> kmod >> jmod >> mmod >> id >> mode;
	if (device_type == 'J') {
		if ( type == 'P' ) {
			addJoystickButtonMap(device, id, RawEvent::JoystickButton::PRESSED, jmod, mode, script);
		} else
		if ( type == 'R' ) {
			addJoystickButtonMap(device, id, RawEvent::JoystickButton::RELEASED, jmod, mode, script);
		} else
		if (type == 'A') {
			if (script.size() != 1) return;
			addJoystickAxisMap(device, id, Axis(script[0].id));  /** @TODO: add mode ? */
		}
	} else
	if (device_type == 'K') {
		if ( type == 'P' ) {
			addKeyMap(id, kmod, RawEvent::Keyboard::PRESSED, script);
		} else
		if ( type == 'R' ) {
			addKeyMap(id, kmod, RawEvent::Keyboard::RELEASED, script);
		}
	} else
	if (device_type == 'M') {
		if ( type == 'P' ) {
			addMouseButtonMap(id, RawEvent::MouseButton::PRESSED, kmod, mode, script);
		} else
		if ( type == 'R' ) {
			addMouseButtonMap(id, RawEvent::MouseButton::RELEASED, kmod, mode, script);
		} else
		if (type == 'M') {
			if (script.size() != 1) return;
			addMouseMotionMap(mmod, kmod, mode, Motion(script[0].id));
		}
	}
}


void EventMapping::parseBinding(const char *line) {
	std::string target(line+1);
	hasht key = target;
	m_Bindings.push_back(key);
}


bool EventMapping::load(std::string const &path) {
	Script script;
	std::ifstream ifs(path.c_str());
	char line[1024];
	if (!ifs) return false;
	while (ifs.getline(line, 1024)) {
		char type = line[0];
		switch (type) {
		case '+':
			parseMap(line, script);
			script.clear();
			break;
		case '@':
			parseAction(line, script);
			break;
		case '=':
			parseBinding(line);
			break;
		default:
			continue;
		}
	}
	return true;
}

} // namespace input
} // namespace csp

