// Copyright (C) 1999-2002 Open Source Telecom Corporation.
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
// If you write modifications of your own for Common C++, it is your
// choice whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/thread.h>
#include "private.h"

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

ThreadLock::ThreadLock()
{
#ifdef HAVE_PTHREAD_RWLOCK
	pthread_rwlockattr_t attr;
	
	pthread_rwlockattr_init(&attr);
	if(pthread_rwlock_init(&_lock, &attr))
	{
		if(getException() == Thread::throwObject)
			throw(this);
#ifdef	COMMON_STD_EXCEPTION
		else if(getException() == Thread::throwException)
			throw(SyncException("Mutex constructor failure"));
#endif
	}
#endif
}

ThreadLock::~ThreadLock()
{
#ifdef HAVE_PTHREAD_RWLOCK
	pthread_rwlock_destroy(&_lock);
#endif
}

void ThreadLock::readLock(void)
{
#ifdef HAVE_PTHREAD_RWLOCK
	pthread_rwlock_rdlock(&_lock);
#else
	mutex.enterMutex();
#endif
}

void ThreadLock::writeLock(void)
{
#ifdef HAVE_PTHREAD_RWLOCK
	pthread_rwlock_wrlock(&_lock);
#else
	mutex.enterMutex();
#endif
}

void ThreadLock::unlock(void)
{
#ifdef HAVE_PTHREAD_RWLOCK
	pthread_rwlock_unlock(&_lock);
#else
	mutex.leaveMutex();
#endif
}

bool ThreadLock::tryReadLock(void)
{
#ifdef	HAVE_PTHREAD_RWLOCK
	if(pthread_rwlock_tryrdlock(&_lock))
		return false;
	return true;
#else
	return mutex.tryEnterMutex();
#endif
}

bool ThreadLock::tryWriteLock(void)
{
#ifdef	HAVE_PTHREAD_RWLOCK
	if(pthread_rwlock_trywrlock(&_lock))
		return false;
	return true;
#else
	return mutex.tryEnterMutex();
#endif
}

#ifndef WIN32
Conditional::Conditional() :
Mutex()
{
	if(pthread_cond_init(&_cond, NULL) && getException() == Thread::throwObject)
		throw(this);
}

Conditional::~Conditional()
{
	pthread_cond_destroy(&_cond);
}

void Conditional::signal(bool broadcast)
{
	if(broadcast)
		pthread_cond_broadcast(&_cond);
	else
		pthread_cond_signal(&_cond);
}

void Conditional::wait(timeout_t timeout)
{
	struct timespec ts;

	if(!timeout)
	{
		pthread_cond_wait(&_cond, &_mutex);
		return;
	}
	getTimeout(&ts, timeout);
	pthread_cond_timedwait(&_cond, &_mutex, &ts);
}
#endif

#ifndef WIN32
Mutex::Mutex()
{
	pthread_mutexattr_t _attr;

	pthread_mutexattr_init(&_attr);
#ifdef	PTHREAD_MUTEXTYPE_RECURSIVE
	pthread_mutexattr_settype(&_attr, PTHREAD_MUTEXTYPE_RECURSIVE);
#endif
	pthread_mutex_init(&_mutex, &_attr);
	pthread_mutexattr_destroy(&_attr);

#ifndef	PTHREAD_MUTEXTYPE_RECURSIVE
	_level = 0;
	_tid = NULL;
#endif
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&_mutex);
}

#ifdef PTHREAD_MUTEXTYPE_RECURSIVE

bool Mutex::tryEnterMutex(void)
{
	return (pthread_mutex_trylock(&_mutex) == 0) ? true : false;
}

void Mutex::enterMutex(void)
{

	pthread_mutex_lock(&_mutex);
}

void Mutex::leaveMutex(void)
{
	pthread_mutex_unlock(&_mutex);
}

#else // !PTHREAD_MUTEXTYPE_RECURSIVE

void Mutex::enterMutex(void)
{
	if(_level)
		if(_tid == getThread())
		{
			++_level;
			return;
		}
	pthread_mutex_lock(&_mutex);
	++_level;
	_tid = getThread();
}

bool Mutex::tryEnterMutex(void)
{
	if(_level)
	{
		if(_tid == getThread())
		{
			++_level;
			return true;
		}
		else
			return false;
	}
	if ( pthread_mutex_trylock(&_mutex) != 0 )
		return false;
	else
	{
		_tid = getThread();
		++_level;
		return true;
	}
}
#endif

#else // WIN32

Mutex::Mutex():
	mutex(0)
{
	mutex = ::CreateMutex(NULL,FALSE,NULL);
	if(!mutex)
		throw(this);
}

void Mutex::enterMutex(void)
{
	waitThread(mutex, INFINITE);
}

bool Mutex::tryEnterMutex(void)
{
	return (waitThread(mutex, 0) == WAIT_OBJECT_0);
}

