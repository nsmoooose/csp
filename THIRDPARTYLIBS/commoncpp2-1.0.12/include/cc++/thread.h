// Copyright (C) 1999-2003 Open Source Telecom Corporation.
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// As a special exception to the GNU General Public License, permission is 
// granted for additional uses of the text contained in its release 
// of Common C++.
// 
// The exception is that, if you link the Common C++ library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the Common C++ library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
// 
// This exception applies only to the code released under the 
// name Common C++.  If you copy code from other releases into a copy of
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
// 
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

/**
 * @file thread.h
 * @short Synchronization and threading services.
 **/

#ifndef	CCXX_THREAD_H_
#define	CCXX_THREAD_H_

#ifndef WIN32
#define	CCXX_POSIX
#endif // !WIN32

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#ifndef	CCXX_EXCEPTION_H_
#include <cc++/exception.h>
#endif

#ifndef WIN32
#if (defined(__FreeBSD__) && __FreeBSD__ <= 3) || defined(_AIX)
#define	CCXX_SYSV_SEMAPHORES
#endif

#ifdef  MACOSX
#define CCXX_NAMED_SEMAPHORES
#define CCXX_BROKEN_SEMGETVALUE
#endif

#ifndef	HAVE_PTHREAD_H
#include <pthread.h>
#ifndef CCXX_SYSV_SEMAPHORES
#include <semaphore.h>
#endif
#endif
#endif // !WIN32

#undef CCXX_USE_WIN32_ATOMIC
#ifndef WIN32
#include <time.h>
#include <signal.h>
#include <unistd.h>

#ifdef	_THR_UNIXWARE
#undef	PTHREAD_MUTEXTYPE_RECURSIVE
#endif

typedef	pthread_t	cctid_t;
typedef	unsigned long	timeout_t;

/*
#if defined(__CYGWIN32__) 
__declspec(dllimport) long __stdcall InterlockedIncrement(long *);
__declspec(dllimport) long __stdcall InterlockedDecrement(long *);
__declspec(dllimport) long __stdcall InterlockedExchange(long *, long);
#define CCXX_USE_WIN32_ATOMIC 1
#endif
*/

#else // WIN32
typedef	DWORD	cctid_t;
typedef DWORD   timeout_t;

#define	MAX_SEM_VALUE	1000000
#define CCXX_USE_WIN32_ATOMIC 1
#if defined(__CYGWIN32__) 
//#include <Windows32/CommonFunctions.h>
#else
#if _MSC_VER < 1300
__declspec(dllimport) long __stdcall InterlockedIncrement(long *);
__declspec(dllimport) long __stdcall InterlockedDecrement(long *);
__declspec(dllimport) long __stdcall InterlockedExchange(long *, long);
#endif
#endif

#endif // !WIN32

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

class CCXX_CLASS_EXPORT Thread;
class CCXX_CLASS_EXPORT ThreadKey;

#define TIMEOUT_INF ~((timeout_t) 0)

#define	ENTER_CRITICAL	enterMutex();
#define	LEAVE_CRITICAL	leaveMutex();
#define	ENTER_DEFERRED	setCancel(cancelDeferred);
#define LEAVE_DEFERRED 	setCancel(cancelImmediate);

#ifndef WIN32
// These macros override common functions with thread-safe versions. In
// particular the common "libc" sleep() has problems since it normally
// uses SIGARLM (as actually defined by "posix").  The pthread_delay and
// usleep found in libpthread are gaurenteed not to use SIGALRM and offer
// higher resolution.  psleep() is defined to call the old process sleep.

#undef	sleep
#define	psleep(x)	(sleep)(x)

#endif // !WIN32

#undef Yield

#ifdef WIN32
CCXX_EXPORT(DWORD) waitThread(HANDLE hRef, timeout_t timeout);
#endif

class Conditional;
class CCXX_CLASS_EXPORT Event;

/**
 * The Mutex class is used to protect a section of code so that at any
 * given time only a single thread can perform the protected operation.
 * 
 * The Mutex can be used as a base class to protect access in a derived
 * class.  When used in this manner, the ENTER_CRITICAL and LEAVE_CRITICAL
 * macros can be used to specify when code written for the derived class
 * needs to be protected by the default Mutex of the derived class, and
 * hence is presumed to be 'thread safe' from multiple instance execution.
 * One of the most basic Common C++ synchronization object is the Mutex
 * class.  A Mutex only allows one thread to continue execution at a given
 * time over a specific section of code.  Mutex's have a enter and leave
 * method; only one thread can continue from the Enter until the Leave is
 * called.  The next thread waiting can then get through.  Mutex's are also
 * known as "CRITICAL SECTIONS" in win32-speak.
 * 
 * The Mutex is always recursive in that if the same thread invokes
 * the same mutex lock multiple times, it must release it multiple times.
 * This allows a function to call another function which also happens to
 * use the same mutex lock when called directly. This was
 * deemed essential because a mutex might be used to block individual file
 * requests in say, a database, but the same mutex might be needed to block a
 * whole series of database updates that compose a "transaction" for one
 * thread to complete together without having to write alternate non-locking
 * member functions to invoke for each part of a transaction.
 * 
 * Strangely enough, the original pthread draft standard does not directly
 * support recursive mutexes.  In fact this is the most common "NP" extension
 * for most pthread implementations.  Common C++ emulates recursive mutex
 * behavior when the target platform does not directly support it.
 * 
 * In addition to the Mutex, Common C++ supports a rwlock class.  This
 * implements the X/Open recommended "rwlock".  On systems which do not
 * support rwlock's, the behavior is emulated with a Mutex; however, the
 * advantage of a rwlock over a mutex is then entirely lost.  There has been
 * some suggested clever hacks for "emulating" the behavior of a rwlock with
 * a pair of mutexes and a semaphore, and one of these will be adapted for
 * Common C++ in the future for platforms that do not support rwlock's
 * directly.
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Mutex lock for protected access.
 */
class CCXX_CLASS_EXPORT Mutex
{
	friend class Conditional;
	friend class Event;
private:
#ifndef WIN32
#ifndef	PTHREAD_MUTEXTYPE_RECURSIVE
	volatile int _level;
	volatile Thread *_tid;
#endif
	/*
	 * Pthread mutex object.  This is protected rather than private
	 * because some mixed mode pthread operations require a mutex as
	 * well as their primary pthread object.  A good example of this
	 * is the Event class, as waiting on a conditional object must be
	 * associated with an accessable mutex.  An alternative would be
	 * to make such classes "friend" classes of the Mutex.
	 */
	pthread_mutex_t	_mutex;
#else
	HANDLE mutex;
#endif

public:
	/**
	 * The mutex is always initialized as a recursive entity.
	 */
	Mutex();

