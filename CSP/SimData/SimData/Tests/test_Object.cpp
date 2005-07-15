/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2003 Mark Rose <mkrose@users.sf.net>
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


#include <SimData/DataArchive.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/Link.h>
#include <SimData/Object.h>
#include <SimData/ObjectInterface.h>
#include <SimData/Vector3.h>

#include <iostream>
#include <cstdlib>
#include <cassert>


class TestObject: public simdata::Object
{
public:
	SIMDATA_DECLARE_OBJECT(TestObject)

	TestObject() {}
	virtual ~TestObject() {}

public:
	simdata::Vector3 _vector;
	simdata::Link<TestObject> _link;
	bool _bool;
};

SIMDATA_XML_BEGIN(TestObject)
	SIMDATA_DEF("vector", _vector, true)
	SIMDATA_DEF("link", _link, false)
	SIMDATA_DEF("bool", _bool, false)
SIMDATA_XML_END


class SubObject1: public TestObject
{
public:
	SIMDATA_DECLARE_STATIC_OBJECT(SubObject1)

	SubObject1() {}
	virtual ~SubObject1() {}

private:
	simdata::Link<SubObject1> _link1;
	simdata::Link<TestObject> _link2;
};

SIMDATA_XML_BEGIN(SubObject1)
	SIMDATA_DEF("link1", _link1, false)
	SIMDATA_DEF("link2", _link2, true)
SIMDATA_XML_END


class SubObject2: public TestObject
{
public:
	SIMDATA_DECLARE_STATIC_OBJECT(SubObject2)

	SubObject2() {}
	virtual ~SubObject2() {}

public:
	double _value;
};

SIMDATA_XML_BEGIN(SubObject2)
	SIMDATA_DEF("value", _value, false)
SIMDATA_XML_END


void testTestObjectInterface(simdata::InterfaceProxy *i) {
	assert(i != 0);
	assert(i->variableExists("vector"));
	assert(i->variableExists("link"));
	assert(i->variableExists("bool"));
	assert(i->variableRequired("vector"));
	assert(!i->variableRequired("link"));
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

void testSerialize() {
	// FIXME using tmpfile would be much better but DataArchive doesn't accept open
	// stream descriptors.
	const std::string tmpfile("/tmp/simdata.tmptest.dar");
	{
		std::cout << "write\n";
		TestObject obj1;
		SubObject2 obj2;
		obj1._vector.y() = 2.17;
		obj1._link = "obj2";
		obj2._vector.x() = 42.0;
		obj2._value = 3.14;
		simdata::DataArchive ar(tmpfile, /*read=*/false);
		ar.addObject(obj1, "obj1");
		ar.addObject(obj2, "obj2");
		ar.finalize();
	}
	{
		simdata::DataArchive ar(tmpfile, /*read=*/true);
		simdata::Ref<TestObject> obj = ar.getObject("obj1");
		assert(obj.valid());
		assert(std::abs(obj->_vector.y() - 2.17) < 1e-8);
		assert(obj->_link.valid());
		simdata::Path p = obj->_link;
		assert(p == simdata::Path("obj2"));
		assert(std::abs(obj->_link->_vector.x() - 42.0) < 1e-8);
		simdata::Ref<SubObject2> sub2 = obj->_link;
		assert(sub2.valid());
		assert(std::abs(sub2->_value - 3.14) < 1e-8);
	}
}

void test() {
	// TODO
	// 	* set and retrieve values
	// 	* split Object class declarations into a header?
	try {
		simdata::InterfaceRegistry &reg = simdata::InterfaceRegistry::getInterfaceRegistry();
		assert(reg.hasInterface("TestObject"));
		assert(reg.hasInterface("SubObject1"));
		assert(reg.hasInterface(TestObject::_getClassHash()));
		assert(reg.hasInterface(SubObject1::_getClassHash()));
		assert(reg.hasInterface(TestObject::_getClassName()));
		assert(reg.hasInterface(SubObject1::_getClassName()));
		simdata::InterfaceProxy *i;
		i = reg.getInterface("TestObject");
		assert(i->getClassHash() == TestObject::_getClassHash());
		assert(i->getClassName() == TestObject::_getClassName());
		assert(!i->isStatic());
		testTestObjectInterface(i);
		i = reg.getInterface("SubObject1");
		assert(i->getClassHash() == SubObject1::_getClassHash());
		assert(i->getClassName() == SubObject1::_getClassName());
		assert(i->isStatic());
		testTestObjectInterface(i);
		testSubObjectInterface(i);
		testSerialize();
	} catch (simdata::Exception e) {
		e.details();
		::exit(1);
	}
	std::cout << "OK\n";
	::exit(0);
}

int main() {
	test();
	return 0;
}

