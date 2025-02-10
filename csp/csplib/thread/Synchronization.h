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
 * @file Synchronization.h
 * @brief PosixThreads synchronization and utility classes.
 */

#include <csp/csplib/util/Properties.h>

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace csp {

// This code is not used. But could be valuable if we need to use this type
// of code in the future.
#if 0

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

#endif

/** A thin wrapper around cc++ Event.
 *
 *  Provides a simple api to allow one or more threads to wait
 *  until an event occurs.  The event condition is indicated by
 *  an internal flag that is shared between threads.
 */
class Event: public NonCopyable {
public:
	Event() : m_signaled(false) {}

	/** Signal all waiting threads to wake up.  The event must be reset
	 *  after calling signal() before another signal can be sent.
	 */
	void signal() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_signaled = true;
        m_condition.notify_all();
	}

	/** Reset the event (see signal).
	 */
	void reset() {
		std::lock_guard<std::mutex> lock(m_mutex);
        m_signaled = false;
	}

	/** Wait for the event to be signaled by another thread or for until the
	 *  specified time elapses.
	 *
	 *  @param timeout The maximum time (in seconds) to wait.
	 *  @returns true if an event occurred, false if the timeout expired.
	 *
	 *  Example: myEvent.wait_for(std::chrono::seconds(3))
	 */
    template <typename Rep, typename Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_condition.wait_for(lock, timeout, [this]() {
			return m_signaled;
		});
    }

	/** Wait for the event to be signaled by another thread.
	 *
	 *  @returns true if an event occurred..
	 */
	void wait() {
		std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this]() {
			return m_signaled;
		});
	}

private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_signaled;
};

// This code is not used. But could be valuable if we need to use this type
// of code in the future.
#if 0
/** A thin wrapper around cc++ Semaphore.
 *
 * A semaphore can be used to share one or more resources between multiple threads.
 */
class Semaphore: public NonCopyable {
public:
	/** Initialize a semaphore with the specified resource count.
	 */
	Semaphore(size_t count): m_semaphore(count) { }

	/** Wait for an available resource.
	 */
	void wait() { m_semaphore.wait(); }

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

CSP_SCOPEDLOCK(ReadLock, ReadWriteLock, lockRead, unlock)
CSP_SCOPEDLOCK(WriteLock, ReadWriteLock, lockWrite, unlock)
CSP_SCOPEDLOCK(SemaphoreLock, Semaphore, wait, post)
CSP_SCOPEDLOCK(ReadUnlock, ReadWriteLock, unlock, lockRead)
CSP_SCOPEDLOCK(WriteUnlock, ReadWriteLock, unlock, lockWrite)
CSP_SCOPEDLOCK(SemaphoreUnlock, Semaphore, post, wait)

#endif

} // namespace csp
