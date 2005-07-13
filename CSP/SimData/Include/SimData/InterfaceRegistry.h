/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002-2005 Mark Rose <mkrose@users.sf.net>
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
 * @brief Classes for storing and accessing object interfaces.
 */


#ifndef __SIMDATA_INTERFACEREGISTRY_H__
#define __SIMDATA_INTERFACEREGISTRY_H__

#include <string>
#include <vector>
#include <map>

#include <SimData/Export.h>
#include <SimData/HashUtility.h>
#include <SimData/TypeAdapter.h>
#include <SimData/ObjectInterface.h>
#include <SimData/Singleton.h>
#include <SimData/Namespace.h>
#include <SimData/ExceptionBase.h>


NAMESPACE_SIMDATA


class Object;

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
 *  an Object subclass.  ObjectInterface instances, in turn,
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
 *  @author Mark Rose <mkrose@users.sf.net>
 */
class SIMDATA_EXPORT InterfaceProxy {

private:

	typedef std::map<std::string, ObjectInterfaceBase*> InterfaceMap;
	typedef std::vector<ObjectInterfaceBase*> InterfaceList;
	std::vector<std::string> _classNames;
	std::vector<hasht> _classHashes;
	std::vector<std::string> _variableNames;
	std::vector<std::string> _requiredNames;
	InterfaceList _interfaces;
	InterfaceMap _interfacesByVariableName;

	/** Default constructor.
	 *
	 *  This private constructor should not be called and is undefined.
	 */
	InterfaceProxy();

	void globalRegister(const char *classname, hasht const classhash);

protected:

	/** Add an ObjectInterface.
	 *
	 *  Used internally to register ObjectInterface instances.  Each
	 *  InterfaceProxy constructor creates a ObjectInterface instance
	 *  for the variables accessible in the corresponding Object subclass.
	 *  These interfaces are stored in the InterfaceProxy base class and
	 *  used to access all variables in the Object hierarchy.
	 *
	 *  @note Do not call this method directly.
	 *
	 *  @param objectinterface The ObjectInterface to add.
	 *  @param classname The name of the Object subclass corresponding
	 *                   to the interface being added.
	 *  @param classhash The hash of the Object subclass.
	 */
	void addInterface(ObjectInterfaceBase* objectinterface,
	                  std::string const &classname,
	                  hasht const &classhash);

	/// TODO document, replace original addInterface
	void addInterface(ObjectInterfaceBase* objectinterface, bool global_register) {
		addInterface(objectinterface, objectinterface->getClassName(), objectinterface->getClassHash());
		if (global_register) globalRegister(objectinterface->getClassName(), objectinterface->getClassHash());
	}

	/** Find the ObjectInterface corresponding to a given variable name.
	 *
	 *  @param varname The name of the variable to be accessed.
	 *  @param required If true, an exception will be thrown if no interface is
	 *                  found for the specified variable name.
	 *  @returns The interface correcsponding to the specified variable name, or
	 *           0 if no interface is found and required is false.
	 */
	ObjectInterfaceBase *findInterface(std::string const &varname, bool required) const;

	/** Standard constructor for new-style interfaces.  Interface proxies are now
	 *  registered by addInterface after all interface components are defined in
	 *  order to include them in the class hash, so this constructor does nothing.
	 */
	InterfaceProxy(bool global_register) { assert(!global_register); }

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

	/** Create a new instance of the corresponding object.
	 */
	virtual Object *createObject() const;

	/** Get the class hash of the corresponding object.
	 */
	virtual hasht getClassHash() const;
	
	/** Get the class name of the corresponding object.
	 */
	virtual const char *getClassName() const;
	
#ifndef SWIG

	/** Get the value of an interface variable.
	 */
	const TypeAdapter get(Object *o, std::string const &varname) const {
		ObjectInterfaceBase *oi = findInterface(varname, true);
		return oi->get(o, varname);
	}

	/** Set the value of an interface variable.
	 */
	void set(Object *o, std::string const &varname, const TypeAdapter &v) {
		ObjectInterfaceBase *oi = findInterface(varname, true);
		oi->set(o, varname, v);
	}

