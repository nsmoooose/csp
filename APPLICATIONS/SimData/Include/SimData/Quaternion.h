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
#include <SimData/Vector3.h>


NAMESPACE_SIMDATA

class Matrix3;


/**
 * class Quaternion
 *
 * Quaternions are four dimensional objects that form a compact
 * representation for rotations.  Many thorough treatments of 
 * quaternions and their use in simulations can be readily found
 * on the web.
 */
class SIMDATA_EXPORT Quaternion: public BaseType
{
public:
	double w, x, y, z;

	/**
	 * Construct a new quaternion.
	 *
	 * Specifiy the four real-valued components, or use the defaults
	 * to construct a unit quaternion.
	 */
	Quaternion(double fW = 1.0f, double fX = 0.0f, double fY = 0.0f, double fZ = 0.0f);

	/**
	 * Copy constructor.
	 */
	Quaternion(const Quaternion& rkQ);
	
	// BaseType interface
	virtual void pack(Packer&) const; 
	virtual void unpack(UnPacker&);
	virtual void parseXML(const char* cdata);

	// conversion between Quaternions, matrices, and angle-axes

	/**
	 * Convert from a rotation matrix.
	 */
	void FromRotationMatrix(const Matrix3& kRot);

	/**
	 * Convert to a rotation matrix.
	 */
	void ToRotationMatrix(Matrix3& kRot) const;

	/**
	 * Convert from a rotation specified by an axis and angle.
	 */
	void FromAngleAxis(const double& rfAngle, const Vector3& rkAxis);

	/**
	 * Convert to a rotation specified by an axis and angle.
	 */
	void ToAngleAxis(double& rfAngle, Vector3& rkAxis) const;

	/**
	 * Convert from a rotation matrix specified by three axes.
	 */
	void FromAxes(const Vector3* akAxis);

	/**
	 * Convert to a rotation matrix specified by three axes.
	 */
	void ToAxes(Vector3* akAxis) const;

	// arithmetic operations

#ifndef SWIG
	/**
	 * In-place addition.
	 */
	Quaternion & operator+=(const Quaternion & q);

	/**
	 * In place division by a scalar.
	 */
	Quaternion & operator/=(double s);

	/**
	 * Copy operator.
	 */
	Quaternion& operator=(const Quaternion& rkQ);
#endif // SWIG

	/**
	 * Add two quaternions.
	 */
	inline Quaternion operator+(const Quaternion& rhs) const {
		return Quaternion(w+rhs.w,x+rhs.x,y+rhs.y,z+rhs.z);
	}

