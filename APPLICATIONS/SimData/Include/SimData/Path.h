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
 * @file Path.h
 *
 * Classes for refering to data sources.
 */


#ifndef __PATH_H__
#define __PATH_H__

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#pragma warning (disable : 4290)
#endif


#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#include <cassert>
#endif

#include <string>


#include <SimData/HashUtility.h>
#include <SimData/BaseType.h>


NAMESPACE_SIMDATA


class DataArchive;

typedef hasht ObjectID;


SIMDATA_EXCEPTION(ObjectTypeMismatch)



/**
 * BaseType path to a data source.
 *
 * Object paths use '.' to separate path elements, and uniquely 
 * identify objects in a data archive.  Path objects store the
 * path as a 64-bit hash of the path string, called an Object ID.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT Path: public BaseType {
protected:
	ObjectID _path; 
public:

	/**
	 * Create a new Path using a path string.
	 */
	explicit Path(const char* path=0) {
		setPath(path);
	}

	/**
	 * Create a new Path from an Object ID.
	 */
	explicit Path(ObjectID path): _path(path) {
	}

	virtual ~Path() {
	}

	/**
	 * Assign to a specific Object ID.
	 */
	void setPath(ObjectID path) { 
	        _path = path;
	}

	/**
	 * Assign to an object path string (after converting to an 
	 * Object ID).
	 */
	void setPath(const char* path);
	
	/**
	 * Assign to Object ID 0 (no-path).
	 */
	virtual void setNone() {
		setPath((ObjectID) 0);
	}

	/**
	 * Get the path's Object ID.
	 */
	const ObjectID getPath() const {
		return _path;
	}

	/**
	 * Serialize.
	 */
	virtual void pack(Packer& p) const;
	
	/**
	 * Deserialize.
	 */
	virtual void unpack(UnPacker& p);

	/**
	 * Test for 'no-path' (Object ID == 0).
	 */
	bool isNone() const {
		return (_path == (ObjectID) 0);
	}

	bool operator==(Path const &p) const {
		return _path == p._path;
	}

	/**
	 * String representation.
	 */
	virtual std::string asString() const; 
};




class Object;


class SIMDATA_EXPORT ReferencePointer {
public:

	// SWIG python specific comparisons
	bool __eq__(const ReferencePointer& other);
	bool __ne__(const ReferencePointer& other); 
	
	/**
	 * Construct a PathReferencePointer with no path or object reference (null 
	 * pointer)
	 */
	explicit ReferencePointer();

	/**
	 * Assign an object path and bind to a specific object.
	 */
	explicit ReferencePointer(Object* ptr);

	/**
	 * Decrements the object's reference count and destroys the
	 * object if the count reaches zero.
	 */
	virtual ~ReferencePointer();

	/**
	 * Returns true if this is the only reference to an object.
	 */
	bool unique() const;

	/**
	 * Light-weight copy with reference counting.
	 */
	ReferencePointer(const ReferencePointer& r);

	/**
	 * Light-weight copy with reference counting.
	 */
#ifndef SWIG
	ReferencePointer& operator=(const ReferencePointer& r);
#endif

	/**
	 * Raw pointer assignment for NULL only
	 */
#ifndef SWIG
	void *operator=(void *p) {
		assert(p==0);
		_reference = 0;
		return p;
	}
#endif

	/**
	 * Test for null pointer.
	 */
	bool isNull() const { 
		return _reference == 0; 
	}

	/**
	 * Test for null pointer.
	 */
#ifndef SWIG
	bool operator!() const {
		return _reference == 0;
	}
#endif

	/**
	 * Test for non-null pointer.
	 */
	bool valid() const {
		return _reference != 0;
	}
	
	/**
	 * Comparison with other simdata pointers.
	 */
	bool operator==(ReferencePointer const &p) const {
		return _reference == p._reference;
	}

	virtual Object *__get__() { return _reference; }

protected:
	/**
	 * Rebind to a new object.
	 */
	void _assign(Object* ptr);

	/**
	 * Rebind to null.
	 */
	void _release();

	/**
	 * Change object pointer without reference counting.
	 */
	virtual void _update(Object* p);

	/**
	 * Get the current object pointer.
	 */
	Object* _get() const; 

	/**
	 * The actual object pointer.
	 */
	Object* _reference;
};


