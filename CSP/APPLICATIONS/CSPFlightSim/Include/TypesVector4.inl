#ifndef _STANDARDVECTOR4_INL_
#define _STANDARDVECTOR4_INL_

// Initial CSP Version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.



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

/** StandardVector4(double x, double y, double z, double w)
 * Constructor from doubles
 */
inline StandardVector4::StandardVector4(double _x, double _y, double _z, double _w)
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
    
inline double & StandardVector4::operator[] (int i)
{
  return ((double*)this)[i];
}

inline const double & StandardVector4::operator[] (int i) const
{
  return ((double*)this)[i];
}

inline  StandardVector4::operator double * ()
{
  return (double *)this;
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

inline StandardVector4 & StandardVector4::operator*=(double f)
{
  x *= f;
  y *= f;
  z *= f;
  w *= f;
  return *this;
}

inline StandardVector4 & StandardVector4::operator /=( double f )
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

//StandardVector4 StandardVector4::operator*(const double f) const
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
inline StandardVector4 StandardVector4::operator - () const
{
  return StandardVector4(-x, -y, -z, -w);
}
    
/** friend arithmatic operators
 */
inline StandardVector4 operator+(const StandardVector4 &a, const StandardVector4 &b)
{
  return StandardVector4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}
   
inline StandardVector4 StandardVector4::operator-(const StandardVector4 &b) const
{
  return StandardVector4(x-b.x, y-b.y, z-b.z, w-b.w);
}
    
inline StandardVector4 operator*(double f, StandardVector4 &a)
{
  return StandardVector4(f*a.x, f*a.y, f*a.z, f*a.w);
}
   
inline StandardVector4 operator*(StandardVector4 &a, double f)
{
  return StandardVector4(f*a.x, f*a.y, f*a.z, f*a.w);
}

/** Additional methods
 */
inline void StandardVector4::Set(double _x, double _y, double _z, double _w)
{
  x = _x; y = _y; z = _z; w = _w;
}

inline double StandardVector4::Length() const
{
  return (double)(sqrt(x*x + y*y + z*z + w*w));
}

inline double StandardVector4::LengthSquared() const
{
  return (x*x+y*y+z*z+w*w);
}
   
inline bool StandardVector4::IsNull()
{
  return ((x == 0.0f) && (y == 0.0f) && (z == 0.0f) && (w == 0.0f));
}

#endif

