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
 * @file ThreadBase.h
 * @brief PosixThreads wrapper and utility classes.
 */


#ifndef SIMDATA_NOTHREADS

#ifndef __SIMDATA_THREADBASE_H__
#define __SIMDATA_THREADBASE_H__

#include <SimData/Namespace.h>
#include <SimData/Properties.h>
#include <SimData/ThreadUtil.h>

#include <cerrno>
#include <cmath>
#include <pthread.h>


NAMESPACE_SIMDATA


// TODO
// semaphore
// barrier
// unit tests


/** Thin wrapper for pthreads mutually exclusive locks, which provide
 *  a means of serializing access to a shared resource, such that only
 *  one thread may use the resource at a time.
 */
class ThreadMutex: public NonCopyable {
	friend class ThreadCondition;
public:
	/** Mutex types:
	 *   <dl>
	 *   <dt>DEFAULT    <dd>same as NORMAL
	 *   <dt>NORMAL     <dd>non-reentrant; attempts to relock by a single thread,
	 *                      or to lock a mutex held by another thread that terminated
	 *                      will cause a deadlock.
	 *   <dt>RECURSIVE  <dd>reentrant; can be relocked by a thread without causing
	 *                      a deadlock.  The mutex will be held until it has been
	 *                      unlocked as many times as it was locked.
	 *   <dt>ERRORCHECK <dd>like NORMAL; but returns an error in situations that would
	 *                      normally result in a deadlock.
	 *   </dl>
	 */
	typedef enum { DEFAULT, NORMAL, RECURSIVE, ERRORCHECK } MutexType;

public:
	/** Create a new mutex.
	 *
	 *  @param type @see MutexType
	 */
	ThreadMutex(MutexType type = DEFAULT) {
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		switch (type) {
			case NORMAL:
				pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
				break;
			case RECURSIVE:
				pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
				break;
			case ERRORCHECK:
				pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
				break;
			default:
				break;
		}
		pthread_mutex_init(&m_mutex, &attr);
		pthread_mutexattr_destroy(&attr);
	}

	/** Destroy the mutex, freeing internal resources.
	 */
	~ThreadMutex() {
		const int result = pthread_mutex_destroy(&m_mutex);
		// not safe to throw from a dtor, so just log the error
		ThreadException::checkLog(result);
	}

	/** Lock the mutex to gain exclusive use of the resources the mutex
	 *  protects.
	 *
	 *  This method will block if another thread holds the mutex lock.
	 *  If the current thread has already locked the mutex, the behavior
	 *  depends on the type of mutex specified during construction (see
	 *  ThreadMutex::MutexType).
	 */
	void lock() {
		const int result = pthread_mutex_lock(&m_mutex);
		ThreadException::checkThrow(result);
	}

	/** Try to lock the mutex.
	 *
	 *  Similar to lock(), but returns false immediately if the mutex is
	 *  already locked by another thread.
	 */
	bool tryLock() {
		const int result = pthread_mutex_trylock(&m_mutex);
		if (result == EBUSY) return false;
		ThreadException::checkThrow(result);
		return true;
	}

	/** Unlock the mutex so that other threads may use the resources the
	 *  mutex protects.
	 */
	void unlock() {
		const int result = pthread_mutex_unlock(&m_mutex);
		ThreadException::checkThrow(result);
	}
private:
	pthread_mutex_t m_mutex;
	pthread_mutex_t & getRawMutex() { return m_mutex; }
	ThreadMutex(const ThreadMutex &);
	ThreadMutex &operator=(const ThreadMutex &);
};


/** Thin wrapper around an rwlock, which allows a resource to be safely
 *  shared between threads that modify the resource and those that do
 *  not.
 *
 *  For resources that are accessed but not modified by multiple
 *  threads, this type of lock is generally more efficient than a
 *  mutex.  Multiple readers can access the resource simultaneously
 *  without waiting for locks.  Writers must wait until all reader
 *  locks are released (which can lead to writer starvation---see
 *  ThreadSafeQueue).  When a thread holds a write lock, no other
 *  threads (reader or writer) can obtain a lock, giving the writer
 *  exclusive access to the resource.
 */
