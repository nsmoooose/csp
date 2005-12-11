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
 * @file Synchronization.h
 * @brief PosixThreads synchronization and utility classes.
 */


#ifndef __CSPLIB_THREAD_SYNCHRONIZATION_H__
#define __CSPLIB_THREAD_SYNCHRONIZATION_H__

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Properties.h>
#include <csp/csplib/thread/ThreadUtil.h>

#include <cc++/thread.h>
#include <cerrno>
#include <cmath>

// TODO barrier, unit tests

CSP_NAMESPACE


/** Convert a timeout in seconds to a timeout_t in milliseconds.  The latter
 *  is used by cc++ synchronization classes.  Returns zero if timeout is
 *  less than or equal to zero.  Otherwise returns a value greater than zero
 *  (i.e., minimum one millisecond).
 */
inline timeout_t makeMilliTimeout(double timeout) {
	return (timeout <= 0) ? timeout_t(0) : static_cast<timeout_t>(std::max(1.0, 1000.0 * timeout));
}


/** Thin wrapper for cc++ Mutex.  Provides a recursive, mutually exclusive lock that
 *  can be used to serializing access to a shared resource, such that only one thread
 *  may use the resource at a time.  A mutex can be relocked by a thread without
 *  causing a deadlock.  The mutex will remain locked until it has been unlocked as
 *  many times as it was locked.
 */
class CSP_EXPORT Mutex: public NonCopyable {
public:
	/** Create a new mutex.
	 *
	 *  @param id An optional identifier string for debugging.
	 */
	Mutex(const char *id=0): m_mutex(id) { }

	/** Lock the mutex to gain exclusive use of the resources the mutex protects.
	 *  This method will block if another thread holds the mutex lock.
	 */
	void lock() { m_mutex.enterMutex(); }

	/** Try to lock the mutex.  Similar to lock(), but returns false immediately if
	 *  the mutex is already locked by another thread.
	 */
	bool tryLock() { return m_mutex.tryEnterMutex(); }

	/** Unlock the mutex so that other threads may use the resources the mutex
	 *  protects.  If the mutex has been locked multiple times by the same thread,
	 *  unlock must be called the same number of times to release the lock.
	 */
	void unlock() { m_mutex.leaveMutex(); }

private:
	ost::Mutex m_mutex;
};


/** Thin wrapper around cc++ ThreadLock, which allows a resource to be safely
 *  shared between threads that modify the resource and those that do not.
 *
 *  For resources that are accessed but not modified by multiple threads, this
 *  type of lock is generally more efficient than a mutex.  Multiple readers
 *  can access the resource simultaneously without waiting for locks.  Writers
 *  must wait until all reader locks are released (which can lead to writer
 *  starvation---see ThreadSafeQueue).  When a thread holds a write lock, no
 *  other threads (reader or writer) can obtain a lock, giving the writer
 *  exclusive access to the resource.
 */
class ReadWriteLock: public NonCopyable {
public:
	/** Construct a new lock
	 */
	ReadWriteLock(): m_rwlock() { }

	/** Obtain a lock for reading.
	 *
	 *  This method will block if any threads currently hold this lock for
	 *  writing.  Multiple threads may simultaneously hold a read lock.
	 */
	void lockRead() { m_rwlock.readLock(); }

	/** Obtain a lock for writing.
	 *
	 *  This method will block if any theads currently hold this lock for
	 *  reading or writing.  Write locks are exclusive: no other threads may
	 *  hold either a read or write lock at the same time.
	 */
	void lockWrite() { m_rwlock.writeLock(); }

	/** Try to acquire a read lock.
	 *
	 *  Similar to lockRead(), but returns immediately if any thread currently
	 *  holds a write lock.
	 *
	 *  @returns true if the lock was acquired, false otherwise.
	 */
	bool tryLockRead() { return m_rwlock.tryReadLock(); }

	/** Try to obtain a write lock.
	 *
	 *  Similar to lockWrite(), but returns immediately if any thread
	 *  currently holds either a read or a write lock.
	 *
	 *  @returns true if the lock was acquired, false otherwise.
	 */
	bool tryLockWrite() { return m_rwlock.tryWriteLock(); }

	/** Unlock (read or write).
	 *
	 *  This method must be called once for each successfully acquired read or
	 *  write lock.
	 */
	void unlock() { m_rwlock.unlock(); }

private:
	ost::ThreadLock m_rwlock;
};


// CC++ does not yet implement Conditional on win32, so it has been disabled
// here.  For more information on the difficulty of implementing condition
// variables using win32 threading primitives, see
//   http://www.cs.wustl.edu/~schmidt/win32-cv-1.html and
//   http://www.cs.wustl.edu/~schmidt/win32-cv-2.html

#if 0
/** Thin wrapper for cc++ Conditional.  Condition variables provide a signaling
 * mechanism to synchronize multiple threads.  Each condition variable is
 * associated with a mutex.  A thread first locks the mutex, then waits on the
 * condition variable.  If another thread signals the condition variable, one
 * (or all) of the threads waiting on that variable will be activated.
 */
class CSP_EXPORT Conditional: public NonCopyable {
public:

