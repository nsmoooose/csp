#ifndef _STANDARDVECTOR4_INL_
#define _STANDARDVECTOR4_INL_

// Initial CSP Version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

#include <math.h>


/////////////////////////////////////////////////////////
// Vector4
/////////////////////////////////////////////////////////

/** StandardVector4()
 * Default constructor
 */ 
inline StandardVector4::StandardVector4()
{
  // don't initialize 
}

/** StandardVector4(float x, float y, float z, float w)
 * Constructor from floats
 */
inline StandardVector4::StandardVector4(float _x, float _y, float _z, float _w)
{
  x = _x; y = _y; z = _z; w = _w;
}

/** StandardVector4(StandardVector4 &v) 
 * Copy constructor
 */
inline StandardVector4::StandardVector4(const StandardVector4 &v)
{
  x = v.x; y = v.y; z = v.z;w = v.w;
}

/** Array Indexing
 */
    
inline float & StandardVector4::operator[] (int i)
{
  return ((float*)this)[i];
}

inline const float & StandardVector4::operator[] (int i) const
{
  return ((float*)this)[i];
}

inline  StandardVector4::operator float * ()
{
  return (float *)this;
}

/** Arithmatic assignment operators
 */

inline StandardVector4 & StandardVector4::operator=(const StandardVector4 &v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  w = v.w;
  return *this;
}


inline StandardVector4 & StandardVector4::operator+=(const StandardVector4 & v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  w += v.w;
  return *this;
}
    
inline StandardVector4 & StandardVector4::operator-=(const StandardVector4 & v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  w -= v.w;
  return *this;
}

inline StandardVector4 & StandardVector4::operator*=(float f)
{
  x *= f;
  y *= f;
  z *= f;
  w *= f;
  return *this;
}

inline StandardVector4 & StandardVector4::operator /=( float f )
{
	x /= f;
	y /= f;
	z /= f;
	w /= f;
	return *this;
}

/** Arithmatic operator
 */
//StandardVector4 StandardVector4::operator+(const StandardVector4 &v) const
//{
//  return StandardVector4(x+v.x, y+v.y, z+v.z, w+v.w);
//}

//StandardVector4 StandardVector4::operator-(const StandardVector4 &v) const
//{
//  return StandardVector4(x-v.x, y-v.y, z-v.z, w-v.w);
//}

//StandardVector4 StandardVector4::operator*(const float f) const
//{
//  return StandardVector4(x*f, y*f, z*f, w*f);
//}

/** Equality operators
 */
inline bool StandardVector4::operator==(const StandardVector4 &a)
{
  return ((a.x == x) && (a.y == y) && (a.z == z) && (a.w == w));
}

inline bool StandardVector4::operator!=(const StandardVector4 &a)
{
  return ((a.x != x) || (a.y != y) || (a.z != z) || (a.w != w));
}

/** Negation operator
 */
inline StandardVector4 operator - (const StandardVector4 &a)
{
  return StandardVector4(-a.x, -a.y, -a.z, -a.w);
}
    
/** friend arithmatic operators
 */
inline StandardVector4 operator+(const StandardVector4 &a, const StandardVector4 &b)
{
  return StandardVector4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}
   
inline StandardVector4 operator-(const StandardVector4 &a, const StandardVector4 &b)
{
  return StandardVector4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
}
    
inline StandardVector4 operator*(float f, StandardVector4 &a)
{
  return StandardVector4(f*a.x, f*a.y, f*a.z, f*a.w);
}
   
inline StandardVector4 operator*(StandardVector4 &a, float f)
{
  return StandardVector4(f*a.x, f*a.y, f*a.z, f*a.w);
}

/** Additional methods
 */
inline void StandardVector4::Set(float _x, float _y, float _z, float _w)
{
  x = _x; y = _y; z = _z; w = _w;
}

inline float StandardVector4::Length() const
{
  return (float)(sqrt(x*x + y*y + z*z + w*w));
}

inline float StandardVector4::LengthSquared() const
{
  return (x*x+y*y+z*z+w*w);
}
   
inline bool StandardVector4::IsNull()
{
  return ((x == 0.0f) && (y == 0.0f) && (z == 0.0f) && (w == 0.0f));
}

#endif

