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
//#  define _WINSOCKAPI_
//#  define NOMINMAX
//#  include <Windows.h>
//#  undef ERROR
#else
#  define LIN_ATOMIC
extern "C" {
#  include <asm/atomic.h>
#  ifdef LOCK
#    undef LOCK
#  endif
};
#endif

// Atomic counter using linux-specific atomic counter primitives (much
// faster than mutex locking).
#if defined(LIN_ATOMIC)
#	define SIMDATA_ATOMIC_SET(x, count) atomic_set(&x, count)
#	define SIMDATA_ATOMIC_DEC(x) atomic_dec_and_test(&x) ? false : true;
#	define SIMDATA_ATOMIC_INC(x) atomic_inc_and_test(&x) ? false : true;
#	define SIMDATA_ATOMIC_GET(x) atomic_read(&x);
#	define SIMDATA_ATOMIC_TYPE atomic_t
// Atomic counter using windows-specific atomic counter primitives (much
// faster than mutex locking).
#elif defined(WIN_ATOMIC)
#	define SIMDATA_ATOMIC_SET(x, count) _InterlockedExchange(&x, count)
#	define SIMDATA_ATOMIC_DEC(x) _InterlockedDecrement(&x) ? false : true;
#	define SIMDATA_ATOMIC_INC(x) _InterlockedIncrement(&x) ? false : true;
#	define SIMDATA_ATOMIC_GET(x) x
#	define SIMDATA_ATOMIC_TYPE volatile LONG
#	pragma intrinsic (_InterlockedExchange)
#	pragma intrinsic (_InterlockedDecrement)
#	pragma intrinsic (_InterlockedIncrement)
// Atomic counter using pthreads locking (slow but safe).
#else
	NAMESPACE_SIMDATA
	class _AtomicCounter {
	private:
		int __count;
		ThreadMutex __mutex;
	public:
		_AtomicCounter(): __count(0) { }
		inline void set(int count) {
			ScopedLock<ThreadMutex> lock(__mutex);
			__count = count;
		}
		inline bool dec() {
			ScopedLock<ThreadMutex> lock(__mutex);
			return --__count != 0;
		}
		inline bool inc() {
			ScopedLock<ThreadMutex> lock(__mutex);
			return ++__count != 0;
		}
		inline int get() { return __count; }
	};
	NAMESPACE_SIMDATA_END
#	define SIMDATA_ATOMIC_SET(x, count) x.set(count)
#	define SIMDATA_ATOMIC_DEC(x) x.dec();
#	define SIMDATA_ATOMIC_INC(x) x.inc();
#	define SIMDATA_ATOMIC_GET(x) x.get();
#	define SIMDATA_ATOMIC_TYPE SIMDATA(_AtomicCounter)
#endif


NAMESPACE_SIMDATA


/** An atomic counter that can be used with ReferenceBase to
 *  implement thread-safe reference counting.
 */
class SIMDATA_EXPORT AtomicCounter: public NonCopyable {
private:
	SIMDATA_ATOMIC_TYPE __count;

public:

	/** Construct a new atomic counter initialized to zero.
	 */
	AtomicCounter() {
		SIMDATA_ATOMIC_SET(__count, 0);
	}

	/** Construct a new atomic counter.
	 *
	 *  @param count the initial value of the counter.
	 */
	AtomicCounter(int count) {
		SIMDATA_ATOMIC_SET(__count, count);
	}

	/** Increment the counter.
	 *
	 *  @returns true if the incremented count is non-zero, regardless
	 *  of the actual value.  Since this is a preincrement operator, it
	 *  will always return true (unless the count overflows or is otherwise
	 *  decremented past zero).
	 */
	inline bool operator++() {
		return SIMDATA_ATOMIC_INC(__count);
	}

	/** Decrement the counter.
	 *
	 *  @returns true if the pre-decremented count is non-zero,
	 *  regardless of the actual value; false otherwise.
	 */
	inline bool operator--() {
		return SIMDATA_ATOMIC_DEC(__count);
	}

	/** Return the counter value.
	 */
	inline operator int() { return SIMDATA_ATOMIC_GET(__count); }

};

NAMESPACE_SIMDATA_END


#endif // __SIMDATA_ATOMICCOUNTER_H__

#endif // SIMDATA_NOTHREADS