	/** Construct a condition variable, using an internal mutex.
	 *  @param id An optional identifier string for debugging.
	 */
	Conditional(const char *id=0): m_cond(id) { }

	/** Signal threads waiting on this condition variable to execute.
	 *
	 *  @param n the number of signals to send, with each signal
	 *         activating at most one thread.
	 */
	void signal(int n = 1) {
		while (--n >= 0) { m_cond.signal(false); }
	}

	/** Signal to all threads waiting on this condition variable to
	 *  execute.  Since each thread attempts to lock the associated
	 *  mutex when awoken, the threads will execute in series.
	 */
	void signalAll() { m_cond.signal(true); }

	/** Lock the underlying mutex.  This must be done before calling
	 *  wait().
	 */
	void lock() { m_cond.enterMutex(); }

	/** Try to lock the underlying mutex.
	 *
	 *  Similar to lock(), but returns false immediately if the mutex is
	 *  already locked by another thread.
	 */
	bool tryLock() { return m_cond.tryEnterMutex(); }

	/** Unlock the underlying mutex.
	 */
	void unlock() { m_cond.leaveMutex(); }

	/** Wait for the condition variable to be signaled.
	 *
	 *  @param timeout the maximum time, in seconds, to wait on the condition variable.
	 *    If zero, the call will wait indefinitely.
	 *  @param locked If true, the condition variable must be manually locked
	 *    before calling wait, and unlocked afterward.  If false, the locking
	 *    is done automatically.
	 *  @returns true if a signal occured, or false if the wait timed out.
	 */
	bool wait(double timeout, bool locked) {
		return m_cond.wait(makeMilliTimeout(timeout), locked);
	}

private:
	ost::Conditional m_cond;
};
#else
class CSP_EXPORT Conditional {};
#endif


/** A thin wrapper around cc++ Event.
 *
 *  Provides a simple api to allow one or more threads to wait
 *  until an event occurs.  The event condition is indicated by
 *  an internal flag that is shared between threads.
 */
class Event: public NonCopyable {
public:
	/** Signal all waiting threads to wake up.  The event must be reset
	 *  after calling signal() before another signal can be sent.
	 */
	void signal() { m_event.signal(); }

	/** Reset the event (see signal).
	 */
	void reset() { m_event.reset(); }

	/** Wait for the event to be signaled by another thread or for until the
	 *  specified time elapses.
	 *
	 *  @param timeout The maximum time (in seconds) to wait.
	 *  @returns true if an event occurred, false if the timeout expired.
	 */
	bool wait(double timeout) { return m_event.wait(makeMilliTimeout(timeout)); }

	/** Wait for the event to be signaled by another thread.
	 *
	 *  @returns true if an event occurred..
	 */
	bool wait() { return m_event.wait(); }

private:
	ost::Event m_event;
};


/** A thin wrapper around cc++ Semaphore.
 *
 * A semaphore can be used to share one or more resources between multiple threads.
 */
class Semaphore: public NonCopyable {
public:
	/** Initialize a semaphore with the specified resource count.
	 */
	Semaphore(size_t count): m_semaphore(count) { }

	/** Wait for an available resource, or until the specified timeout expires.
	 *
	 *  @param timeout The maximum time (in seconds) to wait, or zero for no limit.
	 *  @return true if a resource is available, false if the timeout expired first.
	 */
	bool wait(double timeout=0) { return m_semaphore.wait(makeMilliTimeout(timeout)); }

	/** Release a resource so that other waiting threads can use it.  Should only
	 *  be called after wait() has succeeded.
	 */
	void post() { m_semaphore.post(); }

private:
	ost::Semaphore m_semaphore;
};


// Define scoped locks that lock a synchronization primitive for the
// current scope and automatically release the lock when execution
// exits the current scope.  Using scoped locks is generally safer
// than manual lock and unlock calls.  Scoped "unlocks" are also
// provided to perform the reverse operation: unlock on creation and
// relock when the current scope exits.
#define CSP_SCOPEDLOCK(NAME, TARGET, LOCK, UNLOCK) \
	class NAME : public NonCopyable { \
		TARGET &m_target; \
	public: \
		NAME(TARGET &target): m_target(target) { target.LOCK(); } \
		~NAME() { m_target.UNLOCK(); } \
	};

CSP_SCOPEDLOCK(MutexLock, Mutex, lock, unlock)
CSP_SCOPEDLOCK(ReadLock, ReadWriteLock, lockRead, unlock)
CSP_SCOPEDLOCK(WriteLock, ReadWriteLock, lockWrite, unlock)
CSP_SCOPEDLOCK(SemaphoreLock, Semaphore, wait, post)
CSP_SCOPEDLOCK(MutexUnlock, Mutex, unlock, lock)
CSP_SCOPEDLOCK(ReadUnlock, ReadWriteLock, unlock, lockRead)
CSP_SCOPEDLOCK(WriteUnlock, ReadWriteLock, unlock, lockWrite)
CSP_SCOPEDLOCK(SemaphoreUnlock, Semaphore, post, wait)


CSP_NAMESPACE_END

#endif // __CSPLIB_THREAD_SYNCHRONIZATION_H__