	/**
	 * Destroying the mutex removes any system resources associated
	 * with it.  If a mutex lock is currently in place, it is presumed
	 * to terminate when the Mutex is destroyed.
	 */
	virtual ~Mutex();

	/**
	 * Entering a Mutex locks the mutex for the current thread.  This
	 * also can be done using the ENTER_CRITICAL macro or by using the
	 * ++ operator on a mutex.
	 * 
	 * @see #leaveMutex
	 */
	void enterMutex(void);

	/**
	 * Tries to lock the mutex for the current thread. Behaves like
	 * #enterMutex , except that it doesn't block the calling thread
	 * if the mutex is already locked by another thread.
	 *
	 * @return true if locking the mutex was succesful otherwise false
	 *
	 * @see enterMutex
	 * @see leaveMutex
	 */
	bool tryEnterMutex(void);

	/**
	 * Leaving a mutex frees that mutex for use by another thread.  If
	 * the mutex has been entered (invoked) multiple times (recursivily)
	 * by the same thread, then it will need to be exited the same number
	 * of instances before it is free for re-use.  This operation can
	 * also be done using the LEAVE_CRITICAL macro or by the -- operator
	 * on a mutex.
	 * 
	 * @see #enterMutex
	 */
	void leaveMutex(void);
};

/**
 * The MutexLock class is used to protect a section of code so that at any
 * given time only a single thread can perform the protected operation.
 * 
 * It use Mutex to protect operation. Using this class is usefull and 
 * exception safe.
 * 
 * A common use is
 * 
 * void func_to_protect()
 * {
 *   MutexLock lock(mutex);
 *   ... operation ...
 * }
 *
 * NOTE: do not declare variable as "MutexLock (mutex)", the mutex will be 
 * released at statement end.
 * 
 * @author Frediano Ziglio <freddy77@angelfire.com>
 * @short Mutex automatic locker for protected access.
 */
class MutexLock
{
private:
	Mutex& mutex;
public:
	/**
	 * Acquire the mutex
	 */
	MutexLock( Mutex& _mutex ) : mutex( _mutex ) 
		{ mutex.enterMutex(); }
	/**
	 * Release the mutex automatically
	 */
	// this should be not-virtual
	~MutexLock()
		{ mutex.leaveMutex(); }
};

/**
 * The ThreadLock class impliments a thread rwlock for optimal reader performance
 * on systems which have rwlock support, and reverts to a simple mutex for those
 * that do not.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Posix rwlock extension for protected access.
 */
class CCXX_CLASS_EXPORT ThreadLock
{
private:
#ifdef HAVE_PTHREAD_RWLOCK
	pthread_rwlock_t _lock;
#else
	Mutex mutex;
#endif

public:
	/**
	 * Create a process shared thread lock object.
	 */
	ThreadLock();

	/**
	 * Destroy a process shared thread lock object.
	 */
	virtual ~ThreadLock();

	/**
	 * Aquire a read lock for the current object.
	 */
	void readLock(void);

	/**
	 * Aquire a write lock for the current object.
	 */
	void writeLock(void);

	/**
	 * Attempt read lock for current object.
	 *
	 * @return true on success.
	 */
	bool tryReadLock(void);

	/**
	 * Attempt write lock for current object.
	 *
	 * @return true on success.
	 */
	bool tryWriteLock(void);

	/**
	 * Release any held locks.
	 */
	void unlock(void);
};

/**
 * The Mutex Counter is a counter variable which can safely be incremented
 * or decremented by multiple threads.  A Mutex is used to protect access
 * to the counter variable (an integer).  An initial value can be specified 
 * for the counter, and it can be manipulated with the ++ and -- operators.
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Thread protected integer counter.
 */
class CCXX_CLASS_EXPORT MutexCounter : public Mutex
{
private:
	int	counter;

public:
	MutexCounter();
	MutexCounter(int initial);

	friend CCXX_EXPORT(int) operator++(MutexCounter &mc);
	friend CCXX_EXPORT(int) operator--(MutexCounter &mc);
};

/**
 * The AtomicCounter class offers thread-safe manipulation of an integer
 * counter.  These are commonly used for building thread-safe "reference"
 * counters for C++ classes.  The AtomicCounter depends on the platforms
 * support for "atomic" integer operations, and can alternately substitute
 * a "mutex" if no atomic support exists.
 *
 * @author Sean Cavanaugh <sean@dimensionalrift.com>
 * @short atomic counter operation.
 */
class CCXX_CLASS_EXPORT AtomicCounter
{
#ifndef CCXX_USE_WIN32_ATOMIC
private:
#ifdef	HAVE_ATOMIC
	atomic_t atomic;
#else
	int counter;
	Mutex lock;
#endif

public:
	/**
	 * Initialize an atomic counter to 0.
	 */
	AtomicCounter();

	/**
	 * Initialize an atomic counter to a known value.
	 *
	 * @param value initial value.
	 */
	AtomicCounter(int value);

	int operator++(void);
	int operator--(void);
	int operator+=(int change);
	int operator-=(int change);
	int operator+(int change);
	int operator-(int change);
	int operator=(int value);
	bool operator!(void);
	operator int();
#else
private:
	long atomic;

public:
	inline AtomicCounter()
		{atomic = 0;};

	inline AtomicCounter(int value)
		{atomic = value;};

	inline int operator++(void)
		{return InterlockedIncrement(&atomic);};

	inline int operator--(void)
		{return InterlockedDecrement(&atomic);};

	int operator+=(int change);

	int operator-=(int change);

	inline int operator+(int change)
		{return atomic + change;};

	inline int operator-(int change)
		{return atomic - change;};
	
	inline int operator=(int value)
		{return InterlockedExchange(&atomic, value);};

	inline bool operator!(void)
		{return (atomic == 0) ? true : false;};

	inline operator int()
		{return atomic;};
#endif
};

#ifndef WIN32
/**
 * A conditional variable synchcronization object for one to one and
 * one to many signal and control events between processes.
 * Conditional variables may wait for and receive signals to notify
 * when to resume or perform operations.  Multiple waiting threads may
 * be woken with a broadcast signal.
 *
 * @warning While this class inherits from Mutex, the methods of the
 * class Conditional just handle the system conditional variable, so
 * the user is responsible for calling enterMutex and leaveMutex so as
 * to avoid race conditions. Another thing to note is that if you have
 * several threads waiting on one condition, not uncommon in thread
 * pools, each thread must take care to manually unlock the mutex if
 * cancellation occurs. Otherwise the first thread cancelled will
 * deadlock the rest of the thread.
 *
 * @author David Sugar
 * @short conditional.
 * @todo implement in win32
 */
