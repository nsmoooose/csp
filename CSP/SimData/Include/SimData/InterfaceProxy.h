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
 * @file InterfaceProxy.h
 * @brief Classes for storing and accessing object interfaces.
 */


#ifndef __SIMDATA_INTERFACEPROXY_H__
#define __SIMDATA_INTERFACEPROXY_H__

#include <string>
#include <vector>
#include <map>

#include <SimData/Export.h>
#include <SimData/HashUtility.h>
#include <SimData/Namespace.h>


NAMESPACE_SIMDATA


class Object;
class ObjectInterfaceBase;
template <class T> class ObjectInterface;
class TypeAdapter;


/** @page InterfaceDetails Object Interfaces and Interface Proxies
 *
 *  Classes derived from simdata::Object can define interfaces that allow
 *  runtime introspection and assignment of specified member variables.  Such
 *  interfaces are used by the data compiler to bind XML data to class
 *  instances, which are then serialized to create a binary data archive.
 *  Objects can later be deserialized from the archive to create instances that
 *  are preloaded with the data originally specified in XML.
 *
 *  The ObjectInterface class is the primary, internal interface to an Object
 *  subclass.  ObjectInterface instances, in turn, rely on the
 *  MemberAccessorBase class and its derivatives to access individual member
 *  variables.  None of these classes are created directly by the user.
 *  Instead, a series of macros are used in the declaration of any class
 *  derived from simdata::Object to declare an InterfaceProxy class.  See @ref
 *  InterfaceMacros for a detailed discussion of these macros.  The net effect
 *  is to create an inner class derived from InterfaceProxy, of which a single
 *  instance is created.  This instance is created during static construction,
 *  and automatically registers itself with the SimData InterfaceRegistry.  Any
 *  registered InterfaceProxy can be accessed through the registry by
 *  specifying the corresponding class name.
 *
 *  The InterfaceProxy classes act as both object factories and member
 *  accessors. New instances of the corresponding Object classes can be created
 *  dynamically, and data members can be read and set by using string
 *  identifiers specified in the macros used to define the interface.  Code
 *  using an InterfaceProxy needs to know nothing in advance about the
 *  corresponding object class.  The interface provides a list of names of all
 *  accessible member variables, the data type of each variable, and methods to
 *  get and set the value of each member variable.
 *
 *  Structurally, when you use the macros provided to declare an InterfaceProxy
 *  for an Object class, you are defining a new class derived from
 *  InterfaceProxy.  That class contains an ObjectInterface member variable,
 *  and each of the macros such as SIMDATA_XML(name, member, required) adds a
 *  new MemberAccessor instance to this interface.  Most of the methods of
 *  InterfaceProxy are just thin wrappers around calls to the underlying
 *  ObjectInterface.  In the case of inherited interfaces, however, calls to
 *  InterfaceProxy methods may propogate up the inheritance chain until an
 *  ObjectInterface is found to satisfy the request.  If no matching interface
 *  is found, the base class InterfaceProxy defines default implementations
 *  (such as throwing an exception).
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
	 *  InterfaceProxy constructor creates a ObjectInterface instance for the
	 *  variables accessible in the corresponding Object subclass.  These
	 *  interfaces are stored in the InterfaceProxy base class and used to
	 *  access all variables in the Object hierarchy.
	 *
	 *  @note Do not call this method directly.
	 *
	 *  @param objectinterface The ObjectInterface to add.
	 *  @param global_register True only for object interface corresponding
	 *    to the class being declared.  False for all subclass interfaces.
	 */
	void addInterface(ObjectInterfaceBase* objectinterface, bool global_register);

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
	InterfaceProxy(bool global_register) { if (global_register) fatal("Attempt to register base InterfaceProxy"); }

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
	const TypeAdapter get(Object *o, std::string const &varname) const;

	/** Set the value of an interface variable.
	 */
	void set(Object *o, std::string const &varname, const TypeAdapter &v);

	/** Append a value to an interface variable list.
	 */
	void push_back(Object *o, std::string const &varname, const TypeAdapter &v);

#endif // SWIG
	
	/** Assign an Enum value to an object member variable.
	 *
	 *  @param o The object
	 *  @param varname The name of the member variable.
	 *  @param v The (string) value of the Enum.
	 */
	void set_enum(Object *o, std::string const &varname, std::string const &v);

	/** Remove all members of an object member variable container.
	 *
	 *  @param o The object
	 *  @param varname The name of the member variable container.
	 */
	void clear(Object *o, std::string const &varname);

	/** Test if a member variable is defined in the object interface.
	 */
	bool variableExists(std::string const &varname) const {
		return findInterface(varname, false) != 0;
	}

	/** Test if a member variable in the object interface must be
	 *  assigned a value in XML Object definitions of the object.
	 */
	bool variableRequired(std::string const &varname) const;

	/** Get a string representation of the type of an interface
	 *  variable.
	 */
	std::string variableType(std::string const &varname) const;

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

	/** Test if this object interface is a subclass of the specified class.
	 *
	 *  This method only checks the interface hierarchy of the object classes
	 *  in question, not the full class hierarchy of the object.  Non-object
	 *  base classes will not test true, nor will object classes that do not
	 *  extend the interface.
	 *
	 *  @param cname The name of an object class.
	 *  @returns True if the object class is a superclass.
	 */
	bool isSubclass(std::string const &cname) const;

	/** Test if this object interface is a subclass of the specified class.
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
	 *  Instances of static object classes are cached when loaded from a data
	 *  archive and shared by all users of the object.  See Object.h for
	 *  details.
	 */
	virtual bool isStatic() const { return false; }


