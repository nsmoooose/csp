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
 * @file Object.h
 *
 * Object class and related boilerplate macros.
 */


#ifndef __SIMDATA_OBJECT_H__
#define __SIMDATA_OBJECT_H__


#include <cstdio>
#include <cassert>
#include <string>

#include <SimData/HashUtility.h>
#include <SimData/BaseType.h>
#include <SimData/Export.h>
#include <SimData/Ref.h>


class DataArchive;
class LinkBase;


// Various object macros for internal use only.
#define __SIMDATA_LEAKCHECK(a) \
	a() { printf(#a " %p\n", this); } \
	~a() { printf("~" #a " %p\n", this); }

#define __SIMDATA_GETCLASSNAME(a) \
	static const char* _getClassName() { return #a; } \
	virtual const char* getClassName() const { return _getClassName(); }

#define __SIMDATA_GETCLASSVERSION(major, minor) \
	static const char* _getClassVersion() { return #major "." #minor; } \
	virtual const char* getClassVersion() const { return _getClassVersion(); }
	
#define __SIMDATA_GETCLASSHASH(a, major) \
	static SIMDATA(hasht) _getClassHash() { \
		static SIMDATA(hasht) hash = 0; \
		if (hash == 0) hash = _getHash(#a ":" #major); \
		return hash; \
	} \
	virtual SIMDATA(hasht) getClassHash() const { return _getClassHash(); }

#define __SIMDATA_ISSTATIC(static_) \
	static bool _isClassStatic() { return static_; } \
	static bool isClassStatic() { return _isClassStatic(); }

#define __SIMDATA_CLASSDEF(a, major, minor) \
	__SIMDATA_GETCLASSNAME(a) \
	__SIMDATA_GETCLASSHASH(a, major) \
	__SIMDATA_GETCLASSVERSION(major, minor) \

#define __SIMDATA_NEW(a) virtual SIMDATA(Object)* _new() const { return new a(); }

/** Declare a SimData Object subclass
 *
 *  This macro adds standard boilerplate code to object classes.  The
 *  first parameter is the object class, while the second and third
 *  are major and minor version numbers.  The class name and version
 *  numbers are used to test binary compatibility during object 
 *  deserialization.  This version of the macro declares a non-static
 *  object, meaning that new instances will be created each time a
 *  particular object of this class is loaded from an archive.  To 
 *  share one instance, see SIMDATA_STATIC_OBJECT.
 */
#define SIMDATA_OBJECT(a, major, minor)	\
	__SIMDATA_CLASSDEF(a, major, minor) \
	__SIMDATA_ISSTATIC(false) \
	__SIMDATA_NEW(a)

/** Declare a SimData Object static subclass.
 *
 *  This macro is similar to SIMDATA_OBJECT, but declares this object
 *  class to be "static".  Static object classes are cached by the
 *  archive loader, so that at most one instance of the object exists.
 *  Multiple attempts to load a static object from an archive will
 *  result in references pointing to a shared object instance.  This
 *  is particularly useful for object classes that do not contain
 *  dynamic data.
 */
#define SIMDATA_STATIC_OBJECT(a, major, minor)	\
	__SIMDATA_CLASSDEF(a, major, minor) \
	__SIMDATA_ISSTATIC(true) \
	__SIMDATA_NEW(a)
	
// Macro to automatically register an object class with the 
// object registry.  This macro should be called for each
// object class, preferably at the start of the cpp file
// defining the class.
#define SIMDATA_REGISTER(a) SIMDATA(ObjectProxy)<a> proxy##a;



NAMESPACE_SIMDATA

/** Base class for all classes representing packable data objects.
 *
 *  Derived classes must include the SIMDATA_OBJECT(classname, major, minor)
 *  macro in their class definition and the SIMDATA_REGISTER(classname) macro 
 *  in their implementation.
 * 
 *  The following methods must be extended in derived classes:
 *  @li @c pack        serialize object to archive (call superclass method 
 *                     first)
 *  @li @c unpack      unserialize object from archive (call superclass method 
 *  	               first)
 *  @li @c parseXML    parse loose XML cdata if present
 *  @li @c convertXML  post-process XML data
 *  @li @c postCreate  additional processing after deserialization
 *
 *  Objects should never be copied or handled directly by user code.  Use 
 *  Ref<> handles to manipulate them by reference, and Link<> member variables
 *  to load them from data archives.  Objects set as 'static' are singletons 
 *  managed by the DataArchive.
 *
 *  @author Mark Rose <mrose@stm.lbl.gov>
 *  @ingroup BaseTypes
 */
class SIMDATA_EXPORT Object: public virtual Referenced, public BaseType {
	friend class DataArchive;
	friend class LinkBase;

private:
	// Objects should never be copied
	Object(Object const &o);
	Object const &operator=(Object const &);

	void _setPath(hasht);
	
	hasht _path;

protected:
	/** Initialize an object after deserialization.
	 *
	 *  Called after the newly created object has been
	 *  deserialized.
	 *
	 *  Extend this method to do any initial processing 
	 *  of the external data.
	 */
	virtual void postCreate() {}

public:
	explicit Object();
	virtual ~Object();

	/** Create a new instance of the class.
	 *
	 *  The method is automatically overridden by the SIMDATA_OBJECT
	 *  macro, so you should never need to extend it manually.
	 */
	virtual Object* _new() const { 
		assert(0); 
		return 0; 
	}

	__SIMDATA_CLASSDEF(Object, 0, 0)
		
	/** Serialize an object to or from a data archive 
	 *
	 *  Extend this method to serialize member variables to and
	 *  from data archives.  Call the base class method first, then 
	 *  apply the archive functor to each archived variable.
	 *  Any additional processing of the data following retrieval
	 *  should be done in the postCreate() method.
	 */
	virtual void serialize(Archive& archive);

	/** Get a string representation of the object.
	 *
	 *  This method is also used for writing to ostreams
	 *  using operator<<().
	 */
	virtual std::string asString() const;

	/** Return a string representation of the type.
	 *
	 *  The type string for classes derived from Object
	 *  is just the classname.  The implementation in
	 *  Object returns the true classname using getClassName(),
	 *  so this method should not be overridden in derived
	 *  classes.
	 */
	virtual std::string typeString() const { return getClassName(); }

	/** Get the path hash from which the object was instantiated.
	 *
	 *  Given the source DataArchive, this path hash can be 
	 *  converted back to a path string using getPathString().
	 */
	hasht getPath() const;

	/** Hash function for converting data archive paths to path hashes.
	 */
	static hasht _getHash(const char* c);

};


NAMESPACE_SIMDATA_END


#endif //__SIMDATA_OBJECT_H__