class Conditional : public Mutex
{
private:
	pthread_cond_t _cond;

public:
	/**
	 * Create an instance of a conditional.
	 */
	Conditional();

	/**
	 * Destroy the conditional.
	 */
	virtual ~Conditional();

	/**
	 * Signal a conditional object and a waiting threads.
	 *
	 * @param broadcast this signal to all waiting threads if true.
	 */
	void signal(bool broadcast);

	/**
 	 * Wait to be signaled from another thread.
	 */
	void wait(timeout_t timer = 0);	
};
#endif

/**
 * A semaphore is generally used as a synchronization object between multiple
 * threads or to protect a limited and finite resource such as a memory or
 * thread pool.  The semaphore has a counter which only permits access by
 * one or more threads when the value of the semaphore is non-zero.  Each
 * access reduces the current value of the semaphore by 1.  One or more
 * threads can wait on a semaphore until it is no longer 0, and hence the
 * semaphore can be used as a simple thread synchronization object to enable
 * one thread to pause others until the thread is ready or has provided data
 * for them.  Semaphores are typically used as a
 * counter for protecting or limiting concurrent access to a given
 * resource, such as to permitting at most "x" number of threads to use
 * resource "y", for example.   
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Semaphore counter for thread synchronization. 
 */
class CCXX_CLASS_EXPORT Semaphore
{
private:
#ifndef WIN32
#ifdef	CCXX_SYSV_SEMAPHORES
	int _semaphore;
#else
	sem_t _semaphore;
	sem_t *_semobject;
#endif
#else // WIN32
	HANDLE	semObject;
#endif // !WIN32

public:
	/**
	 * The initial value of the semaphore can be specified.  An initial
	 * value is often used When used to lock a finite resource or to 
	 * specify the maximum number of thread instances that can access a 
	 * specified resource.
	 * 
	 * @param resource specify initial resource count or 0 default.
	 */
	Semaphore(size_t resource = 0);

	/**
	 * Destroying a semaphore also removes any system resources
	 * associated with it.  If a semaphore has threads currently waiting
	 * on it, those threads will all continue when a semaphore is
	 * destroyed.
	 */
	virtual ~Semaphore();

	/**
	 * Wait is used to keep a thread held until the semaphore counter
	 * is greater than 0.  If the current thread is held, then another
	 * thread must increment the semaphore.  Once the thread is accepted, 
	 * the semaphore is automatically decremented, and the thread 
	 * continues execution.
	 * 
	 * The pthread semaphore object does not support a timed "wait", and
	 * hence to maintain consistancy, neither the posix nor win32 source
	 * trees support "timed" semaphore objects.
	 * 
	 * @see #post
	 */
	void wait(void);

	/**
	 * TryWait is a non-blocking variant of Wait. If the semaphore counter
	 * is greater than 0, then the thread is accepted and the semaphore
	 * counter is decreased. If the semaphore counter is 0 TryWait returns
	 * immediately with false.
	 *
	 * @return true if thread is accepted otherwise false
	 *
	 * @see #wait
	 * @see #post
	 */
	bool tryWait(void);

	/**
	 * Posting to a semaphore increments its current value and releases
	 * the first thread waiting for the semaphore if it is currently at
	 * 0.  Interestingly, there is no support to increment a semaphore by
	 * any value greater than 1 to release multiple waiting threads in
	 * either pthread or the win32 API.  Hence, if one wants to release
	 * a semaphore to enable multiple threads to execute, one must perform
	 * multiple post operations.
	 * 
	 * @see #wait
	 */
	void post(void);

	// FIXME: how implement getValue for posix compatibility ?
	/**
	 * Get the current value of a semaphore.
	 *
	 * @return current value.
	 */
#ifndef WIN32
#ifndef __CYGWIN32__
	int getValue(void);
#endif
#endif
};

/**
 * The Event class implements a feature originally found in the WIN32 API;
 * event notification.  A target thread waits on a resetable Event, and one
 * or more other threads can then signal the waiting thread to resume 
 * execution.  A timeout can be used to specify a wait duration in 
 * milliseconds.  The Event class must be reset before it can be used again 
 * as a trigger.  These event objects
 * use a trigger/reset mechanism and are related to low level conditional
 * variables.
 * 
 * @author: David Sugar <dyfet@ostel.com>
 * @short Thread synchornization on event notification.
 */
class CCXX_CLASS_EXPORT Event
{
private:
#ifndef WIN32
	Mutex mutex;
	pthread_cond_t _cond;
	bool _signaled;
	int _count;
#else
	HANDLE cond;
#endif

public:
	Event();

	virtual ~Event();

	/**
	 * Once signaled, the Event class must be "reset" before responding
	 * to a new signal.
	 * 
	 * @see #signal
	 */
	void reset(void);

	/**
	 * Signal the event for the waiting thread.
	 */
	void signal(void);

	/**
	 * Wait either for the event to be signaled by another thread or
	 * for the specified timeout duration.
	 * 
	 * @see #signal
	 * @return true if signaled, false if timed out.
	 * @param timer timeout in milliseconds to wait for a signal.
	 */
	bool wait(timeout_t timer);
	bool wait(void);
};

/**
 * The buffer class represents an IPC service that is built upon a buffer
 * of fixed capacity that can be used to transfer objects between one or
 * more producer and consumer threads.  Producer threads post objects
 * into the buffer, and consumer threads wait for and receive objects from
 * the buffer.  Semaphores are used to to block the buffer from overflowing
 * and indicate when there is data available, and mutexes are used to protect
 * multiple consumers and producer threads from stepping over each other.
 * 
 * The buffer class is an abstract class in that the actual data being
 * buffered is not directly specified within the buffer class itself.  The
 * buffer class should be used as a base class for a class that actually
 * impliments buffering and which may be aware of the data types actually
 * are being buffered.  A template class could be created based on buffer
 * for this purpose.  Another possibility is to create a class derived
 * from both Thread and Buffer which can be used to implement message passing
 * threads.
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Producer/Consumer buffer for use between threads.
 */
