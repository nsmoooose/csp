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


#include <string>
#include <vector>

#include <SimData/Log.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/Object.h>
#include <SimData/HashUtility.h>
#include <SimData/TypeAdapter.h>
#include <SimData/ObjectInterface.h>
#include <SimData/ns-simdata.h>
#include <SimData/Exception.h>
#include <SimData/Enum.h>
#include <SimData/Path.h>

using std::cout;
using std::endl;

NAMESPACE_SIMDATA


///////////////////////////////////////////////////////////////////////////
// InterfaceProxy



/**
 * The master interface registry.
 */

/*
#ifdef _WIN32
#pragma comment(linker, "/SECTION:.shared,RWS")
#pragma data_seg(".shared")
#endif

InterfaceRegistry g_InterfaceRegistry;
*/

//InterfaceRegistry::proxy_map *InterfaceRegistry::__map = 0;
//InterfaceRegistry::proxy_id_map *InterfaceRegistry::__id_map = 0;
//InterfaceRegistry::interface_list *InterfaceRegistry::__list = 0;

/*
#ifdef _WIN32
#pragma data_seg()
#endif
*/

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

//MemberAccessorBase * InterfaceProxy::getAccessor(const char *name, const char *cname = 0) const throw(InterfaceError) { 
MemberAccessorBase * InterfaceProxy::getAccessor(const char *name, const char *cname) const { 
	if (!cname) cname = "?";
	throw InterfaceError("variable \"" + std::string(name) + "\" not defined in interface to class " + cname); 
}

const TypeAdapter InterfaceProxy::get(Object *o, const char *name) const {
	return getAccessor(name)->get(o); 
} 

void InterfaceProxy::set(Object *o, const char *name, const TypeAdapter &v) { 
	getAccessor(name)->set(o, v);
}

void InterfaceProxy::push_back(Object *o, const char *name, const TypeAdapter &v) {
	try {
		getAccessor(name)->push_back(o, v);
	} catch (TypeMismatch &e) {
		e.appendMessage(std::string("Error encountered adding to list variable \"") + name + "\" in class \"" + getClassName() + "\".");
		throw;
	}
}

void InterfaceProxy::set_enum(Object *o, const char *name, const char *v) { 
	getAccessor(name)->set(o, TypeAdapter(v));
}

void InterfaceProxy::clear(Object *o, const char *name) {
	getAccessor(name)->clear(o);
}

bool InterfaceProxy::variableExists(const char *name) const {
	return false;
}

bool InterfaceProxy::variableRequired(const char *name) const {
	return false;
}

hasht InterfaceProxy::getClassHash() const { 
	error("INTERNAL ERROR: InterfaceProxy::getClassHash()");
	return 0;
}

const char * InterfaceProxy::getClassName() const { 
	error("INTERNAL ERROR: InterfaceProxy::getClassName()");
	return 0;
}

void InterfaceProxy::pack(Object *, Packer &) const {
}

void InterfaceProxy::unpack(Object *, UnPacker &) const {
}

std::vector<std::string> InterfaceProxy::getVariableNames() const {
	return std::vector<std::string>();
}

std::vector<std::string> InterfaceProxy::getRequiredNames() const {
	return std::vector<std::string>();
}


///////////////////////////////////////////////////////////////////////////
// InterfaceRegistry

InterfaceRegistry::InterfaceRegistry() {
}

InterfaceRegistry::~InterfaceRegistry() {
	__cleanup();
}

