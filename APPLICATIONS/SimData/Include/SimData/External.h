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
 * @file External.h
 */

#ifndef __EXTERNAL_H__
#define __EXTERNAL_H__

#include <string>
#include <SimData/BaseType.h>


NAMESPACE_SIMDATA

/**
 * Holds the file system path to an external data source.
 *
 * Paths are stored interally in a platform independent format, and
 * automatically converted to and from the native format.
 *
 * The utility of this class is rather questionable.  At this point
 * I recommend using std::string and using functions in simdata::ospath
 * to convert to native format at runtime. (-MR)
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT External: public BaseType {
protected:
	std::string _path;
	std::string _native_path;

public:
	/**
	 * Destructor.
	 */
	virtual ~External();

#ifndef SWIG
	/**
	 * Assign from a path string, converting to the internal farmat.
	 */
	const External &operator=(std::string const &);

	/**
	 * Copy method.
	 */
	const External &operator=(External const &);
#endif // SWIG

	/** 
	 * Assign from a path string, converting to the internal farmat.
	 */
	void setSource(const char* path);

	/**
	 * Get the source path in the platform specific format.
	 */
	const std::string& getSource() const;

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


#endif //__EXTERNAL_H__

