#ifndef _STANDARDVECTOR3_INL__
#define _STANDARDVECTOR3_INL__

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.


//namespace Math
//{

////////////////////////////////////////////////////////
// Vector3
////////////////////////////////////////////////////////

/** StandardVector3()
 * Default constructor
 */ 
inline StandardVector3::StandardVector3()
{
  // don't initialize 
}

/** StandardVector3(double x, double y, double z)
 * Constructor from doubles
 */
inline StandardVector3::StandardVector3(double _x, double _y, double _z)
{
  x = _x; y = _y; z = _z;
}

/** StandardVector3(StandardVector3 &v) 
 * Copy constructor
 */
inline StandardVector3::StandardVector3(const StandardVector3 &v)
{
  x = v.x; y = v.y; z = v.z;
}

/** Array Indexing
 */
    
inline double & StandardVector3::operator[] (int i)
{
  return ((double*)this)[i];
}

inline const double & StandardVector3::operator[] (int i) const
{
  return ((double*)this)[i];
}

inline StandardVector3::operator double * ()
{
  return (double *)this;
}

/* Arithmatic assignment operators
 */

/** Set this vector equal to another one.
 */
inline StandardVector3 & StandardVector3::operator=(const StandardVector3 &v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  return *this;
}


/** add another vector to this one.
 */
inline StandardVector3 & StandardVector3::operator+=(const StandardVector3 & v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}
   
/** subtract another vector from this one.
 */ 
inline StandardVector3 & StandardVector3::operator-=(const StandardVector3 & v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

/** multiply this vector by a double 
 */
inline StandardVector3 & StandardVector3::operator*=(double f)
{
  x *= f;
  y *= f;
  z *= f;
  return *this;
}

/** Equality operator
 */
inline bool operator==(const StandardVector3 &a, const StandardVector3 &b)
{
  return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
}

/** Inequality operator
 */
inline bool operator!=(const StandardVector3 &a, const StandardVector3 &b)
{
  return ((a.x != b.x) || (a.y != b.y) || (a.z != b.z));
}

/** Negation operator
 */
inline StandardVector3 StandardVector3::operator-() const
{
  return StandardVector3(-x,-y,-z);
}
   
/** Add two vectors
 */ 
//inline StandardVector3 operator+(const StandardVector3 &a, const StandardVector3 &b)
//{
//  return StandardVector3(a.x+b.x, a.y+b.y, a.z+b.z);
//}
   
/** Subtract one vector from another.
 */
inline StandardVector3 StandardVector3::operator-(const StandardVector3 &b) const
{
  StandardVector3  aVector3 = *this;
  aVector3 -= b;
  return aVector3;
}
   


/** Multiply a scalar by a vector.
 */
inline StandardVector3 operator*(double f, const StandardVector3 &a)
{
  return StandardVector3(f*a.x, f*a.y, f*a.z);
}

inline StandardVector3 operator*(double f, StandardVector3 &a)
{
  return StandardVector3(f*a.x, f*a.y, f*a.z);
}


/** Set the values of the vector
 */
inline void StandardVector3::Set(double _x, double _y, double _z)
{
  x = _x; y = _y; z = _z;
}

/** Calculate the length of the StandardVector3
 */
inline double StandardVector3::Length() const // Checked (delta)
{
  return (double)(sqrt(x*x + y*y + z*z));
}

/** Calculate the length of the vector squared.
 * This routine is faster then the normal length
 * one because the sqrt does not need to be calculated.
 */
inline double StandardVector3::LengthSquared() const
{
  return (x*x+y*y+z*z);
}
  
/** Determine if all the components of the vector are zero.
 */ 
inline bool StandardVector3::IsNull()
{
  return ((x == 0.0f) && (y == 0.0f) && (z == 0.0f));
}

/** Compute the dot product of two StandardVector3's
*/
inline double Dot (const StandardVector3& aVector,
								   const StandardVector3& bVector)
{
    return aVector.x*bVector.x + aVector.y*bVector.y + aVector.z*bVector.z;
}

/** Compute the cross product of two vectors
 */
inline StandardVector3 Cross (const StandardVector3& aVector,
							  const StandardVector3& bVector)
{
    return StandardVector3(
		aVector.y*bVector.z-aVector.z*bVector.y,
		aVector.z*bVector.x-aVector.x*bVector.z,
        aVector.x*bVector.y-aVector.y*bVector.x);
}

/** return a normalized vector of the input vector.
 */
inline StandardVector3 Normalized(const StandardVector3 &a)
{
  StandardVector3 ret(a);
  return ret.Normalize();
}                

// Vector cross product (u cross v)
inline	StandardVector3 operator^(const StandardVector3 & u, const StandardVector3  & v)
{
	return StandardVector3(	u.y*v.z - u.z*v.y,
					-u.x*v.z + u.z*v.x,
					u.x*v.y - u.y*v.x );
}

    

//}

#endif

