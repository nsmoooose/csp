/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
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
#include <SimData/Namespace.h>
#include <SimData/Exception.h>


NAMESPACE_SIMDATA


class Object;
class Packer;
class UnPacker;

/** @page InterfaceDetails Object Interfaces and Interface Proxies
 *
 *  Classes derived from simdata::Object can define interfaces that
 *  allow runtime introspection and assignment of specified member
 *  variables.  Such interfaces are used by the data compiler to
 *  bind XML data to class instances, which are then serialized to
 *  create a binary data archive.  Objects can later be deserialized
 *  from the archive to create instances that are preloaded with the
 *  data originally specified in XML.
 *
 *  The ObjectInterface class is the primary, internal interface to
 *  an Object-derived class.  ObjectInterface instances, in turn,
 *  rely on the MemberAccessorBase class and its derivatives to
 *  access individual member variables.  None of these classes are
 *  created directly by the user.  Instead, a series of macros are
 *  used in the declaration of any class derived from simdata::Object
 *  to declare an InterfaceProxy class.  See @ref InterfaceMacros
 *  for a detailed discussion of these macros.  The net effect is
 *  to create an inner class derived from InterfaceProxy, of which
 *  a single instance is created.  This instance is created during 
 *  static construction, and automatically registers itself with the 
 *  SimData InterfaceRegistry.  Any registered InterfaceProxy can be 
 *  accessed through the registry by specifying the corresponding 
 *  class name.
 *
 *  The InterfaceProxy classes act as both object factories and member 
 *  accessors. New instances of the corresponding Object classes can
 *  be created dynamically, and data members can be read and set
 *  by using string identifiers specified in the macros used to
 *  define the interface.  Code using an InterfaceProxy needs to
 *  know nothing in advance about the corresponding object class.
 *  The interface provides a list of names of all accessible member 
 *  variables, the data type of each variable, and methods to get 
 *  and set the value of each member variable.
 *
 *  Structurally, when you use the macros provided to declare an
 *  InterfaceProxy for an Object class, you are defining a new
 *  class derived from InterfaceProxy.  That class contains an
 *  ObjectInterface member variable, and each of the macros
 *  such as SIMDATA_XML(name, member, required) adds a new
 *  MemberAccessor instance to this interface.  Most of the methods
 *  of InterfaceProxy are just thin wrappers around calls to the
 *  underlying ObjectInterface.  In the case of inherited 
 *  interfaces, however, calls to InterfaceProxy methods may
 *  propogate up the inheritance chain until an ObjectInterface
 *  is found to satisfy the request.  If no matching interface
 *  is found, the base class InterfaceProxy defines default
 *  implementations (such as throwing an exception). 
 *
 */

/** Base class for interface proxy classes.
 *
 *  Stores a reference to the actual interface and provides accessor methods
 *  to create and manipulate objects dynamically at runtime.  See
 *  @ref InterfaceDetails for details.
 * 
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT InterfaceProxy {

private:
	/** Default constructor.
	 *
	 *  This private constructor should not be called and is undefined.
	 */
	InterfaceProxy();
public:
	/** Constructor.
	 *
	 *  Construct a new interface proxy, and register it with the global
	 *  interface registry.
	 *
	 *  @param cname the class name
	 *  @param chash the class hash
	 */
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

	virtual std::string variableType(const char *variable) const;

	virtual void pack(Object *o, Packer &p) const;

	virtual void unpack(Object *o, UnPacker &p) const;
	
	virtual hasht getClassHash() const;
	
	virtual const char *getClassName() const;
	
	virtual std::vector<std::string> getVariableNames() const;
	
	virtual std::vector<std::string> getRequiredNames() const;

	virtual bool isSubclass(std::string const &cname) const;

	virtual bool isSubclass(hasht const &chash) const;


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


/** Creates a single, static instance of the templated class.
 */
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

