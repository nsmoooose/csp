/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * Combat Simulator Project - FlightSim Demo
 * Copyright (C) 2002 The Combat Simulator Project
 * http://csp.sourceforge.net
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


/**
 * @file Vector3.cpp
 *
 * Initial CSP version by Wolverine69
 * Code based on libraries from magic-software.com and Game Programming Gems.
 **/


#include <SimData/Vector3.h>
#include <SimData/Matrix3.h>
#include <SimData/Pack.h>

#include <iomanip>


NAMESPACE_SIMDATA


/// Null vector
const Vector3 Vector3::ZERO (0.0, 0.0, 0.0);
/// Unit vector in X
const Vector3 Vector3::XAXIS(1.0, 0.0, 0.0);
/// Unit vector in Y
const Vector3 Vector3::YAXIS(0.0, 1.0, 0.0);
/// Unit vector in Z
const Vector3 Vector3::ZAXIS(0.0, 0.0, 1.0);


Vector3 operator/ (const Vector3 & a, double f) //Checked (delta)
{
	Vector3 newVect;

	if ( f != 0.0 )
	{
		double fInv = 1.0f/f;
		newVect.x = fInv*a.x;
		newVect.y = fInv*a.y;
		newVect.z = fInv*a.z;
		return newVect;
	}
	else
	{
		// need to properly handle error condition
		return Vector3::ZERO;
	}
}

Vector3& Vector3::operator/= (double f)
{
	if ( f != 0.0 )
	{
		double fInv = 1.0f/f;
		x *= fInv;
		y *= fInv;
		z *= fInv;
	}
	else
	{
		// need to properly handle error condition.
		x = 0;
		y = 0;
		z = 0;
	}
	return *this;
}

Vector3& Vector3::Normalize() // Checked (delta)
{
	double m = Length();
	if (m > 0.0F)
		m = 1.0F / m;
	else
		m = 0.0F;
	x *= m;
	y *= m;
	z *= m;
	return *this;
}


double Vector3::Unitize (double fTolerance)
{
	double fLength = Length();

	if ( fLength > fTolerance )
	{
		double fInvLength = 1.0f/fLength;
		x *= fInvLength;
		y *= fInvLength;
		z *= fInvLength;
	}
	else
	{
		fLength = 0.0;
	}
	return fLength;
}

void Vector3::Print(FILE * stream) const
{
	fprintf(stream, "[%f, %f, %f]\n", x, y, z);
}


Matrix3 Vector3::StarMatrix() const
{
	Matrix3 mat;
	mat[0][0] = 0.0;
	mat[0][1] = -z;
	mat[0][2] = y;
	mat[1][0] = z;
	mat[1][1] = 0.0;
	mat[1][2] = -x;
	mat[2][0] = -y;
	mat[2][1] = x;
	mat[2][2] = 0.0;
	return mat;
}

std::string Vector3::asString() const
{
	char buff[128];
	sprintf(buff, "[%.3f, %.3f, %.3f]", x, y, z);
	return buff;
}

/**
 * Print string representation to a stream.
 */
std::ostream & operator << (std::ostream & os, const Vector3& v)
{
  os << "["  << std::setw(8) << v.x 
     << ", " << std::setw(8) << v.y 
     << ", " << std::setw(8) << v.z << "]";
  return os;
}

Vector3 Vector3::Cross(const Vector3 & a) const {
	return SIMDATA(Cross)(*this, a);
}

double Vector3::Dot(const Vector3 & a) const {
	return SIMDATA(Dot)(*this, a);
}

std::vector<double> Vector3::GetElements() const {
	std::vector<double> elements;
	elements.push_back(x);
	elements.push_back(y);
	elements.push_back(z);
	return elements;
}

void Vector3::pack(Packer& p) const {
	p.pack(x);
	p.pack(y);
	p.pack(z);
}

void Vector3::unpack(UnPacker& p) {
	p.unpack(x);
	p.unpack(y);
	p.unpack(z);
}

void Vector3::parseXML(const char* cdata) {
	double X, Y, Z;
	int n = sscanf(cdata, "%lf %lf %lf", &X, &Y, &Z);
	x = X;
	y = Y;
	z = Z;
	if (n!=3) throw ParseException("SYNTAX ERROR: expecting 3 floats");
}


NAMESPACE_END // namespace simdata
