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
 * @file Vector3.h
 *
 * Initial CSP version by Wolverine69
 * Code based on libraries from magic-software.com and Game Programming Gems.
 **/

#ifndef __VECTOR3_H__
#define __VECTOR3_H__


#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cmath>

#include <SimData/BaseType.h>


NAMESPACE_SIMDATA


class Matrix3;
class Quaternion;


/** 
 * This class represents a 3 element vector. It uses overloaded
 * operators to perform basic operations.
 */
class SIMDATA_EXPORT Vector3: public BaseType
{
public:
	double x;
	double y;
	double z;

	/** 
	 * Default constructor
	 */ 
	Vector3();

	/** 
	 * Constructor from doubles
	 */
	Vector3(double x, double y, double z);

	/** 
	 * Copy constructor
	 */
	Vector3(const Vector3 &v);

	// Array Indexing
#ifndef SWIG
	/**
	 * Array indexing (0,1,2)->(x,y,z)
	 */
	double & operator[] (int i);
	/**
	 * Constant array indexing (0,1,2)->(x,y,z)
	 */
	const double & operator[] (int i) const;

	/**
	 * Access components as a list.
	 */
	operator double * ();
#endif // SWIG

	// Arithmetic assignment operators
#ifndef SWIG
	/** 
	 * Assign from another vector.
	 */
	Vector3 & operator=(const Vector3 &v);
	/**
	 * Add another vector.
	 */
	Vector3 & operator+=(const Vector3 & v);
	/**
	 * Subtract another vector.
	 */
	Vector3 & operator-=(const Vector3 & v);
	/**
	 * Scale the vector.
	 */
	Vector3 & operator*=(double f);
	/**
	 * Scale the vector.
	 */
	Vector3 & operator/=(double f);

	/**
	 * Compare two vectors for equality.
	 */
	friend bool operator==(const Vector3 &a, const Vector3 &b);
	
	/**
	 * Compare two vectors for inequality.
	 */
	friend bool operator!=(const Vector3 &a, const Vector3 &b);
#endif // SWIG

	/**
	 * Compute the difference with another vector.
	 */
	Vector3 operator - (const Vector3 &b) const;
    
	/**
	 * Compute the sum with another vector.
	 */
	inline const Vector3 operator + (const Vector3 &rhs) const {
		return Vector3(rhs.x+x, rhs.y+y, rhs.z+z);
	}
    
	/**
	 * Negation operator
	 */
	Vector3 operator -() const;

#ifndef SWIG
	/**
	 * Return this vector multiplied by a scalar.
	 */ 
	inline const Vector3 operator*(double f) const
	{
		return Vector3(f*x, f*y, f*z);
	}

	// friend arithmetic operators
	friend Vector3 operator/(const Vector3 &a, double f);
	friend Vector3 operator*(double f, const Vector3 &a);
	friend Vector3 operator^(const Vector3 & u, const Vector3 & v);
#endif // SWIG

	// Additional methods
	/**
	 * Set the components of this vector.
	 */
	void Set(double _x, double _y, double _z);

	/**
	 * Compute the length of this vector.
	 */
	double Length() const;

	/**
	 * Compute the square of the length of this vector.
	 */
	double LengthSquared() const;

	/**
	 * Test for a zero vector.
	 */
	bool IsNull() const;

	/**
	 * Scale this vector to unit length.
	 */
	Vector3 & Normalize();

	/**
	 * Normalize a vector and return the original length.
	 *
	 * @param fTolerance Vectors shorter than this length are considered to
	 *                   be of length zero and are not normalized.
	 */
	double Unitize(double  fTolerance = 1e-06);

#ifndef SWIG
	/**
	 * Compute the dot product of two vectors.
	 */
	friend double Dot(const Vector3 & aVector,
	                  const Vector3 & bVector);

	/**
	 * Compute the cross product of two vectors.
	 */
	friend Vector3 Cross(const Vector3 & aVector, 
	                             const Vector3 & bVector);

	/**
	 * Generate a unit vector in the direction of the given vector.
	 *
	 * @param a the original vector.
	 * @returns a unit vector pointing in the same direction as \a a.
	 */
	friend Vector3 Normalized(const Vector3 &a);
#endif // SWIG
	
	/**
	 * Compute the cross product with another vector.
	 */
	Vector3 Cross(const Vector3 & a) const;
	
	/**
	 * Compute the dot product with another vector.
	 */
	double Dot(const Vector3 & a) const;
	
	/**
	 * Get the vector elements as a std::vector (useful for Python bindings)
	 */
	std::vector<double> GetElements() const;

	/**
	 * Compute the star matrix of this vector.
	 */
	Matrix3 StarMatrix() const;

	/**
	 * Print string representation to a stream.
	 */
	void Print(FILE * stream) const;

	static const Vector3 ZERO;
	static const Vector3 XAXIS;
	static const Vector3 YAXIS;
	static const Vector3 ZAXIS;

#ifndef SWIG
	SIMDATA_EXPORT friend std::ostream & operator << (std::ostream & os, const Vector3& v);
#endif // SWIG

	/**
	 * String representation.
	 */
	virtual std::string asString() const;
	
	/*
	double __mul__(const Vector3 & a) const { return Dot(a); }
	Vector3 __mul__(const Matrix3 & a) const;
	Vector3 __mul__(double a) const { return (*this)*a; }
	Vector3 __rmul__(double a) const { return (*this)*a; }
	*/
	
#ifdef SWIG
	%extend {
		bool operator==(const Vector3 & a) { return *self == a; }
		bool operator!=(const Vector3 & a) { return *self != a; }
		double __mul__(const Vector3 & a) const { return self->Dot(a); }
		Vector3 __mul__(const Matrix3 & a) const { return (*self)*a; }
		Vector3 __mul__(double a) const { return (*self)*a; }
		Vector3 __rmul__(double a) const { return (*self)*a; }
		Quaternion __mul__(Quaternion const& q) { return q * (*self); }
	}
#endif // SWIG 

	/**
	 * Serialize
	 */
	virtual void pack(Packer&) const;

	/**
	 * Deserialize
	 */
	virtual void unpack(UnPacker&);

	/**
	 * Parse cdata from XML tag <Vector>
	 */
	virtual void parseXML(const char*);

};



#include <SimData/Vector3.inl>


NAMESPACE_END // namespace simdata


#endif // __VECTOR3_H__

