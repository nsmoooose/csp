#pragma once
/* Combat Simulator Project
 * Copyright (C) 2002, 2003, 2004 Mark Rose <mkrose@users.sf.net>
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
 * Reference counting smart-pointer classes.
 */

#include <list>
#include <vector>

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Exception.h>
#include <csp/csplib/util/Referenced.h>
#include <csp/csplib/util/Verify.h>

#ifdef SWIG
// silence SWIG warning about NonCopyable
%import "csp/csplib/util/Properties.h"
#endif

namespace csp {


class LinkCore;

CSP_EXCEPTION(ConversionError)

void CSPLIB_EXPORT _log_reference_conversion_error();


/** Reference counting smart-pointer.
 *
 *  Reference counting smart-pointer for use with csplib::Referenced
 *  objects.
 *
 *  Classes derived from Reference should be handled with Ref<T> smart
 *  pointers, or Link<T> dynamic references.
 */
template<class CLASS>
class Ref {
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
	Ref(): _reference(0) { }

	/** Create a new reference.
	 */
	Ref(CLASS* ptr): _reference(ptr) {
		if (_reference) _reference->_incref();
	}

	/** Light-weight copy with reference counting.
	 */
	inline Ref(LinkCore const & r);

	/** Light-weight copy with reference counting.
	 */
	template <typename Q>
	Ref(Ref<Q> const & r): _reference(0) {
		Q *rp = r.get();
		if (rp != 0) {
			//rp->_incref();
			//_reference = static_cast<CLASS*>(rp);
			_rebind(rp);
		}
	}

	/** Light-weight copy with reference counting.
	 */
	Ref(Ref const & r): _reference(r._reference) {
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
	inline Ref const & operator=(LinkCore const & r);

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
		CSP_VERIFY(ptr==0);
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

	/** Test for null pointer.
	 */
	inline bool isNull() const {
		return _reference == 0;
	}
	
	/** Comparison with other Ref<> pointers.
	 */
	template <class Q>
	inline bool operator==(Ref<Q> const & p) const {
		return _reference == p.get();
	}

	/** Comparison with other CLASS pointers.
	 *  XXX the base class NonCopyable is used here instead of CLASS since
	 *  swig generates bad code if CLASS is const (const const).
	 */
	inline bool operator==(NonCopyable const * p) const {
		return _reference == p;
	}

	/** Comparison with other Ref<> pointers.
	 */
	template <class Q>
	inline bool operator!=(Ref<Q> const & p) const {
		return _reference != p.get();
	}

	/** Test assignment compatability.  If this test fails, then assignment
	 *  would generate a ConversionError exception.
	 */
	template <class Q>
	static inline bool compatible(Ref<Q> const & p) {
		return p.isNull() || dynamic_cast<CLASS*>(p.get()) != 0;
	}

	/** Try to assignment from a potentially incompatible reference.  Returns
	 *  true on success, or false if the pointers are incompatible.  This
	 *  reference will be set to null if the assignment fails (or the other
	 *  reference is null).
	 */
	template <class Q>
	bool tryAssign(Ref<Q> const & p) {
		_rebind(p.get(), false);
		return (valid() || p.isNull());
	}

	/** Try to assignment from a potentially incompatible LinkCore.  Returns
	 *  true on success, or false if the pointers are incompatible.  This
	 *  reference will be set to null if the assignment fails (or the other
	 *  reference is null).
	 */
	inline bool tryAssign(LinkCore const & p);

	/** Try to assignment from a potentially incompatible pointer.  Returns
	 *  true on success, or false if the pointers are incompatible.  This
	 *  reference will be set to null if the assignment fails (or the other
	 *  reference is null).
	 */
	template <class Q>
	bool tryAssign(Q *p) {
		_rebind(p, false);
		return (valid() || (p == 0));
	}

	/** Comparison with other CLASS pointers.
	 *  XXX the base class NonCopyable is used here instead of CLASS since
	 *  swig generates bad code if CLASS is const (const const).
	 */
	inline bool operator!=(NonCopyable const * p) const {
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
	void _rebind(Q* ptr, bool throw_on_fail=true) {
		if (ptr) ptr->_incref();
		_unbind();
		_reference = dynamic_cast<CLASS*>(ptr);
		if (_reference == 0 && ptr != 0 && throw_on_fail) {
			_log_reference_conversion_error();
			throw ConversionError();
		}
	}

	/** The actual pointer.
	 */
	CLASS* _reference;
};


} // namespace csp
