#include "StandardVector3.h"

//----------------------------------------------------------------------------
_Vector3 operator/ (const _Vector3 & a, float f)
{
    _Vector3 newVect;

    if ( f != 0.0 )
    {
        float fInv = 1.0/f;
        newVect.x = fInv*a.x;
        newVect.y = fInv*a.y;
        newVect.z = fInv*a.z;
        return newVect;
    }
    else
    {
      // need to properly handle error condition
        return _Vector3(0,0,0);
    }
}
//----------------------------------------------------------------------------
_Vector3& _Vector3::operator/= (float f)
{
    if ( f != 0.0 )
    {
        float fInv = 1.0/f;
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


_Vector3 & _Vector3::Normalize()
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