/** Singleton class to store and access all ObjectInterfaces in the application.
 *
 *  ObjectInterfaces automatically register themselves with the global instance of this
 *  class at startup.  Interfaces can be accessed by name to provide dynamic object
 *  creation and variable assignment.  See @ref InterfaceDetails for details.
 * 
 *  @author Mark Rose <mrose@stm.lbl.gov>
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

	typedef HASH_MAPS<const char*, InterfaceProxy*, HASH<const char*>, eqstr>::Type proxy_map;
	typedef HASHT_MAP<InterfaceProxy*>::Type proxy_id_map;

	proxy_map *__map;
	proxy_id_map *__id_map;
	interface_list *__list;

};



/** @page InterfaceMacros Object Interface Macros
 * 
 *  SimData defines several macros that are used to create 
 *  interfaces to object classes.  Interfaces are defined
 *  inside of the corresponding object class declaration,
 *  and contain information about both the inheritance
 *  hierarchy and the object class member variable that
 *  can be assigned from external data sources.
 *
 *  Interface should be defined in a public section of
 *  the enclosing Object class.  The general structure
 *  in a "BEGIN" macro, followed by a number of data
 *  member declarations, terminated by an "END" macro.
 *
 *  There are several forms of "BEGIN" macro, depending
 *  on the circumstances:
 *
 *    @li @c BEGIN_SIMDATA_XML_INTERFACE(class) should be
 *           used for non-virtual Object classes, when none
 *           of the parent classes define interfaces.
 *
 *    @li @c EXTEND_SIMDATA_XML_INTERFACE(class, parent)
 *           should be used for non-virtual Object classes
 *           with base classes that define interfaces.
 *           The @c parent parameter is the first class up
 *           the inheritance chain that defines an interface,
 *           which is often but not necessarily one of the
 *           immidiate base classes.
 *
 *    @li @c BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(class) is
 *           the same as @c BEGIN_SIMDATA_XML_INTERFACE
 *           except that it does not define methods for
 *           dynamic object creation.  It should be used
 *           for abstract classes (which declare one or more 
 *           pure virtual methods).
 *
 *    @li @c EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(class, parent) is
 *           the same as @c EXTEND_SIMDATA_XML_INTERFACE
 *           except that it does not define methods for
 *           dynamic object creation.  It should be used
 *           for abstract classes (which declare one or more 
 *           pure virtual methods).
 *
 *  Between the "BEGIN" and "END" macros, include 
 *  @c SIMDATA_XML(name, member, required) and 
 *  @c SIMDATA_BIT(name, member, bit, required) macros to bind class 
 *  member variables to string identifiers which are used to
 *  assign values in external data files.   The @c member parameter
 *  must include the class-quialified member name.  The @c required
 *  field specifies whether a particular variable must be set in
 *  the external data source.  If @c required is @c false, you must
 *  assign a reasonable default value in the class constructor.
 *  The @c bit parameter, used by the @c SIMDATA_BIT macro, sets a
 *  mask for assigning boolean values to specific bits in an
 *  integer member variable (e.g. char, int, unsigned short, etc.)
 *  Each bit acts as a distinct variable in the source data file,
 *  and SimData takes care of packing the bits correctly into the
 *  associated member variable.
 *
 *  Finally, the "END" macro is always @c END_SIMDATA_XML_INTERFACE.
 *
 *  Here are a couple examples:
 *
 *  @code
 *  class myclass: public simdata::Object {
 *    std::string name;
 *    simdata::Real size;
 *  public:
 *    ...
 *    BEGIN_SIMDATA_XML_INTERFACE(myclass)
 *      SIMDATA_XML("name", myclass::name, true)
 *      SIMDATA_XML("size", myclass::size, true)
 *      ...
 *    END_SIMDATA_XML_INTERFACE
 *    ...
 *  };
 *  @endcode
 *
 *  and
 *
 *  @code
 *  class Monster: public ScaryAnimal {
 *  public:
 *    ...
 *    EXTEND_SIMDATA_XML_INTERFACE(Monster, ScaryAnimal)
 *      SIMDATA_XML("count", Monster::count, false)
 *      SIMDATA_BIT("has_ears", Monster::traits, 0x01, true)
 *      SIMDATA_BIT("has_eyes", Monster::traits, 0x02, true)
 *      SIMDATA_BIT("has_nose", Monster::traits, 0x04, true)
 *      SIMDATA_BIT("has_toes", Monster::traits, 0x08, true)
 *      ...
 *    END_SIMDATA_XML_INTERFACE
 *    ...
 *  private:
 *    int count;
 *    unsigned char traits;
 *  };
 *  @endcode
 *
 *  Interface classes must be registered with SimData.  To do so,
 *  use the @c SIMDATA_REGISTER_INTERFACE(class) macro in the
 *  source file for the class.  Since this macro creates an 
 *  instance of the interface class, it should not be used in a
 *  header file.  The @c SIMDATA_REGISTER_INNER_INTERFACE(prefix, class)
 *  macro serves the same purpose, but works for classes declared inside
 *  a named scope (e.g. a namespace or inner class).
 *
 *  The examples above would be registered by
 *
 *  @code
 *  SIMDATA_REGISTER_INTERFACE(myclass);
 *  SIMDATA_REGISTER_INTERFACE(Monster);
 *  @endcode
 */


/** interface macro 0
 *
 *  <b><i>For internal use only.</i></b>  See @ref InterfaceMacros for details.
 */
