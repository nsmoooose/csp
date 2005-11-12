/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002-2005 Mark Rose <mkrose@users.sf.net>
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
 * @file Referenced.h
 *
 * Provides a base class for reference counted objects.  See Ref.h for
 * more details.
 */


#ifndef __SIMDATA_REFERENCED_H__
#define __SIMDATA_REFERENCED_H__

#include <cassert>

#include <SimData/Namespace.h>
#include <SimData/Export.h>
#include <SimData/ExceptionBase.h>
#include <SimData/Properties.h>
#include <SimData/AtomicCounter.h>

#ifdef SWIG
// silence SWIG warning about NonCopyable
%import "SimData/Properties.h"
#endif

NAMESPACE_SIMDATA

template <class T> class Ref;

void SIMDATA_EXPORT _log_reference_count_error(int count, void* pointer);


/** Base class for reference counted objects.
 *
 *  Inspired by OpenSceneGraph's osg::Referenced class.
 *
 *  @author Mark Rose <mkrose@users.sf.net>
 */
template <typename COUNTER>
class SIMDATA_EXPORT ReferencedBase: public NonCopyable {

template <class T> friend class Ref;
friend class LinkCore;

protected:
	ReferencedBase(): __count(0) {
	}

	virtual ~ReferencedBase() {
		if (__count != 0) _log_reference_count_error(__count, reinterpret_cast<void*>(this));
	}

	inline int _count() const { return static_cast<int>(__count); }

private:

	inline void _incref() const {
		++__count;
	}
	inline void _decref() const {
		assert(__count > 0);
		if (!--__count) delete this;
	}
	mutable COUNTER __count;
};


/** Base class for referenced counted objects that are used within a single
 *  thread.  <b>Not thread-safe</b>, use ThreadSafeReferenced for references
 *  that are shared between threads.
 */
typedef ReferencedBase<int> Referenced;

#ifndef SIMDATA_NOTHREADS
/** Base class for referenced counted objects that are shared between
 *  multiple threads.  Thread-safe, supporting atomic updates of the
 *  internal reference counter.
 */
typedef ReferencedBase<AtomicCounter> ThreadSafeReferenced;
#endif // SIMDATA_NOTHREADS

NAMESPACE_SIMDATA_END


#endif //__SIMDATA_REFERENCED_H__

