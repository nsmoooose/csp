/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002, 2003 Mark Rose <tm2@stm.lbl.gov>
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
 * @file BaseType.h
 * @brief Base class for objects that support serialization to data archives.
 */

/**
 * @defgroup BaseTypes Data classes
 */


#ifndef __SIMDATA_BASETYPE_H__
#define __SIMDATA_BASETYPE_H__

#include <string>
#include <iostream>

#include <SimData/Export.h>
#include <SimData/Namespace.h>
#include <SimData/ExceptionBase.h>


NAMESPACE_SIMDATA

class Archive;
class Packer;
class UnPacker;


/** Error parsing XML cdata.
 */
SIMDATA_EXCEPTION(ParseException)


/** Base class for objects that support serialization to data archives.
 *
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT BaseType {
public:
	/** Ensure virtual destructor
	 */
	virtual ~BaseType();

	/** Serialize an object to or from a data archive 
	 */
	virtual void serialize(Archive&) {}

	/** Parse cdata from within the XML tags for this object.
	 */
	virtual void parseXML(const char* cdata);

	/** Convert XML data to internal format prior to serialization.
	 * 
	 *  This method is currently only called by the XML parser for 
	 *  Object classes.  If another BaseType class requires this
	 *  method, a call must be added to the appropriate XML handler
	 *  in the parser. 
	 */
	virtual void convertXML();
	
	/** Return a string representation of the instance.
	 *  
	 *  This method is used to provide a string representation of
	 *  the object for output to ostreams (using <<), and also 
	 *  serves as __repr__ in Python.
	 */
	virtual std::string asString() const=0;

	/** Return a string representation of the type.
	 */
	virtual std::string typeString() const=0;
};


/** Convenience function for printing BaseTypes to ostreams.
 */
std::ostream &operator <<(std::ostream &o, BaseType const &t);


NAMESPACE_SIMDATA_END


#endif //__SIMDATA_BASETYPE_H__

