
// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

#include "Framework.h"

using namespace std;

//namespace Math
//{

const float StandardMatrix3::EPSILON = 1e-06f;
const StandardMatrix3 StandardMatrix3::ZERO(0,0,0,0,0,0,0,0,0);
const StandardMatrix3 StandardMatrix3::IDENTITY(1,0,0,0,1,0,0,0,1);
  //const float StandardMatrix3::ms_fSvdEpsilon = 1e-04;
  //const int StandardMatrix3::ms_iSvdMaxIterations = 32;

//----------------------------------------------------------------------------
StandardMatrix3::StandardMatrix3 ()
{
    // For efficiency reasons, do not initialize matrix.
}
//----------------------------------------------------------------------------
StandardMatrix3::StandardMatrix3 (const float aafEntry[3][3])
{
    memcpy(rowcol,aafEntry,9*sizeof(float));
}
//----------------------------------------------------------------------------
StandardMatrix3::StandardMatrix3 (const StandardMatrix3& rkMatrix)
{
    memcpy(rowcol,rkMatrix.rowcol,9*sizeof(float));
}
//----------------------------------------------------------------------------
StandardMatrix3::StandardMatrix3 (float fEntry00, float fEntry01, float fEntry02,
    float fEntry10, float fEntry11, float fEntry12, float fEntry20,
    float fEntry21, float fEntry22)
{
    rowcol[0][0] = fEntry00;
    rowcol[0][1] = fEntry01;
    rowcol[0][2] = fEntry02;
    rowcol[1][0] = fEntry10;
    rowcol[1][1] = fEntry11;
    rowcol[1][2] = fEntry12;
    rowcol[2][0] = fEntry20;
    rowcol[2][1] = fEntry21;
    rowcol[2][2] = fEntry22;
}
//----------------------------------------------------------------------------
float* StandardMatrix3::operator[] (int iRow) const
{
    return (float*)&rowcol[iRow][0];
}
//----------------------------------------------------------------------------
StandardMatrix3::operator float* ()
{
    return &rowcol[0][0];
}
//----------------------------------------------------------------------------
StandardVector3 StandardMatrix3::GetColumn (int iCol) const
{
    assert( 0 <= iCol && iCol < 3 );
    return StandardVector3(rowcol[0][iCol],rowcol[1][iCol],
        rowcol[2][iCol]);
}
//----------------------------------------------------------------------------
StandardMatrix3& StandardMatrix3::operator= (const StandardMatrix3& rkMatrix)
{
    memcpy(rowcol,rkMatrix.rowcol,9*sizeof(float));
    return *this;
}
//----------------------------------------------------------------------------
bool StandardMatrix3::operator== (const StandardMatrix3& rkMatrix) const
{
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            if ( rowcol[iRow][iCol] != rkMatrix.rowcol[iRow][iCol] )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool StandardMatrix3::operator!= (const StandardMatrix3& rkMatrix) const
{
    return !operator==(rkMatrix);
}

//operators with assignment

StandardMatrix3 & StandardMatrix3::operator+=(const StandardMatrix3 & rkMatrix)
{
  for (int iRow = 0; iRow < 3; iRow++)
    {
      for (int iCol = 0; iCol < 3; iCol++)
	{
	  rowcol[iRow][iCol] += rkMatrix.rowcol[iRow][iCol];
	}
    }


  return *this;
}

StandardMatrix3 & StandardMatrix3::operator-=(const StandardMatrix3 & rkMatrix)
{
  for (int iRow = 0; iRow < 3; iRow++)
    {
      for (int iCol = 0; iCol < 3; iCol++)
	{
	  rowcol[iRow][iCol] -= rkMatrix.rowcol[iRow][iCol];
	}
    }


  return *this;

}

StandardMatrix3 & StandardMatrix3::operator*=(const StandardMatrix3 & rkMatrix)
{
  StandardMatrix3 t;
  for (int iRow = 0; iRow < 3; iRow++)
    {
      for (int iCol = 0; iCol < 3; iCol++)
	{

            t[iRow][iCol] =
                rowcol[iRow][0]*rkMatrix.rowcol[0][iCol] +
                rowcol[iRow][1]*rkMatrix.rowcol[1][iCol] +
                rowcol[iRow][2]*rkMatrix.rowcol[2][iCol];
	}
    }

  *this = t;
  return *this;

}



//----------------------------------------------------------------------------
StandardMatrix3 StandardMatrix3::operator+ (const StandardMatrix3& rkMatrix) const
{
    StandardMatrix3 kSum;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kSum.rowcol[iRow][iCol] = rowcol[iRow][iCol] +
                rkMatrix.rowcol[iRow][iCol];
        }
    }
    return kSum;
}
//----------------------------------------------------------------------------
StandardMatrix3 StandardMatrix3::operator- (const StandardMatrix3& rkMatrix) const
{
    StandardMatrix3 kDiff;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kDiff.rowcol[iRow][iCol] = rowcol[iRow][iCol] -
                rkMatrix.rowcol[iRow][iCol];
        }
    }
    return kDiff;
}
//----------------------------------------------------------------------------
StandardMatrix3 StandardMatrix3::operator* (const StandardMatrix3& rkMatrix) const
{
    StandardMatrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kProd.rowcol[iRow][iCol] =
                rowcol[iRow][0]*rkMatrix.rowcol[0][iCol] +
                rowcol[iRow][1]*rkMatrix.rowcol[1][iCol] +
                rowcol[iRow][2]*rkMatrix.rowcol[2][iCol];
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
StandardVector3 operator* (const StandardMatrix3& rkMatrix,
					    const StandardVector3& rkPoint)
{
    StandardVector3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        kProd[iRow] =
            rkMatrix.rowcol[iRow][0]*rkPoint[0] +
            rkMatrix.rowcol[iRow][1]*rkPoint[1] +
            rkMatrix.rowcol[iRow][2]*rkPoint[2];
    }
    return kProd;
}
//----------------------------------------------------------------------------
StandardVector3 operator* (const StandardVector3& rkPoint, const StandardMatrix3& rkMatrix)
{
    StandardVector3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        kProd[iRow] =
            rkPoint[0]*rkMatrix.rowcol[0][iRow] +
            rkPoint[1]*rkMatrix.rowcol[1][iRow] +
            rkPoint[2]*rkMatrix.rowcol[2][iRow];
    }
    return kProd;
}
//----------------------------------------------------------------------------
StandardMatrix3 StandardMatrix3::operator- () const
{
    StandardMatrix3 kNeg;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            kNeg[iRow][iCol] = -rowcol[iRow][iCol];
    }
    return kNeg;
}
//----------------------------------------------------------------------------
StandardMatrix3 StandardMatrix3::operator* (float fScalar) const
{
    StandardMatrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            kProd[iRow][iCol] = fScalar*rowcol[iRow][iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
StandardMatrix3 operator* (float fScalar, const StandardMatrix3& rkMatrix)
{
    StandardMatrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            kProd[iRow][iCol] = fScalar*rkMatrix.rowcol[iRow][iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
StandardMatrix3 StandardMatrix3::Transpose () const
{
    StandardMatrix3 kTranspose;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            kTranspose[iRow][iCol] = rowcol[iCol][iRow];
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
bool StandardMatrix3::Inverse (StandardMatrix3& rkInverse, float fTolerance) const
{
    // Invert a 3x3 using cofactors.  This is about 8 times faster than
    // the Numerical Recipes code which uses Gaussian elimination.

    rkInverse[0][0] = rowcol[1][1]*rowcol[2][2] -
        rowcol[1][2]*rowcol[2][1];
    rkInverse[0][1] = rowcol[0][2]*rowcol[2][1] -
        rowcol[0][1]*rowcol[2][2];
    rkInverse[0][2] = rowcol[0][1]*rowcol[1][2] -
        rowcol[0][2]*rowcol[1][1];
    rkInverse[1][0] = rowcol[1][2]*rowcol[2][0] -
        rowcol[1][0]*rowcol[2][2];
    rkInverse[1][1] = rowcol[0][0]*rowcol[2][2] -
        rowcol[0][2]*rowcol[2][0];
    rkInverse[1][2] = rowcol[0][2]*rowcol[1][0] -
        rowcol[0][0]*rowcol[1][2];
    rkInverse[2][0] = rowcol[1][0]*rowcol[2][1] -
        rowcol[1][1]*rowcol[2][0];
    rkInverse[2][1] = rowcol[0][1]*rowcol[2][0] -
        rowcol[0][0]*rowcol[2][1];
    rkInverse[2][2] = rowcol[0][0]*rowcol[1][1] -
        rowcol[0][1]*rowcol[1][0];

    float fDet =
        rowcol[0][0]*rkInverse[0][0] +
        rowcol[0][1]*rkInverse[1][0]+
        rowcol[0][2]*rkInverse[2][0];

    if ( fabs(fDet) <= fTolerance )
        return false;

    float fInvDet = 1.0f/fDet;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            rkInverse[iRow][iCol] *= fInvDet;
    }

    return true;
}
//----------------------------------------------------------------------------
StandardMatrix3 StandardMatrix3::Inverse (float fTolerance) const
{
    StandardMatrix3 kInverse = StandardMatrix3::ZERO;
    Inverse(kInverse,fTolerance);
    return kInverse;
}
//----------------------------------------------------------------------------
float StandardMatrix3::Determinant () const
{
    float fCofactor00 = rowcol[1][1]*rowcol[2][2] -
        rowcol[1][2]*rowcol[2][1];
    float fCofactor10 = rowcol[1][2]*rowcol[2][0] -
        rowcol[1][0]*rowcol[2][2];
    float fCofactor20 = rowcol[1][0]*rowcol[2][1] -
        rowcol[1][1]*rowcol[2][0];

    float fDet =
        rowcol[0][0]*fCofactor00 +
        rowcol[0][1]*fCofactor10 +
        rowcol[0][2]*fCofactor20;

    return fDet;
}


//----------------------------------------------------------------------------
void StandardMatrix3::ToAxisAngle (StandardVector3& rkAxis, float& rfRadians) const
{
    // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
    // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 -z +y |
    //       | +z  0 -x |
    //       | -y +x  0 |
    //       +-        -+
    //
    // If A > 0, R represents a counterclockwise rotation about the axis in
    // the sense of looking from the tip of the axis vector towards the
    // origin.  Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
    //
    // In the event that A = pi, R-R^t = 0 which prevents us from extracting
    // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
    // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
    // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
    // it does not matter which sign you choose on the square roots.

    float fTrace = rowcol[0][0] + rowcol[1][1] + rowcol[2][2];
    float fCos = 0.5f*(fTrace-1.0f);
    rfRadians = (float)acos(fCos);  // in [0,PI]

    if ( rfRadians > 0.0 )
    {
        if ( rfRadians < PI )
        {
            rkAxis.x = rowcol[2][1]-rowcol[1][2];
            rkAxis.y = rowcol[0][2]-rowcol[2][0];
            rkAxis.z = rowcol[1][0]-rowcol[0][1];
            rkAxis.Unitize();
        }
        else
        {
            // angle is PI
            float fHalfInverse;
            if ( rowcol[0][0] >= rowcol[1][1] )
            {
                // r00 >= r11
                if ( rowcol[0][0] >= rowcol[2][2] )
                {
                    // r00 is maximum diagonal term
                    rkAxis.x = 0.5f*(float)sqrt(rowcol[0][0] -
                        rowcol[1][1] - rowcol[2][2] + 1.0f);
                    fHalfInverse = 0.5f/rkAxis.x;
                    rkAxis.y = fHalfInverse*rowcol[0][1];
                    rkAxis.z = fHalfInverse*rowcol[0][2];
                }
                else
                {
                    // r22 is maximum diagonal term
                    rkAxis.z = 0.5f*(float)sqrt(rowcol[2][2] -
                        rowcol[0][0] - rowcol[1][1] + 1.0f);
                    fHalfInverse = 0.5f/rkAxis.z;
                    rkAxis.x = fHalfInverse*rowcol[0][2];
                    rkAxis.y = fHalfInverse*rowcol[1][2];
                }
            }
            else
            {
                // r11 > r00
                if ( rowcol[1][1] >= rowcol[2][2] )
                {
                    // r11 is maximum diagonal term
                    rkAxis.y = 0.5f*(float)sqrt(rowcol[1][1] -
                        rowcol[0][0] - rowcol[2][2] + 1.0f);
                    fHalfInverse  = 0.5f/rkAxis.y;
                    rkAxis.x = fHalfInverse*rowcol[0][1];
                    rkAxis.z = fHalfInverse*rowcol[1][2];
                }
                else
                {
                    // r22 is maximum diagonal term
                    rkAxis.z = 0.5f*(float)sqrt(rowcol[2][2] -
                        rowcol[0][0] - rowcol[1][1] + 1.0f);
                    fHalfInverse = 0.5f/rkAxis.z;
                    rkAxis.x = fHalfInverse*rowcol[0][2];
                    rkAxis.y = fHalfInverse*rowcol[1][2];
                }
            }
        }
    }
    else
    {
        // The angle is 0 and the matrix is the identity.  Any axis will
        // work, so just use the x-axis.
        rkAxis.x = 1.0;
        rkAxis.y = 0.0;
        rkAxis.z = 0.0;
    }
}
//----------------------------------------------------------------------------
void StandardMatrix3::FromAxisAngle (const StandardVector3& rkAxis, float fRadians)
{
    float fCos = (float)cos(fRadians);
    float fSin = (float)sin(fRadians);
    float fOneMinusCos = 1.0f-fCos;
    float fX2 = rkAxis.x*rkAxis.x;
    float fY2 = rkAxis.y*rkAxis.y;
    float fZ2 = rkAxis.z*rkAxis.z;
    float fXYM = rkAxis.x*rkAxis.y*fOneMinusCos;
    float fXZM = rkAxis.x*rkAxis.z*fOneMinusCos;
    float fYZM = rkAxis.y*rkAxis.z*fOneMinusCos;
    float fXSin = rkAxis.x*fSin;
    float fYSin = rkAxis.y*fSin;
    float fZSin = rkAxis.z*fSin;
    
    rowcol[0][0] = fX2*fOneMinusCos+fCos;
    rowcol[0][1] = fXYM-fZSin;
    rowcol[0][2] = fXZM+fYSin;
    rowcol[1][0] = fXYM+fZSin;
    rowcol[1][1] = fY2*fOneMinusCos+fCos;
    rowcol[1][2] = fYZM-fXSin;
    rowcol[2][0] = fXZM-fYSin;
    rowcol[2][1] = fYZM+fXSin;
    rowcol[2][2] = fZ2*fOneMinusCos+fCos;
}
//----------------------------------------------------------------------------
bool StandardMatrix3::ToEulerAnglesXYZ (float& rfXAngle, float& rfYAngle,
    float& rfZAngle) const
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    if ( rowcol[0][2] < 1.0 )
    {
        if ( rowcol[0][2] > -1.0 )
        {
            rfXAngle = (float)atan2(-rowcol[1][2],rowcol[2][2]);
            rfYAngle = (float)asin(rowcol[0][2]);
            rfZAngle = (float)atan2(-rowcol[0][1],rowcol[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            rfXAngle = (float)-atan2(rowcol[1][0],rowcol[1][1]);
            rfYAngle = (float)-HALF_PI;
            rfZAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        rfXAngle = (float)atan2(rowcol[1][0],rowcol[1][1]);
        rfYAngle = (float)HALF_PI;
        rfZAngle = 0.0f;
        return false;
    }
}
//----------------------------------------------------------------------------
bool StandardMatrix3::ToEulerAnglesXZY (float& rfXAngle, float& rfZAngle,
    float& rfYAngle) const
{
    // rot =  cy*cz          -sz              cz*sy
    //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
    //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

    if ( rowcol[0][1] < 1.0 )
    {
        if ( rowcol[0][1] > -1.0 )
        {
            rfXAngle = (float)atan2(rowcol[2][1],rowcol[1][1]);
            rfZAngle = (float)asin(-rowcol[0][1]);
            rfYAngle = (float)atan2(rowcol[0][2],rowcol[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - YA = atan2(r20,r22)
            rfXAngle = (float)atan2(rowcol[2][0],rowcol[2][2]);
            rfZAngle = (float)HALF_PI;
            rfYAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
        rfXAngle = (float)atan2(-rowcol[2][0],rowcol[2][2]);
        rfZAngle = (float)-HALF_PI;
        rfYAngle = 0.0f;
        return false;
    }
}
//----------------------------------------------------------------------------
bool StandardMatrix3::ToEulerAnglesYXZ (float& rfYAngle, float& rfXAngle,
    float& rfZAngle) const
{
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

    if ( rowcol[1][2] < 1.0 )
    {
        if ( rowcol[1][2] > -1.0 )
        {
            rfYAngle = (float)atan2(rowcol[0][2],rowcol[2][2]);
            rfXAngle = (float)asin(-rowcol[1][2]);
            rfZAngle = (float)atan2(rowcol[1][0],rowcol[1][1]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
            rfYAngle = (float)atan2(rowcol[0][1],rowcol[0][0]);
            rfXAngle = (float)HALF_PI;
            rfZAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
        rfYAngle = (float)atan2(-rowcol[0][1],rowcol[0][0]);
        rfXAngle = (float)-HALF_PI;
        rfZAngle = 0.0f;
        return false;
    }
}
//----------------------------------------------------------------------------
bool StandardMatrix3::ToEulerAnglesYZX (float& rfYAngle, float& rfZAngle,
    float& rfXAngle) const
{
    // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
    //        sz              cx*cz          -cz*sx
    //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

    if ( rowcol[1][0] < 1.0 )
    {
        if ( rowcol[1][0] > -1.0 )
        {
            rfYAngle = (float)atan2(-rowcol[2][0],rowcol[0][0]);
            rfZAngle = (float) asin(rowcol[1][0]);
            rfXAngle = (float)atan2(-rowcol[1][2],rowcol[1][1]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
            rfYAngle = (float)-atan2(rowcol[2][1],rowcol[2][2]);
            rfZAngle = (float)-HALF_PI;
            rfXAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + XA = atan2(r21,r22)
        rfYAngle = (float)atan2(rowcol[2][1],rowcol[2][2]);
        rfZAngle = (float)HALF_PI;
        rfXAngle = 0.0f;
        return false;
    }
}
//----------------------------------------------------------------------------
bool StandardMatrix3::ToEulerAnglesZXY (float& rfZAngle, float& rfXAngle,
    float& rfYAngle) const
{
    // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
    //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
    //       -cx*sy           sx              cx*cy

    if ( rowcol[2][1] < 1.0 )
    {
        if ( rowcol[2][1] > -1.0 )
        {
            rfZAngle = (float)atan2(-rowcol[0][1],rowcol[1][1]);
            rfXAngle = (float) asin(rowcol[2][1]);
            rfYAngle = (float)atan2(-rowcol[2][0],rowcol[2][2]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
            rfZAngle = (float)-atan2(rowcol[0][2],rowcol[0][0]);
            rfXAngle = (float)-HALF_PI;
            rfYAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
        rfZAngle = (float)atan2(rowcol[0][2],rowcol[0][0]);
        rfXAngle = (float)HALF_PI;
        rfYAngle = 0.0f;
        return false;
    }
}
//----------------------------------------------------------------------------
bool StandardMatrix3::ToEulerAnglesZYX (float& rfZAngle, float& rfYAngle,
    float& rfXAngle) const
{
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy

    if ( rowcol[2][0] < 1.0 )
    {
        if ( rowcol[2][0] > -1.0 )
        {
            rfZAngle = (float)atan2(rowcol[1][0],rowcol[0][0]);
            rfYAngle = (float) asin(-rowcol[2][0]);
            rfXAngle = (float)atan2(rowcol[2][1],rowcol[2][2]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
            rfZAngle = (float)-atan2(rowcol[0][1],rowcol[0][2]);
            rfYAngle = (float)HALF_PI;
            rfXAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
        rfZAngle = (float)atan2(-rowcol[0][1],-rowcol[0][2]);
        rfYAngle = (float)-HALF_PI;
        rfXAngle = 0.0f;
        return false;
    }
}
//----------------------------------------------------------------------------
void StandardMatrix3::FromEulerAnglesXYZ (float fYAngle, float fPAngle,
    float fRAngle)
{
    float fCos, fSin;

    fCos = (float)cos(fYAngle);
    fSin = (float)sin(fYAngle);
    StandardMatrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

    fCos = (float)cos(fPAngle);
    fSin = (float)sin(fPAngle);
    StandardMatrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

    fCos = (float)cos(fRAngle);
    fSin = (float)sin(fRAngle);
    StandardMatrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

    *this = kXMat*(kYMat*kZMat);
}
//----------------------------------------------------------------------------
void StandardMatrix3::FromEulerAnglesXZY (float fYAngle, float fPAngle,
    float fRAngle)
{
    float fCos, fSin;

    fCos = (float)cos(fYAngle);
    fSin = (float)sin(fYAngle);
    StandardMatrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

    fCos = (float)cos(fPAngle);
    fSin = (float)sin(fPAngle);
    StandardMatrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

    fCos = (float)cos(fRAngle);
    fSin = (float)sin(fRAngle);
    StandardMatrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

    *this = kXMat*(kZMat*kYMat);
}
//----------------------------------------------------------------------------
void StandardMatrix3::FromEulerAnglesYXZ (float fYAngle, float fPAngle,
    float fRAngle)
{
    float fCos, fSin;

    fCos = (float)cos(fYAngle);
    fSin = (float)sin(fYAngle);
    StandardMatrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

    fCos = (float)cos(fPAngle);
    fSin = (float)sin(fPAngle);
    StandardMatrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

    fCos = (float)cos(fRAngle);
    fSin = (float)sin(fRAngle);
    StandardMatrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

    *this = kYMat*(kXMat*kZMat);
}
//----------------------------------------------------------------------------
void StandardMatrix3::FromEulerAnglesYZX (float fYAngle, float fPAngle,
    float fRAngle)
{
    float fCos, fSin;

    fCos = (float)cos(fYAngle);
    fSin = (float)sin(fYAngle);
    StandardMatrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

    fCos = (float)cos(fPAngle);
    fSin = (float)sin(fPAngle);
    StandardMatrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

    fCos = (float)cos(fRAngle);
    fSin = (float)sin(fRAngle);
    StandardMatrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

    *this = kYMat*(kZMat*kXMat);
}
//----------------------------------------------------------------------------
void StandardMatrix3::FromEulerAnglesZXY (float fYAngle, float fPAngle,
    float fRAngle)
{
    float fCos, fSin;

    fCos = (float)cos(fYAngle);
    fSin = (float)sin(fYAngle);
    StandardMatrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

    fCos = (float)cos(fPAngle);
    fSin = (float)sin(fPAngle);
    StandardMatrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

    fCos = (float)cos(fRAngle);
    fSin = (float)sin(fRAngle);
    StandardMatrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

    *this = kZMat*(kXMat*kYMat);
}
//----------------------------------------------------------------------------
void StandardMatrix3::FromEulerAnglesZYX (float fYAngle, float fPAngle,
    float fRAngle)
{
    float fCos, fSin;

    fCos = (float)cos(fYAngle);
    fSin = (float)sin(fYAngle);
    StandardMatrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

    fCos = (float)cos(fPAngle);
    fSin = (float)sin(fPAngle);
    StandardMatrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

    fCos = (float)cos(fRAngle);
    fSin = (float)sin(fRAngle);
    StandardMatrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

    *this = kZMat*(kYMat*kXMat);
}

//----------------------------------------------------------------------------
void StandardMatrix3::TensorProduct (const StandardVector3& rkU, const StandardVector3& rkV,
    StandardMatrix3& rkProduct)
{
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            rkProduct[iRow][iCol] = rkU[iRow]*rkV[iCol];
    }
}
//----------------------------------------------------------------------------


// These routines are using right handed transformations and are based
// on row-column based matrices. They will work with OpenGL or DirectX
// because the linear representation of the matrix will be the same.

StandardMatrix3 TranslationMatrix3( float x, float y, float z )
{
  StandardMatrix3 mat = StandardMatrix3::IDENTITY;
  mat[3][0] = x;
  mat[3][1] = y;
  mat[3][2] = z;
  return mat;
}

StandardMatrix3 RotationXMatrix3( float angle )
{
    StandardMatrix3 mat = StandardMatrix3::IDENTITY;
 	mat[1][1] = (float)cos( angle );
	mat[1][2] = (float)sin( angle );
	mat[2][1] = -mat[1][2];
	mat[2][2] = mat[1][1];
    return mat;
}

StandardMatrix3 RotationYMatrix3( float angle )
{
  StandardMatrix3 mat = StandardMatrix3::IDENTITY;
  mat[0][0] = (float)cos( angle );
  mat[2][0] = (float)sin( angle );
  mat[0][2] = -mat[2][0];
  mat[2][2] = mat[0][0];
  return mat;
}

StandardMatrix3 RotationZMatrix3( float angle )
{
  StandardMatrix3 mat = StandardMatrix3::IDENTITY;
	mat[0][0] = (float)cos( angle );
    mat[1][0] = (float)-sin(angle);
	mat[0][1] = -mat[1][0];
	mat[1][1] = mat[0][0];
    return mat;
}


void StandardMatrix3::Print(FILE * stream)
{
  fprintf(stream, "[%f, %f, %f]\n", rowcol[0][0], rowcol[0][1], rowcol[0][2]);
  fprintf(stream, "[%f, %f, %f]\n", rowcol[1][0], rowcol[1][1], rowcol[1][2]);
  fprintf(stream, "[%f, %f, %f]\n", rowcol[2][0], rowcol[2][1], rowcol[2][2]);
}

void StandardMatrix3::Print(char *String)
{
  sprintf(String, "[%+f, %+f, %+f]\n[%+f, %+f, %+f]\n[%+f, %+f, %+f]\n",
    rowcol[0][0], rowcol[0][1], rowcol[0][2],
    rowcol[1][0], rowcol[1][1], rowcol[1][2],
    rowcol[2][0], rowcol[2][1], rowcol[2][2]);

}

ostream & operator<<(ostream & os, const StandardMatrix3 & m)
{
  os << "[" <<  setw(8) << m[0][0]
     << ", " << setw(8) << m[0][1] 
     << ", " << setw(8) << m[0][2] 
     << "]" << endl
     << "[" <<  setw(8) << m[1][0]
     << ", " << setw(8) << m[1][1] 
     << ", " << setw(8) << m[1][2] 
     << "]" << endl
     << "[" <<  setw(8) << m[2][0]
     << ", " << setw(8) << m[2][1] 
     << ", " << setw(8) << m[2][2] 
     << "]" << endl;
    return os;

}


//}
