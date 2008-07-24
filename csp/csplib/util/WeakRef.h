/* Combat Simulator Project
 * Copyright (C) 2002, 2003, 2004 Mark Rose <mkrose@users.sf.net>
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
 * @file WeakRef.h
 *
 * Reference counting smart-pointer classes.
 */


#ifndef __CSPLIB_UTIL_WEAKREF_H__
#define __CSPLIB_UTIL_WEAKREF_H__

#include <csp/csplib/util/WeakReferenced.h>

namespace csp {

template<class T> class WeakRef {
public:
	typedef T element_type;

	WeakRef() : reference(NULL), validator(NULL) {
	}

  	WeakRef(Ref<T> & r) : reference(NULL), validator(NULL) {
  		attach(r.get());
  	}
  	  	
	WeakRef(WeakRef<T> & r) {
		attach(r.get());
	}

	~WeakRef() {
		reset();
	}

  	WeakRef<T> & operator=(WeakRef<T> & r) {
  		reset();
  		attach(r.get());
		return *this;
  	}
  	
	WeakRef<T> & operator=(Ref<T> & r) {
		reset();
  		attach(r.get());
  		return *this;
	}
	
	WeakRef<T> & operator=(T* r) {
		attach(r);
		return *this;
	}
		
	T* operator->() {
		return get();
	}
	
	const T* operator->() const {
		return get();
	}

	T* get() {
		if(valid()) {
			return reference;
		}
		return NULL;
	}

	T* get() const {
		if(valid()) {
			return reference;
		}
		return NULL;
	}

	// Attaches a weak reference to an existing reference.
	void attach(T* r) {
		if(r == NULL) {
			return;
		}
		
		// Test to see if the class is implementing WeakReferenced base class.
		WeakReferenced* weakReferenced = dynamic_cast<WeakReferenced*>(r);
		if(weakReferenced == NULL) {
			reference = NULL;
			return;
		}
		
		validator = weakReferenced->getReferenceValidator();
		reference = r;	
	}

	// Obtains a strong reference to the object.
	Ref<T> lock() const {
		if(valid()) {
			return Ref<T>(reference);
		}
		return Ref<T>(NULL);
	}

	// Resets this weak reference to null.
	void reset() {
		reference = NULL;
		if(validator != NULL) {
			validator->decref();
			validator = NULL;
		}
	}

	// Returns true if this weak reference is still valid.		
	bool valid() const {
		if(validator != NULL) {
			return validator->valid();
		}
		return false;
	}

private:
	element_type* reference;
	ReferenceValidator* validator; 
};

} // namespace csp
  
#endif // __CSPLIB_UTIL_WEAKREF_H__
