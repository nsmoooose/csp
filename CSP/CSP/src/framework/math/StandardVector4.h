#ifndef __STANDARDVECTOR4_H__
#define __STANDARDVECTOR4_H__

/** \Class _Vector4
 * This class represents a 4 element vector. It uses overloaded
 * operators to preform basic operations.
 */
class _Vector4
{
 public:
  float x;
  float y;
  float z;
  float w;

    /** _Vector4()
     * Default constructor
     */ 
    _Vector4();

    /** _Vector4(float x, float y, float z, float w)
     * Constructor from floats
     */
    _Vector4(float x, float y, float z, float w);

    /** _Vector4(_Vector4 &v) 
     * Copy constructor
     */
    _Vector4(const _Vector4 &v);

    /** Array Indexing
     */
    float & operator[] (int i);
    const float & operator[] (int i) const;
    operator float * ();

    /** Arithmatic assignment operators
     */
    _Vector4 & operator=(const _Vector4 & v);
    _Vector4 & operator+=(const _Vector4 & v);
    _Vector4 & operator-=(const _Vector4 & v);
    _Vector4 & operator*=(float f);
    _Vector4 & operator/=(float f);

    /** Arithmatic operator
     */
    //    _Vector4 operator+(const _Vector4 &v) const;
    //    _Vector4 operator-(const _Vector4 &v) const;
    //    _Vector4 operator*(const float f) const;
    //    _Vector4 operator/(const float f) const;

    /** Equality operators
     */
    bool operator==(const _Vector4 &a);
    bool operator!=(const _Vector4 &a);

    /** Negation operator
     */
    friend _Vector4 operator - (const _Vector4 &a);
    
    /** friend arithmatic operators
     */
    friend _Vector4 operator+(const _Vector4 &a, const _Vector4 &b);
    friend _Vector4 operator-(const _Vector4 &a, const _Vector4 &b);
    friend _Vector4 operator*(const _Vector4 &a , float f);
    friend _Vector4 operator*(float f, const _Vector4 &a);
    friend _Vector4 operator/(const _Vector4 &a, float f);

    /** Additional methods
     */
    void Set(float _x, float _y, float _z, float _w);
    float Length() const;
    float LengthSquared() const;
    bool IsNull();
    _Vector4 & Normalize();

};

#include "StandardVector4.inl"

#endif
