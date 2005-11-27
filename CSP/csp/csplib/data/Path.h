/* Combat Simulator Project
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Path.h
 * @brief Classes for referring to data sources.
 */


#ifndef __CSPLIB_DATA_PATH_H__
#define __CSPLIB_DATA_PATH_H__

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#pragma warning (disable : 4290)
#endif

#include <csp/csplib/data/BaseType.h>
#include <csp/csplib/util/HashUtility.h>
#include <string>

CSP_NAMESPACE


class DataArchive;

typedef hasht ObjectID;


CSP_EXCEPTION(ObjectTypeMismatch)


/**
 * @brief Path to an object record in a data archive.
 *
 * Object paths use '.' to separate path elements, and uniquely
 * identify objects in a data archive.  Path objects store the
 * path as a 64-bit hash of the path string, called an Object ID.
 *
 * @author Mark Rose <mkrose@users.sf.net>
 * @ingroup BaseTypes
 */
class CSP_EXPORT Path
{
protected:
	ObjectID _path;

public: // BaseType

	/// String representation.
	std::string asString() const;

	/// Type representation.
	std::string typeString() const { return "type::Path"; }

	/// Serialize from a Reader.
	void serialize(Reader&);

	/// Serialize to a Writer.
	void serialize(Writer&) const;

	/// The Path tag contents are passed directly to the constructor.
	void parseXML(const char*) { }

	/// XML post processing.
	void convertXML() {}

public:
	/** Create a new Path using a path string.
	 */
	explicit Path(const char* path=0) {
		setPath(path);
	}

	/** Create a new Path from an Object ID.
	 */
	explicit Path(ObjectID path): _path(path) {
	}

	/** Assign to a specific Object ID.
	 */
	inline void setPath(ObjectID path) {
		_path = path;
	}

	/** Assign to an object path string (after converting to an
	 *  Object ID).
	 */
	void setPath(const char* path);
	
	/** Assign to Object ID 0 (no-path).
	 */
	inline void setNone() {
		setPath((ObjectID) 0);
	}

	/** Get the path's Object ID.
	 */
	inline const ObjectID getPath() const {
		return _path;
	}

	/** Test for 'no-path' (Object ID == 0).
	 */
	inline bool isNone() const {
		return (_path == (ObjectID) 0);
	}

	/** Test for equality of two paths.
	 */
	inline bool operator==(Path const &p) const {
		return _path == p._path;
	}

	/** Test for equality of two paths.
	 */
	inline bool operator!=(Path const &p) const {
		return _path != p._path;
	}
};


CSP_EXPORT std::ostream &operator <<(std::ostream &o, Path const &p);

CSP_NAMESPACE_END

#endif // __CSPLIB_DATA_PATH_H__

