/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
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
 * @file Key.h
 *
 * 32-Bit hash key identifier class.
 */

#ifndef __SIMDATA_KEY_H__
#define __SIMDATA_KEY_H__

#include <string>
#include <SimData/BaseType.h>
#include <SimData/HashUtility.h>


NAMESPACE_SIMDATA

/**
 * @brief A 32-bit hash key identifier.
 *
 * Can be used in place of strings and enums to bind internal
 * and external identifiers efficiently.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 * @ingroup BaseTypes
 */
class SIMDATA_EXPORT Key: public BaseType {
protected:
	uint32 _key;

public:

	/** Default constructor.
	 */
	Key(): _key(0) {}

	/** Construct a new key from a string.
	 */
	Key(std::string const &k) { *this = k; }

	/** Copy constructor.
	 */
	Key(Key const &k): BaseType(k), _key(k._key) {}

	/** Destructor.
	 */
	virtual ~Key() {}

#ifndef SWIG
	/** Assign from a string, computing the corresponding key.
	 */
	const Key &operator=(std::string const &);

	/** Copy method.
	 */
	inline const Key &operator=(Key const &k) { _key = k._key; return *this; }

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

	/** Serialize to or from a data archive.
	 */
	virtual void serialize(Archive&);
	
	/** Standard string representation.
	 */
	virtual std::string asString() const;

	/** Return a string representation of the type.
	 */
	virtual std::string typeString() const { return "type::Key"; }
};

NAMESPACE_SIMDATA_END


#endif //__SIMDATA_KEY_H__

