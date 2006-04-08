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
 * @file Vector3.cpp
 *
 * A three-dimensional vector class.
 *
 * This source code was originally based on the Vec3 class of
 * the OpenSceneGraph library, Copyright 1998-2003 Robert Osfield.
 * Source code from OpenSceneGraph is used here under the GNU General
 * Public License Version 2 or later, as permitted under the
 * OpenSceneGraph Public License Version 0.0 (exception 3) and the GNU
 * Lesser Public  License Version 2 (clause 3).
 **/


#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Archive.h>

#include <iomanip>
#include <sstream>


CSP_NAMESPACE


/// Null vector
const Vector3 Vector3::ZERO (0.0, 0.0, 0.0);
/// Unit vector in X
const Vector3 Vector3::XAXIS(1.0, 0.0, 0.0);
/// Unit vector in Y
const Vector3 Vector3::YAXIS(0.0, 1.0, 0.0);
/// Unit vector in Z
const Vector3 Vector3::ZAXIS(0.0, 0.0, 1.0);

Matrix3 Vector3::starMatrix() const {
	return Matrix3(0.0, -_z, _y, _z, 0.0, -_x, -_y, _x, 0.0);
}

std::string Vector3::asString() const {
	std::ostringstream os;
	os << "[" << std::setw(8) << x() << " "
	          << std::setw(8) << y() << " "
	          << std::setw(8) << z() << "]";
	return os.str();
}

void Vector3::serialize(Reader &reader) {
	reader >> _x >> _y >> _z;
}

void Vector3::serialize(Writer &writer) const {
	writer << _x << _y << _z;
}

void Vector3::parseXML(const char* cdata) {
	int n = sscanf(cdata, "%lf %lf %lf", &_x, &_y, &_z);
	if (n!=3) throw ParseException("SYNTAX ERROR: expecting 3 floats");
}

std::ostream &operator <<(std::ostream &o, Vector3 const &v) { return o << v.asString(); }

CSP_NAMESPACE_END
