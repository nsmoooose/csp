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
 * @file Link.h
 *
 */


#ifndef __SIMDATA_LINK_H__
#define __SIMDATA_LINK_H__


#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#pragma warning (disable : 4290)
#endif


#include <SimData/Export.h>
#include <SimData/Path.h>
#include <SimData/Object.h>

#include <string>
#include <cassert>



NAMESPACE_SIMDATA


class DataArchive;


/**
 * @brief Base class for smart-pointers to Object classes.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT ReferencePointer {
public:

	// SWIG python specific comparisons
	bool __eq__(const ReferencePointer& other);
	bool __ne__(const ReferencePointer& other); 
	
	/** Construct a ReferencePointer with no object reference (null 
	 *  pointer)
	 */
	explicit ReferencePointer(): _reference(0) { }

	/** Assign to a specific object.
	 */
	explicit ReferencePointer(Object* ptr): _reference(0) {
		_assign_safe(ptr);
	}

	/** Light-weight copy with reference counting.
	 */
	explicit ReferencePointer(const ReferencePointer& r): _reference(0) {
		_assign_safe(r._reference);
	}

	/** Decrement the object's reference count and destroy the
	 *  object if the count reaches zero.
	 */
	virtual ~ReferencePointer() {
		_release();
	}

	/** Returns true if this is the only reference to an object.
	 */
	inline bool unique() const {
		return (_reference ? _reference->_count()==1 : true);
	}

#ifndef SWIG
	/** Light-weight copy with reference counting.
	 */
	inline ReferencePointer& operator=(const ReferencePointer& r) {
		_assign_safe(r._reference);
		return *this;
	}
#endif

#ifndef SWIG
	/** Raw pointer assignment for NULL only
	 */
	inline void *operator=(void *p) {
		assert(p==0);
		_reference = 0;
		return p;
	}
#endif

	/** Clear pointer. 
	 */
	inline void setNull() {
		_assign_fast(0);
	}


	/** Test for null pointer.
	 */
	inline bool isNull() const { 
		return _reference == 0; 
	}

	/** Test for null pointer.
	 */
#ifndef SWIG
	inline bool operator!() const {
		return _reference == 0;
	}
#endif

	/** Test for non-null pointer.
	 */
	inline bool valid() const {
		return _reference != 0;
	}
	
	/** Comparison with other simdata pointers.
	 */
	inline bool operator==(ReferencePointer const &p) const {
		return _reference == p._reference;
	}

	virtual Object *__get__() { return _reference; }

protected:
	/** Rebind to a new object, testing for type compatibility.
	 *
	 *  This method calls _update(), which is overridden in the
	 *  Pointer<> class to attempt a dynamic cast to the template
	 *  type and throws an exception if the cast fails.
	 */
	inline void _assign_safe(Object* ptr) {
		_release();
		_update(ptr);
		if (!isNull()) _reference->_incref();
	}

	/** Rebind to a new object, without testing for type compatibility.
	 *
	 * Use this method only when you know that the pointer you are
	 * assignng from has the correct type.
	 */
	inline void _assign_fast(Object *ptr) {
		_release();
		_reference = ptr;
		if (!isNull()) _reference->_incref();
	}

	/** Rebind to null.
	 */
	inline void _release() {
		if (!isNull()) {
			_reference->_decref();
			_reference = 0;
		}
	}

	/** Change object pointer without reference counting. 
	 *
	 * This method is extended in the Link<> class to test for 
	 * type compatibility.
	 */
	virtual void _update(Object* p) { _reference = p; }

	/** Get the current object pointer.
	 */
	inline Object* _get() const { 
		return _reference; 
	}

	/** The actual object pointer.
	 */
	Object* _reference;
};


/**
 * @brief Base class for auto-loading, smart-pointers to Objects.
 * 
 * This class combines the Path type with reference-counting for Object
 * types.  It should seldom be used directly.  See Link<> and Ref<> for
 * details on how to refer to Objects (and other Referenced types).
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT LinkBase: public Path, public ReferencePointer {
	template <class T> friend class Ref;
public:

	// SWIG python specific comparisons
	bool __eq__(const LinkBase& other);
	bool __ne__(const LinkBase& other); 
	
	/** Construct a LinkBase with no path or object reference (null 
	 *  pointer)
	 */
	explicit LinkBase(): Path((const char*)0), ReferencePointer() { }

	/** Assign an object path with no referenced object (null pointer)
	 */
	LinkBase(const char* path): Path(path), ReferencePointer() { }

	/** Assign an object path and bind to a specific object.
	 */
	explicit LinkBase(const Path& path, Object* ptr): 
		Path(path), ReferencePointer(ptr) { }
	
	/** Assign an object reference, but no path.
	 */
	explicit LinkBase(Object* ptr): Path(), ReferencePointer(ptr) {}

	/** Decrements the object's reference count and destroys the
	 * object if the count reaches zero.
	 */
	virtual ~LinkBase() { }

	/** Light-weight copy with reference counting.
	 */
	LinkBase(const LinkBase& r): Path(r.getPath()), ReferencePointer(r) {
	}