/////////////////////////////////////////////////////// SWIG
#ifdef SWIG
public:
%extend {
	virtual void _set(Object *p, std::string const &varname, TypeAdapter const &v) {
		self->set(p, varname, v);
	}
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


/** @page InterfaceMacros Object Interface Macros
 *
 *  SimData relies on several macros to declare objects and define interfaces to
 *  external data.  These macros add common methods to all Object subclasses and
 *  create an introspection and serialization framework.  Together with a global
 *  registry of objects, these frameworks allow objects to manipulated dynamically
 *  at runtime.  For instance, one can query the registry for the names of all
 *  object classes and then instantiate specific classes by name.  Using the
 *  introspection framework all the externalized data variables of the object
 *  hierarchy can be determined and accessed dynamically.
 *
 *  This facility is at the heart of the data model, whereby external data stored
 *  in XML format is "compiled" by dynamically loading it into the corresponding
 *  object instances and then serializing these objects to a binary data archive.
 *  At runtime the objects can be efficiently deserialized from the archive so that
 *  they are initialized with the values originally specified in XML files.
 *
 *  Each object subclass that defines external variables must include one of the
 *  following three macros in the public section of the class declaration:
 *
 *  @c SIMDATA_DECLARE_OBJECT(classname)
 *  @c SIMDATA_DECLARE_STATIC_OBJECT(classname)
 *  @c SIMDATA_DECLARE_ABSTRACT_OBJECT(classname)
 *
 *  The first version is used for objects that may be instantiated and dynamically
 *  updated at runtime.  The second version is similar, but is intended for objects
 *  that store static data.  Multiple instantiations of such objects will typically
 *  return references to a single shared object.  The library does not guarantee
 *  thread safety for these shared objects, and in fact does not even guarantee
 *  that only a single copy will be created.  Thus it is recommended that static
 *  objects provide only const access to internal data and keep no dynamic state.
 *  The last macro is used for abstract base classes that cannot be instantiated.
 *
 *  If any of the SIMDATA_DECLARE_ macros is included in an object class, the
 *  object must define an XML interface in the corresponding source file (outside
 *  of the class declaration and not in a header file).  The format of the XML
 *  interface is as follows:
 *
 *  @c SIMDATA_XML_BEGIN(classname)
 *  @c   SIMDATA_DEF(name, var, is_required)
 *  @c   ...
 *  @c SIMDATA_XML_END
 *
 *  The declaration block begins with the @c SIMDATA_XML_BEGIN macro and end with
 *  the @c SIMDATA_XML_END macro.  Any number of @c SIMDATA_DEF macros may be included
 *  in the block.  Do not place semicolons after any of these macros.  Only the
 *  immediate member variables of the class need to be declared in the interface;
 *  subclass and superclass interfaces will be chained automatically.
 *
 *  The @c SIMDATA_DEF macro takes three parameters.  The first is a string that
 *  gives the name attribute used to refer the variable in XML data files and through
 *  the runtime introspection interface.  The second is the actual member variable
 *  without the class prefix.  The last is a boolean that determines whether the
 *  variable is required in XML object definitions.  For example:
 *
 *  @code
 *  // myclass.h
 *  #include <SimData/Object.h>
 *  class MyClass: public simdata::Object {
 *  public:
 *   SIMDATA_DECLARE_OBJECT(MyClass)
 *  private:
 *   int m_Foo;
 *   simdata::Key m_Bar;
 *  };
 *
 *  // myclass.cpp
 *  #include "myclass.h"
 *  #include <SimData/ObjectInterface.h>
 *  SIMDATA_XML_BEGIN(MyClass)
 *    SIMDATA_DEF("foo", m_Foo, true)
 *    SIMDATA_DEF("bar", m_Bar, false)
 *  SIMDATA_XML_END
 *
 *  // myobject.xml
 *  <Object class="MyClass">
 *    <Int name="foo">42</Int>
 *    <Key name="bar">Life, the Universe and Everything</Key>
 *  </Object>
 *  @endcode
 *
 */


/** Helper template for creating new Object instances.  Partially specialized on the
 *  ABSTRACT template argument to prevent construction of abstract classes.
 */
template<class OBJECT, bool ABSTRACT=false>
struct __simdata_object_factory {
	static inline Object *create() {
		Object *obj = new OBJECT;
		if (!obj) fatal(std::string("Failed to instantiate ") + OBJECT::_getClassName());
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
	virtual const char *getClassName() const { return _getClassName(); } \
	static SIMDATA(hasht) _getClassHash(); \
	virtual SIMDATA(hasht) getClassHash() const { return _getClassHash(); } \
	static bool _isClassStatic() { return _M_static; } \
	virtual bool isClassStatic() const { return _isClassStatic(); } \
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
	};


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_INTERFACEPROXY_H__

