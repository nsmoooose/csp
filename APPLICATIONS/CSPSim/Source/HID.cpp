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
 * @file HID.cpp
 *
 **/

# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning(disable : 4786)
# endif

#include "HID.h"
#include "InputInterface.h"
#include <SDL/SDL_events.h>
//#include "Event.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <string>


/////////////////////////////////////////////////////////////////////////////
// class HID 



void HID::translate(SDL_Event &event) {
	if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
		static std::vector< std::list<int> > device(16);
		int device_n = event.key.which;
		assert(device_n < 16);
		std::list<int> &pressed = device[device_n];
		int sym = event.key.keysym.sym;
		if (event.key.state == SDL_PRESSED) {
			pressed.push_front((event.key.keysym.mod << 16) | sym);
			// incase we miss removing some keys, don't let the
			// list grow by much... noone is using so many fingers 
			// at once ;-)
			if (pressed.size() > 8) pressed.pop_back();
		} else {
			std::list<int>::iterator i = pressed.begin();
			std::list<int>::iterator j = pressed.end();
			for (; i != j; i++) {
				if ((*i & 0xFFFF) == sym) {
					event.key.keysym.mod = static_cast<SDLMod>(*i >> 16);
					pressed.erase(i);
					return;
				}
			}
			// not found!?
			// can happen.... just ignore it.
		}
	}
}


bool HID::onEvent(SDL_Event &event) {
	switch (event.type) {
		case SDL_MOUSEMOTION:
			return onMouseMove(event.motion);
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			return onMouseButton(event.button);
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			return onKey(event.key);
		case SDL_JOYAXISMOTION:
			return onJoystickAxisMotion(event.jaxis);
		case SDL_JOYHATMOTION:
			//return onJoystickHatMotion(event.jhat);
			return false;
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:
			return onJoystickButton(event.jbutton);
		default:
			break;
	}
	return false;
}





/////////////////////////////////////////////////////////////////////////////
// class VirtualHID: public HID



VirtualHID::VirtualHID() { 
	m_Map = 0; 
	m_VirtualMode = 0;
	m_JoystickModifier = 0;
	m_ActiveScript = 0;
}

VirtualHID::~VirtualHID() {}

void VirtualHID::setMapping(simdata::Ref<EventMapping const> map) { 
	m_Map = map; 
}

void VirtualHID::bindObject(InputInterface *object) { m_Object = object; }

bool VirtualHID::onKey(SDL_KeyboardEvent const &event) {
	if (!m_Object) return false;
	if (m_Object->onKey(event)) return true;
	if (!m_Map) return false;
	EventMapping::Script const *s = m_Map->getKeyScript(event.which, event.keysym, event.state, 0);
	if (s == NULL) return false;
	setScript(s);
	return true;
}

bool VirtualHID::onJoystickButton(SDL_JoyButtonEvent const &event) {
	if (!m_Object) return false;
	if (m_Object->onJoystickButton(event)) return true;
	if (!m_Map) return false;
	EventMapping::Script const *s = 
		m_Map->getJoystickButtonScript(event.which, event.button, event.state, m_JoystickModifier, m_VirtualMode);
	if (s == NULL) return false;
	setScript(s);
	return true;
}

bool VirtualHID::onJoystickAxisMotion(SDL_JoyAxisEvent const &event) {
	if (!m_Object) return false;
	if (m_Object->onJoystickAxisMotion(event)) return true;
	if (!m_Map) return false;
	EventMapping::Axis const *a = m_Map->getJoystickAxis(event.which, event.axis);
	if (a == NULL || a->id == "") return false;
	if (!m_Object->onMapEvent(MapEvent::AxisEvent(a->id, event.value * (1.0 / 32768.0)))) {
		std::cout << "Missing HID interface for command '" << a->id << "'\n";
	}
	return true;
}

void VirtualHID::setVirtualMode(int mode) {
	m_VirtualMode = mode;
}

void VirtualHID::setJoystickModifier(int jmod) {
	m_JoystickModifier = (jmod != 0);
}

bool VirtualHID::onMouseMove(SDL_MouseMotionEvent const &event) {
	if (!m_Object) return false;
	if (m_Object->onMouseMove(event)) return true;
	if (!m_Map) return false;
	int kmod = SDL_GetModState();
	EventMapping::Motion const *m = m_Map->getMouseMotion(event.which, event.state, kmod, m_VirtualMode);
	if (m == NULL || m->id == "") return false;
	if (!m_Object->onMapEvent(MapEvent::MotionEvent(m->id, event.x, event.y, event.xrel, event.yrel))) {
		std::cout << "Missing HID interface for command '" << m->id << "'\n";
	}
	return true;
}

bool VirtualHID::onMouseButton(SDL_MouseButtonEvent const &event) {
	if (!m_Object) return false;
	if (m_Object->onMouseButton(event)) return true;
	if (!m_Map) return false;
	int kmod = SDL_GetModState();
	EventMapping::Script const *s = m_Map->getMouseButtonScript(event.which, event.button, event.state, kmod, m_VirtualMode);
	if (s == NULL) return false;
	setScript(s, event.x, event.y);
	return true;
}

void VirtualHID::onUpdate(double dt) {
	int iterations = 0;
	if (!m_Object || !m_ActiveScript) return;
	// wait out the delay until the next action should start
	// strings of actions with delay = 0 will be executed in
	// a single call of onUpdate().  a break after 100 actions
	// occurs to prevent infinite loops from completely hanging 
	// the sim.
	m_ScriptTime += dt;
	EventMapping::Action const *action = m_ActiveScript->getAction();
	while (action->time <= m_ScriptTime && iterations < 100) {
		const char *id = action->id.c_str();
		// action time represents a relative delay instead of absolute
		m_ScriptTime = 0.0;  
		if (action->mode >= 0) {
			setVirtualMode(action->mode);
		}
		if (action->jmod >= 0) {
			setJoystickModifier(action->jmod);
		}
		if (*id) {
			if (!m_Object->onMapEvent(MapEvent::CommandEvent(id, m_MouseEventX, m_MouseEventY))) {
				std::cout << "Missing HID interface for command '" << id << "'\n";
			}
		}
		// advance, end, or loop the script
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

void VirtualHID::setScript(EventMapping::Script const *s, int x, int y) {
	if (m_ActiveScript) {
		m_ActiveScript->jump(0);
	}
	m_MouseEventX = x;
	m_MouseEventY = y;
	m_ActiveScript = s;
	m_ScriptTime = 0.0;
	onUpdate(0.0);
}