/**
 * Base class for "smart-pointer" to Objects.
 * 
 * Use this and the template Pointer<> class below for all Object references.
 * This class serves as a generic version of the specialized Pointer template
 * class.  It contains most of the smart-pointer functionality, except 
 * dereferencing to a specific object type.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT PointerBase: public Path, public ReferencePointer {
public:

	// SWIG python specific comparisons
	bool __eq__(const PointerBase& other);
	bool __ne__(const PointerBase& other); 
	
	/**
	 * Construct a PointerBase with no path or object reference (null 
	 * pointer)
	 */
	explicit PointerBase();

	/**
	 * Assign an object path with no referenced object (null pointer)
	 */
	PointerBase(const char* path);

	/**
	 * Assign an object path and bind to a specific object.
	 */
	explicit PointerBase(const Path& path, Object* ptr);
	
	/**
	 * Assign an object reference, but no path.
	 */
	explicit PointerBase(Object* ptr): Path(), ReferencePointer(ptr) {}

	/**
	 * Decrements the object's reference count and destroys the
	 * object if the count reaches zero.
	 */
	virtual ~PointerBase();

	/**
	 * Light-weight copy with reference counting.
	 */
	PointerBase(const PointerBase& r);

	/**
	 * Light-weight copy with reference counting.
	 */
	PointerBase& operator=(const PointerBase& r);

	/**
	 * Pack to a data archive.
	 *
	 * Saves the path, and also saves the referenced object
	 * if the path is 'None'.  Packing a None and Null 
	 * PointerBase is an error.
	 */
	virtual void pack(Packer& p) const;

	/**
	 * Unpack from a data archive.
	 *
	 * Reads the saved path and binds to the correct object.
	 * If the path is None, the object is unpacked from the
	 * subsequent data.  Otherwise the object is created by
	 * asking the current DataArchive to instantiate an
	 * instance of the path.
	 */
	virtual void unpack(UnPacker& p);

	/**
	 * String representation.
	 */
	virtual std::string asString() const;
	
	/**
	 * Comparison with other simdata pointers.
	 */
	bool operator==(PointerBase const &p) const {
		return ReferencePointer::operator==(p);
	}

	/**
	 * Clear both path and pointer (None and Null).
	 */
	virtual void setNone();
	
	/**
	 * Clear pointer but retain path. 
	 */
	virtual void setNull();


protected:
	/**
	 * Create a new object instance from source data.
	 *
	 * @param archive The data archive from which to load the new object's data.
	 * @param path The object path to load.
	 */
	void _load(DataArchive* archive, ObjectID path=0);

};


/**
 * Class-specialized "smart-pointer" to Objects.
 *
 * Use this class as you would an ordinary pointer to refer to all
 * objects descended from simdata::Object.  Given that all objects 
 * should be referred to by Pointers, when creating a new 
 * Pointer you have a couple of options.  You can instantiate the 
 * new Pointer by assigning from an existing Pointer.  This
 * is purely a reference counting operation; no actual copying
 * of the underlying object takes place.  The other primary means
 * of instantiating a Pointer is to unpack it from a data
 * archive.   The archive stores the object path that the Pointer
 * should refer to, and the deserialization process automatically
 * finds (and if need be creates) the appropriate object and binds
 * the Pointer to it.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
template<class T> class Pointer: public PointerBase {
public:

	typedef std::vector< Pointer<T> > vector;

	/**
	 * Create a null Pointer
	 */
	explicit Pointer(): PointerBase() {}
	
	/**
	 * Create a Pointer with both a path and an object
	 */
	explicit Pointer(const Path& path, T* ptr): PointerBase(path, ptr) {}
	
	/**
	 * Create a Pointer with a path but no object (null)
	 */
	explicit Pointer(const char* path): PointerBase(path) {}

	/**
	 * Create a Pointer with an object referenc, but no path
	 */
	explicit Pointer(T* t): PointerBase() {
		*this = t;
	}

	// fast copy
	Pointer(const Pointer<T>& p) { 
		_path = p.getPath();
		_release();
		_reference = p._reference;
		if (!isNull()) _reference->ref();
	}
	
	// safe copy
	Pointer(const PointerBase& p) { 
		PointerBase::operator=(p);
	}
	
	/*
	Pointer<T>& operator=(const PointerBase& p) {
		PointerBase::operator=(p);
	}
	*/

#ifndef SWIG
	/**
	 * Assign a pointer
	 */
	T *operator =(T *t) {
		_path = 0;
		_release();		
		_reference = t;
		if (!isNull()) _reference->ref();
		return t;
	}
#endif // SWIG

	/**
	 * Dereference.
	 */
	T* operator->() { 
		return (T*) _reference;
	}
	
	/**
	 * Const dereference.
	 */
	const T* operator->() const { 
		return (T*) _reference;
	}
	
	/**
	 * Dereference.
	 */
	T& operator*() {
		return *((T*) _reference);
	}
	
	/**
	 * Const dereference.
	 */
	const T& operator*() const {
		return *((T*) _reference);
	}
	
	/**
	 * Const dereference.
	 */
	const T* get() const {
		return (T*) _reference;
	}
	
	/**
	 * Dereference.
	 */
	T* get() {
		return (T*) _reference;
	}
	
protected:
	/**
	 * Change object pointer without reference counting, checking that the
	 * new object type matches the template type.
	 */
	virtual void _update(Object* ptr) throw(ObjectTypeMismatch) {
		PointerBase::_update(ptr);
		T* _special = dynamic_cast<T*>(ptr);
		if (ptr != 0 && _special == 0) 
			throw ObjectTypeMismatch("dynamic_cast<> failed in Pointer<>::_update()");
	}
};



NAMESPACE_END // namespace simdata

#endif //__PATH_H__

