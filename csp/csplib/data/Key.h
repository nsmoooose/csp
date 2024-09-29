#pragma once
/* Combat Simulator Project
 * Copyright 2002-2005 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Key.h
 *
 * 32-Bit hash key identifier class.
 */

#include <csp/csplib/util/HashUtility.h>
#include <csp/csplib/data/BaseType.h>
#include <string>


namespace csp {

/**
 * @brief A 32-bit hash key identifier.
 *
 * Can be used in place of strings and enums to bind internal
 * and external identifiers efficiently.
 *
 * @ingroup BaseTypes
 */
class CSPLIB_EXPORT Key {
	uint32 _key;

public: // BaseType

	/// String representation.
	std::string asString() const;

	/// Type representation.
	std::string typeString() const { return "type::Key"; }

	/// Serialize from a Reader.
	void serialize(Reader&);

	/// Serialize to a Writer.
	void serialize(Writer&) const;

	/** Parse the character data from an XML \<Key\> tag.
	 */
	void parseXML(const char* cdata) { checkEmptyTag(cdata); }

	/// XML post processing.
	void convertXML() {}

public:

	/** Default constructor.
	 */
	Key(): _key(0) {}

	/** Construct a new key from a string.
	 */
	Key(std::string const &k) { *this = k; }

	/** Copy constructor.
	 */
	Key(Key const &k): _key(k._key) {}

#ifndef SWIG
	/** Assign from a string, computing the corresponding key.
	 */
	const Key &operator=(std::string const &);

	/** Copy method.
	 */
	inline const Key &operator=(Key const &k) { _key = k._key; return *this; }

	/** Test if uninitialized.
	 */
	inline bool operator!() const { return _key == 0; }
#endif // SWIG

	/** Compare equal
	 */
	inline bool operator==(Key const &k) const { return _key == k._key; }

	/** Compare unequal
	 */
	inline bool operator!=(Key const &k) const { return _key != k._key; }

	/** String compare equal.
	 */
	bool operator==(std::string const &) const;

	/** String compare unequal
	 */
	bool operator!=(std::string const &k) const { return !(*this==k); }

	/** Order comparison, for use with standard stl comparisons
	 */
	bool operator<(Key const &k) const { return _key < k._key; }
};


CSPLIB_EXPORT std::ostream &operator <<(std::ostream &o, Key const &k);


} // namespace csp
