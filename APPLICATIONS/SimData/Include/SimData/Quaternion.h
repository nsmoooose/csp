// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file Quaternion.h
 *
 **/

#ifndef __QUATERNION_H__
#define __QUATERNION_H__


// Modified for CSP by Wolverine
// Modified for SimData by Onsight, Jan 2003.


// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2002.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf


#include <iostream>

#include <SimData/Matrix3.h>
#include <SimData/ns-simdata.h>


NAMESPACE_SIMDATA


/**
 * class Quaternion
 *
 * @author unknown
 */
class SIMDATA_EXPORT Quaternion
{
public:
	double w, x, y, z;

	// construction and destruction
	Quaternion (double fW = 1.0f, double fX = 0.0f, double fY = 0.0f, double fZ = 0.0f);
	Quaternion (const Quaternion& rkQ);

	// conversion between Quaternions, matrices, and angle-axes
	void FromRotationMatrix (const Matrix3& kRot);
	void ToRotationMatrix (Matrix3& kRot) const;
	void FromAngleAxis (const double& rfAngle, const Vector3& rkAxis);
	void ToAngleAxis (double& rfAngle, Vector3& rkAxis) const;
	void FromAxes (const Vector3* akAxis);
	void ToAxes (Vector3* akAxis) const;

	Quaternion & operator+=(const Quaternion & q);
	Quaternion & operator/=(double s);

	// arithmetic operations
	Quaternion& operator= (const Quaternion& rkQ);

	inline Quaternion operator+ (const Quaternion& rhs) const
	{
		return Quaternion(w+rhs.w,x+rhs.x,y+rhs.y,z+rhs.z);
	}

	inline Quaternion operator- (const Quaternion& rhs) const
	{
		return Quaternion(w-rhs.w,x-rhs.x,y-rhs.y,z-rhs.z);
	}

	Quaternion operator* (const Quaternion& rkQ) const;

	inline const Quaternion operator* (const double fScalar) const
	{
		return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
	}

	SIMDATA_EXPORT friend Quaternion operator* (double fScalar, const Quaternion& rkQ);

	Quaternion operator- () const;
	Quaternion operator~ (void) const { return Quaternion(w, -x, -y, -z);}

	// Multiplication of a Quaternion and with a Vector, yielding a quaternion.
	SIMDATA_EXPORT friend Quaternion operator*(Quaternion q, Vector3 v); // const &?
	friend Quaternion operator*(Vector3 v, Quaternion q); // const &?


	// functions of a Quaternion
	double Dot (const Quaternion& rkQ) const;  // dot product
	double Norm () const;  // squared-length
	double Magnitude(void) const;
	Vector3	GetVector(void);
	double	GetScalar(void);

	Quaternion Bar() const;
	Quaternion Inverse () const;       // apply to non-zero Quaternion
	Quaternion UnitInverse () const;  // apply to unit-length Quaternion
	Quaternion Exp () const;
	Quaternion Log () const;

	// Use QVRotate instead.
	// rotation of a vector by a Quaternion
	//    Vector3 operator* (const Vector3& rkVector) const;

	// spherical linear interpolation
	static Quaternion Slerp (double fT, const Quaternion& rkP, const Quaternion& rkQ);

	static Quaternion SlerpExtraSpins (double fT, const Quaternion& rkP, const Quaternion& rkQ,
	int iExtraSpins);

	// setup for spherical quadratic interpolation
	static void Intermediate (const Quaternion& rkQ0,
	const Quaternion& rkQ1, const Quaternion& rkQ2, Quaternion& rka, Quaternion& rkB);

	// spherical quadratic interpolation
	static Quaternion Squad (double fT, 
	                         const Quaternion& rkP, 
	                         const Quaternion& rkA, 
	                         const Quaternion& rkB, 
	                         const Quaternion& rkQ);

	// special values
	static const Quaternion ZERO;
	static const Quaternion IDENTITY;

	static Vector3 MakeEulerAnglesFromQ(Quaternion q);
	static Quaternion MakeQFromEulerAngles(double x, double y, double z);

	SIMDATA_EXPORT friend std::ostream & operator<< (std::ostream & os, const Quaternion & m);

};


SIMDATA_EXPORT Vector3	QVRotate(Quaternion q, Vector3 v);
Quaternion QRotate(Quaternion q1, Quaternion q2);


NAMESPACE_END // namespace simdata


#endif // __QUATERNION_H__