class ReadWriteLock: public NonCopyable {
public:
	/** Construct a new lock
	 */
	ReadWriteLock() {
		const int result = pthread_rwlock_init(&m_lock, 0);
		ThreadException::checkThrow(result);
	}

	/** Delete the lock and free internal resources.
	 */
	~ReadWriteLock() {
		const int result = pthread_rwlock_destroy(&m_lock);
		// not safe to throw from a dtor, so just log the error
		ThreadException::checkLog(result);
	}

	/** Obtain a lock for reading.
	 *
	 *  This method will block if any threads currently hold this lock
	 *  for writing.  Multiple threads may simultaneously hold a read
	 *  lock.
	 */
	void lockRead() {
		const int result = pthread_rwlock_rdlock(&m_lock);
		ThreadException::checkThrow(result);
	}

	/** Obtain a lock for writing.
	 *
	 *  This method will block if any theads currently hold this lock
	 *  for reading or writing.  Write locks are exclusive: no other
	 *  threads may hold either a read or write lock at the same time.
	 */
	void lockWrite() {
		const int result = pthread_rwlock_wrlock(&m_lock);
		ThreadException::checkThrow(result);
	}

	/** Try to acquire a read lock.
	 *
	 *  Similar to lockRead(), but returns immediately if any thread
	 *  currently holds a write lock.
	 *
	 *  @returns true if the lock was acquired, false otherwise.
	 */
	bool tryLockRead() {
		const int result = pthread_rwlock_tryrdlock(&m_lock);
		if (result == EBUSY) return false;
		ThreadException::checkThrow(result);
		return true;
	}

	/** Try to obtain a write lock.
	 *
	 *  Similar to lockWrite(), but returns immediately if any thread
	 *  currently holds either a read or a write lock.
	 *
	 *  @returns true if the lock was acquired, false otherwise.
	 */
	bool tryLockWrite() {
		const int result = pthread_rwlock_trywrlock(&m_lock);
		if (result == EBUSY) return false;
		ThreadException::checkThrow(result);
		return true;
	}

	/** Unlock (read or write).
	 *
	 *  This method must be called once for each successfully acquired
	 *  read or write lock.
	 */
	void unlock() {
		const int result = pthread_rwlock_unlock(&m_lock);
		ThreadException::checkThrow(result);
	}

private:
	pthread_rwlock_t m_lock;
};


/** Thin wrapper for a pthreads condition variable.  Condition variables
 *  provide a signaling mechanism to synchronize multiple threads.  Each
 *  condition variable is associated with a mutex.  A thread first locks
 *  the mutex, then waits on the condition variable.  If another thread
 *  signals the condition variable, one (or all) of the threads waiting
 *  on that variable will be activated.
 */
class ThreadCondition: public NonCopyable {
public:

	/** Construct a condition variable, using an internal mutex.
	 */
	ThreadCondition(): m_mutex(m_local_mutex) {
		int result = pthread_cond_init(&m_cond, 0);
		ThreadException::checkThrow(result);
		result = pthread_mutex_init(&m_local_mutex, 0);
		ThreadException::checkThrow(result);
	}

	/** Construct a condition variable, using an external mutex.
	 */
	ThreadCondition(ThreadMutex &mutex): m_mutex(mutex.getRawMutex()) {
		int result = pthread_cond_init(&m_cond, 0);
		ThreadException::checkThrow(result);
	}

	/** Delete the condition variable and free internal resources.
	 */
	~ThreadCondition();

	/** Signal threads waiting on this condition variable to execute.
	 *
	 *  @param n the number of signals to send, with each signal
	 *         activating at most one thread.
	 */
	void signal(int n = 1) {
		while (--n >= 0) {
			const int result = pthread_cond_signal(&m_cond);
			ThreadException::checkThrow(result);
		}
	}

	/** Signal to all threads waiting on this condition variable to
	 *  execute.  Since each thread attempts to lock the associated
	 *  mutex when awoken, the threads will execute in series.
	 */
	void signalAll() {
		const int result = pthread_cond_broadcast(&m_cond);
		ThreadException::checkThrow(result);
	}

	/** Lock the underlying mutex.  This must be done before calling
	 *  wait().
	 */
	void lock() {
		const int result = pthread_mutex_lock(&m_mutex);
		ThreadException::checkThrow(result);
	}

