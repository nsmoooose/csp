#ifndef _STANDARDVECTOR3_INL__
#define _STANDARDVECTOR3_INL__

////////////////////////////////////////////////////////
// Vector3
////////////////////////////////////////////////////////

/** _Vector3()
 * Default constructor
 */ 
inline _Vector3::_Vector3()
{
  // don't initialize 
}

/** _Vector3(float x, float y, float z)
 * Constructor from floats
 */
inline _Vector3::_Vector3(float _x, float _y, float _z)
{
  x = _x; y = _y; z = _z;
}

/** _Vector3(_Vector3 &v) 
 * Copy constructor
 */
inline _Vector3::_Vector3(const _Vector3 &v)
{
  x = v.x; y = v.y; z = v.z;
}

/** Array Indexing
 */
    
inline float & _Vector3::operator[] (int i)
{
  return ((float*)this)[i];
}

inline const float & _Vector3::operator[] (int i) const
{
  return ((float*)this)[i];
}

inline _Vector3::operator float * ()
{
  return (float *)this;
}

/** Arithmatic assignment operators
 */

inline _Vector3 & _Vector3::operator=(const _Vector3 &v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  return *this;
}


inline _Vector3 & _Vector3::operator+=(const _Vector3 & v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}
    
inline _Vector3 & _Vector3::operator-=(const _Vector3 & v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

inline _Vector3 & _Vector3::operator*=(float f)
{
  x *= f;
  y *= f;
  z *= f;
  return *this;
}

/** Arithmatic operator
 */
//_Vector3 _Vector3::operator+(const _Vector3 &v) const
//{
//  return _Vector3(x+v.x, y+v.y, z+v.z);
//}
//
//_Vector3 _Vector3::operator-(const _Vector3 &v) const
//{
//  return _Vector3(x-v.x, y-v.y, z-v.z);
//}

//_Vector3 _Vector3::operator*(const float f) const
//{
//  return _Vector3(x*f, y*f, z*f);
//}

   


/** Equality operators
 */
inline bool _Vector3::operator==(const _Vector3 &a)
{
  return ((a.x == x) && (a.y == y) && (a.z == z));
}

inline bool _Vector3::operator!=(const _Vector3 &a)
{
  return ((a.x != x) || (a.y != y) || (a.z != z));
}

/** Negation operator
 */
inline _Vector3 operator - (const _Vector3 &a)
{
  return _Vector3(-a.x, -a.y, -a.z);
}
    
/** friend arithmatic operators
 */
inline _Vector3 operator+(const _Vector3 &a, const _Vector3 &b)
{
  return _Vector3(a.x+b.x, a.y+b.y, a.z+b.z);
}
   
inline _Vector3 operator-(const _Vector3 &a, const _Vector3 &b)
{
  return _Vector3(a.x-b.x, a.y-b.y, a.z-b.z);
}
    
inline _Vector3 operator*(_Vector3 &a, float f)
{
  return _Vector3(f*a.x, f*a.y, f*a.z);
}

inline _Vector3 operator*(float f, _Vector3 &a)
{
  return _Vector3(f*a.x, f*a.y, f*a.z);
}

/** Additional methods
 */
inline void _Vector3::Set(float _x, float _y, float _z)
{
  x = _x; y = _y; z = _z;
}

inline float _Vector3::Length() const
{
  return sqrt(x*x + y*y + z*z);
}

inline float _Vector3::LengthSquared() const
{
  return (x*x+y*y+z*z);
}
   
inline bool _Vector3::IsNull()
{
  return ((x == 0.0f) && (y == 0.0f) && (z == 0.0f));
}

#endif