#ifndef SWIG
	/** Light-weight copy with reference counting.
	 */
	LinkBase& operator=(const LinkBase& r) {
		ReferencePointer::operator=(r);
		_path = r.getPath();
		return *this;
	}
#endif


	/** Serialize to or from a data archive.
	 *
	 *  Saves the path, and also saves the referenced object
	 *  if the path is 'None'.  Packing a None and Null 
	 *  LinkBase is an error.
	 *
	 *  Reads the saved path and binds to the correct object.
	 *  If the path is None, the object is unpacked from the
	 *  subsequent data.  Otherwise the object is created by
	 *  asking the current DataArchive to instantiate an
	 *  instance of the path.
	 */
	virtual void serialize(Archive&);

	/** String representation.
	 */
	virtual std::string asString() const;

	/** Return a string representation of the type.
	 */
	virtual std::string typeString() const { return std::string("type::LinkBase"); }
	
	/** Comparison with other simdata pointers.
	 */
	bool operator==(LinkBase const &p) const {
		return ReferencePointer::operator==(p);
	}

protected:
	/** Create a new object instance from source data.
	 *
	 * @param archive The data archive from which to load the new object's data.
	 * @param path The object path to load.
	 */
	void _load(DataArchive* archive, ObjectID path=0);

};


/**
 * @brief Class-specialized, auto-loading smart-pointer to Objects.
 *
 * Use this class for linking to other Objects in a data archive. 
 * The associated Objects will automatically be created by the 
 * archive loader as directed by the external XML data. 
 *
 * Once loaded, Link<> handles behaves very much like ordinary
 * Ref<> handles.  For storing and passing objects references,
 * and for manually instantiating Objects from a data archive,
 * you should use a Ref<> handle instead of Link<>.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 * @ingroup BaseTypes
 */
template<class T> class Link: public LinkBase {
public:

	/** Convenience typedef.
	 */
	typedef std::vector< Link<T> > vector;

	/** Create a null Link 
	 */
	explicit Link(): LinkBase() {}
	
	/** Create a Link with both a path and an object
	 */
	explicit Link(const Path& path, T* ptr): LinkBase(path, ptr) {}
	
	/** Create a Link with a path but no object (null)
	 */
	explicit Link(const char* path): LinkBase(path) {}

	/** Create a Link with an object reference, but no path
	 */
	explicit Link(T* t): LinkBase() {
		*this = t;
	}

	/** Fast copy constructor.
	 */
	Link(const Link<T>& p) { 
		_path = p.getPath();
		_assign_fast(p._reference);
	}
	
	/** Safe copy constructor.
	 */
	Link(const LinkBase& p) { 
		LinkBase::operator=(p);
	}
	
	/*
	Link<T>& operator=(const LinkBase& p) {
		LinkBase::operator=(p);
	}
	*/

	/** Return a string representation of the type.
	 */
	virtual std::string typeString() const { return std::string("type::Link::") + T::_getClassName(); }

#ifndef SWIG
	/** Assign a pointer.
	 */
	T *operator =(T *t) {
		_path = 0;
		_assign_fast(t);
		return t;
	}
#endif // SWIG

	/** Dereference.
	 */
	T* operator->() { 
		return (T*) _reference;
	}
	
	/** Const dereference.
	 */
	const T* operator->() const { 
		return (T*) _reference;
	}
	
	/** Dereference.
	 */
	T& operator*() {
		return *((T*) _reference);
	}
	
	/** Const dereference.
	 */
	const T& operator*() const {
		return *((T*) _reference);
	}

	/** Const dereference.
	 */
	const T* get() const {
		return (T*) _reference;
	}
	
	/** Dereference.
	 */
	T* get() {
		return (T*) _reference;
	}
	
protected:
	/** Internal pointer update.
	 *
	 *  Changes the object pointer without reference counting, checking 
	 *  that the new object type matches the template type.
	 */
	virtual void _update(Object* ptr) throw(ObjectTypeMismatch) {
		LinkBase::_update(ptr);
		T* _special = dynamic_cast<T*>(ptr);
		if (ptr != 0 && _special == 0) 
			throw ObjectTypeMismatch("dynamic_cast<> failed in Link<>::_update()");
	}
};



NAMESPACE_SIMDATA_END

#endif //__SIMDATA_LINK_H__