	/** Append a value to an interface variable list.
	 */
	void push_back(Object *o, std::string const &varname, const TypeAdapter &v) {
		ObjectInterfaceBase *oi = findInterface(varname, true);
		oi->push_back(o, varname, v);
	}

	
#endif // SWIG
	
	/** Assign an Enum value to an object member variable.
	 *
	 *  @param o The object
	 *  @param varname The name of the member variable.
	 *  @param v The (string) value of the Enum.
	 */
	void set_enum(Object *o, std::string const &varname, std::string const &v) {
		set(o, varname, TypeAdapter(v));
	}

	/** Remove all members of an object member variable container.
	 *
	 *  @param o The object
	 *  @param varname The name of the member variable container.
	 */
	void clear(Object *o, std::string const &varname) {
		ObjectInterfaceBase *oi = findInterface(varname, true);
		oi->clear(o, varname);
	}

	/** Test if a member variable is defined in the object interface.
	 */
	bool variableExists(std::string const &varname) const {
		return findInterface(varname, false) != 0;
	}

	/** Test if a member variable in the object interface must be
	 *  assigned a value in XML Object definitions of the object.
	 */
	bool variableRequired(std::string const &varname) const {
		ObjectInterfaceBase *oi = findInterface(varname, true);
		return oi->variableRequired(varname);
	}

	/** Get a string representation of the type of an interface
	 *  variable.
	 */
	std::string variableType(std::string const &varname) const {
		ObjectInterfaceBase *oi = findInterface(varname, true);
		return oi->variableType(varname);
	}

	/** Get the names of all variables in the interface.
	 */
	std::vector<std::string> getVariableNames() const {
		// XXX this should return a const &, but swig currently doesn't
		// convert such references to Python lists, so for now we copy
		// the full list.
		return _variableNames;
	}
	
	/** Get the names of all required variables in the interface.
	 */
	std::vector<std::string> getRequiredNames() const {
		// XXX this should return a const &, but swig currently doesn't
		// convert such references to Python lists, so for now we copy
		// the full list.
		return _requiredNames;
	}

	/** Test if this object interface is a subclass of the specified
	 *  class.
	 *
	 *  This method only checks the interface hierarchy of the object
	 *  classes in question, not the full class hierarchy of the object.
	 *  Non-object base classes will not test true, nor will object
	 *  classes that do not extend the interface.
	 *
	 *  @param cname The name of an object class.
	 *  @returns True if the object class is a superclass.
	 */
	bool isSubclass(std::string const &cname) const;

	/** Test if this object interface is a subclass of the specified
	 *  class.
	 *
	 *  This method is the same as
	 *      <tt>isSubclass(std::string const &)</tt>,
	 *  but takes an object class hash instead of the class name.
	 *
	 *  @param chash The class hash of an object class.
	 *  @returns True if the object class is a superclass.
	 */
	bool isSubclass(hasht const &chash) const;

	/** Test if the object class of this interface is static.
	 *
	 *  Instances of static object classes are cached when loaded
	 *  from a data archive and shared by all users of the object.
	 *  See Object.h for details.
	 */
	virtual bool isStatic() const { return false; }


/////////////////////////////////////////////////////// SWIG
#ifdef SWIG
public:
%extend {
	/*
	virtual void set(Object *p, std::string const &varname, int v) {
		self->set(p, varname, TypeAdapter(v));
	}
	virtual void set(Object *p, std::string const &varname, double v) {
		self->set(p, varname, TypeAdapter(v));
	}
	virtual void set(Object *p, std::string const &varname, std::string const &v) {
		self->set(p, varname, TypeAdapter(v));
	}
	*/
	virtual void _set(Object *p, std::string const &varname, TypeAdapter const &v) {
		self->set(p, varname, v);
	}
	/*
	virtual void push_back(Object *p, std::string const &varname, std::string const &v) {
		self->push_back(p, varname, TypeAdapter(v));
	}
	virtual void push_back(Object *p, std::string const &varname, int v) {
		self->push_back(p, varname, TypeAdapter(v));
	}
	virtual void push_back(Object *p, std::string const &varname, double v) {
		self->push_back(p, varname, TypeAdapter(v));
	}
	*/
	virtual void _push_back(Object *p, std::string const &varname, const TypeAdapter &v) {
		self->push_back(p, varname, v);
	}
}
%insert("shadow") %{
	def set(self, obj, name, value):
		self._set(obj, name, TypeAdapter(value))
	def push_back(self, obj, name, value):
		self._push_back(obj, name, TypeAdapter(value))
%}
#endif // SWIG

};