class CCXX_CLASS_EXPORT Buffer
{
private:
	Mutex lock_head, lock_tail;
	Semaphore size_head, size_tail;
	size_t _size;
	size_t _used;

protected:
	/**
	 * Invoke derived class buffer peeking method.
	 * @return size of object found.
	 * @param buf pointer to copy contents of head of buffer to.
	 */
	virtual int onPeek(void *buf) = 0;
	/**
	 * Invoke derived class object request from buffer.
	 * @return size of object returned.
	 * @param buf pointer to hold object returned from the buffer.
	 */
	virtual int onWait(void *buf) = 0;
	/**
	 * Invoke derived class posting of object to buffer.
	 * @return size of object posted.
	 * @param buf pointer to object being posted to the buffer.
	 */
	virtual int onPost(void *buf) = 0;

public:
	/**
	 * Create a buffer object of known capacity.
	 * @param capacity is the integer capacity of the buffer.
	 */
	Buffer(size_t capacity);
	/**
	 * In derived functions, may be used to free the actual memory
	 * used to hold buffered data.
	 */
	virtual ~Buffer()
		{return;};

	/**
	 * Return the capacity of the buffer as specified at creation.
	 * @return size of buffer.
	 */
	inline size_t getSize(void)
		{return _size;};
	
	/**
	 * Return the current capacity in use for the buffer.  Free space
	 * is technically getSize() - getUsed().
	 * @return integer used capacity of the buffer.
	 * @see #getSize
	 */
	inline size_t getUsed(void)
		{return _used;};

	/**
	 * Let one or more threads wait for an object to become available
	 * in the buffer.  The waiting thread(s) will wait forever if no
	 * object is ever placed into the buffer.
	 * 
	 * @return size of object passed by buffer in bytes.
	 * @param buf pointer to store object retrieved from the buffer.
	 */
	int wait(void *buf);

	/**
	 * Post an object into the buffer and enable a waiting thread to
	 * receive it.
	 * 
	 * @return size of object posted in bytes.
	 * @param buf pointer to object to store in the buffer.
	 */
	int post(void *buf);

	/**
	 * Peek at the current content (first object) in the buffer.
	 * 
	 * @return size of object in the buffer.
	 * @param buf pointer to store object found in the buffer.
	 */
	int peek(void *buf);

	/**
	 * New virtual to test if buffer is a valid object.
	 * @return true if object is valid.
	 */
	virtual bool isValid(void)
		{return true;};
};

/**
 * A buffer class that holds a known capacity of fixed sized objects defined
 * during creation.
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short producer/consumer buffer for fixed size objects.
 */
class CCXX_CLASS_EXPORT FixedBuffer : public Buffer
{
private:
	char *buf, *head, *tail;
	size_t objsize;

protected:
	/**
	 * Return the first object in the buffer.
	 * @return predefined size of this buffers objects.
	 * @param buf pointer to copy contents of head of buffer to.
	 */
	int onPeek(void *buf);

	/**
	 * Wait for and return a fixed object in the buffer.
	 * @return predefined size of this buffers objects.
	 * @param buf pointer to hold object returned from the buffer.
	 */
	int onWait(void *buf);

	/**
	 * Post an object of the appropriate size into the buffer.
	 * @return predefined size of this buffers objects.
	 * @param buf pointer to data to copy into the buffer.
	 */
	int onPost(void *buf);	

public:
	/**
	 * Create a buffer of known capacity for objects of a specified
	 * size.
	 * 
	 * @param capacity of the buffer.
	 * @param objsize for each object held in the buffer.
	 */
	FixedBuffer(size_t capacity, size_t objsize);

	/**
	 * Create a copy of an existing fixed size buffer and duplicate
	 * it's contents.
	 * 
	 * @param fb existing FixedBuffer object.
	 */
	FixedBuffer(const FixedBuffer &fb);

	/**
	 * Destroy the fixed buffer and free the memory used to store objects.
	 */
	virtual ~FixedBuffer();

	FixedBuffer &operator=(const FixedBuffer &fb);

	bool isValid(void);
};

