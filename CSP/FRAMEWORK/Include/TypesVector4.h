#ifndef __STANDARDVECTOR4_H__
#define __STANDARDVECTOR4_H__

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

/** \Class StandardVector4
 * This class represents a 4 element vector. It uses overloaded
 * operators to preform basic operations.
 */

//namespace Math
//{


#include "Framework.h"


class StandardVector4
{
 public:
  float x;
  float y;
  float z;
  float w;

    /** StandardVector4()
     * Default constructor
     */ 
    StandardVector4();

    /** StandardVector4(float x, float y, float z, float w)
     * Constructor from floats
     */
    StandardVector4(float x, float y, float z, float w);

    /** StandardVector4(StandardVector4 &v) 
     * Copy constructor
     */
    StandardVector4(const StandardVector4 &v);

    /** Array Indexing
     */
    float & operator[] (int i);
    const float & operator[] (int i) const;
    operator float * ();

    /** Arithmatic assignment operators
     */
    StandardVector4 & operator=(const StandardVector4 & v);
    StandardVector4 & operator+=(const StandardVector4 & v);
    StandardVector4 & operator-=(const StandardVector4 & v);
    StandardVector4 & operator*=(float f);
    StandardVector4 & operator/=(float f);

    /** Arithmatic operator
     */
    //    StandardVector4 operator+(const StandardVector4 &v) const;
    //    StandardVector4 operator-(const StandardVector4 &v) const;
    //    StandardVector4 operator*(const float f) const;
    //    StandardVector4 operator/(const float f) const;

    /** Equality operators
     */
    bool operator==(const StandardVector4 &a);
    bool operator!=(const StandardVector4 &a);

    /** Negation operator
     */
    friend StandardVector4 operator - (const StandardVector4 &a);
    
    /** friend arithmatic operators
     */
    friend StandardVector4 operator+(const StandardVector4 &a, const StandardVector4 &b);
    friend StandardVector4 operator-(const StandardVector4 &a, const StandardVector4 &b);
    friend StandardVector4 operator*(const StandardVector4 &a , float f);
    friend StandardVector4 operator*(float f, const StandardVector4 &a);
    friend StandardVector4 operator/(const StandardVector4 &a, float f);

    /** Additional methods
     */
    void Set(float _x, float _y, float _z, float _w);
    float Length() const;
    float LengthSquared() const;
    bool IsNull();
    StandardVector4 & Normalize();

    void Print(FILE * stream);

};

#include "TypesVector4.inl"

//}

#endif