#define __SIMDATA_XML_INTERFACE_0(classname, baseinterface) \
class classname##InterfaceProxy; \
friend class classname##InterfaceProxy; \
class classname##InterfaceProxy: public baseinterface \
{ \
	SIMDATA(ObjectInterface)<classname> *_interface; \
public: 

/** interface macro 1 for normal classes
 *
 *  <b><i>For internal use only.</i></b>  See @ref InterfaceMacros for details.
 */
#define __SIMDATA_XML_INTERFACE_1(classname) \
	virtual SIMDATA(Object)* createObject() const { \
		SIMDATA(Object) *o = new classname; \
		assert(o); \
		return o; \
	} \
	virtual bool isVirtual() const { return false; }
		
/** interface macro 1 for abstract classes: no createObject
 *
 *  <b><i>For internal use only.</i></b>  See @ref InterfaceMacros for details.
 */
#define __SIMDATA_XML_INTERFACE_V(classname) \
	virtual SIMDATA(Object)* createObject() const { assert(0); return 0; } \
	virtual bool isVirtual() const { return true; }


/** interface macro 2
 *
 *  <b><i>For internal use only.</i></b>  See @ref InterfaceMacros for details.
 */
#define __SIMDATA_XML_INTERFACE_2(classname, baseinterface, nqbaseinterface) \
	virtual SIMDATA(hasht) getClassHash() const { return classname::_getClassHash(); } \
	virtual const char * getClassName() const { return classname::_getClassName(); } \
	virtual SIMDATA(MemberAccessorBase) * getAccessor(const char *name, const char *cname = 0) const { \
		if (!cname) cname = #classname; \
		SIMDATA(MemberAccessorBase) *p = _interface->getAccessor(name); \
		if (p) return p; \
		return nqbaseinterface::getAccessor(name, cname); \
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
	virtual std::string variableType(const char *name) const { \
		if (_interface->variableExists(name)) return _interface->variableType(name); \
		return nqbaseinterface::variableType(name); \
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
	virtual bool isSubclass(std::string const &cname) const { \
		return (cname == #classname) || \
		       nqbaseinterface::isSubclass(cname);\
	} \
	virtual bool isSubclass(SIMDATA(hasht) const &chash) const { \
		return (chash == classname::_getClassHash()) || \
		       nqbaseinterface::isSubclass(chash);\
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
	#define EXTEND_SIMDATA_XML_INTERFACE(classname, basename)
	#define EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(classname, basename)

#else

	/** Begin an interface declaration.
	 *
	 *  See @ref InterfaceMacros for details.
	 */
	#define BEGIN_SIMDATA_XML_INTERFACE(classname) \
		__SIMDATA_XML_INTERFACE_0(classname, SIMDATA(InterfaceProxy)) \
		__SIMDATA_XML_INTERFACE_1(classname) \
		__SIMDATA_XML_INTERFACE_2(classname, SIMDATA(InterfaceProxy), InterfaceProxy)

	/** Begin an interface declaration for an abstract class.
	 *
	 *  See @ref InterfaceMacros for details.
	 */
	#define BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(classname) \
		__SIMDATA_XML_INTERFACE_0(classname, SIMDATA(InterfaceProxy)) \
		__SIMDATA_XML_INTERFACE_V(classname) \
		__SIMDATA_XML_INTERFACE_2(classname, SIMDATA(InterfaceProxy), InterfaceProxy)

	/** Extend an interface declaration.
	 *
	 *  See @ref InterfaceMacros for details.
	 */
	#define EXTEND_SIMDATA_XML_INTERFACE(classname, basename) \
		__SIMDATA_XML_INTERFACE_0(classname, basename::basename##InterfaceProxy) \
		__SIMDATA_XML_INTERFACE_1(classname) \
		__SIMDATA_XML_INTERFACE_2(classname, basename::basename##InterfaceProxy, basename##InterfaceProxy) 

	/** Extend an interface declaration for an abstract class.
	 *
	 *  See @ref InterfaceMacros for details.
	 */
	#define EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(classname, basename) \
		__SIMDATA_XML_INTERFACE_0(classname, basename::basename##InterfaceProxy) \
		__SIMDATA_XML_INTERFACE_V(classname) \
		__SIMDATA_XML_INTERFACE_2(classname, basename::basename##InterfaceProxy, basename##InterfaceProxy)

#endif // SWIG


//-----------------------------------------

/** Declare a member variable accessible to external data sources 
 *
 *  See @ref InterfaceMacros for details.
 */
#ifdef SWIG
	#define SIMDATA_XML(id, var, req)
#else
	#define SIMDATA_XML(id, var, req)       .def(id, &var, req) 
#endif
		
//-----------------------------------------

/** Declare a bit-masked member variable accessible to external data sources 
 *
 *  See @ref InterfaceMacros for details.
 */
#ifdef SWIG
	#define SIMDATA_BIT(id, var, bit, req)
#else
	#define SIMDATA_BIT(id, var, bit, req)  .def(id, &var, bit, req) 
#endif
		
//-----------------------------------------

/** End an object interface declaration.
 *
 *  See @ref InterfaceMacros for details.
 */
#ifdef SWIG
	#define END_SIMDATA_XML_INTERFACE
#else
	#define END_SIMDATA_XML_INTERFACE \
			.pass(); \
			checkDuplicates(); \
		} \
	};
#endif

/** Register an object class interface.  
 *
 *  See @ref InterfaceMacros for details.
 */
#define SIMDATA_REGISTER_INTERFACE(classname) \
namespace { \
	classname::classname##InterfaceProxy __##classname##_interface; \
} /* anonymous namespace */

/** Register an object class interface declared within a named scope. 
 *
 *  See @ref InterfaceMacros for details.
 */
#define SIMDATA_REGISTER_INNER_INTERFACE(prefix, classname) \
namespace { \
	parent::classname::classname##InterfaceProxy __##prefix##_##classname##_interface; \
} /* anonymous namespace */



NAMESPACE_SIMDATA_END


#endif // __SIMDATA_INTERFACEREGISTRY_H__


