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
*/

InterfaceRegistry g_InterfaceRegistry;

/*
#ifdef _WIN32
#pragma data_seg()
#endif
*/

InterfaceProxy::InterfaceProxy(const char *cname, hasht (*chash)())
{
	assert(chash);
	g_InterfaceRegistry.addInterface(cname, (*chash)(), this);
}

InterfaceProxy::InterfaceProxy(const char *cname, hasht chash)
{
	assert(chash != 0);
	g_InterfaceRegistry.addInterface(cname, chash, this);
}

Object *InterfaceProxy::createObject() const { 
	assert(0); 
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
	assert(0); 
	return 0;
}

const char * InterfaceProxy::getClassName() const { 
	assert(0); 
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
	if (map) delete map;
	if (id_map) delete id_map;
}

InterfaceProxy *InterfaceRegistry::getInterface(const char *name) {
	if (!map) return 0;
	proxy_map::iterator i = map->find(name);
	if (i == map->end()) return 0;
	return i->second;
}

InterfaceProxy *InterfaceRegistry::getInterface(hasht key) {
	if (!id_map) return 0;
	proxy_id_map::iterator i = id_map->find(key);
	if (i == id_map->end()) return 0;
	return i->second;
}

bool InterfaceRegistry::hasInterface(const char *name) {
	if (!map) return false;
	return (map->find(name) != map->end());
}

bool InterfaceRegistry::hasInterface(hasht key) {
	if (!id_map) return false;
	return (id_map->find(key) != id_map->end());
}

std::vector<std::string> InterfaceRegistry::getInterfaceNames() const {
	std::vector<std::string> names;
	interface_list::const_iterator i;
	for (i = list.begin(); i != list.end(); i++) {
		names.push_back((*i)->getClassName());
	}
	return names;
}

std::vector<InterfaceProxy *> const & InterfaceRegistry::getInterfaces() const {
	return list;
}

void InterfaceRegistry::addInterface(const char *name, hasht id, InterfaceProxy *proxy) throw(InterfaceError) {
	if (map == 0) {
		cout << "Initializing interface registry." << endl;
		map = new proxy_map;
		id_map = new proxy_id_map;
	}
	if (hasInterface(name)) {
		throw InterfaceError("interface \"" + std::string(name) + "\" multiply defined");
	}
	(*map)[name] = proxy;
	(*id_map)[id] = proxy;
	list.push_back(proxy);
	cout << "Registering interface<" << name << "> [" << id << "]" << endl;
}



NAMESPACE_END // namespace simdata



////////////////////////////////////////////////////////////////////////////

#ifdef TEST0

#include <vector>
#include <SimData/Spread.h>
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
	simdata::Spread x;
	std::vector<simdata::Spread> y;
	std::vector<X> z;
	X q;
	virtual ~A() {
		cout << size << endl;
		cout << (float)x << endl;
		cout << "y has " << y.size() << " items:" << endl;
		std::vector<simdata::Spread>::iterator i;
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
	simdata::Spread spread_test;
	simdata::SimDate date_test;
	simdata::Vector3 vector_test;
	simdata::Matrix3 matrix_test;
	simdata::External external_test;
	simdata::Enumeration enumeration;
	simdata::Enum enum_test;
	simdata::Curve curve_test;
	simdata::Table table_test;
	simdata::PathPointer<A> path_test;
	std::vector<simdata::Spread> list_test;
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
		std::vector<simdata::Spread>::const_iterator j;
		for (j=list_test.begin(); j != list_test.end(); j++) cout << j->asString();
		cout << endl;
	}
	virtual void unpack(simdata::UnPacker &) {
	}
};

SIMDATA_REGISTER_INTERFACE(TEST)

#endif // !NOTEST
