/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2003 Mark Rose <tm2@stm.lbl.gov>
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
 * @file test_Object.h
 * @brief Test basic Object and ObjectInterface functionality.
 */


#include <SimData/Object.h>
#include <SimData/Vector3.h>
#include <SimData/Link.h>
#include <SimData/InterfaceRegistry.h>

#include <iostream>
#include <cstdlib>
#include <cassert>


class TestObject: public simdata::Object
{
public:

	SIMDATA_OBJECT(TestObject,0,0)
	BEGIN_SIMDATA_XML_INTERFACE(TestObject)
		SIMDATA_XML("vector", TestObject::_vector, true)
		SIMDATA_XML("link", TestObject::_link, true)
		SIMDATA_XML("bool", TestObject::_bool, false)
	END_SIMDATA_XML_INTERFACE

	TestObject() {}
	virtual ~TestObject() {}

private:

	simdata::Vector3 _vector;
	simdata::Link<TestObject> _link;
	bool _bool;
};


class SubObject: public TestObject
{
public:

	SIMDATA_STATIC_OBJECT(SubObject,0,0)
	EXTEND_SIMDATA_XML_INTERFACE(SubObject, TestObject)
		SIMDATA_XML("link1", SubObject::_link1, false)
		SIMDATA_XML("link2", SubObject::_link2, true)
	END_SIMDATA_XML_INTERFACE

	SubObject() {}
	virtual ~SubObject() {}

private:

	simdata::Link<SubObject> _link1;
	simdata::Link<TestObject> _link2;
};

SIMDATA_REGISTER_INTERFACE(TestObject)
SIMDATA_REGISTER_INTERFACE(SubObject)

void testTestObjectInterface(simdata::InterfaceProxy *i) {
	assert(i != 0);
	assert(i->variableExists("vector"));
	assert(i->variableExists("link"));
	assert(i->variableExists("bool"));
	assert(i->variableRequired("vector"));
	assert(i->variableRequired("link"));
	assert(!i->variableRequired("bool"));
	assert(i->variableType("vector") == "type::Vector3");
	assert(i->variableType("bool") == "builtin::bool");
}

void testSubObjectInterface(simdata::InterfaceProxy *i) {
	assert(i != 0);
	assert(i->variableExists("link1"));
	assert(i->variableExists("link2"));
	assert(!i->variableRequired("link1"));
	assert(i->variableRequired("link2"));
	assert(i->isSubclass(TestObject::_getClassName()));
	assert(i->isSubclass(TestObject::_getClassHash()));
}

void test() {
	// TODO
	// 	* set and retrieve values
	// 	* split Object class declarations into a header?
	simdata::InterfaceRegistry &reg = simdata::InterfaceRegistry::getInterfaceRegistry();
	assert(reg.hasInterface("TestObject"));
	assert(reg.hasInterface("SubObject"));
	assert(reg.hasInterface(TestObject::_getClassHash()));
	assert(reg.hasInterface(SubObject::_getClassHash()));
	assert(reg.hasInterface(TestObject::_getClassName()));
	assert(reg.hasInterface(SubObject::_getClassName()));
	simdata::InterfaceProxy *i;
	i = reg.getInterface("TestObject");
	assert(i->getClassHash() == TestObject::_getClassHash());
	assert(i->getClassName() == TestObject::_getClassName());
	assert(!i->isStatic());
	testTestObjectInterface(i);
	i = reg.getInterface("SubObject");
	assert(i->getClassHash() == SubObject::_getClassHash());
	assert(i->getClassName() == SubObject::_getClassName());
	assert(i->isStatic());
	testTestObjectInterface(i);
	testSubObjectInterface(i);
	std::cout << "OK\n";
	::exit(0);
}

int main() {
	test();
	return 0;
}

