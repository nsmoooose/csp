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
 * @file WeakReferenced.h
 *
 * Provides a base class for reference counted objects.  See Ref.h for
 * more details.
 */
 
#ifndef __CSPLIB_UTIL_WEAKREFERENCED_H__
#define __CSPLIB_UTIL_WEAKREFERENCED_H__

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Export.h>

CSP_NAMESPACE

class CSPLIB_EXPORT ReferenceValidator {
public:
	ReferenceValidator();
	bool valid() const;
	void reset();
	void incref();
	void decref();
private:
	mutable int counter;
	bool _valid;
};

class CSPLIB_EXPORT WeakReferencedBase {
public:
	WeakReferencedBase();
	~WeakReferencedBase();
	
	ReferenceValidator* getReferenceValidator();
	
private:
	ReferenceValidator* referenceValidator;
};

typedef WeakReferencedBase WeakReferenced;

CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_WEAKREFERENCED_H__
