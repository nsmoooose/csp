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
 * @file InterfaceRegistry.h
 */


#ifndef __SIMDATA_INTERFACEREGISTRY_H__
#define __SIMDATA_INTERFACEREGISTRY_H__

#include <string>
#include <vector>

#include <SimData/Export.h>
#include <SimData/HashUtility.h>
#include <SimData/TypeAdapter.h>
#include <SimData/ObjectInterface.h>
#include <SimData/ns-simdata.h>
#include <SimData/Exception.h>


NAMESPACE_SIMDATA


class Object;
class Packer;
class UnPacker;


/**
 * class InterfaceProxy - Base class for interface proxy classes.
 *
 * Stores a reference to the actual interface and provides accessor methods to
 * create and manipulate objects dynamically at runtime.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT InterfaceProxy {
public:
	InterfaceProxy() {}
	InterfaceProxy(const char *cname, hasht chash);
#ifndef SWIG
	InterfaceProxy(const char *cname, hasht (*chash)());
#endif // SWIG

	virtual ~InterfaceProxy() { }

	virtual Object *createObject() const;

#ifndef SWIG
	virtual MemberAccessorBase * getAccessor(const char *name, const char *cname = 0) const;

	virtual const TypeAdapter get(Object *o, const char *name) const;
	
	virtual void set(Object *o, const char *name, const TypeAdapter &v);
	
	virtual void push_back(Object *o, const char *name, const TypeAdapter &v);
	
#endif // SWIG
	
	virtual void set_enum(Object *o, const char *name, const char *v);

	virtual void clear(Object *o, const char *name);

	virtual bool variableExists(const char *name) const;
	
	virtual bool variableRequired(const char *name) const;

	virtual void pack(Object *o, Packer &p) const;

	virtual void unpack(Object *o, UnPacker &p) const;
	
	virtual hasht getClassHash() const;
	
	virtual const char *getClassName() const;
	
	virtual std::vector<std::string> getVariableNames() const;
	
	virtual std::vector<std::string> getRequiredNames() const;


/////////////////////////////////////////////////////// SWIG
#ifdef SWIG
public:
%extend {
	virtual void set(Object *p, const char *name, int v) {
		self->set(p, name, TypeAdapter(v));
	}
	virtual void set(Object *p, const char *name, double v) {
		self->set(p, name, TypeAdapter(v));
	}
	virtual void set(Object *p, const char *name, const char *v) {
		self->set(p, name, TypeAdapter(v));
	}
	virtual void set(Object *p, const char *name, BaseType *v) {
		self->set(p, name, TypeAdapter(v));
	}
	virtual void push_back(Object *p, const char *name, const char *v) {
		self->push_back(p, name, TypeAdapter(v));
	}
	virtual void push_back(Object *p, const char *name, int v) {
		self->push_back(p, name, TypeAdapter(v));
	}
	virtual void push_back(Object *p, const char *name, double v) {
		self->push_back(p, name, TypeAdapter(v));
	}
	virtual void push_back(Object *p, const char *name, const BaseType &v) {
		self->push_back(p, name, TypeAdapter(v));
	}
}
#endif // SWIG

};


template <class C>
class Singleton {
public:
	static C& getInstance() {
		static C __instance;
		return __instance;
	}
private:
	Singleton() {}
	~Singleton() {}
};

/**
 * class InterfaceRegistry - Singleton class to store and access all ObjectInterfaces
 * in the application.
 *
 * ObjectInterfaces automatically register themselves with the global instance of this
 * class at startup.  Interfaces can be accessed by name to provide dynamic object
 * creation and variable assignment.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class InterfaceRegistry {
	
friend class InterfaceProxy;

public:
	typedef std::vector<InterfaceProxy *> interface_list;

	InterfaceProxy *getInterface(const char *name);
	
	InterfaceProxy *getInterface(hasht key);
	
	bool hasInterface(const char *name);
	
	bool hasInterface(hasht key);
	
	std::vector<std::string> getInterfaceNames() const;

	std::vector<InterfaceProxy *> getInterfaces() const;
	
	void addInterface(const char *name, hasht id, InterfaceProxy *proxy) throw(InterfaceError);

	static InterfaceRegistry &getInterfaceRegistry() {
		return Singleton<InterfaceRegistry>::getInstance();
	}

#if defined(_MSC_VER ) && (_MSC_VER <= 1200)
    virtual ~InterfaceRegistry();
#endif

private:

#if !defined(_MSC_VER ) || (_MSC_VER > 1200)
    virtual ~InterfaceRegistry();
#endif

	friend class Singleton<InterfaceRegistry>;
	InterfaceRegistry();
	
	void __cleanup();

	typedef HASH_MAPS<const char *, InterfaceProxy *, HASH<const char *>, eqstr>::Type proxy_map;
	typedef HASH_MAPS<hasht, InterfaceProxy *, hasht_hash, hasht_eq>::Type proxy_id_map;

	proxy_map *__map;
	proxy_id_map *__id_map;
	interface_list *__list;

};


// #define g_InterfaceRegistry Singleton<InterfaceRegistry>::getInstance()

/**
 * The master interface registry.
 */