	/**
	 * Subtract two quaternions.
	 */
	inline Quaternion operator-(const Quaternion& rhs) const {
		return Quaternion(w-rhs.w,x-rhs.x,y-rhs.y,z-rhs.z);
	}

#ifndef SWIG
	/**
	 * Multiply by a scalar.
	 */
	inline const Quaternion operator*(const double fScalar) const {
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

	/**
	 * Multiplication of a Quaternion and with a Scalar, yielding a quaternion.
	 */
	SIMDATA_EXPORT friend Quaternion operator*(double fScalar, Quaternion const& rkQ);

	/**
	 * Multiplication of a Quaternion and with a Vector, yielding a quaternion.
	 */
	SIMDATA_EXPORT friend Quaternion operator*(Quaternion const& q, Vector3 const& v);

	/**
	 * Multiplication of a Vector with a Quaternion, yielding a quaternion.
	 */
	SIMDATA_EXPORT friend Quaternion operator*(Vector3 const& v, Quaternion const& q);

	/**
	 * Stream output of a quaternion.
	 */
	SIMDATA_EXPORT friend std::ostream& operator<<(std::ostream& os, Quaternion const& m);

	/**
	 * Multply by a quaternion.
	 */
	Quaternion operator*(const Quaternion& rkQ) const;
#endif // SWIG

	/**
	 * get (-w, -x, -y, -z).
	 */
	Quaternion operator-() const;

	/**
	 * get the conjugate (w, -x, -y, -z).
	 */
	Quaternion operator~(void) const { return Quaternion(w, -x, -y, -z);}

	// functions of a Quaternion
	
	/**
	 * get the dot product with another quaternion.
	 */
	double Dot(const Quaternion& rkQ) const;  // dot product

	/**
	 * get the squared length (magnitude squared).
	 */
	double Norm() const; 

	/**
	 * get the length (4-vector modulus).
	 */
	double Magnitude(void) const;

	/**
	 * get the vector component of the quaternion (x, y, z).
	 */
	Vector3	GetVector(void);

	/**
	 * get the scalar component of the quaternion (w).
	 */
	double	GetScalar(void);

	/**
	 * get the conjugate (w, -x, -y, -z).
	 */
	Quaternion Bar() const;

	/** 
	 * get the inverse a a quaternion.
	 *
	 * This is the same as Bar() and ~() for normalized quaternions.
	 */
	Quaternion Inverse() const;       // apply to non-zero Quaternion

	Quaternion UnitInverse() const;  // apply to unit-length Quaternion

	/**
	 * Compute the exponential.
	 *
	 * If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	 * exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
	 * we use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has a
	 * limit of 1.
	 */
	Quaternion Exp() const;

	/**
	 * Compute the logarithm.
	 *
	 * If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, 
	 * then log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, we use 
	 * log(q) = sin(A)*(x*i+y*j+z*k) since sin(A)/A has a limit of 1.
	 */
	Quaternion Log() const;

	/**
	 * rotate a vector.
	 *
	 * NOTE: assumes the quaternion has unit length.
	 */
	Vector3 GetRotated(Vector3 const &) const;

	/**
	 * anti-rotate a vector.
	 *
	 * NOTE: assumes the quaternion has unit length.
	 */
	Vector3 GetInverseRotated(Vector3 const &) const;

	/**
	 * rotate a vector in place.
	 *
	 * NOTE: assumes the quaternion has unit length.
	 */
	void Rotate(Vector3 &) const;

	/**
	 * anti-rotate a vector in place.
	 *
	 * NOTE: assumes the quaternion has unit length.
	 */
	void InverseRotate(Vector3 &) const;

	/*
	 * spherical linear interpolation
	 */
	static Quaternion Slerp(double fT, const Quaternion& rkP, const Quaternion& rkQ);

	/**
	 *
	 */
	static Quaternion SlerpExtraSpins(double fT, 
			                  const Quaternion& rkP, 
	                                  const Quaternion& rkQ,
	                                  int iExtraSpins);

	/**
	 * setup for spherical quadratic interpolation.
	 */
	static void Intermediate(const Quaternion& rkQ0,
	                         const Quaternion& rkQ1, 
	                         const Quaternion& rkQ2, 
	                         Quaternion& rka, 
	                         Quaternion& rkB);

	/**
	 * spherical quadratic interpolation
	 */
	static Quaternion Squad(double fT, 
	                        const Quaternion& rkP, 
	                        const Quaternion& rkA, 
	                        const Quaternion& rkB, 
	                        const Quaternion& rkQ);

	// special values

	static const Quaternion ZERO;
	static const Quaternion IDENTITY;

	/**
	 * Convert a quaternion to Euler angles.
	 */
	static Vector3 MakeEulerAnglesFromQ(Quaternion const &q);

	/**
	 * Construct a new quaternion from Euler angles.
	 */
	static Quaternion MakeQFromEulerAngles(double x, double y, double z);
	
	/**
	 * String representation.
	 */
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


//SIMDATA_EXPORT Vector3 QVRotate(Quaternion const& q, Vector3 const& v);
//Quaternion QRotate(Quaternion const& q1, Quaternion const&  q2);

//----------------------------------------------------------------------------
// INLINE
//

inline Quaternion::Quaternion(double fW, double fX, double fY, double fZ)
{
	w = fW;
	x = fX;
	y = fY;
	z = fZ;
}

inline Quaternion::Quaternion(const Quaternion& rkQ)
{
	w = rkQ.w;
	x = rkQ.x;
	y = rkQ.y;
	z = rkQ.z;
}

inline Quaternion& Quaternion::operator=(const Quaternion& rkQ)
{
	w = rkQ.w;
	x = rkQ.x;
	y = rkQ.y;
	z = rkQ.z;
	return *this;
}


inline Quaternion Quaternion::operator-() const
{
	return Quaternion(-w,-x,-y,-z);
}


inline double Quaternion::Dot(const Quaternion& rkQ) const
{
	return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
}


inline double Quaternion::Norm() const
{
	return w*w+x*x+y*y+z*z;
}


inline Quaternion Quaternion::UnitInverse() const //checked (delta)
{
	// assert:  'this' is unit length
	return Quaternion(w,-x,-y,-z);
}


inline double Quaternion::Magnitude(void) const
{
	return sqrt(w*w + x*x + y*y + z*z);
}

/*
inline Vector3 Quaternion::GetRotated(Vector3 const &v) const
{
	Quaternion t = (*this*v)*Bar();
	return t.GetVector();
}
*/

inline Vector3 Quaternion::GetInverseRotated(Vector3 const &v) const
{
	Quaternion t = (Bar()*v)*(*this);
	return t.GetVector();
}

inline void Quaternion::Rotate(Vector3 &v) const
{
	v = GetRotated(v);
}

inline void Quaternion::InverseRotate(Vector3 &v) const
{
	v = GetInverseRotated(v);
}

/**
 * Use a quaternion to rotate a vector.
 *
 * Given a vector u = (x0,y0,z0) and a unit length Quaternion
 * q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
 * rotation of u by q is v = q*u*q^{-1} where * indicates Quaternion
 * multiplication and where u is treated as the Quaternion <0,x0,y0,z0>.
 * Note that q^{-1} = <w,-x,-y,-z>, so no double work is required to
 * invert q.
 */
inline Vector3 QVRotate(Quaternion const& q, Vector3 const& v)
{
	return q.GetRotated(v);
}

inline Quaternion Quaternion::Bar() const
{
	return Quaternion(w,-x,-y,-z);
}

inline Quaternion QRotate(Quaternion const& q1, Quaternion const& q2)
{
	return q1*q2*(~q1);
}

inline Vector3 Quaternion::GetVector(void)
{
	return Vector3(x, y, z);
}

inline double Quaternion::GetScalar(void)
{
	return w;
}


NAMESPACE_END // namespace simdata


#endif // __SIMDATA_QUATERNION_H__

