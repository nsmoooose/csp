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
 * @file EventMapIndex.cpp
 *
 **/


#include <csp/cspsim/input/EventMapIndex.h>
#include <csp/cspsim/Config.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/FileUtility.h>

#include <cassert>

#include <SDL2/SDL.h>

namespace csp {
namespace input {

EventMapIndex::~EventMapIndex() {
	OpenedJoysticks::const_iterator openedJoystickIt;
	for (openedJoystickIt = m_openedJoysticks.begin(); openedJoystickIt != m_openedJoysticks.end(); ++openedJoystickIt) {
		if (openedJoystickIt->second) {
			SDL_JoystickClose(openedJoystickIt->second);
		}
	}
}

EventMapping::RefT EventMapIndex::getMap(const hasht &key) {
	MapHash::iterator i = m_Index.find(key);
	if (i == m_Index.end()) return NULL;
	return i->second;
}

EventMapping::RefT EventMapIndex::getMap(const std::string &id) {
	return getMap(hasht(id));
}

void EventMapIndex::load(std::string const &path) {
	EventMapping::RefT m = new EventMapping;
	assert(m.valid());
	CSPLOG(INFO, APP) << "Loading human interface device mapping '" << path << "'";
	if (m->load(path)) {
		m_Maps.push_back(m);
		std::vector<hasht>::const_iterator idx;
		std::vector<hasht> const &bindings = m->getBindings();
		for (idx = bindings.begin(); idx != bindings.end(); idx++) {
			m_Index[*idx] = m;
		}
		openNewJoysticks( m->getUsedJoysticks() );
	}
}

void EventMapIndex::loadAllMaps() {
	std::string path = getConfigPath("InputMapPath");
	CSPLOG(INFO, APP) << "Looking for human interface device mappings in '" << path << "'";
	ospath::DirectoryContents dc = ospath::getDirectoryContents(path);
	for (ospath::DirectoryContents::const_iterator file = dc.begin(); file != dc.end(); ++file) {
		std::string fn = ospath::join(path, *file);
		if (ospath::getFileExtension(fn) == "hid") {
			load(fn);
		}
	}
}

void EventMapIndex::openNewJoysticks(const EventMapping::UsedJoysticks & usedJoysticks) {
	EventMapping::UsedJoysticks::const_iterator usedJoystickIt;
	for (usedJoystickIt = usedJoysticks.begin(); usedJoystickIt != usedJoysticks.end(); ++usedJoystickIt) {
		typedef std::pair<OpenedJoysticks::iterator, bool> InsertResult;
		InsertResult insertResult = m_openedJoysticks.insert( OpenedJoysticks::value_type(*usedJoystickIt, 0) );
		if (insertResult.second) {
			insertResult.first->second = SDL_JoystickOpen(insertResult.first->first);
			if (insertResult.first->second == 0) {
				CSPLOG(ERROR, APP) << "Failed to open joystick #"
					<< std::dec << insertResult.first->first
					<< " (" << SDL_GetError() << ")";
			}
		}
	}
}

} // namespace input
} // namespace csp

