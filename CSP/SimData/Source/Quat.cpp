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
 * @file Quat.cpp
 *
 * A quaternion class.
 *
 * This source code was originally based on the Quat class of
 * the OpenSceneGraph library, Copyright 1998-2003 Robert Osfield.
 * Source code from OpenSceneGraph is used here under the GNU General
 * Public License Version 2 or later, as permitted under the
 * OpenSceneGraph Public License Version 0.0 (exception 3) and the GNU
 * Lesser Public  License Version 2 (clause 3).
 **/


#include <SimData/Quat.h>
#include <SimData/Vector3.h>
#include <SimData/Archive.h>

#include <cstdio>
#include <cmath>
#include <sstream>


/// Good introductions to Quaternions at:
/// http://www.gamasutra.com/features/programming/19980703/quaternions_01.htm
/// http://mathworld.wolfram.com/Quaternion.html


NAMESPACE_SIMDATA


const Quat Quat::IDENTITY(0.0, 0.0, 0.0, 1.0);
const Quat Quat::ZERO(0.0, 0.0, 0.0, 0.0);

void Quat::serialize(Reader &reader) {
	reader >> _w >> _x >> _y >> _z;
}

void Quat::serialize(Writer &writer) const {
	writer << _w << _x << _y << _z;
}

void Quat::parseXML(const char* cdata) {
	std::stringstream ss(cdata);
	std::string token;
	double v[9];
	for (int i = 0; i < 9; i++) {
		if (!(ss >> token)) {
			if (i == 4) {
				set(v[0], v[1], v[2], v[3]);
				return;
			} else {
				throw ParseException("Expect either four (4) or  nine (9) elements in quaternion");
			}
		}
  		v[i] = atof(token.c_str());
	}
	if (ss >> token) {
		throw ParseException("Expect exactly four (4) or nine (9) elements in quaternion");
	}
	set(Matrix3(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8]));
}

std::string Quat::asString() const {
	std::stringstream repr;
	repr << *this;
	return repr.str();
}


/// Set the elements of the Quat to represent a rotation of angle
/// (radians) around the axis (x,y,z)
void Quat::makeRotate(double angle, double x_, double y_, double z_)
{
	double inversenorm  = 1.0/sqrt(x_*x_ + y_*y_ + z_*z_);
	double coshalfangle = cos(0.5*angle);
	double sinhalfangle = sin(0.5*angle);

	_x = x_ * sinhalfangle * inversenorm;
	_y = y_ * sinhalfangle * inversenorm;
	_z = z_ * sinhalfangle * inversenorm;
	_w = coshalfangle;
}


void Quat::makeRotate(double angle, const Vector3& vec) {
	makeRotate(angle, vec.x(), vec.y(), vec.z());
}


void Quat::makeRotate(double angle1, const Vector3& axis1,
                      double angle2, const Vector3& axis2,
                      double angle3, const Vector3& axis3)
{
	Quat q1; q1.makeRotate(angle1, axis1);
	Quat q2; q2.makeRotate(angle2, axis2);
	Quat q3; q3.makeRotate(angle3, axis3);
	*this = q1*q2*q3;
}

// Make a rotation Quat which will rotate vec1 to vec2
// Generally take adot product to get the angle between these
// and then use a cross product to get the rotation axis
// Watch out for the two special cases of when the vectors
// are co-incident or opposite in direction.
void Quat::makeRotate(const Vector3& from, const Vector3& to)
{
	const double epsilon = 0.00001f;

	// dot product vec1*vec2
	double cosangle = (from*to)/(from.length()*to.length());

	if (fabs(cosangle - 1) < epsilon) {
		// cosangle is close to 1, so the vectors are close to being coincident
		// Need to generate an angle of zero with any vector we like
		// We'll choose (1,0,0)
		makeRotate(0.0, 1.0, 0.0, 0.0);
	} else
	if (fabs(cosangle + 1.0) < epsilon) {
		// vectors are close to being opposite, so will need to find a
		// vector orthongonal to from to rotate about.
		simdata::Vector3 tmp;
		if (fabs(from.x())<fabs(from.y()))
		    if (fabs(from.x())<fabs(from.z())) tmp.set(1.0,0.0,0.0); // use x axis.
		    else tmp.set(0.0,0.0,1.0);
		else if (fabs(from.y())<fabs(from.z())) tmp.set(0.0,1.0,0.0);
		else tmp.set(0.0,0.0,1.0);
		// find orthogonal axis.
		Vector3 axis(from^tmp);
		axis.normalize();
		_x = axis[0]; // sin of half angle of PI is 1.0.
		_y = axis[1]; // sin of half angle of PI is 1.0.
		_z = axis[2]; // sin of half angle of PI is 1.0.
		_w = 0.0; // cos of half angle of PI is zero.
	} else {
		// This is the usual situation - take a cross-product of vec1 and vec2
		// and that is the axis around which to rotate.
		Vector3 axis(from^to);
		double angle = acos(cosangle);
		makeRotate(angle, axis);
	}
}

