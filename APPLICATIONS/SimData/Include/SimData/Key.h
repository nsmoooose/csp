/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
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
 */

#ifndef __SIMDATA_KEY_H__
#define __SIMDATA_KEY_H__

#include <string>
#include <SimData/BaseType.h>
#include <SimData/HashUtility.h>


NAMESPACE_SIMDATA

/**
 * A 32-bit hash key identifier.
 *
 * Can be used in place of strings and enums to bind internal
 * and external identifiers efficiently.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT Key: public BaseType {
protected:
	u4 _key;

public:

	/**
	 * Default constructor.
	 */
	Key(): _key(0) {}

	/**
	 * Construct a new key from a string.
	 */
	Key(std::string const &k) { *this = k; }

	/** 
	 * Copy constructor.
	 */
	Key(Key const &k): _key(k._key) {}

	/**
	 * Destructor.
	 */
	virtual ~Key() {}

#ifndef SWIG
	/**
	 * Assign from a string, computing the corresponding key.
	 */
	const Key &operator=(std::string const &);

	/**
	 * Copy method.
	 */
	inline const Key &operator=(Key const &k) { _key = k._key; return *this; }

#endif // SWIG

	/**
	 * Comparison
	 */
	inline bool operator==(Key const &k) const { return _key == k._key; }

	/**
	 * String comparison.
	 */
	bool operator==(std::string const &) const;

	/**
	 * Serialize.
	 */
	virtual void pack(Packer& p) const;
	
	/**
	 * Deserialize.
	 */
	virtual void unpack(UnPacker& p);
	
	/**
	 * Standard string representation.
	 */
	virtual std::string asString() const;
};

NAMESPACE_END // namespace simdata


#endif //__SIMDATA_KEY_H__
