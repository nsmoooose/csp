/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


/**
 * @file InterfaceRegistry.cpp
 */


#include <SimData/InterfaceRegistry.h>
#include <SimData/Log.h>
#include <SimData/Object.h>
#include <SimData/HashUtility.h>
#include <SimData/TypeAdapter.h>
#include <SimData/ObjectInterface.h>
#include <SimData/Namespace.h>
#include <SimData/ExceptionBase.h>
#include <SimData/Enum.h>
#include <SimData/Path.h>
#include <SimData/Version.h>

#include <algorithm>
#include <string>
#include <vector>

using std::cout;
using std::endl;

NAMESPACE_SIMDATA



///////////////////////////////////////////////////////////////////////////
// InterfaceProxy


InterfaceProxy::InterfaceProxy(const char *cname, hasht (*chash)())
{
	assert(chash);
	InterfaceRegistry::getInterfaceRegistry().addInterface(cname, (*chash)(), this);
}

InterfaceProxy::InterfaceProxy(const char *cname, hasht chash)
{
	assert(chash != 0);
	InterfaceRegistry::getInterfaceRegistry().addInterface(cname, chash, this);
}

Object *InterfaceProxy::createObject() const { 
	error("INTERNAL ERROR: InterfaceProxy::createObject()");
	return 0;
}

void InterfaceProxy::addInterface(ObjectInterfaceBase* objectinterface, 
                                  std::string const &classname,
				  hasht const &classhash) { 
	std::vector<std::string> names = objectinterface->getVariableNames();
	std::vector<std::string>::iterator name = names.begin();
	for (; name != names.end(); ++name) {
		if (_interfacesByVariableName.find(*name) != _interfacesByVariableName.end()) {
			// variable multiply defined
			std::stringstream ss;
			ss << "variable \"" << *name << "\""
			   << " multiply defined in interface to class " 
			   << classname << " or parent interface.";
			std::cout << ss.str() << std::endl;
			throw InterfaceError(ss.str());
		}
		_interfacesByVariableName[*name] = objectinterface;
		_variableNames.push_back(*name);
		if (objectinterface->variableRequired(*name)) {
			_requiredNames.push_back(*name);
		}
	}
	_interfaces.push_back(objectinterface);
	_classNames.push_back(classname);
	_classHashes.push_back(classhash);
}

ObjectInterfaceBase *InterfaceProxy::findInterface(std::string const &varname, bool required) const {
	InterfaceMap::const_iterator iter = _interfacesByVariableName.find(varname);
	if (iter == _interfacesByVariableName.end()) {
		if (!required) return 0;
		throw InterfaceError("Variable \"" + varname + "\" not defined in interface to class \"" + getClassName() + "\"");
	}
	return iter->second;
}

hasht InterfaceProxy::getClassHash() const { 
	error("INTERNAL ERROR: InterfaceProxy::getClassHash()");
	return 0;
}

const char * InterfaceProxy::getClassName() const { 
	error("INTERNAL ERROR: InterfaceProxy::getClassName()");
	return 0;
}

bool InterfaceProxy::isSubclass(std::string const &classname) const {
	return std::find(_classNames.begin(), _classNames.end(), classname) != _classNames.end();
}

bool InterfaceProxy::isSubclass(hasht const &classhash) const {
	return std::find(_classHashes.begin(), _classHashes.end(), classhash) != _classHashes.end();
}



///////////////////////////////////////////////////////////////////////////
// InterfaceRegistry


InterfaceRegistry::InterfaceRegistry(): __map(0), __id_map(0), __list(0) {
}

InterfaceRegistry::~InterfaceRegistry() {
	__cleanup();
}

void InterfaceRegistry::__cleanup() {
	SIMDATA_LOG(LOG_REGISTRY, LOG_DEBUG, "Destroying the interface registry.");
	if (__map) delete __map; __map = 0;
	if (__id_map) delete __id_map; __id_map = 0;
	if (__list) delete __list; __list = 0;
}

InterfaceProxy *InterfaceRegistry::getInterface(const char *name) {
	if (!__map) return 0;
	proxy_map::iterator i = __map->find(name);
	if (i == __map->end()) return 0;
	return i->second;
}

InterfaceProxy *InterfaceRegistry::getInterface(hasht key) {
	if (!__id_map) return 0;
	proxy_id_map::iterator i = __id_map->find(key);
	if (i == __id_map->end()) return 0;
	return i->second;
}

bool InterfaceRegistry::hasInterface(const char *name) {
	if (!__map) return false;
	return (__map->find(name) != __map->end());
}

bool InterfaceRegistry::hasInterface(hasht key) {
	if (!__id_map) return false;
	return (__id_map->find(key) != __id_map->end());
}

std::vector<std::string> InterfaceRegistry::getInterfaceNames() const {
	std::vector<std::string> names;
	if (__list) {
		interface_list::const_iterator i;
		for (i = __list->begin(); i != __list->end(); i++) {
			names.push_back((*i)->getClassName());
		}
	}
	return names;
}

std::vector<InterfaceProxy *> InterfaceRegistry::getInterfaces() const {
	if (__list) return *__list;
	return interface_list();
}

void InterfaceRegistry::addInterface(const char *name, hasht id, InterfaceProxy *proxy) throw(InterfaceError) {
	if (__map == 0) {
		//cout << "Initializing interface registry." << endl;
		SIMDATA_LOG(LOG_REGISTRY, LOG_DEBUG, "Initializing interface registry.");
		__map = new proxy_map;
		__id_map = new proxy_id_map;
		__list = new interface_list;
	}
	if (hasInterface(name)) {
		throw InterfaceError("interface \"" + std::string(name) + "\" multiply defined");
	}
	(*__map)[name] = proxy;
	(*__id_map)[id] = proxy;
	__list->push_back(proxy);
	//cout << "Registering interface<" << name << "> [" << id << "]" << endl;
	SIMDATA_LOG(LOG_REGISTRY, LOG_DEBUG, "Registering interface<" << name << "> [" << id << "]");
}


NAMESPACE_SIMDATA_END


