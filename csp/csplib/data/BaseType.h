/* Combat Simulator Project
 * Copyright (C) 2002, 2003 Mark Rose <mkrose@users.sf.net>
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
 *
 * BaseType is an interface specification implemented by all csplib data
 * types.  Note that the types do not inherit from BaseType.
 *
 * @code
 * Interface BaseType {
 *
 *   // Read and write from a data archive.
 *   void serialize(Reader&)=0;
 *   void serialize(Writer&) const=0;
 *
 *   // Parse cdata from within the XML tags for this object.
 *   void parseXML(const char* cdata);
 *
 *   // Convert XML data to internal format prior to serialization.
 *   //
 *   // This method is currently only called by the XML parser for
 *   // Object classes.  If another BaseType class requires this
 *   // method, a call must be added to the appropriate XML handler
 *   // in the parser.
 *   void convertXML();
 *
 *   // Return a string representation of the instance.
 *   //
 *   // This method is used to provide a string representation of
 *   // the object for output to ostreams (using <<), and also
 *   // serves as __repr__ in Python.
 *   std::string asString() const=0;
 *
 *   // Return a string representation of the type.
 *   std::string typeString() const=0;
 * }
 *
 * // Convenience function for printing BaseTypes to ostreams.
 * std::ostream &operator <<(std::ostream &o, BaseType const &t);
 * @endcode
 *
 */


#ifndef __CSPLIB_DATA_BASETYPE_H__
#define __CSPLIB_DATA_BASETYPE_H__

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Exception.h>

#include <string>
#include <iosfwd>


namespace csp {

class Reader;
class Writer;


/** Error parsing XML cdata.
 */
CSP_EXCEPTION(ParseException)

/** Convenience method for implementing parseXML methods that expect no cdata.
 */
inline void checkEmptyTag(const char* cdata) throw(ParseException) {
	std::string s(cdata);
	if (s.find_first_not_of("\t \r\n") == std::string::npos) return;
	throw ParseException("WARNING: #cdata ignored");
}

// Forward declare all BaseTypes.
class ECEF;
class EnumLink;
class External;
class Key;
template <class T> class Link;
class LinkCore;
class LinkBase;
class LLA;
template <int N, typename X> class LUT;
typedef LUT<1,float> Table1;
typedef LUT<2,float> Table2;
typedef LUT<3,float> Table3;
class Matrix3;
class Object;
class Path;
class Quat;
class Real;
class SimDate;
class UTM;
class Vector2;
class Vector3;


} // namespace csp


#endif // __CSPLIB_DATA_BASETYPE_H__

