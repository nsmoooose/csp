#ifndef __STANDARDVECTOR_H__
#define __STANDARDVECTOR_H__

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

#include <ostream>
#include <math.h>

class StandardMatrix3;

using namespace std;

/** \Class StandardVector3
 * This class represents a 3 element vector. It uses overloaded
 * operators to preform basic operations.
 */

class StandardVector3
{
 public:
    float x;
    float y;
    float z;

    /** StandardVector3()
     * Default constructor
     */ 
    StandardVector3();

    /** StandardVector3(float x, float y, float z)
     * Constructor from floats
     */
    StandardVector3(float x, float y, float z);

    /** StandardVector3(StandardVector3 &v) 
     * Copy constructor
     */
    StandardVector3(const StandardVector3 &v);

    /** Array Indexing
     */
    float & operator[] (int i);
    const float & operator[] (int i) const;
    operator float * ();

    /** Arithmatic assignment operators
     */
    StandardVector3 & operator=(const StandardVector3 &v);
    StandardVector3 & operator+=(const StandardVector3 & v);
    StandardVector3 & operator-=(const StandardVector3 & v);
    StandardVector3 & operator*=(float f);
    StandardVector3 & operator/=(float f);

    friend bool operator==(const StandardVector3 &a, const StandardVector3 &b);
    friend bool operator!=(const StandardVector3 &a, const StandardVector3 &b);

    /** Negation operator
     */
    friend StandardVector3 operator - (const StandardVector3 &a);
    
    /** friend arithmatic operators
     */
    friend StandardVector3 operator+(const StandardVector3 &a, const StandardVector3 &b);
    friend StandardVector3 operator-(const StandardVector3 &a, const StandardVector3 &b);
    friend StandardVector3 operator*(const StandardVector3 &a, float f);
    friend StandardVector3 operator/(const StandardVector3 &a, float f);
    friend StandardVector3 operator*(float f, const StandardVector3 &a);

    /** Additional methods
     */
    void Set(float _x, float _y, float _z);
    float Length() const;
    float LengthSquared() const;
    bool IsNull();
    StandardVector3 & Normalize();
    float Unitize (float  fTolerance = 1e-06);

    friend float Dot (const StandardVector3 & aVector,
		              const StandardVector3 & bVector);

    friend StandardVector3 Cross (const StandardVector3 & aVector, 
			                      const StandardVector3 & bVector);
	
    friend StandardVector3 Normalized(const StandardVector3 &a);

    StandardMatrix3 StarMatrix();

    void Print(FILE * stream);

    static const StandardVector3 ZERO;

    friend ostream & operator << (ostream & os, const StandardVector3& v);


};


#include "TypesVector3.inl"


#endif
