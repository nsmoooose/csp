/* SimData: Data Infrastructure for Simulations
 * Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file ThreadBase.cpp
 * @brief PosixThreads wrapper and utility classes.
 **/

#ifndef SIMDATA_NOTHREADS

#include <SimData/ThreadBase.h>
#include <SimData/Log.h>
#include <SimData/Timing.h>

#include <string>
#include <sstream>

NAMESPACE_SIMDATA

ThreadException::ThreadException(const int error): Exception("ThreadException"), m_error(error) {
	translateError();
}

void ThreadException::checkThrow(const int result) {
	if (result != 0) {
		throw ThreadException(result);
	}
}

void ThreadException::checkLog(const int result) {
	if (result != 0) {
		ThreadException e(result);
		SIMDATA_ERROR(LOG_THREAD, e);
		e.clear();
	}
}

void ThreadException::translateError() {
	std::stringstream msg;
	switch (m_error) {
		case EDEADLK: msg << "deadlock (EDEADLK)"; break;
		case EAGAIN: msg << "unable to allocate resource (EAGAIN)"; break;
		case EBUSY: msg << "busy: resource already in use (EBUSY)"; break;
		case EINVAL: msg << "invalid parameters (EINVAL)"; break;
		case ENOMEM: msg << "out of memory (ENOMEM)"; break;
		case ETIMEDOUT: msg << "timed out (ETIMEDOUT)"; break;
		case EPERM: msg << "permission denied (EPERM)"; break;
		case ESRCH: msg << "not found (ESRCH)"; break;
		default: msg << "ISO C99 error code: " << m_error;
	}
	addMessage(msg.str());
}


template <class TYPE>
ThreadSpecific<TYPE>::~ThreadSpecific() {
	if (m_once) {
		// delete the current thread's instance, if any (the callback will not
		// do so after the key is deleted).
		ValueType *tss_data = reinterpret_cast<ValueType*>(pthread_getspecific(this->m_key));
		delete tss_data;
		const int result = pthread_key_delete(m_key);
		// not safe to throw from a dtor, so just log the error
		ThreadException::checkLog(result);
	}
}

template <class TYPE>
TYPE *ThreadSpecific<TYPE>::operator->() {
	// the first call needs to construct a shared key that is
	// used to access the thread-specific data.
	if (!this->m_once) {
		ScopedLock<ThreadMutex> lock(this->m_keylock);
		if (!this->m_once) {
			const int result = pthread_key_create(&this->m_key, &this->cleanup_hook);
			ThreadException::checkThrow(result);
			this->m_once = true;
		}
	}

	// use the shared key to retrieve the data (ValueType*) for this thread.
	ValueType *tss_data = reinterpret_cast<ValueType*>(pthread_getspecific(this->m_key));

	// if the pointer is null, this is the first access from this
	// thread and we need to allocate a new instance.
	if (tss_data == 0) {
		tss_data = createNew();
		const int result = pthread_setspecific(this->m_key, reinterpret_cast<void*>(tss_data));
		ThreadException::checkThrow(result);
	}

	// return the instance specific to the current thread.
	return tss_data;
}

ThreadCondition::~ThreadCondition() {
	int result = pthread_cond_destroy(&m_cond);
	// not safe to throw from a dtor, so just log the error
	ThreadException::checkLog(result);
	if (&m_local_mutex == &m_mutex) {
		result = pthread_mutex_destroy(&m_local_mutex);
		// not safe to throw from a dtor, so just log the error
		ThreadException::checkLog(result);
	}
}

bool ThreadCondition::wait(double timeout) {
	if (timeout < 0) {
		const int result = pthread_cond_wait(&m_cond, &m_mutex);
		ThreadException::checkThrow(result);
	} else {
		double abstime_sec = get_realtime() + timeout;
		timespec abstime;
		double sec = floor(abstime_sec);
		abstime.tv_sec = static_cast<int>(sec);
		abstime.tv_nsec = static_cast<int>(1e+9 * (abstime_sec - sec));
		const int result = pthread_cond_timedwait(&m_cond, &m_mutex, &abstime);
		if (result == ETIMEDOUT) return false;
		ThreadException::checkThrow(result);
	}
	return true;
}

NAMESPACE_SIMDATA_END

#endif // SIMDATA_NOTHREADS

