/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @brief Smart pointers to Object classes in data archives.
 */


#ifndef __SIMDATA_LINK_H__
#define __SIMDATA_LINK_H__


#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#pragma warning (disable : 4290)
#endif


#include <SimData/Export.h>
#include <SimData/Path.h>
#include <SimData/Object.h>
#include <SimData/Log.h>

#include <string>
#include <cassert>


NAMESPACE_SIMDATA


class DataArchive;


/**
 * @brief Base class for auto-loading, smart-pointers to Objects.
 *
 * This class combines the Path type with reference-counting for Object
 * types.  It should seldom be used directly.  See Link<> and Ref<> for
 * details on how to refer to Objects (and other Referenced types).
 *
 * @author Mark Rose <mkrose@users.sf.net>
 */
class SIMDATA_EXPORT LinkCore: public Path {
private:
	Object *_reference;

protected:
	inline void _release() {
		if (_reference) {
			_reference->_decref();
			_reference = 0;
		}
	}

	inline void _reassign(Object *ptr) {
		if (ptr) ptr->_incref();
		_release();
		_reference = ptr;
	}

	inline Object *_get() const { return _reference; }

	// LinkCore cannot be (publically) copied by assignment since there is no way to
	// test that the types are compatible.  Consider:
	//   Link<T> foo;
	//   Link<U> bar;
	//   LinkCore *core = &foo;
	//   *core = bar;
	// This method is used by base classes to assign after checking type compatibility.
	LinkCore& operator=(const LinkCore& r) {
		Path::operator=(r);
		_reassign(r._reference);
		return *this;
	}

public:

	// SWIG python specific comparisons
	bool __eq__(const LinkCore& other);
	bool __ne__(const LinkCore& other);

	/** Decrement the object's reference count and destroy the
	 *  object if the count reaches zero.
	 */
	virtual ~LinkCore() { _release(); }
	
	/** Construct a LinkCore with no path or object reference (null
	 *  pointer)
	 */
	LinkCore(): Path((const char*)0), _reference(0) { }

	/** Assign an object path with no referenced object (null pointer)
	 */
	LinkCore(const char* path): Path(path), _reference(0) { }

	/** Assign an object path and bind to a specific object.
	 */
	LinkCore(const Path& path, Object* ptr): Path(path), _reference(ptr) {
		if (ptr) ptr->_incref();
	}
	
	/** Assign an object reference, but no path.
	 */
	explicit LinkCore(Object* ptr): Path(), _reference(ptr) {
		if (ptr) ptr->_incref();
	}

	/** Light-weight copy with reference counting.
	 */
	LinkCore(const LinkCore& r): Path(r.getPath()), _reference(r._reference) {
		if (_reference) _reference->_incref();
	}

	/** Returns true if this is the only reference to an object.
	 */
	inline bool unique() const { return (_reference ? _reference->_count()==1 : true); }

#ifndef SWIG
	/** Raw pointer assignment for NULL only
	 */
	inline void *operator=(void *p) {
		assert(p==0);
		_reassign(0);
		return 0;
	}
#endif

	/** Clear pointer.
	 */
	inline void setNull() { _reassign(0); }

	/** Test for null pointer.
	 */
	inline bool isNull() const { return _reference == 0; }

	/** Test for null pointer.
	 */
#ifndef SWIG
	inline bool operator!() const { return _reference == 0; }
#endif

	/** Test for non-null pointer.
	 */
	inline bool valid() const { return _reference != 0; }
	
	/** Comparison with other simdata pointers.
	 */
	bool operator==(LinkCore const &p) const {
		return Path::operator==(p) && _reference == p._reference;
	}

	Object *__get__() const { return _reference; }

