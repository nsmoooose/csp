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


#ifndef __BASETYPE_H__
#define __BASETYPE_H__

#include <string>
#include <iostream>

#include <SimData/Export.h>
#include <SimData/ns-simdata.h>
#include <SimData/Exception.h>

NAMESPACE_SIMDATA

class Packer;
class UnPacker;


SIMDATA_EXCEPTION(ParseException)


/**
 * Base class for objects that support serialization to data archives.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT BaseType {
public:
	/**
	 * Serialize an object to a data archive.
	 */
	virtual void pack(Packer&) const; 
	
	/**
	 * Deserialize an object from a data archive.
	 */
	virtual void unpack(UnPacker&);

	/**
	 * Parse cdata from within the XML tags for this object.
	 */
	virtual void parseXML(const char* cdata);

	/**
	 * Convert XML data to internal format prior to serialization.
	 * 
	 * This method is currently only called by the XML parser for 
	 * Object classes.  If another BaseType class requires this
	 * method, a call must be added to the appropriate XML handler
	 * in the parser. 
	 */
	virtual void convertXML();
	
	/**
	 * Return strig representation of type.
	 */
	virtual std::string asString() const;
};


/**
 * Convenience function for dumping object member variables.
 */
std::ostream &operator <<(std::ostream &o, BaseType const &t);


NAMESPACE_END // namespace simdata


#endif //__BASETYPE_H__