void InterfaceRegistry::__cleanup() {
	if (__map) delete __map;
	if (__id_map) delete __id_map;
	if (__list) delete __list;
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



NAMESPACE_END // namespace simdata



////////////////////////////////////////////////////////////////////////////

#ifdef TEST0

#include <vector>
#include <SimData/Real.h>
#include <SimData/Path.h>

class X: public simdata::Object {
public:
	SIMDATA_OBJECT(X, 0, 0);
	BEGIN_SIMDATA_XML_INTERFACE(X)
		SIMDATA_XML("x", X::x, true);
	END_SIMDATA_XML_INTERFACE
	X() { x = 0; }
	int x;
	void dump() { cout << "HI from X!: " << x << endl; }
	virtual ~X() { cout << "~X: " << x << endl; }
};


class A: public simdata::Object {
public:
	SIMDATA_OBJECT(A, 0, 0);
	BEGIN_SIMDATA_XML_INTERFACE(A)
		SIMDATA_XML("size", A::size, true)
		SIMDATA_XML("x", A::x, true)
		SIMDATA_XML("y", A::y, true)
		SIMDATA_XML("z", A::z, false)
		SIMDATA_XML("q", A::q, false)
	END_SIMDATA_XML_INTERFACE
	int size;
	simdata::Real x;
	std::vector<simdata::Real> y;
	std::vector<X> z;
	X q;
	virtual ~A() {
		cout << size << endl;
		cout << (float)x << endl;
		cout << "y has " << y.size() << " items:" << endl;
		std::vector<simdata::Real>::iterator i;
		for (i=y.begin(); i != y.end(); i++) cout << "   " << (float)(*i) << endl;
		cout << "z has " << z.size() << " items:" << endl;
		std::vector<X>::iterator j;
		for (j=z.begin(); j != z.end(); j++) j->dump();
	}
};



class I: public simdata::Object {
public:
        SIMDATA_OBJECT(I, 0, 0);
	BEGIN_SIMDATA_XML_INTERFACE(I)
		SIMDATA_XML("size_i", I::size_i, true)
	END_SIMDATA_XML_INTERFACE
	int size_i;
};


class J: public I {
public:
        SIMDATA_OBJECT(J, 0, 0);
	EXTEND_SIMDATA_XML_INTERFACE(J, I)
		SIMDATA_XML("size_j", J::size_j, true)
	END_SIMDATA_XML_INTERFACE
	int size_j;
};

SIMDATA_REGISTER_INTERFACE(I)
SIMDATA_REGISTER_INTERFACE(J)
SIMDATA_REGISTER_INTERFACE(X)
SIMDATA_REGISTER_INTERFACE(A)

#endif


////////////////////////////////////////////////////////////////////////////

#ifdef TEST

#include <SimData/Types.h>

class TEST: public simdata::Object {
public:
	SIMDATA_OBJECT(TEST, 0, 0);
	BEGIN_SIMDATA_XML_INTERFACE(TEST)
		SIMDATA_XML("int", TEST::int_test, false)
		SIMDATA_XML("float", TEST::float_test, false)
		SIMDATA_XML("string", TEST::string_test, false)
		SIMDATA_XML("spread", TEST::spread_test, false)
		SIMDATA_XML("date", TEST::date_test, false)
		SIMDATA_XML("vector", TEST::vector_test, false)
		SIMDATA_XML("matrix", TEST::matrix_test, false)
		SIMDATA_XML("external", TEST::external_test, false)
		SIMDATA_XML("enum", TEST::enum_test, false)
		SIMDATA_XML("curve", TEST::curve_test, false)
		SIMDATA_XML("table", TEST::table_test, false)
		SIMDATA_XML("path", TEST::path_test, false)
		SIMDATA_XML("list", TEST::list_test, false)
	END_SIMDATA_XML_INTERFACE
	int int_test;
	float float_test;
	std::string string_test;
	simdata::Real spread_test;
	simdata::SimDate date_test;
	simdata::Vector3 vector_test;
	simdata::Matrix3 matrix_test;
	simdata::External external_test;
	simdata::Enumeration enumeration;
	simdata::Enum enum_test;
	simdata::Curve curve_test;
	simdata::Table table_test;
	simdata::PathPointer<A> path_test;
	std::vector<simdata::Real> list_test;
	TEST(): simdata::Object(), enumeration("A B C D"), enum_test(enumeration) {}
	virtual void pack(simdata::Packer &) const {
		cout << "PACKING C++ TEST CLASS" << endl;
		cout << "      int_test " << int_test << endl;
		cout << "    float_test " << float_test << endl;
		cout << "   string_test " << string_test << endl;
		cout << "     date_test " << date_test.asString() << endl;
		cout << "  vector3_test " << vector_test.asString() << endl;
		cout << "  matrix3_test " << matrix_test.asString() << endl;
		cout << " external_test " << external_test.asString() << endl;
		cout << "   spread_test " << spread_test.asString() << endl;
		cout << "     path_test " << path_test.asString() << endl;
		cout << "     enum_test " << enum_test.asString() << endl;
		cout << "    curve_test " << curve_test.asString() << endl;
		cout << "    table_test " << table_test.asString() << endl;
		cout << "     list_test ";
		std::vector<simdata::Real>::const_iterator j;
		for (j=list_test.begin(); j != list_test.end(); j++) cout << j->asString();
		cout << endl;
	}
	virtual void unpack(simdata::UnPacker &) {
	}
};

SIMDATA_REGISTER_INTERFACE(TEST)

#endif // !NOTEST
