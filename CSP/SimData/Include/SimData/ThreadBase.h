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


#ifndef __SIMDATA_THREADBASE_H__
#define __SIMDATA_THREADBASE_H__

#include <SimData/Namespace.h>
#include <SimData/Properties.h>
#include <SimData/Timing.h>
#include <SimData/Ref.h>
#include <SimData/AtomicCounter.h>
#include <string>
#include <pthread.h>


NAMESPACE_SIMDATA


// TODO
// do something smart with thread return values
// semaphores
// inline some methods; move others to cpp?
// unit tests
// integrate win32 pthreads


/**
 */
class ThreadException: public std::exception {
public:
	typedef enum {
		NONE,
		BUSY,
		INTERNAL,
		DENIED,
		DESTROYED,
		DEADLOCK,
		REENTRANT,
	} ErrorType;

	ThreadException(const int error): m_error(NONE), m_message() {
		translate(error);
	}

	virtual ~ThreadException() throw() {}

	static void check(const int result) {
		if (result != 0) throw ThreadException(result);
	}

private:

	void translate(const int error) {
		switch (error) {
			case EDEADLK:
				m_error = DEADLOCK;
				break;
			case EAGAIN:
				m_error = REENTRANT;
				break;
			case EBUSY:
				m_error = BUSY;
			case EPERM:
				m_error = DENIED;
			default:
				m_error = INTERNAL;
		}
	}

private:
	int m_error;
	std::string m_message;
};


// forward declaration for friend relationship with ThreadMutex
class ThreadCondition;


/** Thin wrapper for pthreads mutually exclusive locks, which provide
 *  a means of serializing access to a shared resource, such that only
 *  one thread may use the resource at a time.
 */