	/** Return a string representation of the type.
	 *  Overrides Path::typeString.
	 */
	std::string typeString() const { return std::string("type::LinkCore"); }
	
protected:
	LinkCore _internal_load(DataArchive* archive, ObjectID path);
	void _serialize(Writer &writer) const;
	static LinkCore _serialize(Reader &reader, DataArchive* &archive);
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
 * @author Mark Rose <mkrose@users.sf.net>
 * @ingroup BaseTypes
 */
template<class T> class Link: public LinkCore {
friend class TypeAdapter;

template <class U> friend class Ref;
	// For assigning from Link<> to a Ref<>.
	Object *_get() const { return LinkCore::_get(); }

public:  // BaseType

	/** Return a string representation of the type.
	 *  Overrides LinkCore::typeString.
	 */
	std::string typeString() const { return std::string("type::Link::") + T::_getClassName(); }

	std::string asString() const { return std::string("Link<") + T::_getClassName() + ">"; }

	/** Serialize to a data archive.
	 *
	 *  Saves the path, and also saves the referenced object if the path is
	 *  'None'.  Packing a None and Null LinkCore is an error.
	 */
	void serialize(Writer &writer) const { LinkCore::_serialize(writer); }

	/** Serialize from a data archive.
	 *
	 *  Reads the saved path and binds to the correct object.  If the path is
	 *  None, the object is unpacked from the subsequent data.  Otherwise the
	 *  object is created by asking the current DataArchive to instantiate an
	 *  instance of the path.
	 */
	void serialize(Reader &reader) {
		DataArchive *archive = 0;
		*this = LinkCore::_serialize(reader, archive);
		if (archive) _load(archive);
	}

	/// BaseType interface.
	void parseXML(const char*) {}

	/// BaseType interface.
	void convertXML() {}

public:

	/** Convenience typedef.
	 */
	typedef std::vector< Link<T> > vector;

	/** Create a null Link
	 */
	Link(): LinkCore() {}
	
	/** Create a Link with both a path and an object
	 */
	Link(const Path& path, T* ptr): LinkCore(path, ptr) {}
	
	/** Create a Link with a path but no object (null)
	 */
	explicit Link(const char* path): LinkCore(path) {}

	/** Create a Link with an object reference, but no path
	 */
	explicit Link(T* t): LinkCore(t) { }

	/** Fast copy constructor.
	 */
	Link(const Link<T>& p): LinkCore(p) { }
	
	/** Safe copy constructor.
	 */
	Link(const LinkCore& p) { *this = p; }

public:

#ifndef SWIG
	Link<T>& operator=(const LinkCore& p) { _checkType(p.__get__()); LinkCore::operator=(p); return *this; }

	Link<T>& operator=(const Link<T>& p) { LinkCore::operator=(p); return *this; }

	/** Assign a pointer.
	 */
	T *operator =(T *t) {
		_path = 0;
		_assign_fast(t);
		return t;
	}
#endif // SWIG

#ifndef SWIG

	/** Dereference.
	 */
	T* operator->() { return (T*) _get(); }
	
	/** Const dereference.
	 */
	const T* operator->() const { return (T*) _get(); }
	
	/** Dereference.
	 */
	T& operator*() { return *((T*) _get()); }
	
	/** Const dereference.
	 */
	const T& operator*() const { return *((T*) _get()); }

	/** Const dereference.
	 */
	const T* get() const { return (T*) _get(); }
	
	/** Dereference.
	 */
	T* get() { return (T*) _get(); }
	
#endif // SWIG

private:
	/** Internal pointer update.
	 *
	 *  Changes the object pointer without reference counting, checking
	 *  that the new object type matches the template type.
	 */
	void _checkType(Object const* ptr) {
		if (ptr) {
			T const* _special = dynamic_cast<T const*>(ptr);
			if (_special == 0) {
				throw ObjectTypeMismatch("dynamic_cast<> failed in Link<>::_checkType() [" + typeString() + "<=" + ptr->getClassName() + "]");
			}
		}
	}

	/** Create a new object instance from source data.
	 *
	 * @param archive The data archive from which to load the new object's data.
	 * @param path The object path to load.
	 */
	void _load(DataArchive* archive, ObjectID path=0) {
		*this = _internal_load(archive, path);
	}
};

#ifdef SWIG
%template(LinkObj) Link<Object>;
#endif

class LinkBase: public Link<Object> {
public:
	LinkBase(): Link<Object>() { }
	explicit LinkBase(const char* path): Link<Object>(path) { }
	LinkBase(const Path& path, Object* ptr): Link<Object>(path, ptr) { }
	explicit LinkBase(Object* ptr): Link<Object>(ptr) { }
	LinkBase(const LinkBase& r): Link<Object>(r) { }
};

NAMESPACE_SIMDATA_END

#endif //__SIMDATA_LINK_H__

