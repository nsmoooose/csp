#ifndef __STANDARDVECTOR_H__
#define __STANDARDVECTOR_H__

/** \Class _Vector3
 * This class represents a 3 element vector. It uses overloaded
 * operators to preform basic operations.
 */
class _Vector3
{
 public:
    float x;
    float y;
    float z;

    /** _Vector3()
     * Default constructor
     */ 
    _Vector3();

    /** _Vector3(float x, float y, float z)
     * Constructor from floats
     */
    _Vector3(float x, float y, float z);

    /** _Vector3(_Vector3 &v) 
     * Copy constructor
     */
    _Vector3(const _Vector3 &v);

    /** Array Indexing
     */
    float & operator[] (int i);
    const float & operator[] (int i) const;
    operator float * ();

    /** Arithmatic assignment operators
     */
    _Vector3 & operator=(const _Vector3 &v);
    _Vector3 & operator+=(const _Vector3 & v);
    _Vector3 & operator-=(const _Vector3 & v);
    _Vector3 & operator*=(float f);
    _Vector3 & operator/=(float f);

    bool operator==(const _Vector3 &a);
    bool operator!=(const _Vector3 &a);

    /** Negation operator
     */
    friend _Vector3 operator - (const _Vector3 &a);
    
    /** friend arithmatic operators
     */
    friend _Vector3 operator+(const _Vector3 &a, const _Vector3 &b);
    friend _Vector3 operator-(const _Vector3 &a, const _Vector3 &b);
    friend _Vector3 operator*(const _Vector3 &a, float f);
    friend _Vector3 operator/(const _Vector3 &a, float f);
    friend _Vector3 operator*(float f, const _Vector3 &a);

    /** Additional methods
     */
    void Set(float _x, float _y, float _z);
    float Length() const;
    float LengthSquared() const;
    bool IsNull();
    _Vector3 & Normalize();

};

#include "StandardVector3.inl"

typedef _Vector3 _Vector;


#endif
