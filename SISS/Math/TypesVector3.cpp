

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.


#include "TypesVector3.h"
#include "TypesMatrix3.h"
#include <iostream>
#include <iomanip>

using namespace std;

const StandardVector3 StandardVector3::ZERO(0,0,0);


/** divide a vector by a float 
 */
StandardVector3 operator/ (const StandardVector3 & a, float f)
{
    StandardVector3 newVect;

    if ( f != 0.0 )
    {
        float fInv = 1.0f/f;
        newVect.x = fInv*a.x;
        newVect.y = fInv*a.y;
        newVect.z = fInv*a.z;
        return newVect;
    }
    else
    {
      // need to properly handle error condition
        return StandardVector3(0,0,0);
    }
}

/** divide this vector by a float.
 */
StandardVector3& StandardVector3::operator/= (float f)
{
    if ( f != 0.0 )
    {
        float fInv = 1.0f/f;
        x *= fInv;
        y *= fInv;
        z *= fInv;
    }
    else
    {
      // need to properly handle error condition.
        x = 0;
        y = 0;
        z = 0;
    }

    return *this;
}

/** Normalize the vector so its length is 1.
 */
StandardVector3 & StandardVector3::Normalize()
{
    float m = Length();
    if (m > 0.0F)
      m = 1.0F / m;
    else
      m = 0.0F;
    x *= m;
    y *= m;
    z *= m;
    return *this;
}

//----------------------------------------------------------------------------
float StandardVector3::Unitize (float fTolerance)
{
    float fLength = Length();

    if ( fLength > fTolerance )
    {
        float fInvLength = 1.0f/fLength;
        x *= fInvLength;
        y *= fInvLength;
        z *= fInvLength;
    }
    else
    {
        fLength = 0.0;
    }

    return fLength;
}

void StandardVector3::Print(FILE * stream)
{
  fprintf(stream, "[%f, %f, %f]\n", x, y, z);
}

//}

StandardMatrix3 StandardVector3::StarMatrix()
{
  StandardMatrix3 mat = StandardMatrix3::ZERO;
  mat[0][1] = -z;
  mat[0][2] = y;
  mat[1][0] = z;
  mat[1][2] = -x;
  mat[2][0] = -y;
  mat[2][1] = x;
 
  return mat;

}

ostream & operator << (ostream & os, const StandardVector3& v)
{
  os << "[" << setw(8) << v.x 
     << ", " << setw(8) << v.y 
     << ", " << setw(8) << v.z << "]";
  return os;
}
