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
 * @file ThreadSpecific.h
 * @brief PosixThreads thread-specific storage.
 */


#ifndef SIMDATA_NOTHREADS

#ifndef __SIMDATA_THREADSPECIFIC_H__
#define __SIMDATA_THREADSPECIFIC_H__

#include <SimData/Namespace.h>
#include <SimData/Properties.h>
#include <SimData/Synchronization.h>

#include <pthread.h>


NAMESPACE_SIMDATA


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


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_THREADSPECIFIC_H__

#endif // SIMDATA_NOTHREADS
