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
 * @file Optional.h
 *
 */

#ifndef __CSPLIB_UTIL_OPTIONAL_H__
#define __CSPLIB_UTIL_OPTIONAL_H__

#include <csp/csplib/util/Namespace.h>

namespace csp {

/** A optional class inspired by boost::optional.
 */
template<class T>
class optional {
public:
	/** Constructs the optional without a value. */
	optional() : m_object(0) {}
	optional(const optional<T>& opt) : m_object(0) {
		*this = opt;
	}
	
	/** Constructs the optional with a value.*/
	optional(const T& val) : m_object(0) { assign(val); }
	~optional() { reset(); }
	
	/** Assigns a new value. Reseting the existing value if needed */
	void assign(const T& val) {
		reset();
		m_object = new T(val);
	}

	/** Returns the pointer to the value or null if no value has been assigned. */
	T* get_ptr() { return m_object == 0 ? 0 : m_object; }
	const T* get_ptr() const { return m_object == 0 ? 0 : m_object; }
	
	/** Resets the existing value if there is one. */
	void reset() {
		if(m_object != NULL) {
			delete m_object;
			m_object = 0;
		}
	}
	
	optional& operator=(const T& val) {
		assign(val);
		return *this;
	}
	
	optional& operator=(const optional<T>& opt) {
		reset();
		if(opt.get_ptr() != 0) {
			assign(*opt);
		}
		return *this;
	}
	
	operator bool() const { return hasValue(); }
	
	T* operator->() { return get_ptr(); }
	const T* operator->() const { return get_ptr(); }
	
	const T& operator*() const { return *m_object; }
	T& operator*() { return *m_object; }
	
	bool operator!() const { return !hasValue(); }
	
	bool operator==(const optional<T>& opt) const {
		return equals(opt);
	}
	
	bool operator!=(const optional<T>& opt) const {
		return !equals(opt);
	}

	bool hasValue() const {	return this->get_ptr() != 0; }
	
	bool equals(const optional<T>& opt) const {
		if(hasValue() && opt.hasValue()) {
			return *m_object == *opt.m_object;
		}
		else if(hasValue() == opt.hasValue()) {
			return true;
		}
		return false;
	}
	
private:
	T* m_object;
};

} // namespace csp

#endif //__CSPLIB_UTIL_OPTIONAL_H__