/**
 * Every thread of execution in an application is created by
 * instantiating an object of a class derived from the Thread
 * class. Classes derived from Thread must implement the run() method,
 * which specifies the code of the thread. The base Thread class
 * supports encapsulation of the generic threading methods implemented
 * on various target operating systems.  This includes the ability to
 * start and stop threads in a synchronized and controllable manner,
 * the ability to specify thread execution priority, and thread
 * specific "system call" wrappers, such as for sleep and yield.  A
 * thread exception is thrown if the thread cannot be created.
 * Threading was the first part of Common C++ I wrote, back when it
 * was still the APE library.  My goal for Common C++ threading has
 * been to make threading as natural and easy to use in C++
 * application development as threading is in Java.  With this said,
 * one does not need to use threading at all to take advantage of
 * Common C++.  However, all Common C++ classes are designed at least
 * to be thread-aware/thread-safe as appropriate and necessary.
 * 
 * Common C++ threading is currently built either from the Posix "pthread"
 * library or using the win32 SDK.  In that the Posix "pthread" draft
 * has gone through many revisions, and many system implementations are
 * only marginally compliant, and even then usually in different ways, I
 * wrote a large series of autoconf macros found in ost_pthread.m4 which
 * handle the task of identifying which pthread features and capabilities
 * your target platform supports.  In the process I learned much about what
 * autoconf can and cannot do for you..
 * 
 * Currently the GNU Portable Thread library (GNU pth) is not directly
 * supported in Common C++.  While GNU "Pth" doesn't offer direct
 * native threading support or benefit from SMP hardware, many of the design
 * advantages of threading can be gained from it's use, and the  Pth pthread
 * "emulation" library should be usable with Common C++.  In the future,
 * Common C++ will directly support Pth, as well as OS/2 and BeOS native
 * threading API's.
 * 
 * Common C++ itself defines a fairly "neutral" threading model that is
 * not tied to any specific API such as pthread, win32, etc.  This neutral
 * thread model is contained in a series of classes which handle threading
 * and synchronization and which may be used together to build reliable
 * threaded applications.
 * 
 * Common C++ defines application specific threads as objects which are
 * derived from the Common C++ "Thread" base class.  At minimum the "Run"
 * method must be implemented, and this method essentially is the "thread",
 * for it is executed within the execution context of the thread, and when
 * the Run method terminates the thread is assumed to have terminated.
 * 
 * Common C++ allows one to specify the running priority of a newly created
 * thread relative to the "parent" thread which is the thread that is
 * executing when the constructor is called.  Since most newer C++
 * implementations do not allow one to call virtual constructors or virtual
 * methods from constructors, the thread must be "started" after the
 * constructor returns.  This is done either by defining a "starting"
 * semaphore object that one or more newly created thread objects can wait
 * upon, or by invoking an explicit "start" member function.
 * 
 * Threads can be "suspended" and "resumed".  As this behavior is not defined
 * in the Posix "pthread" specification, it is often emulated through
 * signals.  Typically SIGUSR1 will be used for this purpose in Common C++
 * applications, depending in the target platform.  On Linux, since threads
 * are indeed processes, SIGSTP and SIGCONT can be used.  On solaris, the
 * Solaris thread library supports suspend and resume directly.
 * 
 * Threads can be canceled.  Not all platforms support the concept of
 * externally cancelable threads.  On those platforms and API
 * implementations that do not, threads are typically canceled through the
 * action of a signal handler.
 * 
 * As noted earlier, threads are considered running until the "Run" method
 * returns, or until a cancellation request is made.  Common C++ threads can
 * control how they respond to cancellation, using setCancellation().
 * Cancellation requests can be ignored, set to occur only when a
 * cancellation "point" has been reached in the code, or occur immediately.
 * Threads can also exit by returning from Run() or by invoking the Exit()
 * method.
 * 
 * Generally it is a good practice to initialize any resources the thread may
 * require within the constructor of your derived thread class, and to purge
 * or restore any allocated resources in the destructor.  In most cases, the
 * destructor will be executed after the thread has terminated, and hence
 * will execute within the context of the thread that requested a join rather
 * than in the context of the thread that is being terminated.  Most
 * destructors in derived thread classes should first call Terminate() to
 * make sure the thread has stopped running before releasing resources.
 * 
 * A Common C++ thread is normally canceled by deleting the thread object.
 * The process of deletion invokes the thread's destructor, and the
 * destructor will then perform a "join" against the thread using the
 * Terminate() function.  This behavior is not always desirable since the
 * thread may block itself from cancellation and block the current "delete"
 * operation from completing.  One can alternately invoke Terminate()
 * directly before deleting a thread object.
 * 
 * When a given Common C++ thread exits on it's own through it's Run()
 * method, a "Final" method will be called.  This Final method will be called
 * while the thread is "detached".  If a thread object is constructed through
 * a "new" operator, it's final method can be used to "self delete" when
 * done, and allows an independent thread to construct and remove itself
 * autonomously.
 * 
 * A special global function, getThread(), is provided to identify the thread
 * object that represents the current execution context you are running
 * under.  This is sometimes needed to deliver signals to the correct thread.
 * Since all thread manipulation should be done through the Common C++ (base) 
 * thread class itself, this provides the same functionality as things like
 * "pthread_self" for Common C++.
 *
 * All Common C++ threads have an exception "mode" which determines
 * their behavior when an exception is thrown by another Common C++
 * class.  Extensions to Common C++ should respect the current
 * exception mode and use getException() to determine what to do when
 * they are about to throw an object.  The default exception mode
 * (defined in the Thread() constructor) is throwObject, which causes
 * a pointer to an instance of the class where the error occured to be
 * thrown.  Other exception modes are throwException, which causes a
 * class-specific exception class to be thrown, and throwNothing,
 * which causes errors to be ignored.  
 *
 * As an example, you could try to call the Socket class with an
 * invalid address that the system could not bind to.  This would
 * cause an object of type Socket * to be thrown by default, as the
 * default exception mode is throwObject.  If you call
 * setException(throwException) before the bad call to the Socket
 * constructor, an object of type SockException (the exception class
 * for class Socket) will be thrown instead.  
 *
 * To determine what exception class is thrown by a given Common C++
 * class when the exception mode is set to throwException, search the
 * source files for the class you are interested in for a class which
 * inherits directly or indirectly from class Exception.  This is the
 * exception class which would be thrown when the exception mode is
 * set to throwException.
 *
 * The advantage of using throwException versus throwObject is that
 * more information is available to the programmer from the thrown
 * object.  All class-specific exceptions inherit from class
 * Exception, which provides a getString() method which can be called
 * to get a human-readable error string.
 * 
 * Common C++ threads are often aggregated into other classes to provide
 * services that are "managed" from or operate within the context of a
 * thread, even within the Common C++ framework itself.  A good example of
 * this is the TCPSession class, which essentially is a combination of a TCP
 * client connection and a separate thread the user can define by deriving a
 * class with a Run() method to handle the connected service.  This
 * aggregation logically connects the successful allocation of a given
 * resource with the construction of a thread to manage and perform 
 * operations for said resource.
 * 
 * Threads are also used in "service pools".  In Common C++, a service pool
 * is one or more threads that are used to manage a set of resources.  While
 * Common C++ does not provide a direct "pool" class, it does provide a model
 * for their implementation, usually by constructing an array of thread
 * "service" objects, each of which can then be assigned the next new
 * instance of a given resource in turn or algorithmically.
 * 
 * Threads have signal handlers associated with them.  Several signal types
 * are "predefined" and have special meaning.  All signal handlers are
 * defined as virtual member functions of the Thread class which are called
 * when a specific signal is received for a given thread.  The "SIGPIPE"
 * event is defined as a "Disconnect" event since it's normally associated
 * with a socket disconnecting or broken fifo.  The Hangup() method is
 * associated with the SIGHUP signal.  All other signals are handled through
 * the more generic Signal().
 * 
 * Incidently, unlike Posix, the win32 API has no concept of signals, and
 * certainly no means to define or deliver signals on a per-thread basis.
 * For this reason, no signal handling is supported or emulated in the win32
 * implementation of Common C++ at this time.
 * 
 * In addition to TCPStream, there is a TCPSession class which combines a
 * thread with a TCPStream object.  The assumption made by TCPSession is that
 * one will service each TCP connection with a separate thread, and this
 * makes sense for systems where extended connections may be maintained and
 * complex protocols are being used over TCP.
 * 
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short base class used to derive all threads of execution.
 */
class CCXX_CLASS_EXPORT Thread
{
public:
	/**
	 * How to raise error
	 */
	typedef enum Throw {
		throwNothing,  /**< continue without throwing error */
		throwObject,   /**< throw object that cause error (throw this) */
		throwException /**< throw an object relative to error */
	} Throw;
	
	/**
	 * How work cancellation
	 */
	typedef enum Cancel
	{
		cancelInitial=0,  /**< used internally, do not use */
		cancelDeferred=1, /**< exit thread on cancellation pointsuch as yield */
		cancelImmediate,  /**< exit befor cancellation */
		cancelDisabled,   /**< ignore cancellation */
		cancelManual,     /**< unimplemented (working in progress)
					    @todo implement */
		cancelDefault=cancelDeferred
			/**< default you should use this for compatibility instead of deferred */
	} Cancel;

	/**
	 * How work suspend
	 */
	typedef enum Suspend
	{
		suspendEnable, /**< suspend enabled */
	        suspendDisable /**< suspend disabled, Suspend do nothing */
	} Suspend;

#ifndef WIN32
/** @internal */
friend class PosixThread;
#endif
/** @internal */
friend class DummyThread;
private:
	friend class Slog;

	Semaphore joinSem;
	static Thread* _main;

