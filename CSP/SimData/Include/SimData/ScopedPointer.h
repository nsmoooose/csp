/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file ScopedPointer.h
 * @brief A pointer wrapper with scope-based auto deletion.
 */


#ifndef __SIMDATA_SCOPEDPOINTER_H__
#define __SIMDATA_SCOPEDPOINTER_H__

#include <SimData/Namespace.h>
#include <SimData/Properties.h>


NAMESPACE_SIMDATA


/** A smart pointer class that automatically deletes the
 *  referenced object when leaving scope.
 *
 *  This class is similar to std::auto_ptr, but does not
 *  allow ownership to be transferred.  The interface
 *  was borrowed from Boost C++.
 */
template <class T>
class ScopedPointer: public NonCopyable {
	T *_ptr;
public:
	typedef T Type;

	/** Construct a new scoped pointer.
	 *
	 *  The assigned pointer, if non-null, will be automatically
	 *  deleted when this instance goes out of scope.
	 */
	explicit ScopedPointer(T * p = 0): _ptr(p) { }

	inline ~ScopedPointer() { if (_ptr) reset(); }

	/** Reassign this instance to a new pointer.
	 *
	 *  If the current pointer is non-null it will be deleted.
	 */
	void reset(T * p = 0) {
		assert(_ptr == 0 || _ptr != p);
		if (_ptr) delete _ptr;
		_ptr = p;
	}

	/** Obtain a non-const referenc to the underlying object.
	 *
	 *  Will abort if the pointer is null.
	 */
	inline T & operator*() const { assert(ptr); return *_ptr; }

	/** Dereference the pointer.
	 *
	 *  Will abort if the pointer is null.
	 */
	inline T * operator->() const { assert(ptr); return _ptr; }

	/** Get the raw pointer.
	 */
	inline T * get() const { return _ptr; }

	/** Return true if the pointer is null.
	 */
	inline bool isNull() const { return 0 == _ptr; }

	/** Return true if the pointer is not null.
	 */
	inline bool valid() const { return 0 != _ptr; }

};

NAMESPACE_SIMDATA_END

#endif // __SIMDATA_SCOPEDPOINTER_H__
