// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file Modules.h
 *
 **/

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Modules.h>

#include <functional>
#include <iostream>
#include <map>
#include <boost/dll.hpp>

typedef std::map<std::string, std::function<void()> > ModuleRegistryMap;

static ModuleRegistryMap *getModuleRegistry() {
	static ModuleRegistryMap *registry = 0;
	if (!registry) {
		registry = new ModuleRegistryMap();
	}
	return registry;
}

namespace csp {

bool ModuleLoader::load(std::string const &path) {
	std::cout << "Loading: " << path << std::endl;
	try {
		if (!isLoaded(path)) {
			std::function<void()> plugin = boost::dll::import_symbol<void()>(path,	"initModule");
			plugin();
			getModuleRegistry()->insert(std::make_pair(path, plugin));
		}
	}
	catch (...) {
		CSPLOG(ERROR, REGISTRY) << "Failed to load module " << path;
		return false;
	}
	return true;
}

bool ModuleLoader::unload(std::string const &path) {
	ModuleRegistryMap::iterator iter = getModuleRegistry()->find(path);
	if (iter != getModuleRegistry()->end()) {
		getModuleRegistry()->erase(iter);
		return true;
	}
	return false;
}

void ModuleLoader::unloadAll() {
	getModuleRegistry()->clear();
}

bool ModuleLoader::isLoaded(std::string const &path) {
	return getModuleRegistry()->find(path) != getModuleRegistry()->end();
}

} // namespace csp
