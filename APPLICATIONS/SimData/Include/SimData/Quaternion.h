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

#ifndef __SIMDATA_QUATERNION_H__
#define __SIMDATA_QUATERNION_H__


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

#include <SimData/BaseType.h>
#include <SimData/ns-simdata.h>
#include <SimData/Export.h>


NAMESPACE_SIMDATA

class Vector3;
class Matrix3;


/**
 * class Quaternion
 *
 * @author unknown
 */
class SIMDATA_EXPORT Quaternion: public BaseType
{
public:
	double w, x, y, z;

	// construction and destruction
	Quaternion (double fW = 1.0f, double fX = 0.0f, double fY = 0.0f, double fZ = 0.0f);
	Quaternion (const Quaternion& rkQ);
	
	// BaseType interface
	virtual void pack(Packer&) const; 
	virtual void unpack(UnPacker&);
	virtual void parseXML(const char* cdata);

	// conversion between Quaternions, matrices, and angle-axes
	void FromRotationMatrix (const Matrix3& kRot);
	void ToRotationMatrix (Matrix3& kRot) const;
	void FromAngleAxis (const double& rfAngle, const Vector3& rkAxis);
	void ToAngleAxis (double& rfAngle, Vector3& rkAxis) const;
	void FromAxes (const Vector3* akAxis);
	void ToAxes (Vector3* akAxis) const;

	// arithmetic operations

#ifndef SWIG
	Quaternion & operator+=(const Quaternion & q);
	Quaternion & operator/=(double s);
	Quaternion& operator= (const Quaternion& rkQ);
#endif // SWIG

	inline Quaternion operator+ (const Quaternion& rhs) const {
		return Quaternion(w+rhs.w,x+rhs.x,y+rhs.y,z+rhs.z);
	}

	inline Quaternion operator- (const Quaternion& rhs) const {
		return Quaternion(w-rhs.w,x-rhs.x,y-rhs.y,z-rhs.z);
	}

#ifndef SWIG
	inline const Quaternion operator* (const double fScalar) const {
		return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
	}
	/**
	 * Compare two quaternions for equality.
	 */
	SIMDATA_EXPORT friend bool operator==(const Quaternion &a, const Quaternion &b);
	/**
	 * Compare two quaternions for inequality.
	 */
	SIMDATA_EXPORT friend bool operator!=(const Quaternion &a, const Quaternion &b);
	SIMDATA_EXPORT friend Quaternion operator*(double fScalar, Quaternion const& rkQ);
	// Multiplication of a Quaternion and with a Vector, yielding a quaternion.
	SIMDATA_EXPORT friend Quaternion operator*(Quaternion const& q, Vector3 const& v);
	SIMDATA_EXPORT friend Quaternion operator*(Vector3 const& v, Quaternion const& q);
	SIMDATA_EXPORT friend std::ostream& operator<< (std::ostream& os, Quaternion const& m);
	Quaternion operator* (const Quaternion& rkQ) const;
#endif // SWIG

	Quaternion operator- () const;
	Quaternion operator~ (void) const { return Quaternion(w, -x, -y, -z);}

	// functions of a Quaternion
	double Dot (const Quaternion& rkQ) const;  // dot product
	double Norm () const;  // squared-length
	double Magnitude(void) const;
	Vector3	GetVector(void);
	double	GetScalar(void);

	Quaternion Bar() const;
	Quaternion Inverse() const;       // apply to non-zero Quaternion
	Quaternion UnitInverse() const;  // apply to unit-length Quaternion
	Quaternion Exp() const;
	Quaternion Log() const;

	// Use QVRotate instead.
	// rotation of a vector by a Quaternion
	//    Vector3 operator* (const Vector3& rkVector) const;

	// spherical linear interpolation
	static Quaternion Slerp(double fT, const Quaternion& rkP, const Quaternion& rkQ);

	static Quaternion SlerpExtraSpins(double fT, 
			                  const Quaternion& rkP, 
	                                  const Quaternion& rkQ,
	                                  int iExtraSpins);

	// setup for spherical quadratic interpolation
	static void Intermediate(const Quaternion& rkQ0,
	                         const Quaternion& rkQ1, 
	                         const Quaternion& rkQ2, 
	                         Quaternion& rka, 
	                         Quaternion& rkB);

	// spherical quadratic interpolation
	static Quaternion Squad(double fT, 
	                        const Quaternion& rkP, 
	                        const Quaternion& rkA, 
	                        const Quaternion& rkB, 
	                        const Quaternion& rkQ);

	// special values
	static const Quaternion ZERO;
	static const Quaternion IDENTITY;

	static Vector3 MakeEulerAnglesFromQ(Quaternion const &q);
	static Quaternion MakeQFromEulerAngles(double x, double y, double z);
	
	std::string asString() const;
	
	// explicit operators for Python
#ifdef SWIG
	%extend {
		bool __eq__(Quaternion const& a) { return *self == a; }
		bool __ne__(Quaternion const& a) { return *self != a; }
		Quaternion __mul__(double f) { return (*self) * f; }
		Quaternion __rmul__(double f) { return (*self) * f; }
		Quaternion __mul__(Vector3 const& v) { return (*self) * v; }
		Quaternion __mul__(const Quaternion& q) { return (*self) * q; }
	}
#endif // SWIG 

};


SIMDATA_EXPORT Vector3 QVRotate(Quaternion const& q, Vector3 const& v);
Quaternion QRotate(Quaternion const& q1, Quaternion const&  q2);


NAMESPACE_END // namespace simdata


#endif // __SIMDATA_QUATERNION_H__

