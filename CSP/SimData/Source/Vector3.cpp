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


#include <SimData/Vector3.h>
#include <SimData/Matrix3.h>
#include <SimData/Archive.h>

#include <iomanip>
#include <sstream>


NAMESPACE_SIMDATA


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
	std::stringstream repr;
	repr << *this;
	return repr.str();
}

/**
 * Print string representation to a stream.
 */
std::ostream & operator << (std::ostream & os, const Vector3& v) {
	os << "[" << std::setw(8) << v.x() 
	   << " " << std::setw(8) << v.y() 
 	   << " " << std::setw(8) << v.z() << "]";
	return os;
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

/**
 * Copy a vector into a binary buffer
 */
int Vector3::readBinary(const unsigned char * data, int size) {
	
	if (size != 3*sizeof(double))
	{
		fprintf(stderr, "Illegal mem copy operation attempted\n");
		return 0;
	}
        
	memcpy((void*)&_x, (void*)data, sizeof(double)); data += sizeof(double);	
	memcpy((void*)&_y, (void*)data, sizeof(double)); data += sizeof(double);
	memcpy((void*)&_z, (void*)data, sizeof(double));

	return 3*sizeof(double);
}

/**
 * Copy a binary buffer into a vector.
 */
int Vector3::writeBinary(unsigned char * data, int size) {

	if (size != 3*sizeof(double))
	{
		fprintf(stderr, "Illegal mem copy operation attempted\n");
		return 0;
	}
	
	memcpy( (void*)data, (void*)&_x, sizeof(double)); data += sizeof(double);	
	memcpy( (void*)data, (void*)&_y, sizeof(double)); data += sizeof(double);
	memcpy( (void*)data, (void*)&_z, sizeof(double));

	return 3*sizeof(double);

}



NAMESPACE_SIMDATA_END