class ThreadMutex {
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
		ThreadException::check(result);
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
		ThreadException::check(result);
	}

	/** Try to lock the mutex.
	 *
	 *  Similar to lock(), but returns false immediately if the mutex is
	 *  already locked by another thread.
	 */
	bool tryLock() {
		const int result = pthread_mutex_trylock(&m_mutex);
		if (result == EBUSY) return false;
		ThreadException::check(result);
		return true;
	}

	/** Unlock the mutex so that other threads may use the resources the
	 *  mutex protects.
	 */
	void unlock() {
		const int result = pthread_mutex_unlock(&m_mutex);
		ThreadException::check(result);
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
class ReadWriteLock {
public:
	/** Construct a new lock
	 */
	ReadWriteLock() {
		const int result = pthread_rwlock_init(&m_lock, 0);
		ThreadException::check(result);
	}

	/** Delete the lock and free internal resources.
	 */
	~ReadWriteLock() {
		const int result = pthread_rwlock_destroy(&m_lock);
		ThreadException::check(result);
	}

	/** Obtain a lock for reading.
	 *
	 *  This method will block if any threads currently hold this lock
	 *  for writing.  Multiple threads may simultaneously hold a read
	 *  lock.
	 */
	void lockRead() {
		const int result = pthread_rwlock_rdlock(&m_lock);
		ThreadException::check(result);
	}

	/** Obtain a lock for writing.
	 *
	 *  This method will block if any theads currently hold this lock
	 *  for reading or writing.  Write locks are exclusive: no other
	 *  threads may hold either a read or write lock at the same time.
	 */
	void lockWrite() {
		const int result = pthread_rwlock_wrlock(&m_lock);
		ThreadException::check(result);
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
		ThreadException::check(result);
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
		ThreadException::check(result);
		return true;
	}

	/** Unlock (read or write).
	 *
	 *  This method must be called once for each successfully acquired
	 *  read or write lock.
	 */
	void unlock() {
		const int result = pthread_rwlock_unlock(&m_lock);
		ThreadException::check(result);
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
class ThreadCondition {
public:

	/** Construct a condition variable, using an internal mutex.
	 */
	ThreadCondition(): m_mutex(m_local_mutex) {
		int result = pthread_cond_init(&m_cond, 0);
		ThreadException::check(result);
		result = pthread_mutex_init(&m_local_mutex, 0);
		ThreadException::check(result);
	}

	/** Construct a condition variable, using an external mutex.
	 */
	ThreadCondition(ThreadMutex &mutex): m_mutex(mutex.getRawMutex()) {
		int result = pthread_cond_init(&m_cond, 0);
		ThreadException::check(result);
	}

	/** Delete the condition variable and free internal resources.
	 */
	~ThreadCondition() {
		int result = pthread_cond_destroy(&m_cond);
		ThreadException::check(result);
		if (&m_local_mutex == &m_mutex) {
			result = pthread_mutex_destroy(&m_local_mutex);
			ThreadException::check(result);
		}
	}

	/** Signal threads waiting on this condition variable to execute.
	 *
	 *  @param n the number of signals to send, with each signal
	 *         activating at most one thread.
	 */
	void signal(int n = 1) {
		while (--n >= 0) {
			const int result = pthread_cond_signal(&m_cond);
			ThreadException::check(result);
		}
	}

	/** Signal to all threads waiting on this condition variable to
	 *  execute.  Since each thread attempts to lock the associated
	 *  mutex when awoken, the threads will execute in series.
	 */
	void signalAll() {
		const int result = pthread_cond_broadcast(&m_cond);
		ThreadException::check(result);
	}

	/** Lock the underlying mutex.  This must be done before calling
	 *  wait().
	 */
	void lock() {
		const int result = pthread_mutex_lock(&m_mutex);
		ThreadException::check(result);
	}

	/** Try to lock the underlying mutex.
	 *
	 *  Similar to lock(), but returns false immediately if the mutex is
	 *  already locked by another thread.
	 */
	bool tryLock() {
		const int result = pthread_mutex_trylock(&m_mutex);
		if (result == EBUSY) return false;
		ThreadException::check(result);
		return true;
	}

	/** Unlock the underlying mutex.
	 */
	void unlock() {
		const int result = pthread_mutex_unlock(&m_mutex);
		ThreadException::check(result);
	}

	/** Wait for the condition variable to be signaled.  Note that you must
	 *  lock the condition variable before calling wait(), and unlock it
	 *  afterwards.
	 *
	 *  @param timeout the maximum time, in seconds, to wait on the condition
	 *    variable.  If less than zero, this method will wait indefinitely.
	 *  @returns true if a signal occured, or false if the wait timed out.
	 */
	bool wait(double timeout = -1.0) {
		if (timeout < 0) {
			const int result = pthread_cond_wait(&m_cond, &m_mutex);
			ThreadException::check(result);
		} else {
			double abstime_sec = get_realtime() + timeout;
			timespec abstime;
			double sec = floor(abstime_sec);
			abstime.tv_sec = static_cast<int>(sec);
			abstime.tv_nsec = static_cast<int>(1e+9 * (abstime_sec - sec));
			const int result = pthread_cond_timedwait(&m_cond, &m_mutex, &abstime);
			if (result == ETIMEDOUT) return false;
			ThreadException::check(result);
		}
		return true;
	}

private:
	pthread_cond_t m_cond;
	pthread_mutex_t m_local_mutex;
	pthread_mutex_t &m_mutex;
	ThreadCondition(const ThreadCondition &);
	ThreadCondition &operator=(const ThreadCondition &);
};


/** A simple scoped mutex.  Used to protect a shared resource within
 *  the current scope, so that the lock is automatically released the
 *  ScopedLock goes out a scope.
 *
 *  For example:
 *
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
 */
template <class LOCK>
class ScopedLock {
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
		m_lock.unlock();
	}

private:
	LOCK &m_lock;
};


/** Similar to ScopedLock, but unlocks the mutex for the duration of
 *  its existence, then relocks the mutex on destruction.
 */
template <class LOCK>
class ScopedUnlock {
public:
	/** Construct a new scoped lock for a existing lockable instance.  If constructed
	 *  an the stack, this will immediately lock the instance for the duration of the
	 *  current scope.
	 */
	ScopedUnlock(LOCK &lock): m_lock(lock) {
		m_lock.unlock();
	}

	/** Release the underlying lock.
	 */
	~ScopedUnLock() {
		m_lock.lock();
	}

private:
	LOCK &m_lock;
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
class ThreadSpecific {
public:
	typedef TYPE ValueType;

	/** Construct a thread specific instance.  The underlying instance is
	 *  <i>not</i> created; it will be allocated and initialized on first
	 *  access.
	 */
	ThreadSpecific(): m_once(0), m_key(0) {
	}

	/** Destroy the thread-specific instances and release internal resources.
	 */
	~ThreadSpecific() {
		if (m_once) {
			TYPE *object = this->operator->();
			const int result = pthread_key_delete(m_key);
			ThreadException::check(result);
			delete object;
		}
	}

	/** Access the underlying instance for the current thread.
	 *
	 *  An instance will be created upon first access by a given thread.
	 *
	 *  @returns a pointer to the instance that is unique to the calling
	 *    thread.
	 */
	ValueType *operator->() {
		// the first call needs to construct a shared key that is
		// used to access the thread-specific data.
		if (!this->m_once) {
			ScopedLock<ThreadMutex> lock(this->m_keylock);
			if (!this->m_once) {
				const int result = pthread_key_create(&this->m_key, &this->cleanup_hook);
				ThreadException::check(result);
				this->m_once = true;
			}
		}

		// use the shared key to retrieve the data (ValueType*) for
		// this thread.
		ValueType *tss_data = reinterpret_cast<ValueType*>(pthread_getspecific(this->m_key));

		// if the pointer is null, this is the first access from this
		// thread and we need to allocate a new instance.
		if (tss_data == 0) {
			tss_data = new ValueType;
			const int result = pthread_setspecific(this->m_key, reinterpret_cast<void*>(tss_data));
			ThreadException::check(result);
		}

		// return the instance specific to the current thread.
		return tss_data;
	}

	/** Access the underlying instance for the current thread as a non-const
	 *  reference.
	 *
	 *  @see operator->()
	 */
	ValueType &operator*() {
		return *(this->operator->());
	}

private:
	pthread_key_t m_key;
	bool m_once;
	ThreadMutex m_keylock;

	/** Callback hook to deallocate the thread-specific instances when
	 *  the shared key is deleted.
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
class Event {
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


/** Base class for referenced counted objects that are shared between
 *  multiple threads.  Thread-safe, supporting atomic updates of the
 *  internal reference counter.
 */
typedef Referenced<AtomicCounter> ThreadSafeReferenced;


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_THREADBASE_H__