Mutex::~Mutex()
{
	::CloseHandle(mutex);
}
	

#endif

#ifndef PTHREAD_MUTEXTYPE_RECURSIVE
void Mutex::leaveMutex(void)
{
#ifndef WIN32
	if(_level > 1)
	{
		--_level;
		return;
	}
	--_level;
	_tid = NULL;
	pthread_mutex_unlock(&_mutex);
#else // WIN32
	if (!ReleaseMutex(mutex))
		throw this;
#endif // !WIN32
}
#endif

#ifdef WIN32
MutexCounter::MutexCounter() : Mutex()
{
	counter = 0;
};
#endif

MutexCounter::MutexCounter(int initial) : Mutex()
{
	counter = initial;
};

int operator++(MutexCounter &mc)
{
	int rtn;

	mc.enterMutex();
	rtn = mc.counter++;
	mc.leaveMutex();
	return rtn;
}

// ??? why cannot be < 0 ???
int operator--(MutexCounter &mc)
{
	int rtn = 0;

	mc.enterMutex();
	if(mc.counter)
	{
		rtn = --mc.counter;
		if(!rtn)
		{
			mc.leaveMutex();
			throw(mc);
		}
	}
	mc.leaveMutex();
	return rtn;
}

#ifndef WIN32
#ifdef	HAVE_ATOMIC
AtomicCounter::AtomicCounter()
{
	atomic.counter = 0;
}

AtomicCounter::AtomicCounter(int value)
{
	atomic.counter = value;
}

int AtomicCounter::operator++(void)
{
	atomic_inc(&atomic);
	return atomic_read(&atomic);
}

int AtomicCounter::operator--(void)
{
	int chk = atomic_dec_and_test(&atomic);
	if(chk)
		return 0;
	chk = atomic_read(&atomic);
	if(!chk)
		++chk;
	return chk;
}

int AtomicCounter::operator+=(int change)
{
	atomic_add(change, &atomic);
	return atomic_read(&atomic);
}

int AtomicCounter::operator-=(int change)
{
	atomic_sub(change, &atomic);
	return atomic_read(&atomic);
}

int AtomicCounter::operator+(int change)
{
	return atomic_read(&atomic) + change;
}

int AtomicCounter::operator-(int change)
{
	return atomic_read(&atomic) - change;
}

int AtomicCounter::operator=(int value)
{
	atomic_set(&atomic, value);
	return atomic_read(&atomic);
}

bool AtomicCounter::operator!(void)
{
	int value = atomic_read(&atomic);
	if(value)
		return false;
	return true;
}

AtomicCounter::operator int()
{
	return atomic_read(&atomic);
}

#else // !HAVE_ATOMIC

AtomicCounter::AtomicCounter()
{
	counter = 0;
}

AtomicCounter::AtomicCounter(int value)
{
	counter = value;
}

int AtomicCounter::operator++(void)
{
	int value;

	lock.enterMutex();
	value = ++counter;
	lock.leaveMutex();
	return value;
}

int AtomicCounter::operator--(void)
{
	int value;
	lock.enterMutex();
	value = --counter;
	lock.leaveMutex();
	return value;
}

int AtomicCounter::operator+=(int change)
{
	int value;
	lock.enterMutex();
	counter += change;
	value = counter;
	lock.leaveMutex();
	return value;
}

int AtomicCounter::operator-=(int change)
{
	int value;
	lock.enterMutex();
	counter -= change;
	value = counter;
	lock.leaveMutex();
	return value;
}

int AtomicCounter::operator+(int change)
{
	int value;
	lock.enterMutex();
	value = counter + change;
	lock.leaveMutex();
	return value;
}

int AtomicCounter::operator-(int change)
{
	int value;
	lock.enterMutex();
	value = counter - change;
	lock.leaveMutex();
	return value;
}

AtomicCounter::operator int()
{
	int value;
	lock.enterMutex();
	value = counter;
	lock.leaveMutex();
	return value;
}

int AtomicCounter::operator=(int value)
{
	int ret;
	lock.enterMutex();
	ret = counter;
	counter = value;
	lock.leaveMutex();
	return ret;
}

bool AtomicCounter::operator!(void)
{
	int value;
	lock.enterMutex();
	value = counter;
	lock.leaveMutex();
	if(value)
		return false;
	return true;
}
#endif // HAVE_ATOMIC
#else // WIN32
int AtomicCounter::operator+=(int change)
{
	// FIXME: enhance with InterlockExchangeAdd
	while(--change>=0)
		InterlockedIncrement(&atomic);
	
	return atomic;
}

int AtomicCounter::operator-=(int change)
{
	// FIXME: enhance with InterlockExchangeAdd
	while(--change>=0)
		InterlockedDecrement(&atomic);

	return atomic;
}
#endif // !WIN32

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
