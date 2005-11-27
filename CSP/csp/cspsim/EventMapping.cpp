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

#include <csp/cspsim/EventMapping.h>

#include <SDL/SDL_events.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <assert.h>


CSP_NAMESPACE

EventMapping::EventMapping() {
}

int EventMapping::getKeyID(int device, SDL_keysym const &key, int state, int mode) const {
	int press = (state == SDL_PRESSED) ? (1 << 31) : 0;
	return (device & 0xF) + ((key.sym & 0x3FF) << 4) + ((key.mod & 0xFFF) << 14) + ((mode & 0x00F) << 26) + press;
}

int EventMapping::getJoystickButtonID(int device, int button, int state, int jmod, int mode) const {
	int press = (state == SDL_PRESSED) ? (1 << 31) : 0;
	return (device & 0xF) + ((button & 0xFFF) << 4) + ((jmod & 0xF) << 16) + ((mode & 0xF) << 20) + press;
}

int EventMapping::getMouseButtonID(int device, int button, int state, int kmod, int mode) const {
	int press = (state == SDL_PRESSED) ? (1 << 31) : 0;
	return (device & 0xF) + ((button & 0xFF) << 4) + ((kmod & 0xFFF) <<  12) + ((mode & 0xF) << 24) + press;
}

int EventMapping::getMouseMotionID(int device, int state, int kmod, int mode) const {
	return (device & 0xF) + ((state & 0xFF) << 4) + ((kmod & 0xFFF) << 12) + ((mode & 0xF) << 24);
}


EventMapping::Script const *EventMapping::getKeyScript(int device, SDL_keysym const &key, int state, int mode) const {
	int id = getKeyID(device, key, state, mode);
	EventScript kScript = m_KeyMap.find(id);
	if (kScript == m_KeyMap.end()) return NULL;
	return &(kScript->second);
}

EventMapping::Script const *EventMapping::getJoystickButtonScript(int device, int button, int state, int jmod, int mode) const {
	int id = getJoystickButtonID(device, button, state, jmod, mode);
	EventScript bScript = m_JoystickButtonMap.find(id);
	if (bScript == m_JoystickButtonMap.end()) return NULL;
	return &(bScript->second);
}

EventMapping::Axis const *EventMapping::getJoystickAxis(int device, int axis) const {
	return &(m_JoystickAxisMap[device & 3][axis & 7]);
}

void EventMapping::addJoystickAxisMap(int device, int axis, Axis const &a) {
	assert(device >= 0 && device < 4);
	assert(axis >= 0 && axis < 8);
	m_JoystickAxisMap[device][axis] = a;
}

void EventMapping::addKeyMap(int device, SDL_keysym &key, int state, int mode, Script const &s) {
	int id = getKeyID(device, key, state, mode);
	m_KeyMap[id] = s;
}

void EventMapping::addJoystickButtonMap(int device, int button, int state, int jmod, int mode, Script const &s) {
	int id = getJoystickButtonID(device, button, state, jmod, mode);
	m_JoystickButtonMap[id] = s;
}

void EventMapping::addMouseMotionMap(int device, int state, int kmod, int mode, Motion const &motion) {
	int id = getMouseMotionID(device, state, kmod, mode);
	m_MouseMotionMap[id] = motion;
}
	
void EventMapping::addMouseButtonMap(int device, int button, int state, int kmod, int mode, Script const &s) {
	int id = getMouseButtonID(device, button, state, kmod, mode);
	m_MouseButtonMap[id] = s;
}

// convenience method for testing purposes only
void EventMapping::addKeyMap(int device, SDLKey vkey, SDLMod kmod, int state, int mode, Action const &action) {
	SDL_keysym key;
	key.sym = vkey;
	key.mod = kmod;
	Script s;
	s.push_back(action);
	addKeyMap(device, key, state, mode, s);
}

// convenience method for testing purposes only
void EventMapping::addJoystickButtonMap(int device, int button, int state, int jmod, int mode, Action const &action) {
	Script s;
	s.push_back(action);
	addJoystickButtonMap(device, button, state, jmod, mode, s);
}

EventMapping::Script const *EventMapping::getMouseButtonScript(int device, int button, int state, int kmod, int mode) const {
	int id = getMouseButtonID(device, button, state, kmod, mode);
	EventScript mScript = m_MouseButtonMap.find(id);
	if (mScript == m_MouseButtonMap.end()) return NULL;
	return &(mScript->second);
}

//int EventMapping::getMouseMotion(int device, int state, int kmod, int mode) const {
EventMapping::Motion const *EventMapping::getMouseMotion(int device, int state, int kmod, int mode) const {
	int id = getMouseMotionID(device, state, kmod, mode);
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
	int device, kmod, jmod, mmod, id, mode, state = SDL_PRESSED;
	s >> device_type >> device >> type >> kmod >> jmod >> mmod >> id >> mode;
	if (type == 'P') {
		state = SDL_PRESSED; 
	} else
	if (type == 'R') {
		state = SDL_RELEASED;
	}
	if (device_type == 'J') {
		if (type == 'P' || type == 'R') {
			addJoystickButtonMap(device, id, state, jmod, mode, script);
		} else
		if (type == 'A') {
			if (script.size() != 1) return;
			addJoystickAxisMap(device, id, Axis(script[0].id)); 
		}
	} else
	if (device_type == 'K') {
		if (type == 'P' || type == 'R') {
			SDL_keysym key;
			key.sym = (SDLKey) id;
			key.mod = (SDLMod) kmod;
			addKeyMap(device, key, state, mode, script);
		}
	} else
	if (device_type == 'M') {
		if (type == 'P' || type == 'R') {
			addMouseButtonMap(device, id, state, kmod, mode, script);
		} else
		if (type == 'M') {
			if (script.size() != 1) return;
			addMouseMotionMap(device, mmod, kmod, mode, Motion(script[0].id));
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

CSP_NAMESPACE_END