	Thread *_parent;
	enum Cancel _cancel;
	Semaphore *_start;

	// private data
	friend class ThreadImpl;
	class ThreadImpl* priv;

#ifndef WIN32
	friend Thread *getThread(void);
#else
	static unsigned __stdcall Execute(Thread *th);
#endif

	// close current thread, free all and call Notify
	void close();
	
protected:
       	/**
	 * All threads execute by deriving the Run method of Thread.
	 * This method is called after Initial to begin normal operation
	 * of the thread.  If the method terminates, then the thread will
	 * also terminate after notifying it's parent and calling it's
	 * Final() method.
	 *
	 * @see #Initial
	 */
	virtual void run(void) = 0;

	/**
	 * A thread that is self terminating, either by invoking exit() or
	 * leaving it's run(), will have this method called.  It can be used
	 * to self delete the current object assuming the object was created
	 * with new on the heap rather than stack local, hence one may often
	 * see final defined as "delete this" in a derived thread class.  A
	 * final method, while running, cannot be terminated or cancelled by
	 * another thread. Final is called for all cancellation type (even
	 * immediate).
	 * 
	 * You can safe delete thread ("delete this") class on final, but 
	 * you should exit ASAP (or do not try to call CommonC++ methods...)
	 * 
         * @note A thread cannot delete its own context or join
         * itself.  To make a thread that is a self running object
         * that self-deletes, one has to detach the thread by using
         * detach() instead of start().
         *
	 * @see #exit
	 * @see #run
	 */
	virtual void final(void)
		{return;};

	/**
	 * The initial method is called by a newly created thread when it
	 * starts execution.  This method is ran with deferred cancellation
	 * disabled by default.  The Initial method is given a separate
	 * handler so that it can create temporary objects on it's own
	 * stack frame, rather than having objects created on run() that
	 * are only needed by startup and yet continue to consume stack space.
	 * 
	 * @see #run
	 * @see #final
	 */
	virtual void initial(void)
		{return;};

	/**
	 * Since getParent() and getThread() only refer to an object of the
	 * Thread "base" type, this virtual method can be replaced in a
	 * derived class with something that returns data specific to the
	 * derived class that can still be accessed through the pointer
	 * returned by getParent() and getThread().
	 *
	 * @return pointer to derived class specific data.
	 */
	virtual void* getExtended(void)
		{return NULL;};

	/**
	 * When a thread terminates, it now sends a notification message
	 * to the parent thread which created it.  The actual use of this
	 * notification is left to be defined in a derived class.
	 * 
	 * @param - the thread that has terminated.
	 */
	virtual void notify(Thread*)
		{return;};

	/**
	 * Used to properly exit from a Thread derived run() or initial()
	 * method.  Terminates execution of the current thread and calls
	 * the derived classes final() method.
	 */
	void exit(void);

	/**
	 * test a cancellation point for deferred thread cancellation.
	 */
	bool testCancel(void);

	/**
	 * Sets thread cancellation mode.  Threads can either be set immune to
	 * termination (cancelDisabled), can be set to terminate when
	 * reaching specific "thread cancellation points"
	 * (cancelDeferred)
	 * or immediately when Terminate is requested (cancelImmediate).
	 * 
	 * @param mode for cancellation of the current thread.
	 */
	void setCancel(Cancel mode);

	/**
	 * Sets the thread's ability to be suspended from execution.  The
	 * thread may either have suspend enabled (suspendEnable) or
	 * disabled (suspendDisable).
	 * 
	 * @param mode for suspend.
	 */
	void setSuspend(Suspend mode);

	/**
	 * Used by another thread to terminate the current thread.  Termination
	 * actually occurs based on the current setCancel() mode.  When the
	 * current thread does terminate, control is returned to the requesting
	 * thread.  terminate() should always be called at the start of any
	 * destructor of a class derived from Thread to assure the remaining
	 * part of the destructor is called without the thread still executing.
	 */
	void terminate(void);

	/**
	 * clear parent thread relationship.
	 */
	inline void clrParent(void)
		{_parent = NULL;};

#ifdef WIN32
	// FIXME: should be private
	DWORD waitHandle(HANDLE obj, timeout_t timeout);
#endif

public:
	/**
	 * This is actually a special constructor that is used to create a
	 * thread "object" for the current execution context when that context
	 * is not created via an instance of a derived Thread object itself.
	 * This constructor does not support First.
	 * 
	 * @param isMain bool used if the main "thread" of the application.
	 */
	Thread(bool isMain);

	/**
	 * When a thread object is contructed, a new thread of execution
	 * context is created.  This constructor allows basic properties
	 * of that context (thread priority, stack space, etc) to be defined.
	 * The starting condition is also specified for whether the thread
	 * is to wait on a semaphore before begining execution or wait until
	 * it's start method is called.
	 * 
	 * @param pri thread base priority relative to it's parent.
	 * @param stack space as needed in some implementations.
	 */
	Thread(int pri = 0, size_t stack = 0);

#ifndef WIN32
	/**
	 * A thread of execution can also be specified by cloning an existing
	 * thread.  The existing thread's properties (cancel mode, priority,
	 * etc), are also duplicated.
	 * 
	 * @param th currently executing thread object to clone.
	 * @todo implement in win32
	 */
	Thread(const Thread &th);
#endif

	/**
	 * The thread destructor should clear up any resources that have
	 * been allocated by the thread.  The desctructor of a derived
	 * thread should begin with Terminate() and is presumed to then
	 * execute within the context of the thread causing terminaton.
	 */
	virtual ~Thread();

	/**
	 * A thread-safe sleep call.  On most Posix systems, "sleep()"
	 * is implimented with SIGALRM making it unusable from multipe
	 * threads.  Pthread libraries often define an alternate "sleep"
	 * handler such as usleep(), nanosleep(), or nap(), that is thread
	 * safe, and also offers a higher timer resolution.
	 * 
	 * @param msec timeout in milliseconds.
	 */
	static void sleep(timeout_t msec);

	/**
	 * Yields the current thread's CPU time slice to allow another thread to
	 * begin immediate execution.
	 */
	static void yield(void);

	/**
	 * When a new thread is created, it does not begin immediate
	 * execution.  This is because the derived class virtual tables
	 * are not properly loaded at the time the C++ object is created
	 * within the constructor itself, at least in some compiler/system 
	 * combinations.  The thread can either be told to wait for an
	 * external semaphore, or it can be started directly after the
	 * constructor completes by calling the start() method.
	 * 
	 * @return error code if execution fails.
	 * @param start optional starting semaphore to alternately use.
	 */
	int start(Semaphore *start = 0);

