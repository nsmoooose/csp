/* Combat Simulator Project
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file AtomicCounter.h
 * @brief An integer counter that supports atomic updates, and can be
 *        used to implement thread-safe reference counters.
 */


#ifndef __CSPLIB_THREAD_ATOMICCOUNTER_H__
#define __CSPLIB_THREAD_ATOMICCOUNTER_H__

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Properties.h>

#ifdef WIN32
#define NOMINMAX
#endif
#include <cc++/thread.h>


CSP_NAMESPACE


/** An atomic counter that can be used with ReferenceBase to implement thread-safe
 *  reference counting.  This is an extremely lightweight wrapper around the GNU
 *  Common C++ atomic counter implementation, which simply restricts the interface
 *  and brings it into the csp namespace.
 */
class CSPLIB_EXPORT AtomicCounter: public NonCopyable {
private:
	ost::AtomicCounter __count;

public:

	/** Construct a new atomic counter initialized to zero.
	 */
	AtomicCounter() { }

	/** Construct a new atomic counter.
	 *
	 *  @param count the initial value of the counter.
	 */
	AtomicCounter(int count): __count(count) { }

	/** Increment the counter.
	 *
	 *  @returns true if the incremented count is non-zero, regardless
	 *  of the actual value.  Since this is a preincrement operator, it
	 *  will always return true (unless the count overflows or is otherwise
	 *  decremented past zero).
	 */
	inline bool operator++() { return (++__count != 0); }

	/** Decrement the counter.
	 *
	 *  @returns true if the pre-decremented count is non-zero,
	 *  regardless of the actual value; false otherwise.
	 */
	inline bool operator--() { return (--__count != 0); }

	/** Return the counter value.
	 */
	inline operator int() { return __count; }

};

CSP_NAMESPACE_END

#endif // __CSPLIB_THREAD_ATOMICCOUNTER_H__

