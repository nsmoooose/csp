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
 * @file AtomicCounter.h
 * @brief An integer counter that supports atomic updates, and can be
 *        used to implement thread-safe reference counters.
 */


#ifndef SIMDATA_NOTHREADS

#ifndef __SIMDATA_ATOMICCOUNTER_H__
#define __SIMDATA_ATOMICCOUNTER_H__

#include <SimData/Export.h>
#include <SimData/Namespace.h>
#include <SimData/Properties.h>


#if defined(WIN32)
#  define WIN_ATOMIC
#  define _WINSOCKAPI_
#  include <Windows.h>
#else
#  define LIN_ATOMIC
extern "C" {
#  include <asm/atomic.h>
#  ifdef LOCK
#    undef LOCK
#  endif
};
#endif


NAMESPACE_SIMDATA


/** An atomic counter that can be used with ReferenceBase to
 *  implement thread-safe reference counting.
 */
class SIMDATA_EXPORT AtomicCounter: public NonCopyable {

// Atomic counter using linux-specific atomic counter primitives (much
// faster than mutex locking).
#if defined(LIN_ATOMIC)

private:
	atomic_t __count;

public:

	/** Construct a new atomic counter initialized to zero.
	 */
	AtomicCounter() {
		atomic_set(&__count, 0);
	}

	/** Construct a new atomic counter.
	 *
	 *  @param count the initial value of the counter.
	 */
	AtomicCounter(int count) {
		atomic_set(&__count, count);
	}

	/** Increment the counter.
	 *
	 *  @returns true if the incremented count is non-zero, regardless
	 *  of the actual value.  Since this is a preincrement operator, it
	 *  will always return true (unless the count overflows or is otherwise
	 *  decremented past zero).
	 */
	inline bool operator++() {
		return atomic_inc_and_test(&__count) ? false : true;
	}

	/** Decrement the counter.
	 *
	 *  @returns true if the pre-decremented count is non-zero,
	 *  regardless of the actual value; false otherwise.
	 */
	inline bool operator--() {
		return atomic_dec_and_test(&__count) ? false : true;
	}

	/** Return the counter value.
	 */
	inline operator int() { return atomic_read(&__count); }

// TODO implement using windows atomic functions.
//
// probably involves the microsoft specific functions described at
//   http://msdn.microsoft.com/library/en-us/vclang/html/vclrf_InterlockedDecrement.asp
//
//#elif defined(WIN_ATOMIC)

// otherwise, fallback on pthread mutex locking (slow but safe)
#else

private:
	int __count;
	ThreadMutex __mutex;

public:

	/** Construct a new atomic counter initialized to zero.
	 */
	AtomicCounter(): __count(0) { }

	/** Construct a new atomic counter.
	 *
	 *  @param count the initial value of the counter.
	 */
	AtomicCounter(int count): __count(count) { }

	/** Increment the counter.
	 *
	 *  @returns 1 if the incremented count is non-zero, regardless
	 *  of the actual value.  Since this is a preincrement operator, it
	 *  will always return 1 (unless the count overflows or is otherwise
	 *  decremented past zero).
	 */
	inline bool operator++() {
		__mutex.lock();
		int count = ++__count;
		__mutex.unlock();
		return count != 0;
	}

	/** Decrement the counter.
	 *
	 *  @returns true if the pre-decremented count is non-zero,
	 *  regardless of the actual value; false otherwise.
	 */
	inline bool operator--() {
		__mutex.lock();
		int count = --__count;
		__mutex.unlock();
		return count != 0;
	}

	/** Return the counter value.
	 */
	inline operator int() { return __count; }

#endif

};

NAMESPACE_SIMDATA_END


#endif // __SIMDATA_ATOMICCOUNTER_H__

#endif // SIMDATA_NOTHREADS
