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
 * @file Ref.h
 *
 * Reference counting and smart-pointer classes.
 */


#ifndef __SIMDATA_REF_H__
#define __SIMDATA_REF_H__

#include <cassert>
#include <list>
#include <vector>

#include <SimData/ns-simdata.h>
#include <SimData/Export.h>
#include <SimData/Exception.h>
#include <SimData/Log.h>


NAMESPACE_SIMDATA


class Referenced;
class ReferencePointer;
class LinkBase;


SIMDATA_EXCEPTION(ConversionError);


/**
 * Reference counting smart-pointer for use with simdata::Referenced
 * objects.
 *
 * Classes derived from Reference should be handled with Ref<T> smart
 * pointers, or Link<T> dynamic references.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
template<class T>
class Ref {
public:
	typedef std::vector< Ref<T> > vector;
	typedef std::list< Ref<T> > list;

	/* TODO add Python bindings
	// SWIG python specific comparisons
	bool __eq__(const Ref& other);
	bool __ne__(const Ref& other); 
	*/
	

	/**
	 * Create a null reference.
	 */
	Ref(): _reference(0) {
	}


	/**
	 * Create a new reference.
	 */
	Ref(T* ptr): _reference(ptr) {
		if (ptr) ptr->_ref();
	}

	Ref(LinkBase const & r): _reference(0) {
		_rebind(r._get());
	}

	/**
	 * Light-weight copy with reference counting.
	 */
	template <typename Q>
	Ref(Ref<Q> const & r): _reference(0) {
		Q *rp = r.get();
		if (rp != 0) {
			rp->_ref();
			_reference = static_cast<T*>(rp);
		}
	}

	Ref(Ref const & r): _reference(r._reference) {
		if (_reference != 0) _reference->_ref();
	}

	/**
	 * Decrements the reference count, potentially destroying
	 * the referenced object.
	 */
	~Ref() {
		if (_reference) _reference->_deref();
	}

	/**
	 * Returns true if this is the only reference.
	 */
	inline bool unique() const {
		if (_reference == 0) return true;
		return (_reference->_count() == 1);
	}

	/**
	 * Get the number of outstanding references.
	 */
	inline int count() const {
		if (_reference == 0) return 1;
		return _reference->_count();
	}

	/**
	 * Light-weight copy with reference counting.
	 */
	template <class Q>
	Ref const & operator=(Ref<Q> const & r) {
		_rebind(r.get());
		return *this;
	}

	Ref const & operator=(Ref const & r) {
		_rebind(r.get());
		return *this;
	}

	LinkBase const & operator=(LinkBase const & r) {
		_rebind(r._get());
		return r;
	}

	/**
	 * Raw pointer assignment
	 */
	template <class Q>
	inline Q *operator=(Q *ptr) {
		_rebind(ptr);
		return ptr;
	}

	inline void *operator=(void *ptr) {
		assert(ptr==0);
		if (_reference != 0) _reference->_deref();
		_reference = 0;
		return ptr;
	}


	/**
	 * dereference.
	 */
	inline T* get() const {
		return _reference;
	}

	/**
	 * dereference.
	 */
	inline T* operator->() const {
		return _reference;
	}

	/**
	 * dereference.
	 */
	inline T & operator*() const {
		return *_reference;
	}

	/**
	 * Test for null pointer.
	 */
	inline bool operator!() const {
		return _reference == 0;
	}

	/**
	 * Test for non-null pointer.
	 */
	inline bool valid() const {
		return _reference != 0;
	}
	
	/**
	 * Comparison with other simdata pointers.
	 */
	template <class Q>
	inline bool operator==(Ref<Q> const & p) const {
		return _reference == p.get();
	}

	/**
	 * Comparison with other simdata pointers.
	 */
	inline bool operator==(Referenced const * p) const {
		return _reference == p;
	}


protected:
	/**
	 * Rebind to a new object.
	 */
	template <class Q>
	void _rebind(Q* ptr) {
		if (ptr) ptr->_ref();
		if (_reference != 0) _reference->_deref();
		_reference = dynamic_cast<T*>(ptr);
		if (_reference == 0 && ptr != 0) {
			SIMDATA_LOG(LOG_ALL, LOG_ERROR, "simdata::Ref() assignment: incompatible types (dynamic cast failed).");
			throw ConversionError();
		}
	}

	/**
	 * The actual pointer.
	 */
	T* _reference;
};


/**
 * Base class for reference counted objects.
 *
 * Inspired by OpenSceneGraph's osg::Referenced class.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT Referenced {

template <class T> friend class Ref;
friend class ReferencePointer;

public:
	Referenced(): __count(0) {}
	Referenced(Referenced const &): __count(0) {}
	inline Referenced& operator=(Referenced const &) { return *this; }

protected:
	virtual ~Referenced() {
		if (__count != 0) {
			SIMDATA_LOG(LOG_ALL, LOG_ERROR, "simdata::Referenced(" << std::hex << int(this) << ") deleted with non-zero reference count (" << __count << "): memory corruption possible.");
		}
	}

private:
	inline void _ref() const { ++__count; }

	inline void _deref() const { 
		if (--__count <= 0) {
			delete this;
		}
	}
	inline unsigned _count() const { return __count; }

	mutable unsigned __count;
};


NAMESPACE_END // namespace simdata


#endif //__SIMDATA_OBJECT_H__

