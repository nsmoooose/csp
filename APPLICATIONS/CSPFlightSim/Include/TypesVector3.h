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
 * @file TypesVector3.h
 *
 **/

#ifndef __STANDARDVECTOR_H__
#define __STANDARDVECTOR_H__

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

#include <iostream>
#include <stdio.h>

class StandardMatrix3;


//namespace Math
//{

/** Class StandardVector3
 * This class represents a 3 element vector. It uses overloaded
 * operators to perform basic operations.
 */
class StandardVector3
{
 public:
    double x;
    double y;
    double z;

    /** StandardVector3()
     * Default constructor
     */ 
    StandardVector3();

    /** StandardVector3(double x, double y, double z)
     * Constructor from doubles
     */
    StandardVector3(double x, double y, double z);

    /** StandardVector3(StandardVector3 &v) 
     * Copy constructor
     */
    StandardVector3(const StandardVector3 &v);

    /** Array Indexing
     */
    double & operator[] (int i);
    const double & operator[] (int i) const;
    operator double * ();

    /** Arithmetic assignment operators
     */
    StandardVector3 & operator=(const StandardVector3 &v);
    StandardVector3 & operator+=(const StandardVector3 & v);
    StandardVector3 & operator-=(const StandardVector3 & v);
    StandardVector3 & operator*=(double f);
    StandardVector3 & operator/=(double f);

    friend bool operator==(const StandardVector3 &a, const StandardVector3 &b);
    friend bool operator!=(const StandardVector3 &a, const StandardVector3 &b);
    
    StandardVector3 operator - (const StandardVector3 &b) const;
    
    inline const StandardVector3 operator + (const StandardVector3 &rhs) const
	{
	  return StandardVector3(rhs.x+x, rhs.y+y, rhs.z+z);
	}
    
	/** Negation operator
    */
    StandardVector3 operator -() const;
	/** friend arithmetic operators
     */
    friend StandardVector3 operator/(const StandardVector3 &a, double f);

	/** Multiply a vector by a scalar.
	*/ 
	inline const StandardVector3 operator*(double f) const
	{
	  return StandardVector3(f*x, f*y, f*z);
	}

    friend StandardVector3 operator*(double f, const StandardVector3 &a);
    friend StandardVector3 operator*(double f, StandardVector3 &a);

    friend StandardVector3 operator^(const StandardVector3 & u, const StandardVector3 & v);



    /** Additional methods
     */
    void Set(double _x, double _y, double _z);
    double Length() const;
    double LengthSquared() const;
    bool IsNull();
    StandardVector3 & Normalize();
    double Unitize (double  fTolerance = 1e-06);

    friend double Dot (const StandardVector3 & aVector,
		              const StandardVector3 & bVector);

    friend StandardVector3 Cross (const StandardVector3 & aVector, 
			                      const StandardVector3 & bVector);
	
    friend StandardVector3 Normalized(const StandardVector3 &a);

    StandardMatrix3 StarMatrix();

    void Print(FILE * stream);

    static const StandardVector3 ZERO;
    static const StandardVector3 XAXIS;
    static const StandardVector3 YAXIS;
    static const StandardVector3 ZAXIS;


	friend std::ostream & operator << (std::ostream & os, const StandardVector3& v);
	std::string toString();


};



#include "TypesVector3.inl"

//typedef StandardVector3 _Vector;

//}

#endif // __TYPESVECTOR3_H__

