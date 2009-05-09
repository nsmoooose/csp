/* Combat Simulator Project
 * Copyright (C) 2006 Mark Rose <mkrose@users.sourceforge.net>
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


#ifndef __CSPLIB_UTIL_BOOLEAN_H__
#define __CSPLIB_UTIL_BOOLEAN_H__

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Uniform.h>

namespace csp {

/** A three-state boolean value.  The value can be false, true, or unspecified.
 *  Supports all normal boolean operations for all three state values, returning
 *  a tribool result in most cases.  For example, (true || unspecified) is true,
 *  while (true && unspecified) is unspecified.  Inspired by boost::tribool.
 */
class tribool {
	typedef enum { UNSET=0, FALSE=1, TRUE=2 } State;
	uint8 _state;
	tribool(State state): _state(state) {}

public:
	// Default constructor.  The value is neither true nor false.
	tribool(): _state(UNSET) {}

	tribool(tribool const &other): _state(other._state) {}
	tribool(bool value): _state(value ? TRUE : FALSE) {}

	tribool &operator =(bool value) { _state = (value ? TRUE : FALSE); return *this; }
	tribool &operator =(tribool other) { _state = other._state; return *this; }

	/** Cast to bool.  Returns true iff the tribool value is true.  Both
	 *  false and unspecified tribool values evaluate to false.
	 */
	operator bool() const { return _state == TRUE; }

	/** Logical not.  True values return false, false values return true,
	 *  and unspecified values return unspecified.
	 */
	tribool operator !() const {
		return tribool(_state == FALSE ? TRUE : (_state == TRUE ? FALSE : UNSET));
	}

	/** Bitwise complement.  For regular bool values, the bitwise complement is
	 *  always true, since integral conversion rules of C++  map false to 0 and
	 *  true to 1.  The integer complement of these values are -1 and -2,
	 *  respectively, which evaluate to true in a bool context.  If the tribool
	 *  state is not set to true or false, this operator returns 0, which
	 *  evaluates to false in a bool context.  Hence 'if (~x)' succeeds if x is
	 *  either true or false, and fails if x has not been set.
	 */
	int operator ~() const {
		return _state == UNSET ? 0 : (_state == TRUE ? ~int(true) : ~int(false));
	}

	tribool operator &&(tribool other) const {
		static const unsigned char and_state[12] = {
			UNSET, FALSE, UNSET, UNSET,
			FALSE, FALSE, FALSE, UNSET,
			UNSET, FALSE, TRUE,  UNSET };
		return static_cast<State>(and_state[(other._state << 2) | _state]);
	}

	tribool operator ||(tribool other) const {
		static const unsigned char or_state[12] = {
			UNSET, UNSET, TRUE, UNSET,
			UNSET, FALSE, TRUE, UNSET,
			TRUE,  TRUE,  TRUE, UNSET };
		return static_cast<State>(or_state[(other._state << 2) | _state]);
	}

	/** Test equality of two tribool values.  Note that if the truth state
	 *  of either value is unspecified, the result of the comparison will
	 *  be unspecified.
	 */
	tribool operator ==(tribool other) const {
		static const unsigned char eq_state[12] = {
			UNSET, UNSET, UNSET, UNSET,
			UNSET,  TRUE, FALSE, UNSET,
			UNSET, FALSE,  TRUE, UNSET };
		return static_cast<State>(eq_state[(other._state << 2) | _state]);
	}

	/** Test inequality of two tribool values.  Note that if the truth state
	 *  of either value is unspecified, the result of the comparison will
	 *  be unspecified.
	 */
	tribool operator !=(tribool other) const {
		return !(*this == other);
	}

	// Binary operators with bool to resolve ambiguity between (bool op bool)
	// and (tribool op tribool).
	tribool operator &&(bool other) const { return *this && tribool(other); }
	tribool operator ||(bool other) const { return *this || tribool(other); }
	tribool operator ==(bool other) const { return *this == tribool(other); }
	tribool operator !=(bool other) const { return *this != tribool(other); }
};

} // namespace csp

#endif // __CSPLIB_UTIL_BOOLEAN_H__