void Quat::makeRotate(double roll, double pitch, double yaw) {
	double sh0 = sin(0.5*roll);
	double ch0 = cos(0.5*roll);
	double sh1 = sin(0.5*pitch);
	double ch1 = cos(0.5*pitch);
	double sh2 = sin(0.5*yaw);
	double ch2 = cos(0.5*yaw);
	_x = sh0 * ch1 * ch2 - ch0 * sh1 * sh2;
	_y = ch0 * sh1 * ch2 + sh0 * ch1 * sh2;
	_z = ch0 * ch1 * sh2 - sh0 * sh1 * sh2;
	_w = ch0 * ch1 * ch2 + sh0 * sh1 * sh2;
}

void Quat::getEulerAngles(double &roll, double &pitch, double &yaw) const {
	double x2 = _x*_x;
	double y2 = _y*_y;
	double z2 = _z*_z;
	double w2 = _w*_w;
	double sin_theta = 2.0 * (_w*_y - _x*_z);
	if (fabs(sin_theta) > 0.99999) {
		roll = 0.0;
		pitch = asin(sin_theta);
		yaw = atan2(2.0*(_w*_z - _x*_y), w2-x2+y2-z2);
	} else {
		roll = atan2(2.0*(_y*_z + _w*_x), w2-x2-y2+z2);
		pitch = asin(sin_theta);
		yaw = atan2(2.0*(_x*_y + _w*_z), w2+x2-y2-z2);
	}
}

void Quat::getRotate(double& angle, Vector3& vec) const {
	getRotate(angle, vec.x(), vec.y(), vec.z());
}


// Get the angle of rotation and axis of this Quat object.
// Won't give very meaningful results if the Quat is not associated
// with a rotation!
void Quat::getRotate(double& angle, double& x_, double& y_, double& z_) const {
	double sinhalfangle = sqrt(_x*_x + _y*_y + _z*_z);
	angle = 2 * atan2(sinhalfangle, _w);
	if(sinhalfangle) {
		x_ = _x / sinhalfangle;
		y_ = _y / sinhalfangle;
		z_ = _z / sinhalfangle;
	} else {
		x_ = 0.0;
		y_ = 0.0;
		z_ = 1.0;
	}
}

Vector3 Quat::rotate(const Vector3 &v) const {
	double ux =  _w*v.x() + _y*v.z() - _z*v.y();
	double uy =  _w*v.y() + _z*v.x() - _x*v.z();
	double uz =  _w*v.z() + _x*v.y() - _y*v.x();
	double uw = -_x*v.x() - _y*v.y() - _z*v.z();
	double vx = -uw*_x + ux*_w - uy*_z + uz*_y;
	double vy = -uw*_y + uy*_w - uz*_x + ux*_z;
	double vz = -uw*_z + uz*_w - ux*_y + uy*_x;
	return Vector3(vx, vy, vz);
}

Vector3 Quat::invrotate(const Vector3 &v) const {
	double uw =  _x*v.x() + _y*v.y() + _z*v.z();
	double ux =  _w*v.x() - _y*v.z() + _z*v.y();
	double uy =  _w*v.y() - _z*v.x() + _x*v.z();
	double uz =  _w*v.z() - _x*v.y() + _y*v.x();
	double vx = uw*_x + ux*_w + uy*_z - uz*_y;
	double vy = uw*_y + uy*_w + uz*_x - ux*_z;
	double vz = uw*_z + uz*_w + ux*_y - uy*_x;
	return Vector3(vx, vy, vz);
}

