/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002, 2003, 2004 Mark Rose <tm2@stm.lbl.gov>
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
 * @file Ref.h
 *
 * Reference counting and smart-pointer classes.
 */


#ifndef __SIMDATA_REF_H__
#define __SIMDATA_REF_H__

#include <cassert>
#include <list>
#include <vector>

#include <SimData/Namespace.h>
#include <SimData/Export.h>
#include <SimData/ExceptionBase.h>
#include <SimData/Log.h>
#include <SimData/Properties.h>

#ifdef SWIG
// silence SWIG warning about NonCopyable
%import "SimData/Properties.h"
#endif

NAMESPACE_SIMDATA


class Referenced;
class ReferencePointer;
class LinkBase;


SIMDATA_EXCEPTION(ConversionError);


/** Base class for reference counted objects.
 *
 *  Inspired by OpenSceneGraph's osg::Referenced class.
 *
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT Referenced: public NonCopyable {

template <class T> friend class Ref;
friend class ReferencePointer;

protected:
	Referenced(): __count(0) {
		//SIMDATA_LOG(LOG_ALL, LOG_ERROR, "Referenced(" << this << ")");
	}

	virtual ~Referenced() {
		//SIMDATA_LOG(LOG_ALL, LOG_ERROR, "~Referenced(" << this << ", " << __count << ")");
		if (__count != 0) {
			SIMDATA_LOG(LOG_ALL, LOG_ERROR, "simdata::Referenced(" << std::hex << int(this) << ") deleted with non-zero reference count (" << __count << "): memory corruption possible.");
		}
	}

private:
	inline void _incref() const {
		//SIMDATA_LOG(LOG_ALL, LOG_ERROR, "_incref(" << this << ", " << __count << ")");
		++__count;
	}
	inline void _decref() const {
		//SIMDATA_LOG(LOG_ALL, LOG_ERROR, "_decref(" << this << ", " << __count << ")");
		assert(__count > 0);
		if (0 == --__count) delete this;
	}
	inline unsigned _count() const { return __count; }
	mutable unsigned __count;
};


/** Reference counting smart-pointer.
 *
 *  Reference counting smart-pointer for use with simdata::Referenced
 *  objects.
 *
 *  Classes derived from Reference should be handled with Ref<T> smart
 *  pointers, or Link<T> dynamic references.
 *
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
template<class CLASS>
class Ref: protected HasBase<CLASS, Referenced> {
public:
	typedef std::vector< Ref<CLASS> > vector;
	typedef std::list< Ref<CLASS> > list;

	/* TODO add Python bindings
	// SWIG python specific comparisons
	bool __eq__(const Ref& other);
	bool __ne__(const Ref& other);
	*/

	/** Create a null reference.
	 */
	Ref(): HasBase<CLASS, Referenced>(), _reference(0) { }

	/** Create a new reference.
	 */
	Ref(CLASS* ptr): HasBase<CLASS, Referenced>(), _reference(ptr) {
		if (_reference) _reference->_incref();
	}

	/** Light-weight copy with reference counting.
	 */
	Ref(LinkBase const & r): HasBase<CLASS, Referenced>(), _reference(0) {
		_rebind(r._get());
	}

	/** Light-weight copy with reference counting.
	 */
	template <typename Q>
	Ref(Ref<Q> const & r): HasBase<CLASS, Referenced>(), _reference(0) {
		Q *rp = r.get();
		if (rp != 0) {
			//rp->_incref();
			//_reference = static_cast<CLASS*>(rp);
			_rebind(rp);
		}
	}

	/** Light-weight copy with reference counting.
	 */
	Ref(Ref const & r): HasBase<CLASS, Referenced>(), _reference(r._reference) {
		if (_reference) _reference->_incref();
	}

	/** Decrement the reference count, and potentially destroy
	 *  the referenced object.
	 */
	~Ref() { 
		_unbind();
	}

	/** Returns true if this is the only reference.
	 */
	inline bool unique() const {
		if (_reference == 0) return true;
		return (_reference->_count() == 1);
	}

	/** Get the number of outstanding references.
	 */
	inline int count() const {
		if (_reference == 0) return 1;
		return _reference->_count();
	}

#ifndef SWIG
	/** Light-weight copy with reference counting.
	 */
	template <class Q>
	Ref const & operator=(Ref<Q> const & r) {
		_rebind(r.get());
		return *this;
	}

	/** Light-weight copy with reference counting.
	 *
	 *  Although this methods _looks_ redundant with the templated Ref<Q>
	 *  assignment operator, it is _essential_.  Without it, C++ generates
	 *  a default copy constructor that breaks reference counting.
	 */
	Ref const & operator=(Ref const & r) {
		_rebind(r.get());
		return *this;
	}

	/** Light-weight copy with reference counting.
	 */
	LinkBase const & operator=(LinkBase const & r) {
		_rebind(r._get());
		return r;
	}

	/** Raw pointer assignment.
	 */
	template <class Q>
	inline Q *operator=(Q *ptr) {
		_rebind(ptr);
		return ptr;
	}

	/** Raw pointer assignment for assigning NULL.
	 *
	 *  Assigning any pointer other than '0' will assert.
	 */
	inline void *operator=(void *ptr) {
		assert(ptr==0);
		_unbind();
		_reference = 0;
		return ptr;
	}
#endif // SWIG

	/** Dereference.
	 */
	inline CLASS* get() const {
		return _reference;
	}

	/** Dereference.
	 */
	inline CLASS* operator->() const {
		return _reference;
	}

	/** Dereference.
	 */
	inline CLASS & operator*() const {
		return *_reference;
	}

#ifndef SWIG
	/** Test for null pointer.
	 */
	inline bool operator!() const {
		return _reference == 0;
	}
#endif // SWIG

	/** Test for non-null pointer.
	 */
	inline bool valid() const {
		return _reference != 0;
	}
	
	/** Comparison with other simdata pointers.
	 */
	template <class Q>
	inline bool operator==(Ref<Q> const & p) const {
		return _reference == p.get();
	}

	/** Comparison with other simdata pointers.
	 */
	inline bool operator==(Referenced const * p) const {
		return _reference == p;
	}

	/** Comparison with other simdata pointers.
	 */
	template <class Q>
	inline bool operator!=(Ref<Q> const & p) const {
		return _reference != p.get();
	}

	/** Comparison with other simdata pointers.
	 */
	inline bool operator!=(Referenced const * p) const {
		return _reference != p;
	}

protected:

	/** Unbind from the instance, deleting it if we are holding the only reference.
	 */
	inline void _unbind() {
		if (_reference) {
			_reference->_decref();
			_reference = 0;
		}
	}

	/** Rebind to a new object.  Allows rebinding to base class pointers
	 *  using dynamic_cast.  For assignments from the same class or child
	 *  classes this reduces to a simple (fast) static_cast.
	 *
	 *  @throws ConversionError if downcasting from an incompatible type.
	 */
	template <class Q>
	void _rebind(Q* ptr) {
		if (ptr) ptr->_incref();
		_unbind();
		_reference = dynamic_cast<CLASS*>(ptr);
		if (_reference == 0 && ptr != 0) {
			SIMDATA_LOG(LOG_ALL, LOG_ERROR, "simdata::Ref() assignment: incompatible types (dynamic cast failed).");
			throw ConversionError();
		}
	}

	/** The actual pointer.
	 */
	CLASS* _reference;
};


NAMESPACE_SIMDATA_END


#endif //__SIMDATA_OBJECT_H__

