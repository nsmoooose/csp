
#include "stdinc.h"

#include "TypesMath.h"

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

//namespace Math 
//{

const StandardVector3 StandardVector3::ZERO (0.0, 0.0, 0.0);
const StandardVector3 StandardVector3::XAXIS(1.0, 0.0, 0.0);
const StandardVector3 StandardVector3::YAXIS(0.0, 1.0, 0.0);
const StandardVector3 StandardVector3::ZAXIS(0.0, 0.0, 1.0);


/** divide a vector by a double 
 */
StandardVector3 operator/ (const StandardVector3 & a, double f) //Checked (delta)
{
    StandardVector3 newVect;

    if ( f != 0.0 )
    {
        double fInv = 1.0f/f;
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

/** divide this vector by a double.
 */
StandardVector3& StandardVector3::operator/= (double f)
{
    if ( f != 0.0 )
    {
        double fInv = 1.0f/f;
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
StandardVector3 & StandardVector3::Normalize() // Checked (delta)
{
    double m = Length();
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
double StandardVector3::Unitize (double fTolerance)
{
    double fLength = Length();

    if ( fLength > fTolerance )
    {
        double fInvLength = 1.0f/fLength;
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
  StandardMatrix3 mat;
  mat[0][1] = -z;
  mat[0][2] = y;
  mat[1][0] = z;
  mat[1][2] = -x;
  mat[2][0] = -y;
  mat[2][1] = x;
 
  return mat;

}

std::string StandardVector3::toString()
{
  char buff[128];
  sprintf(buff, "[%.3f, %.3f, %.3f]\n", x, y, z);
  return buff;

}

std::ostream & operator << (std::ostream & os, const StandardVector3& v)
{
  os << "[" << std::setw(8) << v.x 
     << ", " << std::setw(8) << v.y 
     << ", " << std::setw(8) << v.z << "]";
  return os;
}
