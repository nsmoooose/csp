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


//namespace Math
//{

/** 
 * This class represents a 3 element vector. It uses overloaded
 * operators to perform basic operations.
 */
class Vector3: public BaseType
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
	double & operator[] (int i);
	const double & operator[] (int i) const;
	operator double * ();
#endif // SWIG

	// Arithmetic assignment operators
#ifndef SWIG
	Vector3 & operator=(const Vector3 &v);
	Vector3 & operator+=(const Vector3 & v);
	Vector3 & operator-=(const Vector3 & v);
	Vector3 & operator*=(double f);
	Vector3 & operator/=(double f);

	friend bool operator==(const Vector3 &a, const Vector3 &b);
	friend bool operator!=(const Vector3 &a, const Vector3 &b);
#endif // SWIG

	Vector3 operator - (const Vector3 &b) const;
    
	inline const Vector3 operator + (const Vector3 &rhs) const {
		return Vector3(rhs.x+x, rhs.y+y, rhs.z+z);
	}
    
	/**
	 * Negation operator
	 */
	Vector3 operator -() const;

	/**
	 * Multiply a vector by a scalar.
	 */ 
	inline const Vector3 operator*(double f) const
	{
		return Vector3(f*x, f*y, f*z);
	}

#ifndef SWIG
	// friend arithmetic operators
	friend Vector3 operator/(const Vector3 &a, double f);
	friend Vector3 operator*(double f, const Vector3 &a);
	friend Vector3 operator*(double f, Vector3 &a);
	friend Vector3 operator^(const Vector3 & u, const Vector3 & v);
#endif // SWIG

	// Additional methods
	void Set(double _x, double _y, double _z);
	double Length() const;
	double LengthSquared() const;
	bool IsNull();
	Vector3 & Normalize();
	double Unitize(double  fTolerance = 1e-06);

#ifndef SWIG
	friend double Dot(const Vector3 & aVector,
	                  const Vector3 & bVector);

	friend Vector3 Cross(const Vector3 & aVector, 
	                             const Vector3 & bVector);

	friend Vector3 Normalized(const Vector3 &a);
#endif // SWIG
	
	Vector3 Cross(const Vector3 & a);
	double Dot(const Vector3 & a);
	std::vector<double> GetElements();

	Matrix3 StarMatrix();

	void Print(FILE * stream);

	static const Vector3 ZERO;
	static const Vector3 XAXIS;
	static const Vector3 YAXIS;
	static const Vector3 ZAXIS;

#ifndef SWIG
	friend std::ostream & operator << (std::ostream & os, const Vector3& v);
#endif // SWIG

	virtual std::string asString() const;
	
#ifdef SWIG
	%extend {
		bool operator==(const Vector3 & a) { return *self == a; }
		bool operator!=(const Vector3 & a) { return *self != a; }
		double operator*(const Vector3 & a) { return self->Dot(a); }
	}
#endif // SWIG 

#ifdef SWIG 
%insert("shadow") %{
%}
#endif // SWIG
	
	virtual void pack(Packer&) const;
	virtual void unpack(UnPacker&);
	virtual void parseXML(const char*);

};



#include <SimData/Vector3.inl>

//typedef Vector3 Vector3;


NAMESPACE_END // namespace simdata


#endif // __VECTOR3_H__