	/** Try to lock the underlying mutex.
	 *
	 *  Similar to lock(), but returns false immediately if the mutex is
	 *  already locked by another thread.
	 */
	bool tryLock() {
		const int result = pthread_mutex_trylock(&m_mutex);
		if (result == EBUSY) return false;
		ThreadException::checkThrow(result);
		return true;
	}

	/** Unlock the underlying mutex.
	 */
	void unlock() {
		const int result = pthread_mutex_unlock(&m_mutex);
		ThreadException::checkThrow(result);
	}

	/** Wait for the condition variable to be signaled.  Note that you must
	 *  lock the condition variable before calling wait(), and unlock it
	 *  afterwards.
	 *
	 *  @param timeout the maximum time, in seconds, to wait on the condition
	 *    variable.  If less than zero, this method will wait indefinitely.
	 *  @returns true if a signal occured, or false if the wait timed out.
	 */
	bool wait(double timeout = -1.0);

private:
	pthread_cond_t m_cond;
	pthread_mutex_t m_local_mutex;
	pthread_mutex_t &m_mutex;
};


/** Wrapper for allocating a global resource such that each thread receives
 *  a separate, private instance.  Thread specific instances do not require
 *  any locking, since each thread has its own copy.
 *
 *  Schmidt, Harrison, and Pryce (Thread-Specific Storage for C/C++) provide
 *  a good background on thread specific storage, as well as useful criteria
 *  for when, and when not, to use this pattern.
 */
template <class TYPE>
class ThreadSpecific: public NonCopyable {

public:
	typedef TYPE ValueType;

protected:
	/** Create a new instance of ValueType for a thread.
	 *
	 *  Called on demand, as new threads first access the thread-specific
	 *  pointer.  Override this method to provide customized construction.
	 *  The returned pointer must be allocated with new (it will be freed
	 *  with delete when the thread terminates).
	 */
	virtual ValueType * createNew() const {
		return new ValueType;
	}

public:
	/** Construct a thread specific instance.  The underlying instance is
	 *  <i>not</i> created; it will be allocated and initialized on first
	 *  access.
	 */
	ThreadSpecific(): m_once(0), m_key(0) {
	}

	/** Destroy the thread-specific instances and release internal resources.
	 */
	virtual ~ThreadSpecific();

	/** Access the underlying instance for the current thread.
	 *
	 *  An instance will be created upon first access by a given thread.
	 *
	 *  @returns a pointer to the instance that is unique to the calling
	 *    thread.
	 */
	ValueType *operator->();

	/** Access the underlying instance for the current thread as a non-const
	 *  reference.
	 *
	 *  @see operator->()
	 */
	inline ValueType &operator*() {
		return *(this->operator->());
	}

private:
	bool m_once;
	pthread_key_t m_key;
	ThreadMutex m_keylock;

	/** Callback hook to deallocate the thread-specific instances as
	 *  each thread finishes.
	 */
	static void cleanup_hook(void *ptr) {
		delete static_cast<ValueType*>(ptr);
	}
};


/** A thread-signaling class based on a condition variable.
 *
 *  Provides a simple api to allow one or more threads to wait
 *  until an event occurs.  The event condition is indicated by
 *  an internal flag that is shared between threads.
 */
class Event: public NonCopyable {
public:

    /** Initialize a new event instance.
	 */
	Event(): m_flag(false) { }

	/** Test if the internal flag is true.
	 */
	bool isSet() const {
		return m_flag;
	}

	/** Set the internal flag and signal all waiting threads to wake up.
	 */
	void set() {
		m_cond.lock();
		m_flag = true;
		m_cond.unlock();
		m_cond.signalAll();
	}

	/** Clear the internal flag.
	 */
	void clear() {
		m_cond.lock();
		m_flag = false;
		m_cond.unlock();
	}

	/** Wait for the event flag to be set.
	 *
	 *  @param timeout The maximum time (in seconds) to wait.  Negative
	 *    values mean no timeout.
	 *  @returns true if an event occurred, false if the timeout expired.
	 */
	bool wait(double timeout = -1.0) {
		return m_cond.wait(timeout);
	}

private:
	ThreadCondition m_cond;
	bool m_flag;
};


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_THREADBASE_H__

#endif // SIMDATA_NOTHREADS
