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


#include <csp/cspsim/EventMapIndex.h>
#include <csp/cspsim/Config.h>

//#include <osgDB/FileUtils>
//#include <osgDB/FileNameUtils>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/FileUtility.h>

#include <cassert>

CSP_NAMESPACE

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
	}
}

void EventMapIndex::loadAllMaps() {
	std::string path = getConfigPath("InputMapPath");
	CSPLOG(INFO, APP) << "Looking for human interface device mappings in '" << path << "'";
	ospath::DirectoryContents dc = ospath::getDirectoryContents(path);
	CSPLOG(INFO, APP) << "Found " << dc.size() << " files";
	for (ospath::DirectoryContents::const_iterator file = dc.begin(); file != dc.end(); ++file) {
		std::string fn = ospath::join(path, *file);
		CSPLOG(INFO, APP) << "File: " << *file << ", " << fn << ", " << ospath::getFileExtension(fn);
		if (ospath::getFileExtension(fn) == "hid") load(fn);
	}
}

CSP_NAMESPACE_END

