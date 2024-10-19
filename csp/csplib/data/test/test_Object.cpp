/* Combat Simulator Project
 * Copyright (C) 2003 Mark Rose <mkrose@users.sf.net>
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


#include <csp/csplib/data/DataArchive.h>
#include <csp/csplib/data/InterfaceProxy.h>
#include <csp/csplib/data/InterfaceRegistry.h>
#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/Testing.h>

#include <cstdlib>


class TestObject: public csp::Object
{
public:
	CSP_DECLARE_OBJECT(TestObject)

	TestObject() {}
	virtual ~TestObject() {}

public:
	csp::Vector3 _vector;
	csp::Link<TestObject> _link;
	bool _bool;
};

CSP_XML_BEGIN(TestObject)
	CSP_DEF("vector", _vector, true)
	CSP_DEF("link", _link, false)
	CSP_DEF("bool", _bool, false)
CSP_XML_END


class SubObject1: public TestObject
{
public:
	CSP_DECLARE_STATIC_OBJECT(SubObject1)

	SubObject1() {}
	virtual ~SubObject1() {}

private:
	csp::Link<SubObject1> _link1;
	csp::Link<TestObject> _link2;
};

CSP_XML_BEGIN(SubObject1)
	CSP_DEF("link1", _link1, false)
	CSP_DEF("link2", _link2, true)
CSP_XML_END


class SubObject2: public TestObject
{
public:
	CSP_DECLARE_STATIC_OBJECT(SubObject2)

	SubObject2() {}
	virtual ~SubObject2() {}

public:
	double _value;
};

CSP_XML_BEGIN(SubObject2)
	CSP_DEF("value", _value, false)
CSP_XML_END

CSP_TESTFIXTURE(Object) {
public:

	virtual void setupFixture() {
		static TestObject::__csp_interface_proxy instance1;
		static SubObject1::__csp_interface_proxy instance2;
		static SubObject2::__csp_interface_proxy instance3;
	}

	void testObjectInterface(csp::InterfaceProxy *proxy) {
		CSP_VERIFY(proxy != 0);
		CSP_VERIFY(proxy->variableExists("vector"));
		CSP_VERIFY(proxy->variableExists("link"));
		CSP_VERIFY(proxy->variableExists("bool"));
		CSP_VERIFY(proxy->variableRequired("vector"));
		CSP_VERIFY(!proxy->variableRequired("link"));
		CSP_VERIFY(!proxy->variableRequired("bool"));
		CSP_VERIFY_EQ(proxy->variableType("vector"), "type::Vector3");
		CSP_VERIFY_EQ(proxy->variableType("bool"),"builtin::bool");
	}

	CSP_TESTCASE(Basics) {
		// TODO
		// * set and retrieve values
		// * split Object class declarations into a header?
		csp::InterfaceRegistry &reg = csp::InterfaceRegistry::getInterfaceRegistry();
		CSP_VERIFY(reg.hasInterface("TestObject"));
		CSP_VERIFY(reg.hasInterface("SubObject1"));
		CSP_VERIFY(reg.hasInterface(TestObject::_getClassHash()));
		CSP_VERIFY(reg.hasInterface(SubObject1::_getClassHash()));
		CSP_VERIFY(reg.hasInterface(TestObject::_getClassName()));
		CSP_VERIFY(reg.hasInterface(SubObject1::_getClassName()));
		csp::InterfaceProxy *i;
		i = reg.getInterface("TestObject");
		CSP_VERIFY_EQ(i->getClassHash(), TestObject::_getClassHash());
		CSP_VERIFY_EQ(i->getClassName(), TestObject::_getClassName());
		CSP_VERIFY(!i->isStatic());
		i = reg.getInterface("SubObject1");
		CSP_VERIFY_EQ(i->getClassHash(), SubObject1::_getClassHash());
		CSP_VERIFY_EQ(i->getClassName(), SubObject1::_getClassName());
		CSP_VERIFY(i->isStatic());
	}

	CSP_TESTCASE(ObjectInterface) {
		csp::InterfaceRegistry &reg = csp::InterfaceRegistry::getInterfaceRegistry();
		csp::InterfaceProxy *proxy = reg.getInterface("TestObject");
		testObjectInterface(proxy);
	}

	CSP_TESTCASE(SubObjectInterface) {
		csp::InterfaceRegistry &reg = csp::InterfaceRegistry::getInterfaceRegistry();
		csp::InterfaceProxy *proxy = reg.getInterface("SubObject1");
		CSP_VERIFY(proxy != 0);
		CSP_VERIFY(proxy->variableExists("link1"));
		CSP_VERIFY(proxy->variableExists("link2"));
		CSP_VERIFY(!proxy->variableRequired("link1"));
		CSP_VERIFY(proxy->variableRequired("link2"));
		CSP_VERIFY(proxy->isSubclass(TestObject::_getClassName()));
		CSP_VERIFY(proxy->isSubclass(TestObject::_getClassHash()));
		testObjectInterface(proxy);
	}

	CSP_TESTCASE(Serialize) {
		// FIXME using tmpfile would be much better but DataArchive doesn't accept open
		// stream descriptors.
		const std::string tmpfile("/tmp/csplib.tmptest.dar");
		{
			TestObject obj1;
			SubObject2 obj2;
			obj1._vector.y() = 2.17;
			obj1._link = "obj2";
			obj2._vector.x() = 42.0;
			obj2._value = 3.14;
			csp::DataArchive ar(tmpfile, /*read=*/false);
			ar.addObject(obj1, "obj1");
			ar.addObject(obj2, "obj2");
			ar.finalize();
		}
		{
			csp::DataArchive ar(tmpfile, /*read=*/true);
			csp::Ref<TestObject> obj = ar.getObject("obj1");
			CSP_VERIFY(obj.valid());
			CSP_VERIFY_LT(std::abs(obj->_vector.y() - 2.17), 1e-8);
			CSP_VERIFY(obj->_link.valid());
			csp::Path p = obj->_link;
			CSP_VERIFY(p == csp::Path("obj2"));
			CSP_VERIFY_LT(std::abs(obj->_link->_vector.x() - 42.0), 1e-8);
			csp::Ref<SubObject2> sub2 = obj->_link;
			CSP_VERIFY(sub2.valid());
			CSP_VERIFY_LT(std::abs(sub2->_value - 3.14), 1e-8);
		}
	}
};

