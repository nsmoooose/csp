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
 * @file EventMapIndex.cpp
 *
 **/


# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning(disable : 4786)
# endif

#include "EventMapIndex.h"
#include "Log.h"

#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <SimData/FileUtility.h>

#include <assert.h>



EventMapping::Ref EventMapIndex::getMap(const simdata::hasht &key) {
	MapHash::iterator i = m_Index.find(key);
	if (i == m_Index.end()) return NULL;
	return i->second;
}

EventMapping::Ref EventMapIndex::getMap(const std::string &id) {
	return getMap(simdata::hasht(id));
}

void EventMapIndex::load(std::string const &path) {
	EventMapping::Ref m = new EventMapping;
	assert(m.valid());
	CSP_LOG(APP, INFO, "Loading human interface device mapping '" << path << "'");
	if (m->load(path)) {
		m_Maps.push_back(m);
		std::vector<simdata::hasht>::const_iterator idx;
		std::vector<simdata::hasht> const &bindings = m->getBindings();
		for (idx = bindings.begin(); idx != bindings.end(); idx++) {
			m_Index[*idx] = m;
		}
	}
}

void EventMapIndex::loadAllMaps() {
	std::string path;
	osgDB::DirectoryContents dc;
	osgDB::DirectoryContents::iterator file;
	path = getConfigPath("InputMapPath");
	CSP_LOG(APP, INFO, "Looking for human interface device mappings in '" << path << "'");
	dc = osgDB::getDirectoryContents(path);
	for (file = dc.begin(); file != dc.end(); file++) {
		std::string fn = simdata::ospath::join(path, *file);
		if (osgDB::getFileExtension(fn) != "hid") continue;
		load(fn);
	}
}