/** Singleton class to store and access all ObjectInterfaces in the application.
 *
 *  ObjectInterfaces automatically register themselves with the global instance of this
 *  class at startup.  Interfaces can be accessed by name to provide dynamic object
 *  creation and variable assignment.  See @ref InterfaceDetails for details.
 *
 *  @author Mark Rose <mkrose@users.sf.net>
 */
class InterfaceRegistry: public Singleton<InterfaceRegistry> {
	
friend class Singleton<InterfaceRegistry>;
friend class InterfaceProxy;

public:
	typedef std::vector<InterfaceProxy *> interface_list;

	/** Get an object interface by object class name.
	 *
	 *  @returns 0 if the interface is not found.
	 */
	InterfaceProxy *getInterface(const char *name);
	
	/** Get an object interface by object class hash.
	 *
	 *  @returns 0 if the interface is not found.
	 */
	InterfaceProxy *getInterface(hasht key);
	
	/** Test if an object interface is registered.
	 *
	 *  @param name The object class name.
	 */
	bool hasInterface(const char *name);
	
	/** Test if an object interface is registered.
	 *
	 *  @param key The object class hash.
	 */
	bool hasInterface(hasht key);
	
	/** Get a list of all object class names in the registry.
	 */
	std::vector<std::string> getInterfaceNames() const;

	/** Get a list of all interfaces in the registry.
	 */
	std::vector<InterfaceProxy *> getInterfaces() const;
	
	/** Get the interface registry singleton.
	 */
	static InterfaceRegistry &getInterfaceRegistry() {
		return getInstance();
	}

private:

	virtual ~InterfaceRegistry();

	/** Add an interface to the registry.
	 *
	 *  Interfaces are registered automatically by the
	 *  SIMDATA_*_INTERFACE macros.
	 */
	void addInterface(const char *name, hasht id, InterfaceProxy *proxy);

	//friend class Singleton<InterfaceRegistry>;
	InterfaceRegistry();
	
	typedef HASH_MAPS<const char*, InterfaceProxy*, HASH<const char*>, eqstr>::Type proxy_map;
	typedef HASHT_MAP<InterfaceProxy*>::Type proxy_id_map;

	proxy_map __map;
	proxy_id_map __id_map;
	interface_list __list;

};


/** @page InterfaceMacros Object Interface Macros
 *
 *  @c SIMDATA_XML_INTERFACE(class)
 *  @c SIMDATA_DEF
 *  @c SIMDATA_XML_END
 *
 */

// Defined in LogStream.cpp
extern void SIMDATA_EXPORT fatal(std::string const &msg);


/** Helper template for creating new Object instances.  Partially specialized on the
 *  ABSTRACT template argument to prevent construction of abstract classes.
 */
template<class OBJECT, bool ABSTRACT=false>
struct __simdata_object_factory {
	static inline Object *create() {
		Object *obj = new OBJECT; assert(obj);
		return obj;
	}
};

template <class OBJECT>
struct __simdata_object_factory<OBJECT, true> {
	static inline Object *create() { fatal(std::string("Attempt to construct ABSTRACT Object of type ") + OBJECT::_getClassName()); return 0; }
};


#ifndef SWIG
/** Constructs the class hash by combining the class name and the xml interface signature.
 */
inline hasht classhash_helper(std::string const &class_name, const fprint32 signature) {
	return hasht(hash_uint32(class_name), signature);
}
#endif


