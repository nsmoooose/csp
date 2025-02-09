#pragma once
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

#include <atomic>

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Properties.h>
#include <csp/csplib/util/undef.h>


namespace csp {


/** An atomic counter that can be used with ReferenceBase to implement thread-safe
 *  reference counting.  This is an extremely lightweight wrapper around the GNU
 *  Common C++ atomic counter implementation, which simply restricts the interface
 *  and brings it into the csp namespace.
 */
class CSPLIB_EXPORT AtomicCounter: public NonCopyable {
private:
	std::atomic<int> m_ref_count;

public:

	/** Construct a new atomic counter initialized to zero.
	 */
	AtomicCounter() { }

	/** Construct a new atomic counter.
	 *
	 *  @param count the initial value of the counter.
	 */
	AtomicCounter(int count): m_ref_count(count) { }

	/** Increment the counter.
	 *
	 *  @returns true if the incremented count is non-zero, regardless
	 *  of the actual value.  Since this is a preincrement operator, it
	 *  will always return true (unless the count overflows or is otherwise
	 *  decremented past zero).
	 */
	inline bool operator++() {
		m_ref_count.fetch_add(1, std::memory_order_relaxed);
		return m_ref_count > 0;
	}

	/** Decrement the counter.
	 *
	 *  @returns true if the pre-decremented count is non-zero,
	 *  regardless of the actual value; false otherwise.
	 */
	inline bool operator--() {
        if (m_ref_count.fetch_sub(1, std::memory_order_acquire) == 1) {
            std::atomic_thread_fence(std::memory_order_release);
			return false;
        }
		return true;
	}

	/** Return the counter value.
	 */
	inline operator int() { return m_ref_count; }

};

} // namespace csp
