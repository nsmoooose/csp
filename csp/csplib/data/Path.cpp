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
 * @file Path.cpp
 * @brief Classes for referring to data sources.
 */

#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Archive.h>
#include <csp/csplib/data/DataArchive.h>
#include <csp/csplib/util/Log.h>

#include <sstream>


namespace csp {


//////////////////////////////////////////////////////////////////////////
// Path

void Path::setPath(const char* path) {
	if (path != 0 && *path != 0) {
		setPath(ObjectID(path));
	} else {
		setNone();
	}
}

void Path::serialize(Reader &reader) {
	reader >> _path;
}

void Path::serialize(Writer &writer) const {
	writer << _path;
}

std::string Path::asString() const {
	std::ostringstream repr;
	repr << "<Path " << _path << ">";
	return repr.str();
}

std::ostream &operator <<(std::ostream &o, Path const &p) { return o << p.asString(); }


} // namespace csp