#define __SIMDATA_CLASS_DEFINE(_M_classname, _M_static, _M_abstract) \
	static const char *_getClassName() { return #_M_classname; } \
	virtual const char *getClassName() { return _getClassName(); } \
	static const char *_getClassVersion() { return "99.99"; } \
	virtual const char *getClassVersion() { return _getClassVersion(); } \
	static SIMDATA(hasht) _getClassHash(); \
	virtual SIMDATA(hasht) getClassHash() const { return _getClassHash(); } \
	static bool _isClassStatic() { return _M_static; } \
	virtual bool isClassStatic() { return _isClassStatic(); } \
	static bool _isClassAbstract() { return _M_abstract; } \
	typedef __simdata_object_class __simdata_object_baseclass; \
	typedef _M_classname __simdata_object_class; \
	virtual SIMDATA(Object) *_new() const { return SIMDATA(__simdata_object_factory)<_M_classname, _M_abstract>::create(); } \
	friend class __simdata_interface_proxy_##_M_classname; \
	class __simdata_interface_proxy_##_M_classname: public __simdata_object_baseclass::__simdata_interface_proxy { \
		static SIMDATA(ObjectInterface)<_M_classname> *_interface; \
	public: \
		static SIMDATA(fprint32) signature(); \
		static void serialize(_M_classname *object, SIMDATA(Reader) &reader); \
		static void serialize(_M_classname const *object, SIMDATA(Writer) &writer); \
		virtual SIMDATA(Object)* createObject() const { return SIMDATA(__simdata_object_factory)<_M_classname, _M_abstract>::create(); } \
		virtual bool isVirtual() const { return _M_abstract; } \
		virtual bool isStatic() const { return _M_static; } \
		virtual SIMDATA(hasht) getClassHash() const { return _M_classname::_getClassHash(); } \
		virtual const char * getClassName() const { return _M_classname::_getClassName(); } \
		__simdata_interface_proxy_##_M_classname(bool=true); \
	}; \
	typedef __simdata_interface_proxy_##_M_classname __simdata_interface_proxy; \
	virtual void _serialize(SIMDATA(Writer) &writer) const; \
	virtual void _serialize(SIMDATA(Reader) &reader);

#define SIMDATA_DECLARE_OBJECT(_M_classname) \
	__SIMDATA_CLASS_DEFINE(_M_classname, false, false)

#define SIMDATA_DECLARE_STATIC_OBJECT(_M_classname) \
	__SIMDATA_CLASS_DEFINE(_M_classname, true, false)

#define SIMDATA_DECLARE_ABSTRACT_OBJECT(_M_classname) \
	__SIMDATA_CLASS_DEFINE(_M_classname, false, true)