/// Spherical Linear Interpolation
/// As t goes from 0 to 1, the Quat object goes from "from" to "to"
/// Reference: Shoemake at SIGGRAPH 89
void Quat::slerp(double t, const Quat& from, const Quat& to) {
	const double epsilon = 0.00001;
	double cosomega, scale_from, scale_to;

	simdata::Quat quatTo(to);
	cosomega = from._x*to._x + from._y*to._y + from._z*to._z + from._w*to._w;

	if (cosomega < 0.0) {
		cosomega = -cosomega;
		quatTo.set(-to._x, -to._y, -to._z, -to._w);
	}

	if((1.0 - cosomega) > epsilon) {
		double omega = acos(cosomega);  // 0 <= omega <= Pi (see man acos)
		double invsinomega = 1.0/sin(omega);  // this sinomega should always be +ve so
		scale_from = sin((1.0-t)*omega)*invsinomega;
		scale_to = sin(t*omega)*invsinomega;
	} else {
		 // The ends of the vectors are very close  we can use
		 // simple linear interpolation
		scale_from = 1.0 - t;
		scale_to = t;
	}

	_x = from._x * scale_from + quatTo._x * scale_to;
	_y = from._y * scale_from + quatTo._y * scale_to;
	_z = from._z * scale_from + quatTo._z * scale_to;
	_w = from._w * scale_from + quatTo._w * scale_to;
}



void Quat::set(const Matrix3& m) {
	// Source: Gamasutra, Rotating Objects Using Quaternions

	double  tr, s;
	tr = m(0,0) + m(1,1) + m(2,2);

	// check the diagonal
	if (tr > 0.0) {
		s = (double)sqrt (tr + 1.0);
		_w = s * 0.5;
		s = 0.5 / s;
		_x = (m(2,1) - m(1,2)) * s;
		_y = (m(0,2) - m(2,0)) * s;
		_z = (m(1,0) - m(0,1)) * s;
	} else {
		double tq[3];

		static const int nxt[3] = {1, 2, 0};

		// diagonal is negative
		int i = 0;
		if (m(1,1) > m(0,0)) {
			i = 1;
		}
		if (m(2,2) > m(i,i)) {
			i = 2;
		}
		int j = nxt[i];
		int k = nxt[j];

		s = (double)sqrt((m(i,i) - (m(j,j) + m(k,k))) + 1.0);

		tq[i] = s * 0.5;

		if (s != 0.0) {
			s = 0.5 / s;
		}

		tq[j] = (m(j,i) + m(i,j)) * s;
		tq[k] = (m(k,i) + m(i,k)) * s;
		_w = (m(k,j) - m(j,k)) * s;

		_x = tq[0];
		_y = tq[1];
		_z = tq[2];
	}
}


void Quat::get(Matrix3& m) const {
	// Source: Gamasutra, Rotating Objects Using Quaternions
	double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = _x + _x;
	y2 = _y + _y;
	z2 = _z + _z;

	xx = _x * x2;
	xy = _x * y2;
	xz = _x * z2;

	yy = _y * y2;
	yz = _y * z2;
	zz = _z * z2;

	wx = _w * x2;
	wy = _w * y2;
	wz = _w * z2;

	m(0,0) = 1.0 - (yy + zz);
	m(0,1) = xy - wz;
	m(0,2) = xz + wy;

	m(1,0) = xy + wz;
	m(1,1) = 1.0 - (xx + zz);
	m(1,2) = yz - wx;

	m(2,0) = xz - wy;
	m(2,1) = yz + wx;
	m(2,2) = 1.0 - (xx + yy);
}

int Quat::readBinary(const unsigned char * ptrBuf, int size)
{
	if (size != 4*sizeof(double)) {
		printf("Trying to read a binary Quat with the wrong size\n");
		return 0;
	}
	memcpy((void*)&_x, (void*)ptrBuf, sizeof(double)); ptrBuf += sizeof(double);
	memcpy((void*)&_y, (void*)ptrBuf, sizeof(double)); ptrBuf += sizeof(double);
	memcpy((void*)&_z, (void*)ptrBuf, sizeof(double)); ptrBuf += sizeof(double);
	memcpy((void*)&_w, (void*)ptrBuf, sizeof(double)); ptrBuf += sizeof(double);
	return 4*sizeof(double);
}

int Quat::writeBinary(unsigned char * ptrBuf, int size)
{
	if (size != 4*sizeof(double)) {
		printf("Trying to write a binary Quat with the wrong size\n");
		return 0;
	}
	memcpy((void*)ptrBuf, (void*)&_x, sizeof(double)); ptrBuf += sizeof(double);
	memcpy((void*)ptrBuf, (void*)&_y, sizeof(double)); ptrBuf += sizeof(double);
	memcpy((void*)ptrBuf, (void*)&_z, sizeof(double)); ptrBuf += sizeof(double);
	memcpy((void*)ptrBuf, (void*)&_w, sizeof(double)); ptrBuf += sizeof(double);
	return 4*sizeof(double);
}

std::ostream& operator << (std::ostream& output, const Quat& quat)
{
	output << "[" << quat._x
	       << " " << quat._y
	       << " " << quat._z
	       << " " << quat._w << "]";
	return output;
}

NAMESPACE_SIMDATA_END

