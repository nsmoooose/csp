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
 * @file External.h
 * @brief Classes for referencing external data sources.
 */

#ifndef __SIMDATA_EXTERNAL_H__
#define __SIMDATA_EXTERNAL_H__

#include <string>
#include <SimData/BaseType.h>


NAMESPACE_SIMDATA

/** Holds the file system path to an external data source.
 *
 *  Paths are stored interally in a platform independent format, and
 *  automatically converted to and from the native format.
 *
 *  @note The utility of this class is rather questionable.  At this point
 *  I recommend using std::string and using functions in simdata::ospath
 *  to convert to native format at runtime. (-MR)
 *
 *  @author Mark Rose <mrose@stm.lbl.gov>
 *  @ingroup BaseTypes
 */
class SIMDATA_EXPORT External: public BaseType {
protected:
	std::string _path;
	std::string _native_path;

public:
	/** Destructor.
	 */
	virtual ~External();

#ifndef SWIG
	/** Assign from a path string, converting to the internal farmat.
	 */
	const External &operator=(std::string const &);

	/** Copy method.
	 */
	const External &operator=(External const &);
#endif // SWIG

	/** Test equality.
	 */
	bool operator==(External const &);

	/** Test inequality.
	 */
	bool operator!=(External const &path) { return !(*this==path); }

	/** Test string equality.
	 */
	bool operator==(std::string const &);

	/** Test string inequality.
	 */
	bool operator!=(std::string const &path) { return !(*this==path); }

	/** Assign from a path string, converting to the internal farmat.
	 */
	void setSource(const char* path);

	/** Get the source path in the platform specific format.
	 */
	const std::string& getSource() const;

	/** Serialize to or from a data archive.
	 */
	virtual void serialize(Archive&);
	
	/** Standard string representation.
	 */
	virtual std::string asString() const;

	/** Return a string representation of the type.
	 */
	virtual std::string typeString() const { return "type::External"; }
};

NAMESPACE_SIMDATA_END


#endif //__SIMDATA_EXTERNAL_H__

