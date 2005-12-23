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

#include <csp/csplib/util/Modules.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/ScopedPointer.h>

#ifdef WIN32
#define NOMINMAX
#endif
#include <cc++/file.h>
#include <map>

CSP_NAMESPACE

namespace {

struct ModuleWrapper: public Referenced {
	inline ModuleWrapper(std::string const &path);
	inline ~ModuleWrapper() { CSPLOG(INFO, REGISTRY) << "Unloading module " << m_path; }
	std::string m_path;
	ScopedPointer<ost::DSO> m_dso;
	typedef std::map<std::string, Ref<ModuleWrapper> > Map;
};

ModuleWrapper::Map *getModuleRegistry() {
	static ModuleWrapper::Map *registry = 0;
	if (!registry) registry = new ModuleWrapper::Map;
	return registry;
}

// Force singleton construction at startup to avoid synchronization issues.
struct ModuleRegistryInitializer { ModuleRegistryInitializer() { getModuleRegistry(); } } InitModuleRegistry;

ModuleWrapper::ModuleWrapper(std::string const &path): m_path(path) {
	CSPLOG(INFO, REGISTRY) << "Loading module " << path;
	m_dso.reset(new ost::DSO(path.c_str(), /*bindnow=*/false));
	getModuleRegistry()->insert(std::make_pair(path, this));
}

} // namespace

bool ModuleLoader::load(std::string const &path) {
	try {
		if (!isLoaded(path)) new ModuleWrapper(path);
	} catch (...) {
		CSPLOG(ERROR, REGISTRY) << "Failed to load module " << path;
		return false;
	}
	return true;
}

bool ModuleLoader::unload(std::string const &path) {
	ModuleWrapper::Map::iterator iter = getModuleRegistry()->find(path);
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
	return (getModuleRegistry()->find(path) != getModuleRegistry()->end());
}

CSP_NAMESPACE_END