/*
extern InterfaceRegistry g_InterfaceRegistry;
*/


//-------------------------------------------------------
// macros to create an interface to a specific class.
// since an instance of the interface class is created,
// these macros should not be used in header files.
//
// between the BEGIN and END macros, include SIMDATA_XML
// macros to bind the class member variables to string 
// identifiers:
//
// BEGIN_SIMDATA_XML_INTERFACE(myclass)
//   SIMDATA_XML("name", myclass::name, true)
//   SIMDATA_XML("size", myclass::size, true)
//   ...
// END_SIMDATA_XML_INTERFACE
//-------------------------------------------------------

// interface macro 0
#define __SIMDATA_XML_INTERFACE_0(classname, baseinterface) \
class classname##InterfaceProxy; \
friend class classname##InterfaceProxy; \
class classname##InterfaceProxy: public baseinterface \
{ \
	SIMDATA(ObjectInterface)<classname> *_interface; \
public: 

// interface macro 1 for normal classes
#define __SIMDATA_XML_INTERFACE_1(classname) \
	virtual SIMDATA(Object)* createObject() const { \
		SIMDATA(Object) *o = new classname; \
		assert(o); \
		return o; \
	} \
	virtual bool isVirtual() const { return false; }
		
// interface macro 1 for abstract classes: no createObject
#define __SIMDATA_XML_INTERFACE_V(classname) \
	virtual SIMDATA(Object)* createObject() const { assert(0); return 0; } \
	virtual bool isVirtual() const { return true; }

#ifdef _MSC_VER
	#define CTOR_INIT(a, b) b
#else
	#define CTOR_INIT(a, b) a::b
#endif

// interface macro 2
#define __SIMDATA_XML_INTERFACE_2(classname, baseinterface, nqbaseinterface) \
	virtual SIMDATA(hasht) getClassHash() const { return classname::_getClassHash(); } \
	virtual const char * getClassName() const { return classname::_getClassName(); } \
	virtual SIMDATA(MemberAccessorBase) * getAccessor(const char *name, const char *cname = 0) const { \
		if (!cname) cname = #classname; \
		SIMDATA(MemberAccessorBase) *p = _interface->getAccessor(name); \
		if (!p) { \
			return nqbaseinterface::getAccessor(name, cname); \
		} \
		return p; \
	} \
	virtual void pack(SIMDATA(Object) *o, SIMDATA(Packer) &p) const { \
		nqbaseinterface::pack(o, p); \
		_interface->pack(o, p); \
	} \
	virtual void unpack(SIMDATA(Object) *o, SIMDATA(UnPacker) &p) const { \
		nqbaseinterface::unpack(o, p); \
		_interface->unpack(o, p); \
	} \
	virtual bool variableExists(const char *name) const { \
		return _interface->variableExists(name) || nqbaseinterface::variableExists(name); \
	} \
	virtual bool variableRequired(const char *name) const { \
		return _interface->variableRequired(name) || nqbaseinterface::variableRequired(name); \
	} \
	virtual std::vector<std::string> getVariableNames() const { \
		std::vector<std::string> s = nqbaseinterface::getVariableNames(); \
		std::vector<std::string> t = _interface->getVariableNames(); \
		s.insert(s.end(), t.begin(), t.end()); \
		return s; \
	} \
	virtual std::vector<std::string> getRequiredNames() const { \
		std::vector<std::string> s = nqbaseinterface::getRequiredNames(); \
		std::vector<std::string> t = _interface->getRequiredNames(); \
		s.insert(s.end(), t.begin(), t.end()); \
		return s; \
	} \
	void checkDuplicates() const throw(SIMDATA(InterfaceError)) { \
		std::vector<std::string>::const_iterator name; \
		std::vector<std::string> s = _interface->getVariableNames(); \
		std::vector<std::string> t = _interface->getRequiredNames(); \
		s.insert(s.end(), t.begin(), t.end()); \
		for (name = s.begin(); name != s.end(); name++) \
			if (nqbaseinterface::variableExists(name->c_str())) {\
				std::cout << "variable \"" << *name << "\" multiply defined in interface to class " #classname " or parent interface." << std::endl; \
				throw SIMDATA(InterfaceError)("variable \"" + *name + "\" multiply defined in interface to class " #classname " or parent interface."); \
			} \
	} \
	classname##InterfaceProxy(const char * cname = #classname, SIMDATA(hasht) (*chash)() = &classname::_getClassHash): \
		baseinterface(cname, chash) \
	{ \
		_interface = new SIMDATA(ObjectInterface)<classname>; \
		(*_interface)

//-----------------------------------------

#ifdef SWIG
	#define BEGIN_SIMDATA_XML_INTERFACE(classname)
	#define BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(classname)
#else
	#define BEGIN_SIMDATA_XML_INTERFACE(classname) \
		__SIMDATA_XML_INTERFACE_0(classname, SIMDATA(InterfaceProxy)) \
		__SIMDATA_XML_INTERFACE_1(classname) \
		__SIMDATA_XML_INTERFACE_2(classname, SIMDATA(InterfaceProxy), InterfaceProxy)
	#define BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(classname) \
		__SIMDATA_XML_INTERFACE_0(classname, SIMDATA(InterfaceProxy)) \
		__SIMDATA_XML_INTERFACE_V(classname) \
		__SIMDATA_XML_INTERFACE_2(classname, SIMDATA(InterfaceProxy), InterfaceProxy)
#endif

		
//-----------------------------------------

#ifdef SWIG
	#define EXTEND_SIMDATA_XML_INTERFACE(classname, basename)
	#define EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(classname, basename)
#else
	#define EXTEND_SIMDATA_XML_INTERFACE(classname, basename) \
	    __SIMDATA_XML_INTERFACE_0(classname, basename::basename##InterfaceProxy) \
		__SIMDATA_XML_INTERFACE_1(classname) \
		__SIMDATA_XML_INTERFACE_2(classname, basename::basename##InterfaceProxy, basename##InterfaceProxy) 

	#define EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(classname, basename) \
	    __SIMDATA_XML_INTERFACE_0(classname, basename::basename##InterfaceProxy) \
		__SIMDATA_XML_INTERFACE_V(classname) \
		__SIMDATA_XML_INTERFACE_2(classname, basename::basename##InterfaceProxy, basename##InterfaceProxy)
#endif

//-----------------------------------------

#ifdef SWIG
	#define SIMDATA_XML(id, var, req)
#else
	#define SIMDATA_XML(id, var, req)       .def(id, &var, req) 
#endif
		
//-----------------------------------------

#ifdef SWIG
	#define SIMDATA_BIT(id, var, bit, req)
#else
	#define SIMDATA_BIT(id, var, bit, req)  .def(id, &var, bit, req) 
#endif
		
//-----------------------------------------

#ifdef SWIG
	#define END_SIMDATA_XML_INTERFACE
#else
	#define END_SIMDATA_XML_INTERFACE \
			.pass(); \
			checkDuplicates(); \
		} \
	};
#endif


#define SIMDATA_REGISTER_INTERFACE(classname) \
namespace { \
	classname::classname##InterfaceProxy __##classname##_interface; \
} /* anonymous namespace */

#define SIMDATA_REGISTER_INNER_INTERFACE(parent, classname) \
namespace { \
	parent::classname::classname##InterfaceProxy __##parent##_##classname##_interface; \
} /* anonymous namespace */


NAMESPACE_END // namespace simdata


#endif // __SIMDATA_INTERFACEREGISTRY_H__