	/**
	 * Start a new thread as "detached".  This is an alternative
	 * start() method that resolves some issues with later glibc
	 * implimentations which incorrectly impliment self-detach.
	 *
	 * @return error code if execution fails.
	 * @param start optional starting semaphore to alternately use.
	 */
	int detach(Semaphore *start = 0);

	/**
	 * Gets the pointer to the Thread class which created the current
	 * thread object.
	 * 
	 * @return a Thread *, or "(Thread *)this" if no parent.
	 */
	inline Thread *getParent(void)
		{return _parent;};

	/**
	 * Suspends execution of the selected thread.  Pthreads do not
	 * normally support suspendable threads, so the behavior is
	 * simulated with signals.  On systems such as Linux that
	 * define threads as processes, SIGSTOP and SIGCONT may be used.
	 */
	void suspend(void);

	/**
	 * Resumes execution of the selected thread.
	 */
	void resume(void);

	/**
	 * Used to retrieve the cancellation mode in effect for the
	 * selected thread.
	 * 
	 * @return cancellation mode constant.
	 */
	inline Cancel getCancel(void)
		{return _cancel;};

	/**
	 * Verifies if the thread is still running or has already been
	 * terminated but not yet deleted.
	 * 
	 * @return true if the thread is still executing.
	 */
	bool isRunning(void);

       /**
         * Check if this thread is detached.
         *
         * @return true if the thread is detached.
         */
        bool isDetached(void);

        /**
	 * Blocking call which unlocks when thread terminates.
	 */
	void join(void);

	/**
	 * Tests to see if the current execution context is the same as
	 * the specified thread object.
	 * 
	 * @return true if the current context is this object.
	 */
	bool isThread(void);

	/**
	 * Get exception mode of the current thread.
	 *
	 * @return exception mode.
	 */
	friend CCXX_EXPORT(Throw) getException(void);

	/**
	 * Set exception mode of the current thread.
	 *
	 * @return exception mode.
	 */
	friend CCXX_EXPORT(void) setException(Throw mode);

	/**
	 * Signal the semaphore that the specified thread is waiting for
	 * before beginning execution.
	 * 
	 * @param th specified thread.
	 */
	friend inline void operator++(Thread &th)
		{if (th._start) th._start->post();};

	friend inline void operator--(Thread &th)
		{if (th._start) th._start->wait();};

#ifdef WIN32
	// FIXME: not defined in posix
	bool isCancelled();

	friend CCXX_EXPORT(DWORD) waitThread(HANDLE hRef, timeout_t timeout);
#endif
};

/** @relates Thread 
 * Get current thread object associated with current system thread.
 * getThread should return NULL only if class has been deleted
 * @return Thread class of current system thread
 */
CCXX_EXPORT(Thread*) getThread(void);
/** @relates Thread */
CCXX_EXPORT(Thread::Throw) getException(void);
/** @relates Thread */
CCXX_EXPORT(void) setException(Thread::Throw mode);

#if !defined(WIN32) && !defined(__MINGW32__)
typedef	int		signo_t;

class PosixThread: public Thread
{
private:
#ifndef WIN32
	/** @internal */
	friend class ThreadImpl;
	friend class Thread;
#endif
#ifndef	CCXX_SIG_THREAD_ALARM
	static PosixThread *_timer;
	static Mutex _arm;
#endif
	
	time_t	_alarm;
	static void signalThread(Thread* th,signo_t signo);
protected:
		
	/**
	 * In the Posix version of Common C++, this can be used to send a
	 * signal into the parent thread of the current object.
	 * 
	 * @param signo a posix signal id.
	 */
	inline void signalParent(signo_t signo)
		{ signalThread(_parent,signo); };
	
	/**
	 * In the Posix version of Common C++, this can be used to send a
	 * signal into the main application thread.
	 * 
	 * @param signo a posix signal id.
	 */
	inline void signalMain(signo_t signo)
		{ signalThread(_main,signo);};

	/**
	 * A derivable method to call when a SIGALRM is being delivered
	 * to a specific thread.
	 */
	virtual void onTimer(void)
		{return;};

	/**
	 * A derived method to handle hangup events being delivered
	 * to a specific thread.
	 */
	virtual void onHangup(void)
		{return;};

	/**
	 * A derived method to call when a SIGABRT is being delivered
	 * to a specific thread.
	 */
	virtual void onException(void)
		{return;};

	/**
	 * A derived method to call when a SIGPIPE is being delivered
	 * to a specific thread.
	 */
	virtual void onDisconnect(void)
		{return;};

	/**
	 * A derived method to handle asynchronous I/O requests delivered
	 * to the specified thread.
	 */
	virtual void onPolling(void)
		{return;};

	/**
	 * A derivable method to call for delivering a signal event to
	 * a specified thread.
	 *
	 * @param - posix signal id.
	 */
	virtual void onSignal(int)
		{return;};
	
	/**
	 * Used to specify a timeout event that can be delivered to the
	 * current thread via SIGALRM.  When the timer expires, the onTimer() 
	 * method is called for the thread.  At present, only one thread
	 * timer can be active at any given time.  On some operating
	 * systems (including Linux) a timer can be active on each thread.
	 * 
	 * @param timer timeout in milliseconds.
	 * @param periodic should the timer be periodic.
	 * @note currently, periodic timers are only available on
	 * systems with a working setitimer call.
	 */
	void setTimer(timeout_t timer, bool periodic = false);
	
	/**
	 * Gets the time remaining for the current threads timer before
	 * it expires.
	 * 
	 * @return time remaining before timer expires in milliseconds.
	 */
	timeout_t getTimer(void) const;
	
	/**
	 * Terminates the timer before the timeout period has expired.
	 * This prevents the timer from sending it's SIGALRM and makes
	 * the timer available to other threads.
	 */
	void endTimer(void);
	
	/**
	 * Used to wait on a Posix signal from another thread.  This can be
	 * used as a crude rondevious/synchronization method between threads.
	 * 
	 * @param signo a posix signal id.
	 */
	void waitSignal(signo_t signo);
	
	/**
	 * Used to enable or disable a signal within the current thread.
	 *
	 * @param signo posix signal id.
	 * @param active set to true to enable.
	 */
	void setSignal(int signo, bool active);
public:

	CCXX_MEMBER_EXPORT(CCXX_EMPTY) PosixThread(int pri = 0, size_t stack = 0);
	
	/**
	 * Delivers a Posix signal to the current thread.
	 * 
	 * @param signo a posix signal id.
	 */
	inline void signalThread(int signo)
		{signalThread(this, signo);};

