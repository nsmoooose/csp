/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
 *
 * This file is part of SimData.
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
 * @brief Classes for storing and accessing object interfaces
 */


#include <SimData/InterfaceRegistry.h>
#include <SimData/InterfaceProxy.h>
#include <SimData/Log.h>
#include <SimData/ObjectInterface.h>

#include <string>
#include <vector>


NAMESPACE_SIMDATA


InterfaceRegistry::InterfaceRegistry() {
	SIMDATA_LOG(LOG_REGISTRY, LOG_DEBUG, "Initializing interface registry.");
}

InterfaceRegistry::~InterfaceRegistry() {
	SIMDATA_LOG(LOG_REGISTRY, LOG_DEBUG, "Destroying the interface registry.");
}

InterfaceProxy *InterfaceRegistry::getInterface(const char *name) {
	proxy_map::iterator i = __map.find(name);
	if (i == __map.end()) return 0;
	return i->second;
}

InterfaceProxy *InterfaceRegistry::getInterface(hasht key) {
	proxy_id_map::iterator i = __id_map.find(key);
	if (i == __id_map.end()) return 0;
	return i->second;
}

bool InterfaceRegistry::hasInterface(const char *name) {
	return (__map.find(name) != __map.end());
}

bool InterfaceRegistry::hasInterface(hasht key) {
	return (__id_map.find(key) != __id_map.end());
}

std::vector<std::string> InterfaceRegistry::getInterfaceNames() const {
	std::vector<std::string> names;
	interface_list::const_iterator i;
	for (i = __list.begin(); i != __list.end(); i++) {
		names.push_back((*i)->getClassName());
	}
	return names;
}

std::vector<InterfaceProxy *> InterfaceRegistry::getInterfaces() const {
	return __list;
}

void InterfaceRegistry::addInterface(const char *name, hasht id, InterfaceProxy *proxy) {
	if (hasInterface(name) || hasInterface(id)) {
		throw InterfaceError("interface \"" + std::string(name) + "\" multiply defined");
	}
	__map[name] = proxy;
	__id_map[id] = proxy;
	__list.push_back(proxy);
	SIMDATA_LOG(LOG_REGISTRY, LOG_DEBUG, "Registering interface<" << name << "> [" << id << "]");
}

NAMESPACE_SIMDATA_END


