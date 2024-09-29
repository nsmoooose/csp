#pragma once
/* Combat Simulator Project
 * Copyright (C) 2002, 2005 Mark Rose <mkrose@users.sf.net>
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
 * @brief Data type for referencing external data sources.
 */

#include <string>
#include <csp/csplib/data/BaseType.h>

namespace csp {

/** Holds the file system path to an external data source.
 *
 *  Paths are stored interally in a platform independent format, and
 *  automatically converted to and from the native format.
 *
 *  @ingroup BaseTypes
 */
class CSPLIB_EXPORT External {
protected:
	std::string _path;
	std::string _native_path;

public: // BaseType
	External();
	External(const char* data);

	/// String representation.
	std::string asString() const;

	/// Type representation.
	std::string typeString() const { return "type::External"; }

	/// Serialize from a Reader.
	void serialize(Reader&);

	/// Serialize to a Writer.
	void serialize(Writer&) const;

	/** Parse the character data from an XML \<External\> tag.
	 */
	void parseXML(const char* cdata) { checkEmptyTag(cdata); }

	/// XML post processing.
	void convertXML() {}

public:

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

	/** Test if the path is not set.
	 */
	bool empty() const { return _path.empty(); }

	/** Assign from a path string, converting to the internal farmat.
	 */
	void setSource(const char* path);

	/** Get the source path in the platform specific format.
	 */
	const std::string& getSource() const;
};


CSPLIB_EXPORT std::ostream &operator <<(std::ostream &o, External const &e);


} // namespace csp
