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
 * @file Vector3.inl
 *
 * Initial CSP version by Wolverine69
 * Code based on libraries from magic-software.com and Game Programming Gems.
 **/



#ifndef __VECTOR3_INL__
#define __VECTOR3_INL__



//namespace Math
//{



////////////////////////////////////////////////////////
// Vector3
////////////////////////////////////////////////////////



/**
 * Vector3()
 * Default constructor
 */ 
inline Vector3::Vector3()
{
	// don't initialize 
	// why not? safer and only very slightly slower --MR
	x = y = z = 0.0;
}


/**
 * Vector3(double x, double y, double z)
 * Constructor from doubles
 */
inline Vector3::Vector3(double _x, double _y, double _z)
{
	x = _x; y = _y; z = _z;
}


/** 
 * Vector3(Vector3 &v) 
 * Copy constructor
 */
inline Vector3::Vector3(const Vector3 &v)
{
	x = v.x; y = v.y; z = v.z;
}


/**
 * Array Indexing
 */
inline double & Vector3::operator[] (int i)
{
	return ((double*)&x)[i];
}

inline const double & Vector3::operator[] (int i) const
{
	return ((double*)&x)[i];
}

inline Vector3::operator double * ()
{
	return (double *)&x;
}


/* Arithmatic assignment operators
 */


/**
 * Set this vector equal to another one.
 */
inline Vector3 & Vector3::operator=(const Vector3 &v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}




/** 
 * Add another vector to this one.
 */
inline Vector3 & Vector3::operator+=(const Vector3 & v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}


/** 
 * Subtract another vector from this one.
 */ 
inline Vector3 & Vector3::operator-=(const Vector3 & v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}


/**
 * Multiply this vector by a double 
 */
inline Vector3 & Vector3::operator*=(double f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}



/** 
 * Equality operator
 */
inline bool operator==(const Vector3 &a, const Vector3 &b)
{
	return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
}


/** 
 * Inequality operator
 */
inline bool operator!=(const Vector3 &a, const Vector3 &b)
{
	return ((a.x != b.x) || (a.y != b.y) || (a.z != b.z));
}


/** 
 * Negation operator
 */
inline Vector3 Vector3::operator-() const
{
	return Vector3(-x,-y,-z);
}
  

/** 
 * Add two vectors
 */ 
//inline Vector3 operator+(const Vector3 &a, const Vector3 &b)
//{
//  return Vector3(a.x+b.x, a.y+b.y, a.z+b.z);
//}


/** 
 * Subtract one vector from another.
 */
inline Vector3 Vector3::operator-(const Vector3 &b) const
{
	Vector3  aVector3 = *this;
	aVector3 -= b;
	return aVector3;
}

   
/** 
 * Multiply a scalar by a vector.
 */
inline Vector3 operator*(double f, const Vector3 &a)
{
	return Vector3(f*a.x, f*a.y, f*a.z);
}



/** 
 * Set the values of the vector
 */
inline void Vector3::Set(double _x, double _y, double _z)
{
	x = _x; y = _y; z = _z;
}



/** 
 * Calculate the length of the Vector3
 */
inline double Vector3::Length() const // Checked (delta)
{
	return static_cast<double>(sqrt(x*x + y*y + z*z));
}



/**
 * Calculate the length of the vector squared.
 * 
 * This routine is faster then the normal length
 * one because the sqrt does not need to be calculated.
 */
inline double Vector3::LengthSquared() const
{
	return (x*x+y*y+z*z);
}

  
/** 
 * Determine if all the components of the vector are zero.
 */ 
inline bool Vector3::IsNull()
{
	return ((x == 0.0f) && (y == 0.0f) && (z == 0.0f));
}

/** 
 * Compute the dot product of two Vector3's
 */
inline double Dot (const Vector3& aVector,
                   const Vector3& bVector)
{
	return aVector.x*bVector.x + aVector.y*bVector.y + aVector.z*bVector.z;
}


/** 
 * Compute the cross product of two vectors
 */
inline Vector3 Cross (const Vector3& aVector,
                              const Vector3& bVector)
{
	return Vector3(
		aVector.y*bVector.z-aVector.z*bVector.y,
		aVector.z*bVector.x-aVector.x*bVector.z,
		aVector.x*bVector.y-aVector.y*bVector.x);
}


/** 
 * Return a normalized vector of the input vector.
 */
inline Vector3 Normalized(const Vector3 &a)

{
	Vector3 ret(a);
	return ret.Normalize();
}                


/**
 * Vector cross product (u cross v)
 */
inline	Vector3 operator^(const Vector3 & u, const Vector3  & v)

{
	return Vector3(	
		 u.y*v.z - u.z*v.y,
		-u.x*v.z + u.z*v.x,
		 u.x*v.y - u.y*v.x );
}



//} // namespace Math


#endif // __VECTOR3_INL__


