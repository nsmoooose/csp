/* Combat Simulator Project
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
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
 * @file InterfaceProxy.cpp
 * @brief Classes for storing and accessing object interfaces
 */


#include <csp/csplib/data/InterfaceProxy.h>
#include <csp/csplib/data/InterfaceRegistry.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/TypeAdapter.h>
#include <csp/csplib/util/Log.h>

#include <algorithm>
#include <string>
#include <vector>
#include <sstream>


namespace csp {


void InterfaceProxy::globalRegister(const char *cname, const hasht chash) {
	assert(chash != 0);
	InterfaceRegistry::getInterfaceRegistry().addInterface(cname, chash, this);
}

Object *InterfaceProxy::createObject() const {
	fatal("INTERNAL ERROR: InterfaceProxy::createObject()");
	return 0;
}

void InterfaceProxy::addInterface(ObjectInterfaceBase* objectinterface, bool global_register) {
	const char *classname = objectinterface->getClassName();
	hasht const &classhash = objectinterface->getClassHash();
	std::vector<std::string> names = objectinterface->getVariableNames();
	std::vector<std::string>::iterator name = names.begin();
	for (; name != names.end(); ++name) {
		if (_interfacesByVariableName.find(*name) != _interfacesByVariableName.end()) {
			// variable multiply defined
			std::ostringstream ss;
			ss << "variable \"" << *name << "\""
			   << " multiply defined in interface to class "
			   << classname << " or parent interface.";
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
	if (global_register) globalRegister(classname, classhash);
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
	fatal("INTERNAL ERROR: InterfaceProxy::getClassHash()");
	return 0;
}

const char * InterfaceProxy::getClassName() const {
	fatal("INTERNAL ERROR: InterfaceProxy::getClassName()");
	return 0;
}

bool InterfaceProxy::isSubclass(std::string const &classname) const {
	return std::find(_classNames.begin(), _classNames.end(), classname) != _classNames.end();
}

bool InterfaceProxy::isSubclass(hasht const &classhash) const {
	return std::find(_classHashes.begin(), _classHashes.end(), classhash) != _classHashes.end();
}


const TypeAdapter InterfaceProxy::get(Object *o, std::string const &varname) const {
	ObjectInterfaceBase *oi = findInterface(varname, true);
	return oi->get(o, varname);
}

void InterfaceProxy::set(Object *o, std::string const &varname, const TypeAdapter &v) {
	ObjectInterfaceBase *oi = findInterface(varname, true);
	oi->set(o, varname, v);
}

void InterfaceProxy::push_back(Object *o, std::string const &varname, const TypeAdapter &v) {
	ObjectInterfaceBase *oi = findInterface(varname, true);
	oi->push_back(o, varname, v);
}

const TypeAdapter InterfaceProxy::getScalarPrototype(std::string const &varname) const {
	ObjectInterfaceBase *oi = findInterface(varname, true);
	return oi->getScalarPrototype(varname);
}

void InterfaceProxy::set_enum(Object *o, std::string const &varname, std::string const &v) {
	set(o, varname, TypeAdapter(v));
}

void InterfaceProxy::clear(Object *o, std::string const &varname) {
	ObjectInterfaceBase *oi = findInterface(varname, true);
	oi->clear(o, varname);
}

bool InterfaceProxy::variableRequired(std::string const &varname) const {
	ObjectInterfaceBase *oi = findInterface(varname, true);
	return oi->variableRequired(varname);
}

std::string InterfaceProxy::variableType(std::string const &varname) const {
	ObjectInterfaceBase *oi = findInterface(varname, true);
	return oi->variableType(varname);
}


} // namespace csp