#define SIMDATA_XML_BEGIN(_M_classname) \
	void _M_classname::_serialize(SIMDATA(Writer) &writer) const { \
		__simdata_object_baseclass::_serialize(writer); \
		__simdata_interface_proxy::serialize(this, writer); \
	} \
	void _M_classname::_serialize(SIMDATA(Reader) &reader) { \
		__simdata_object_baseclass::_serialize(reader); \
		__simdata_interface_proxy::serialize(this, reader); \
	} \
	SIMDATA(hasht) _M_classname::_getClassHash() { \
		static SIMDATA(hasht) hash = 0; /*defer evaluation*/\
		if (hash == 0) hash = SIMDATA(classhash_helper)(#_M_classname, __simdata_interface_proxy::signature()); \
		return hash; \
	} \
	SIMDATA(ObjectInterface)<_M_classname> *_M_classname::__simdata_interface_proxy_##_M_classname::_interface = 0; \
	namespace { _M_classname::__simdata_interface_proxy_##_M_classname __##_M_classname##_interface_proxy; } \
	SIMDATA(fprint32) _M_classname::__simdata_interface_proxy_##_M_classname::signature() { return _interface->signature(); } \
	void _M_classname::__simdata_interface_proxy_##_M_classname::serialize(_M_classname *object, SIMDATA(Reader) &reader) { _interface->serialize(object, reader); } \
	void _M_classname::__simdata_interface_proxy_##_M_classname::serialize(_M_classname const *object, SIMDATA(Writer) &writer) { _interface->serialize(object, writer); } \
	_M_classname::__simdata_interface_proxy_##_M_classname::__simdata_interface_proxy_##_M_classname(const bool reg) \
		: _M_classname::__simdata_object_baseclass::__simdata_interface_proxy(false) { \
		typedef _M_classname _class; \
		if (!_interface) { \
			_interface = new SIMDATA(ObjectInterface)<_M_classname>; \
			(*_interface)

#define SIMDATA_DEF(id, var, req)  .def(id, &_class::var, req)
#define SIMDATA_DEF_BIT(id, var, bit, req)  .def(id, &_class::var, bit, req)

#define SIMDATA_XML_END \
			.pass(); \
		} \
		addInterface(_interface, reg); \
	}; \


/////////////////////////////////////////////////////////////////////////////////////
// DEPRECATED INTERFACE MACROS
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SWIG
struct SIMDATA_EXPORT DeprecationWarning { DeprecationWarning(const char *message); };
#endif

/** interface macro 0
 *
 *  <b><i>For internal use only.</i></b>  See @ref InterfaceMacros for details.
 *  @DEPRECATED
 */
#define __SIMDATA_XML_INTERFACE_0(classname, baseinterface) \
	class classname##InterfaceProxy; \
	friend class classname##InterfaceProxy; \
	typedef classname##InterfaceProxy _LocalInterfaceProxy; \
	class classname##InterfaceProxy: public baseinterface \
	{ \
		static SIMDATA(ObjectInterface)<classname> *_interface; \
	public: \
		static inline void serialize(classname *object, SIMDATA(Reader) &reader) { \
			_interface->serialize(object, reader); \
		} \
		static inline void serialize(classname const *object, SIMDATA(Writer) &writer) { \
			_interface->serialize(object, writer); \
		}

/** interface macro 1 for normal classes
 *
 *  <b><i>For internal use only.</i></b>  See @ref InterfaceMacros for details.
 *  @DEPRECATED
 */
#define __SIMDATA_XML_INTERFACE_1(classname) \
	virtual SIMDATA(Object)* createObject() const { \
		SIMDATA(Object) *o = new classname; \
		assert(o); \
		return o; \
	} \
	virtual bool isVirtual() const { return false; } \
	virtual bool isStatic() const { return classname::_isClassStatic(); }
		
/** interface macro 1 for abstract classes: no createObject
 *
 *  <b><i>For internal use only.</i></b>  See @ref InterfaceMacros for details.
 *  @DEPRECATED
 */
#define __SIMDATA_XML_INTERFACE_V(classname) \
	virtual SIMDATA(Object)* createObject() const { assert(0); return 0; } \
	virtual bool isVirtual() const { return true; }


/** interface macro 2
 *
 *  <b><i>For internal use only.</i></b>  See @ref InterfaceMacros for details.
 *  @DEPRECATED
 */
#define __SIMDATA_XML_INTERFACE_2(classname, baseinterface, nqbaseinterface) \
	virtual SIMDATA(hasht) getClassHash() const { return classname::_getClassHash(); } \
	virtual const char * getClassName() const { return classname::_getClassName(); } \
	classname##InterfaceProxy(const char * cname = #classname, SIMDATA(hasht) (*chash)() = &classname::_getClassHash): \
		baseinterface(cname, chash) \
	{ \
		std::string _classname = #classname; \
		SIMDATA(hasht) _classhash = classname::_getClassHash(); \
		if (!_interface) { \
			_interface = new SIMDATA(ObjectInterface)<classname>; \
			(*_interface)

//-----------------------------------------

#define __SIMDATA_XML_SERIALIZE_INTERFACE \
		virtual void _serialize(SIMDATA(Writer) &writer) const { \
			_base_serialize(writer); \
			_LocalInterfaceProxy::serialize(this, writer); \
		} \
		virtual void _serialize(SIMDATA(Reader) &reader) { \
			_base_serialize(reader); \
			_LocalInterfaceProxy::serialize(this, reader); \
		}

#define __SIMDATA_XML_BASE_SERIALIZE \
		inline void _base_serialize(SIMDATA(Writer) &) const { } \
		inline void _base_serialize(SIMDATA(Reader) &) { } \

#define __SIMDATA_XML_BASE_SERIALIZE_EXTEND(basename) \
		inline void _base_serialize(SIMDATA(Writer) &writer) const { basename::_serialize(writer); } \
		inline void _base_serialize(SIMDATA(Reader) &reader) { basename::_serialize(reader); }

#ifdef SWIG

#define BEGIN_SIMDATA_XML_INTERFACE(classname)
#define BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(classname)
#define EXTEND_SIMDATA_XML_INTERFACE(classname, basename)
#define EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(classname, basename)

#else

/** Begin an interface declaration.
 *
 *  See @ref InterfaceMacros for details.
 *  @DEPRECATED
 */
#define BEGIN_SIMDATA_XML_INTERFACE(classname) \
	__SIMDATA_XML_BASE_SERIALIZE \
	__SIMDATA_XML_INTERFACE_0(classname, SIMDATA(InterfaceProxy)) \
	__SIMDATA_XML_INTERFACE_1(classname) \
	__SIMDATA_XML_INTERFACE_2(classname, SIMDATA(InterfaceProxy), InterfaceProxy)

/** Begin an interface declaration for an abstract class.
 *
 *  See @ref InterfaceMacros for details.
 *  @DEPRECATED
 */
#define BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(classname) \
	__SIMDATA_XML_BASE_SERIALIZE \
	__SIMDATA_XML_INTERFACE_0(classname, SIMDATA(InterfaceProxy)) \
	__SIMDATA_XML_INTERFACE_V(classname) \
	__SIMDATA_XML_INTERFACE_2(classname, SIMDATA(InterfaceProxy), InterfaceProxy)

/** Extend an interface declaration.
 *
 *  See @ref InterfaceMacros for details.
 *  @DEPRECATED
 */
#define EXTEND_SIMDATA_XML_INTERFACE(classname, basename) \
	__SIMDATA_XML_BASE_SERIALIZE_EXTEND(basename) \
	__SIMDATA_XML_INTERFACE_0(classname, basename::basename##InterfaceProxy) \
	__SIMDATA_XML_INTERFACE_1(classname) \
	__SIMDATA_XML_INTERFACE_2(classname, basename::basename##InterfaceProxy, basename##InterfaceProxy)

/** Extend an interface declaration for an abstract class.
 *
 *  See @ref InterfaceMacros for details.
 *  @DEPRECATED
 */
#define EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(classname, basename) \
	__SIMDATA_XML_BASE_SERIALIZE_EXTEND(basename) \
	__SIMDATA_XML_INTERFACE_0(classname, basename::basename##InterfaceProxy) \
	__SIMDATA_XML_INTERFACE_V(classname) \
	__SIMDATA_XML_INTERFACE_2(classname, basename::basename##InterfaceProxy, basename##InterfaceProxy)

#endif // SWIG


//-----------------------------------------

/** Declare a member variable accessible to external data sources
 *
 *  See @ref InterfaceMacros for details.
 *  @DEPRECATED
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
 *  @DEPRECATED
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
 *  @DEPRECATED
 */
#ifdef SWIG
	#define END_SIMDATA_XML_INTERFACE
#else
	#define END_SIMDATA_XML_INTERFACE \
				.pass(); \
			} \
			addInterface(_interface, _classname, _classhash); \
		} \
	}; \
	__SIMDATA_XML_SERIALIZE_INTERFACE
#endif


/** Register an object class interface.
 *
 *  See @ref InterfaceMacros for details.
 *  @DEPRECATED
 */
#define SIMDATA_REGISTER_INTERFACE(classname) \
	SIMDATA(ObjectInterface)<classname> *classname::classname##InterfaceProxy::_interface = 0; \
namespace { \
	SIMDATA(DeprecationWarning) __simdata_warn_##classname("WARNING: Using DEPRECATED xml interface macros for " #classname "; see http://csp.sf.net/wiki/DeprecatedMacros") ;\
	classname::classname##InterfaceProxy __##classname##_interface; \
} /* anonymous namespace */

/** Register an object class interface declared within a named scope.
 *
 *  See @ref InterfaceMacros for details.
 *  @DEPRECATED
 */
#define SIMDATA_REGISTER_INNER_INTERFACE(prefix, classname) \
	SIMDATA(ObjectInterface)<prefix::classname> *prefix::classname::classname##InterfaceProxy::_interface = 0; \
namespace { \
	SIMDATA(DeprecationWarning) __simdata_warn_##prefix##_##classname("WARNING: Using DEPRECATED xml interface macros for " #prefix #classname "; see http://csp.sf.net/wiki/DeprecatedMacros") ;\
	prefix::classname::classname##InterfaceProxy __##prefix##_##classname##_interface; \
} /* anonymous namespace */


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_INTERFACEREGISTRY_H__

