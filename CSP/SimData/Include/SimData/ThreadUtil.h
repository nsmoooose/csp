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
 * @file ThreadUtil.h
 * @brief Thread utility classes.
 */


#ifndef __SIMDATA_THREADUTIL_H__
#define __SIMDATA_THREADUTIL_H__

#include <SimData/Namespace.h>
#include <SimData/Properties.h>
#include <SimData/ExceptionBase.h>
#include <SimData/LogConstants.h>

NAMESPACE_SIMDATA


/** Exception class for thread-related errors.  Wraps a small subset
 *  of cerrno constants.
 */
class SIMDATA_EXPORT ThreadException: public Exception {
public:
	ThreadException(const int error);

	static void checkThrow(const int result);

	static void checkLog(const int result);

	int getError() const { return m_error; }

private:
	void translateError();

	int m_error;
};


/** Dummy mutex for use with single-threaded classes.
 */
struct NoMutex {
	inline void lock() { }
	inline void unlock() { }
};


/** A simple scoped mutex.  Used to protect a shared resource within
 *  the current scope, so that the lock is automatically released the
 *  ScopedLock goes out a scope.
 *
 *  For example:
 *
 *  @code
 *  void foo() {
 *    static int x = 0;
 *    static ThreadMutex mutex;
 *    {
 *      ScopedLock<ThreadMutex> guard(mutex);
 *      // mutex is now locked
 *      x = x + 1;
 *    }
 *    // mutex is now unlocked
 *  }
 *  @endcode
 */
template <class LOCK>
class ScopedLock: public NonCopyable {
public:
	/** Construct a new scoped lock for a existing lockable instance.  If constructed
	 *  an the stack, this will immediately lock the instance for the duration of the
	 *  current scope.
	 */
	ScopedLock(LOCK &lock): m_lock(lock) {
		m_lock.lock();
	}

	/** Release the underlying lock.
	 */
	~ScopedLock() {
		// never throw from a dtor; log instead.
		try {
			m_lock.unlock();
		} catch (ThreadException &e) {
			e.logAndClear(LOG_THREAD);
		}
	}

private:
	LOCK &m_lock;
};


/** Similar to ScopedLock, but unlocks the mutex when created and relocks it
 *  on destruction.
 */
template <class LOCK>
class ScopedUnlock: public NonCopyable {
public:
	/** Construct a new scoped unlock for a existing lockable instance.  If constructed
	 *  on the stack, this will immediately unlock the instance for the duration of the
	 *  current scope.
	 */
	ScopedUnlock(LOCK &lock): m_lock(lock) {
		m_lock.unlock();
	}

	/** Reacquire the underlying lock.
	 */
	~ScopedUnlock() {
		// never throw from a dtor; log instead.
		try {
			m_lock.lock();
		} catch (ThreadException &e) {
			e.logAndClear(LOG_THREAD);
		}
	}

private:
	LOCK &m_lock;
};


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_THREADUTIL_H__

