#ifndef _STANDARDVECTOR3_INL__
#define _STANDARDVECTOR3_INL__

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

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

/** StandardVector3(float x, float y, float z)
 * Constructor from floats
 */
inline StandardVector3::StandardVector3(float _x, float _y, float _z)
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
    
inline float & StandardVector3::operator[] (int i)
{
  return ((float*)this)[i];
}

inline const float & StandardVector3::operator[] (int i) const
{
  return ((float*)this)[i];
}

inline StandardVector3::operator float * ()
{
  return (float *)this;
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

/** multiply this vector by a float 
 */
inline StandardVector3 & StandardVector3::operator*=(float f)
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
inline StandardVector3 operator-(const StandardVector3 &a)
{
  return StandardVector3(-a.x, -a.y, -a.z);
}
   
/** Add two vectors
 */ 
inline StandardVector3 operator+(const StandardVector3 &a, const StandardVector3 &b)
{
  return StandardVector3(a.x+b.x, a.y+b.y, a.z+b.z);
}
   
/** Subtract one vector from another.
 */
inline StandardVector3 operator-(const StandardVector3 &a, const StandardVector3 &b)
{
  return StandardVector3(a.x-b.x, a.y-b.y, a.z-b.z);
}
   
/** Multiply a vector by a scalar.
 */ 
inline StandardVector3 operator*(StandardVector3 &a, float f)
{
  return StandardVector3(f*a.x, f*a.y, f*a.z);
}

/** Multiply a vector by a scalar.
 */
inline StandardVector3 operator*(float f, StandardVector3 &a)
{
  return StandardVector3(f*a.x, f*a.y, f*a.z);
}

/** Set the values of the vector
 */
inline void StandardVector3::Set(float _x, float _y, float _z)
{
  x = _x; y = _y; z = _z;
}

/** Calculate the length of the StandardVector3
 */
inline float StandardVector3::Length() const
{
  return (float)(sqrt(x*x + y*y + z*z));
}

/** Calculate the length of the vector squared.
 * This routine is faster then the normal length
 * one because the sqrt does not need to be calculated.
 */
inline float StandardVector3::LengthSquared() const
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
inline float Dot (const StandardVector3& aVector,
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
  
#endif

