#pragma once
/* Combat Simulator Project
 * Copyright (C) 2002-2005 Mark Rose <mkrose@users.sf.net>
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

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Exception.h>
#include <csp/csplib/util/Properties.h>
#include <csp/csplib/util/Verify.h>

#ifdef SWIG
// silence SWIG warning about NonCopyable
%import "csp/csplib/util/Properties.h"
#endif

namespace csp {

template <class T> class Ref;

void CSPLIB_EXPORT _log_reference_count_error(int count, void* pointer);


/** Base class for reference counted objects.
 *
 *  Inspired by OpenSceneGraph's osg::Referenced class.
 */
template <typename COUNTER>
class CSPLIB_EXPORT ReferencedBase: public NonCopyable {

template <class T> friend class Ref;
friend class LinkCore;

public:

	inline void _incref() const {
		++__count;
	}
	inline void _decref() const {
		CSP_VERIFY(__count > 0);
		if (!--__count) delete this;
	}

protected:
	ReferencedBase(): __count(0) {
	}

	virtual ~ReferencedBase() {
		if (__count != 0) _log_reference_count_error(__count, reinterpret_cast<void*>(this));
	}

	inline int _count() const { return static_cast<int>(__count); }

private:
	mutable COUNTER __count;
};


/** Base class for referenced counted objects that are used within a single
 *  thread.  <b>Not thread-safe</b>, use ThreadSafeReferenced for references
 *  that are shared between threads.
 */
typedef ReferencedBase<int> Referenced;


} // namespace csp
