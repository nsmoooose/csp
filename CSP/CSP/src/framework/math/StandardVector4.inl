/////////////////////////////////////////////////////////
// Vector4
/////////////////////////////////////////////////////////

/** _Vector4()
 * Default constructor
 */ 
inline _Vector4::_Vector4()
{
  // don't initialize 
}

/** _Vector4(float x, float y, float z, float w)
 * Constructor from floats
 */
inline _Vector4::_Vector4(float _x, float _y, float _z, float _w)
{
  x = _x; y = _y; z = _z; w = _w;
}

/** _Vector4(_Vector4 &v) 
 * Copy constructor
 */
inline _Vector4::_Vector4(const _Vector4 &v)
{
  x = v.x; y = v.y; z = v.z;w = v.w;
}

/** Array Indexing
 */
    
inline float & _Vector4::operator[] (int i)
{
  return ((float*)this)[i];
}

inline const float & _Vector4::operator[] (int i) const
{
  return ((float*)this)[i];
}

inline  _Vector4::operator float * ()
{
  return (float *)this;
}

/** Arithmatic assignment operators
 */

inline _Vector4 & _Vector4::operator=(const _Vector4 &v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  w = v.w;
  return *this;
}


inline _Vector4 & _Vector4::operator+=(const _Vector4 & v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  w += v.w;
  return *this;
}
    
inline _Vector4 & _Vector4::operator-=(const _Vector4 & v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  w -= v.w;
  return *this;
}

inline _Vector4 & _Vector4::operator*=(float f)
{
  x *= f;
  y *= f;
  z *= f;
  w *= f;
  return *this;
}

/** Arithmatic operator
 */
//_Vector4 _Vector4::operator+(const _Vector4 &v) const
//{
//  return _Vector4(x+v.x, y+v.y, z+v.z, w+v.w);
//}

//_Vector4 _Vector4::operator-(const _Vector4 &v) const
//{
//  return _Vector4(x-v.x, y-v.y, z-v.z, w-v.w);
//}

//_Vector4 _Vector4::operator*(const float f) const
//{
//  return _Vector4(x*f, y*f, z*f, w*f);
//}

/** Equality operators
 */
inline bool _Vector4::operator==(const _Vector4 &a)
{
  return ((a.x == x) && (a.y == y) && (a.z == z) && (a.w == w));
}

inline bool _Vector4::operator!=(const _Vector4 &a)
{
  return ((a.x != x) || (a.y != y) || (a.z != z) || (a.w != w));
}

/** Negation operator
 */
inline _Vector4 operator - (const _Vector4 &a)
{
  return _Vector4(-a.x, -a.y, -a.z, -a.w);
}
    
/** friend arithmatic operators
 */
inline _Vector4 operator+(const _Vector4 &a, const _Vector4 &b)
{
  return _Vector4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}
   
inline _Vector4 operator-(const _Vector4 &a, const _Vector4 &b)
{
  return _Vector4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
}
    
inline _Vector4 operator*(float f, _Vector4 &a)
{
  return _Vector4(f*a.x, f*a.y, f*a.z, f*a.w);
}
   
inline _Vector4 operator*(_Vector4 &a, float f)
{
  return _Vector4(f*a.x, f*a.y, f*a.z, f*a.w);
}

/** Additional methods
 */
inline void _Vector4::Set(float _x, float _y, float _z, float _w)
{
  x = _x; y = _y; z = _z; w = _w;
}

inline float _Vector4::Length() const
{
  return sqrt(x*x + y*y + z*z + w*w);
}

inline float _Vector4::LengthSquared() const
{
  return (x*x+y*y+z*z+w*w);
}
   
inline bool _Vector4::IsNull()
{
  return ((x == 0.0f) && (y == 0.0f) && (z == 0.0f) && (w == 0.0f));
}



