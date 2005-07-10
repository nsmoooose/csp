/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Path.h
 * @brief Classes for referring to data sources.
 */

#include <SimData/Path.h>
#include <SimData/Log.h>
#include <SimData/Archive.h>
#include <SimData/DataArchive.h>
#include <SimData/Namespace.h>

#include <sstream>


NAMESPACE_SIMDATA


//////////////////////////////////////////////////////////////////////////
// Path

void Path::setPath(const char* path) {
	if (path != 0 && *path != 0) {
		setPath(hash_string(path));
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


NAMESPACE_SIMDATA_END

