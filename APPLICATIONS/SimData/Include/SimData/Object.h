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
#define __LEAKCHECK(a) \
	a() { printf(#a " %p\n", this); } \
	~a() { printf("~" #a " %p\n", this); }

#define __GETCLASSNAME(a) \
	static const char* _getClassName() { return #a; } \
	virtual const char* getClassName() const { return _getClassName(); }

#define __GETCLASSVERSION(major, minor) \
	static const char* _getClassVersion() { return #major "." #minor; } \
	virtual const char* getClassVersion() const { return _getClassVersion(); }
	
#define __GETCLASSHASH(a, major) \
	static SIMDATA(hasht) _getClassHash() { \
		static SIMDATA(hasht) hash = 0; \
		if (hash == 0) hash = _getHash(#a ":" #major); \
		return hash; \
	} \
	virtual SIMDATA(hasht) getClassHash() const { return _getClassHash(); }

#define __CLASSDEF(a, major, minor) \
	__GETCLASSNAME(a) \
	__GETCLASSHASH(a, major) \
	__GETCLASSVERSION(major, minor)

#define __NEW(a) virtual SIMDATA(Object)* _new() const { return new a(); }

// Macro to add standard boilerplate code to object classes.  The
// first parameter is the object class, while the second and third
// are major and minor version numbers.  The class name and version
// numbers are used to test binary compatibility during object 
// deserialization.
#define SIMDATA_OBJECT(a, major, minor)	\
	__CLASSDEF(a, major, minor) \
	__NEW(a)
	
// Macro to automatically register an object class with the 
// object registry.  This macro should be called for each
// object class, preferably at the start of the cpp file
// defining the class.
#define SIMDATA_REGISTER(a) SIMDATA(ObjectProxy)<a> proxy##a;



NAMESPACE_SIMDATA

/**
 * Base class for all classes representing packable data objects.
 *
 * Derived classes must include the SIMDATA_OBJECT(classname, major, minor)
 * macro in their class definition and the SIMDATA_REGISTER(classname) macro 
 * in their implementation.
 * 
 * The following methods must be extended in derived classes:
 * 	pack        serialize object to archive (call superclass method first)
 * 	unpack      unserialize object from archive (call superclass method 
 * 	            first)
 * 	parseXML    parse loose XML cdata if present
 * 	convertXML  post-process XML data
 * 	postCreate  additional processing after deserialization
 *
 * Objects should never be copied or handled directly by user code.  Use 
 * Ref<> handles to manipulate them by reference, and Link<> member variables
 * to load them from data archives.  Objects set as 'static' are singletons 
 * managed by the DataArchive.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */

class SIMDATA_EXPORT Object: public Referenced, public BaseType {
	friend class DataArchive;
	friend class LinkBase;

private:
	// Objects should never be copied
	Object(Object const &) { assert(0); }
	Object const &operator=(Object const &) { assert(0); return *this; }

	void _setPath(hasht);
	
	bool _static;
	hasht _path;

protected:
	/**
	 * Called after the newly created object has been
	 * deserialized by unpack().
	 *
	 * Extend this method to do any initial processing 
	 * of the external data.
	 */
	virtual void postCreate() {}

public:
	explicit Object();
	virtual ~Object();

	/**
	 * Create a new instance of the class.
	 *
	 * The method is automatically overridden by the SIMDATA_OBJECT
	 * macro, so you should never need to extend it manually.
	 */
	virtual Object* _new() const { 
		assert(0); 
		return 0; 
	}

	__CLASSDEF(Object, 0, 0)
		
	/**
	 * Serialize to a data archive.
	 *
	 * Extend this method to serialize member variables specified
	 * by external data.  Be sure to call the base class method
	 * first.  Note that the pack() method must be the exact inverse 
	 * of the unpack() method.
	 */
	virtual void pack(Packer& p) const;

	/**
	 * Deserialize from a data archive.
	 *
	 * Extend this method to deserialize member variables stored
	 * by pack().  Call the base class method first, then unpack
	 * each variable in the exact same order as in the pack()
	 * method.  Any additional processing of the data should be
	 * done in postCreate() method.
	 */
	virtual void unpack(UnPacker& p);

	/**
	 * Get a string representation of the object.
	 */
	virtual std::string asString() const;

	/**
	 * Get the path hash from which the object was instantiated.
	 */
	hasht getPath() const;

	/**
	 * Hash function for converting data archive paths to path
	 * hashes.
	 */
	static hasht _getHash(const char* c);

	/**
	 * Set this instance as static or non-static. 
	 *
	 * Static objects are cached by the DataArchive class so that
	 * only one instance is created.  You should never call this
	 * method directly; it is public only so that SWIG can wrap it.
	 */
	void setStatic(bool);

	/**
	 * Test whether this object is static.
	 */
	bool isStatic() const;
};


NAMESPACE_SIMDATA_END


#endif //__SIMDATA_OBJECT_H__

