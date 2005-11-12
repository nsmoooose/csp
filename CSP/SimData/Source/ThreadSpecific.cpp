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
 * @file ThreadSpecific.cpp
 * @brief PosixThreads thread-specific storage.
 **/

#ifndef SIMDATA_NOTHREADS

#include <SimData/ThreadSpecific.h>
#include <SimData/ScopedLock.h>

NAMESPACE_SIMDATA


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

NAMESPACE_SIMDATA_END

#endif // SIMDATA_NOTHREADS