	/**
	 * Install a signal handler for use by threads and
	 * the OnSignal() event notification handler.
	 *
	 * @param signo posix signal id.
	 */
	friend void sigInstall(int signo);
};
#endif

/**
 * This class allows the creation of a thread context unique "pointer"
 * that can be set and retrieved and can be used to create thread specific
 * data areas for implementing "thread safe" library routines.
 * 
 *  Finally, Common C++ supports a
 * thread-safe "AtomicCounter" class.  This can often be used for reference
 * counting without having to protect the counter with a separate Mutex
 * counter.  This lends to lighter-weight code.
 * 
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short container for thread specific data storage.
 */
class ThreadKey
{
private:
#ifndef WIN32
	pthread_key_t key;
	typedef void (*TDestruct)(void*);
	friend class ThreadImpl;
	ThreadKey(TDestruct destruct);
#else
	DWORD	key;
#endif

public:
	/**
	 * Create a unique thread specific container.
	 */
	ThreadKey();
	/**
	 * Destroy a thread specific container and any contents reserved.
	 */
	virtual ~ThreadKey();
	/**
	 * Get the value of the pointer for the thread specific data
	 * container.  A unique pointer can be set for each execution
	 * context.
	 * 
	 * @return a unique void * for each execution context.
	 */
	void *getKey(void);
	/**
	 * Set the value of the pointer for the current thread specific
	 * execution context.  This can be used to store thread context
	 * specific data.
	 * 
	 * @param - ptr to thread context specific data.
	 */
	void setKey(void *);
};

/**
 * Timer ports are used to provide synchronized timing events when managed
 * under a "service thread" such as SocketService.  This is made into a
 * stand-alone base class since other derived libraries (such as the
 * serial handlers) may also use the pooled "service thread" model
 * and hence also require this code for managing timing.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short synchronized millisecond timing for service threads.
 */
class CCXX_CLASS_EXPORT TimerPort
{
#ifndef WIN32
	struct timeval timer;
#else
	DWORD timer;
#endif
	bool active;

public:
	/**
	 * Create a timer, mark it as inactive, and set the initial
	 * "start" time to the creation time of the timer object.  This
	 * allows "incTimer" to initially refer to time delays relative
	 * to the original start time of the object.
	 */
	TimerPort();

	/**
	 * Set a new start time for the object based on when this call is
	 * made and optionally activate the timer for a specified number
	 * of milliseconds.  This can be used to set the starting time
	 * of a realtime session.
	 *
	 * @param timeout delay in milliseconds from "now"
	 */
	void setTimer(timeout_t timeout = 0);

	/**
	 * Set a timeout based on the current time reference value either
	 * from object creation or the last setTimer().  This reference
	 * can be used to time synchronize realtime data over specified
	 * intervals and force expiration when a new frame should be
	 * released in a synchronized manner.  
	 *
	 * @param timeout delay in milliseconds from reference.
	 */
	void incTimer(timeout_t timeout);

	/**
	 * This is used to "disable" the service thread from expiring
	 * the timer object.  It does not effect the reference time from
	 * either creation or a setTimer().
	 */
	void endTimer(void);

	/**
	 * This is used by service threads to determine how much time
	 * remains before the timer expires based on a timeout specified
	 * in setTimer() or incTimer().  It can also be called after
	 * setting a timeout with incTimer() to see if the current timeout
	 * has already expired and hence that the application is already
	 * delayed and should skip frame(s).
	 *
	 * return time remaining in milliseconds, or TIMEOUT_INF if
	 * inactive.
	 */
	timeout_t getTimer(void) const;

	/**
	 * This is used to determine how much time has elapsed since a
	 * timer port setTimer benchmark time was initially set.  This
	 * allows one to use setTimer() to set the timer to the current
	 * time and then measure elapsed time from that point forward.
	 *
	 * return time elapsed in milliseconds, or TIMEOUT_INF if
	 * inactive.
	 */
	timeout_t getElapsed(void) const;
};

/** @relates Buffer */
inline int get(Buffer &b, void *o)
	{return b.wait(o);};

/** @relates Buffer */
inline int put(Buffer &b, void *o)
	{return b.post(o);};

/** @relates Buffer */
inline int peek(Buffer &b, void *o)
	{return b.peek(o);};


// FIXME: not in win32 implementation
#if !defined(WIN32)

// FIXME: private declaration ???
struct	timespec *getTimeout(struct timespec *spec, timeout_t timeout);	

#if !defined(__CYGWIN32__) && !defined(__MINGW32__)
void	wait(signo_t signo);
#endif

#endif // !WIN32

// FIXME: no way to implement in win32
#ifndef WIN32
#if defined(HAVE_POLL_H) || defined(HAVE_SYS_POLL_H)
#if defined(HAVE_SYS_STREAM_H)
#if defined(__linux__)
#define CCXX_USE_POLL 1
#endif
#else
#define	CCXX_USE_POLL 1
#endif
#endif

#ifdef CCXX_USE_POLL

/**
 * The poller class is used to help manage pollfd structs for use in the
 * updated serial and socket "port" code.
 *
 * @author Gianni Mariani <gianni@mariani.ws>
 * @short pollfd assistance class for port classes.
 */
class Poller 
{
private:
	int nufds;
	pollfd *ufds;

public:
	Poller();

	virtual ~Poller();

	/**
	 * reserve a specified number of poll descriptors.  If additional
	 * descriptors are needed, they are allocated.
	 *
	 * @return new array of descriptors.
	 * @param cnt number of desctiptors to reserve
	 */
	pollfd *getList(int cnt);

	/**
	 * Retreive the current array of poll descriptors.
	 *
	 * @return array of descriptors.
	 */
	inline	pollfd *getList(void)
		{return ufds;};
};
#endif
#endif // !WIN32

#ifdef	COMMON_STD_EXCEPTION
/**
 * A sub-hierarchy for thread exceptions.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short thread exception hierarchy.
 */
class ThrException : public Exception
{
public:
	ThrException(const std::string &what_arg) : Exception(what_arg) {};
};

/**
 * A sub-hierarchy for all task synchronizion related exceptions.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Synchronization object exception hierarchy.
 */
class SyncException : public ThrException
{
public:
	SyncException(const std::string &what_arg) : ThrException(what_arg) {};
};

class InterruptException : public ThrException
{
public:
	InterruptException() : ThrException("interrupted") {};
};

#endif

// dummy class objects needed to make MACOSX reliably link Common C++
// shared libraries.  This is very bizzare, but works for images that
// reference cc++ functions on macosx but otherwise do not execute since
// the cc++ shared libs and constructors are never fully linked?! by the
// macosx dynamic loader...

#ifdef	CCXX_NAMESPACES
};
#endif

#endif
/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
