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
 * @file Object.h
 */


#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stdio.h>
#include <assert.h>
#include <string>

#include <SimData/HashUtility.h>
#include <SimData/Pack.h>

class DataArchive;


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
 *
 * Objects should never be copied or handled directly by user code.  Use 
 * Pointer<> or PathPointer<> handles to manipulate them by reference.  
 * Objects set as 'static' are singletons managed by the DataArchive.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */

class SIMDATA_EXPORT Object: public BaseType {
	friend class DataArchive;
public:
	explicit Object();
	virtual ~Object();

	virtual Object* _new() const { 
		assert(0); 
		return 0; 
	}

	// Objects should never be copied
	// Object(const Object& o) { assert(0); }
	Object(const Object& o);
	
	__CLASSDEF(Object, 0, 0)
		
	// archive serializaiton
	virtual void pack(Packer& p) const;
	virtual void unpack(UnPacker& p);

	// static management (don't touch!)
	void setStatic(int s);
	int isStatic() const;
	
#ifndef SWIG
	Object& operator=(const Object& o);
#endif // SWIG

	// reference counting helpers (may move to pointer class)
	unsigned ref() const;
	unsigned deref() const;
	unsigned getCount() const;
	
	virtual std::string asString() const;

	hasht getPath() const;

protected:
	
	static hasht _getHash(const char* c);
	void _setPath(hasht);
	
	int _static;
	mutable unsigned _count;
	hasht _path;

};


NAMESPACE_END // namespace simdata


#endif //__OBJECT_H__

