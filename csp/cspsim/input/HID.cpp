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
 * @file HID.cpp
 *
 **/

# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning(disable : 4786)
# endif

#include <csp/cspsim/input/HID.h>
#include <csp/cspsim/input/InputInterface.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <string>


namespace csp {
namespace input {

/////////////////////////////////////////////////////////////////////////////
// class HID

void HID::translate(RawEvent::Keyboard &event) {
	static std::list<unsigned int> pressed;
	if (event.type == RawEvent::Keyboard::PRESSED) {
		pressed.push_front((event.modifierMask << 16) | event.key);
		// incase we miss removing some keys, don't let the
		// list grow by much... noone is using so many fingers
		// at once ;-)
		if (pressed.size() > 8) pressed.pop_back();
	} else {
		std::list<unsigned int>::iterator i = pressed.begin();
		std::list<unsigned int>::iterator j = pressed.end();
		for (; i != j; i++) {
			if ((*i & 0xFFFF) == event.key) {
				event.modifierMask = *i >> 16;
				pressed.erase(i);
				return;
			}
		}
		// not found!?
		// can happen.... just ignore it.
	}
}



/////////////////////////////////////////////////////////////////////////////
// class VirtualHID: public HID

VirtualHID::VirtualHID(): m_VirtualMode(0), m_JoystickModifier(false), m_Object(0), m_ActiveScript(0) {
}

VirtualHID::~VirtualHID() {}

void VirtualHID::setMapping(Ref<EventMapping const> map) {
	m_Map = map;
}

void VirtualHID::bindObject(InputInterface *object) { m_Object = object; }

bool VirtualHID::onEvent(RawEvent::Keyboard const &event) {
	if (!m_Object) return false;
	if (m_Object->onKey(event)) return true;
	if (!m_Map) return false;
	EventMapping::Script const *s = m_Map->getKeyScript(event.key, event.modifierMask, event.type);
	if (s == NULL) return false;
	setScript(s);
	return true;
}

bool VirtualHID::onEvent(RawEvent::JoystickButton const &event) {
	if (!m_Object) return false;
	if (m_Object->onJoystickButton(event)) return true;
	if (!m_Map) return false;
	EventMapping::Script const *s =
		m_Map->getJoystickButtonScript(event.joystick, event.button, event.type, m_JoystickModifier, m_VirtualMode);
	if (s == NULL) return false;
	setScript(s);
	return true;
}

bool VirtualHID::onEvent(RawEvent::JoystickAxis const &event) {
	if (!m_Object) return false;
	if (m_Object->onJoystickAxisMotion(event)) return true;
	if (!m_Map) return false;
	EventMapping::Axis const *a = m_Map->getJoystickAxis(event.joystick, event.axis);
	if (a == NULL || a->id == "") return false;
	if (!m_Object->onMapEvent(MapEvent::MapAxisEvent(a->id, event.value))) {
		std::cout << "Missing HID interface for command '" << a->id << "'\n";
	}
	return true;
}

bool VirtualHID::onEvent(RawEvent::MouseMotion const &event) {
	if (!m_Object) return false;
	if (m_Object->onMouseMove(event)) return true;
	if (!m_Map) return false;
	EventMapping::Motion const *m = m_Map->getMouseMotion(event.buttonMask, event.modifierMask, m_VirtualMode);
	if (m == NULL || m->id == "") return false;
	if (!m_Object->onMapEvent(MapEvent::MapMotionEvent(m->id, event.x, event.y, event.dx, event.dy))) {
		std::cout << "Missing HID interface for command '" << m->id << "'\n";
	}
	return true;
}

bool VirtualHID::onEvent(RawEvent::MouseButton const &event) {
	if (!m_Object) return false;
	if (m_Object->onMouseButton(event)) return true;
	if (!m_Map) return false;
	EventMapping::Script const *s = m_Map->getMouseButtonScript(event.button, event.type, event.modifierMask, m_VirtualMode);
	if (s == NULL) return false;
	setScript(s, event.x, event.y, event.drag);
	return true;
}

void VirtualHID::onUpdate(double dt) {
	int iterations = 0;
	if (!m_Object || !m_ActiveScript) return;
	/**
	 * wait out the delay until the next action should start
	 * strings of actions with delay = 0 will be executed in
	 * a single call of onUpdate().  a break after 100 actions
	 * occurs to prevent infinite loops from completely hanging
	 * the sim.
	 */
	m_ScriptTime += dt;
	EventMapping::Action const *action = m_ActiveScript->getAction();
	while (action->time <= m_ScriptTime && iterations < 100) {
		const char *id = action->id.c_str();
		/** action time represents a relative delay instead of absolute */
		m_ScriptTime = 0.0;
		if (action->mode >= 0) {
			setVirtualMode(action->mode);
		}
		if (action->jmod >= 0) {
			setJoystickModifier(action->jmod);
		}
		if (*id) {
			if (!m_Object->onMapEvent(MapEvent::MapCommandEvent(id, m_MouseEventX, m_MouseEventY, m_MouseEventDrag))) {
				std::cout << "VirtualHID::onUpdate: Missing HID interface for command '" << id << "'\n";
			}
		}
		/** advance, end, or loop the script */
		int loop = action->loop;
		int stop = action->stop;
		if (loop >= 0) {
			m_ActiveScript->jump(loop);
		} else {
			if (!m_ActiveScript->advance()) {
				m_ActiveScript = 0;
				break;
			}
		}
		if (stop) {
			m_ActiveScript = 0;
			break;
		}
		action = m_ActiveScript->getAction();
		iterations++;
	}
}

void VirtualHID::setScript(EventMapping::Script const *s, float x, float y, bool drag) {
	if (m_ActiveScript) {
		m_ActiveScript->jump(0);
	}
	m_MouseEventX = x;
	m_MouseEventY = y;
	m_MouseEventDrag = drag;
	m_ActiveScript = s;
	m_ScriptTime = 0.0;
	onUpdate(0.0);
}

void VirtualHID::setVirtualMode(int mode) {
	m_VirtualMode = mode;
}

void VirtualHID::setJoystickModifier(int jmod) {
	m_JoystickModifier = (jmod != 0);
}

} // namespace input
} // namespace csp

