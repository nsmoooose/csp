// Modified for CSP by wolverine


// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2002.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#include "TypesQuaternion.h"

using namespace std;


static float gs_fEpsilon = 1e-03f;
const StandardQuaternion StandardQuaternion::ZERO(0.0f,0.0f,0.0f,0.0f);
const StandardQuaternion StandardQuaternion::IDENTITY(1.0f,0.0f,0.0f,0.0f);

//----------------------------------------------------------------------------
StandardQuaternion::StandardQuaternion (float fW, float fX, float fY, float fZ)
{
    w = fW;
    x = fX;
    y = fY;
    z = fZ;
}
//----------------------------------------------------------------------------
StandardQuaternion::StandardQuaternion (const StandardQuaternion& rkQ)
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
}
//----------------------------------------------------------------------------
void StandardQuaternion::FromRotationMatrix (const StandardMatrix3& kRot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "StandardQuaternion Calculus and Fast Animation".

    float fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
    float fRoot;

    if ( fTrace > 0.0f )
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = sqrt(fTrace + 1.0f);  // 2w
        w = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;  // 1/(4w)
        x = (kRot[2][1]-kRot[1][2])*fRoot;
        y = (kRot[0][2]-kRot[2][0])*fRoot;
        z = (kRot[1][0]-kRot[0][1])*fRoot;
    }
    else
    {
        // |w| <= 1/2
        static int s_iNext[3] = { 1, 2, 0 };
        int i = 0;
        if ( kRot[1][1] > kRot[0][0] )
            i = 1;
        if ( kRot[2][2] > kRot[i][i] )
            i = 2;
        int j = s_iNext[i];
        int k = s_iNext[j];

        fRoot = sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0f);
        float* apkQuat[3] = { &x, &y, &z };
        *apkQuat[i] = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;
        w = (kRot[k][j]-kRot[j][k])*fRoot;
        *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
        *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
    }
}
//----------------------------------------------------------------------------
void StandardQuaternion::ToRotationMatrix (StandardMatrix3& kRot) const
{
    float fTx  = 2.0f*x;
    float fTy  = 2.0f*y;
    float fTz  = 2.0f*z;
    float fTwx = fTx*w;
    float fTwy = fTy*w;
    float fTwz = fTz*w;
    float fTxx = fTx*x;
    float fTxy = fTy*x;
    float fTxz = fTz*x;
    float fTyy = fTy*y;
    float fTyz = fTz*y;
    float fTzz = fTz*z;

    kRot[0][0] = 1.0f-(fTyy+fTzz);
    kRot[0][1] = fTxy-fTwz;
    kRot[0][2] = fTxz+fTwy;
    kRot[1][0] = fTxy+fTwz;
    kRot[1][1] = 1.0f-(fTxx+fTzz);
    kRot[1][2] = fTyz-fTwx;
    kRot[2][0] = fTxz-fTwy;
    kRot[2][1] = fTyz+fTwx;
    kRot[2][2] = 1.0f-(fTxx+fTyy);
}
//----------------------------------------------------------------------------
void StandardQuaternion::FromAngleAxis (const float& rfAngle, const StandardVector3& rkAxis)
{
    // assert:  axis[] is unit length
    //
	// The StandardQuaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    float fHalfAngle = 0.5f*rfAngle;
    float fSin = sin(fHalfAngle);
    w = cos(fHalfAngle);
    x = fSin*rkAxis.x;
    y = fSin*rkAxis.y;
    z = fSin*rkAxis.z;
}
//----------------------------------------------------------------------------
void StandardQuaternion::ToAngleAxis (float& rfAngle, StandardVector3& rkAxis) const
{
	// The StandardQuaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    float fSqrLength = x*x+y*y+z*z;
    if ( fSqrLength > 0.0f )
    {
        rfAngle = 2.0f*acos(w);
        float fInvLength = 1.0/sqrt(fSqrLength);
        rkAxis.x = x*fInvLength;
        rkAxis.y = y*fInvLength;
        rkAxis.z = z*fInvLength;
    }
    else
    {
        // angle is 0 (mod 2*pi), so any axis will do
        rfAngle = 0.0f;
        rkAxis.x = 1.0f;
        rkAxis.y = 0.0f;
        rkAxis.z = 0.0f;
    }
}
//----------------------------------------------------------------------------
void StandardQuaternion::FromAxes (const StandardVector3* akAxis)
{
    StandardMatrix3 kRot;

    for (int iCol = 0; iCol < 3; iCol++)
    {
        kRot[0][iCol] = akAxis[iCol].x;
        kRot[1][iCol] = akAxis[iCol].y;
        kRot[2][iCol] = akAxis[iCol].z;
    }

    FromRotationMatrix(kRot);
}
//----------------------------------------------------------------------------
void StandardQuaternion::ToAxes (StandardVector3* akAxis) const
{
    StandardMatrix3 kRot;

    ToRotationMatrix(kRot);

    for (int iCol = 0; iCol < 3; iCol++)
    {
        akAxis[iCol].x = kRot[0][iCol];
        akAxis[iCol].y = kRot[1][iCol];
        akAxis[iCol].z = kRot[2][iCol];
    }
}
//----------------------------------------------------------------------------
StandardQuaternion& StandardQuaternion::operator= (const StandardQuaternion& rkQ)
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
    return *this;
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::operator+ (const StandardQuaternion& rkQ) const
{
    return StandardQuaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::operator- (const StandardQuaternion& rkQ) const
{
    return StandardQuaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::operator* (const StandardQuaternion& rkQ) const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.

    return StandardQuaternion
    (
		w*rkQ.w-x*rkQ.x-y*rkQ.y-z*rkQ.z,
		w*rkQ.x+x*rkQ.w+y*rkQ.z-z*rkQ.y,
		w*rkQ.y+y*rkQ.w+z*rkQ.x-x*rkQ.z,
		w*rkQ.z+z*rkQ.w+x*rkQ.y-y*rkQ.x
    );
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::operator* (float fScalar) const
{
    return StandardQuaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
}
//----------------------------------------------------------------------------
StandardQuaternion operator* (float fScalar, const StandardQuaternion& rkQ)
{
    return StandardQuaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y,
        fScalar*rkQ.z);
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::operator- () const
{
    return StandardQuaternion(-w,-x,-y,-z);
}
//----------------------------------------------------------------------------
float StandardQuaternion::Dot (const StandardQuaternion& rkQ) const
{
    return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
}
//----------------------------------------------------------------------------
float StandardQuaternion::Norm () const
{
    return w*w+x*x+y*y+z*z;
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::Inverse () const
{
    float fNorm = w*w+x*x+y*y+z*z;
    if ( fNorm > 0.0f )
    {
        float fInvNorm = 1.0f/fNorm;
        return StandardQuaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
    }
    else
    {
        // return an invalid result to flag the error
        return ZERO;
    }
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::UnitInverse () const
{
    // assert:  'this' is unit length
    return StandardQuaternion(w,-x,-y,-z);
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::Exp () const
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    float fAngle = sqrt(x*x+y*y+z*z);
    float fSin = sin(fAngle);

    StandardQuaternion kResult;
    kResult.w = cos(fAngle);

    if ( fabs(fSin) >= gs_fEpsilon )
    {
        float fCoeff = fSin/fAngle;
        kResult.x = fCoeff*x;
        kResult.y = fCoeff*y;
        kResult.z = fCoeff*z;
    }
    else
    {
        kResult.x = x;
        kResult.y = y;
        kResult.z = z;
    }

    return kResult;
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::Log () const
{
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

    StandardQuaternion kResult;
    kResult.w = 0.0f;

    if ( fabs(w) < 1.0f )
    {
        float fAngle = acos(w);
        float fSin = sin(fAngle);
        if ( fabs(fSin) >= gs_fEpsilon )
        {
            float fCoeff = fAngle/fSin;
            kResult.x = fCoeff*x;
            kResult.y = fCoeff*y;
            kResult.z = fCoeff*z;
            return kResult;
        }
    }

    kResult.x = x;
    kResult.y = y;
    kResult.z = z;

    return kResult;
}


// Use QVRotate instead.
//----------------------------------------------------------------------------
//StandardVector3 StandardQuaternion::operator* (const StandardVector3& rkVector) const
//{
    // Given a vector u = (x0,y0,z0) and a unit length StandardQuaternion
    // q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
    // rotation of u by q is v = q*u*q^{-1} where * indicates StandardQuaternion
    // multiplication and where u is treated as the StandardQuaternion <0,x0,y0,z0>.
    // Note that q^{-1} = <w,-x,-y,-z>, so no float work is required to
    // invert q.  Now
    //
    //   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
    //     = q*(x0*i+y0*j+z0*k)*q^{-1}
    //     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
    //
    // As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
    // of the rotation matrix computed in StandardQuaternion::ToRotationMatrix.
    // The vector v is obtained as the product of that rotation matrix with
    // vector u.  As such, the StandardQuaternion representation of a rotation
    // matrix requires less space than the matrix and more time to compute
    // the rotated vector.  Typical space-time tradeoff...

//    StandardMatrix3 kRot;
//    ToRotationMatrix(kRot);
//    return kRot*rkVector;
//}



//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::Slerp (float fT, const StandardQuaternion& rkP,
    const StandardQuaternion& rkQ)
{
    float fCos = rkP.Dot(rkQ);
    float fAngle = acos(fCos);

    if ( fabs(fAngle) < gs_fEpsilon )
        return rkP;

    float fSin = sin(fAngle);
    float fInvSin = 1.0f/fSin;
    float fCoeff0 = sin((1.0f-fT)*fAngle)*fInvSin;
    float fCoeff1 = sin(fT*fAngle)*fInvSin;
    return fCoeff0*rkP + fCoeff1*rkQ;
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::SlerpExtraSpins (float fT,
    const StandardQuaternion& rkP, const StandardQuaternion& rkQ, int iExtraSpins)
{
    float fCos = rkP.Dot(rkQ);
    float fAngle = acos(fCos);

    if ( fabs(fAngle) < gs_fEpsilon )
        return rkP;

    float fSin = sin(fAngle);
    float fPhase = PI*iExtraSpins*fT;
    float fInvSin = 1.0f/fSin;
    float fCoeff0 = sin((1.0f-fT)*fAngle - fPhase)*fInvSin;
    float fCoeff1 = sin(fT*fAngle + fPhase)*fInvSin;
    return fCoeff0*rkP + fCoeff1*rkQ;
}
//----------------------------------------------------------------------------
void StandardQuaternion::Intermediate (const StandardQuaternion& rkQ0,
    const StandardQuaternion& rkQ1, const StandardQuaternion& rkQ2, StandardQuaternion& rkA,
    StandardQuaternion& rkB)
{
    // assert:  q0, q1, q2 are unit StandardQuaternions

    StandardQuaternion kQ0inv = rkQ0.UnitInverse();
    StandardQuaternion kQ1inv = rkQ1.UnitInverse();
    StandardQuaternion rkP0 = kQ0inv*rkQ1;
    StandardQuaternion rkP1 = kQ1inv*rkQ2;
    StandardQuaternion kArg = 0.25f*(rkP0.Log()-rkP1.Log());
    StandardQuaternion kMinusArg = -kArg;

    rkA = rkQ1*kArg.Exp();
    rkB = rkQ1*kMinusArg.Exp();
}
//----------------------------------------------------------------------------
StandardQuaternion StandardQuaternion::Squad (float fT, const StandardQuaternion& rkP,
    const StandardQuaternion& rkA, const StandardQuaternion& rkB, const StandardQuaternion& rkQ)
{
    float fSlerpT = 2.0f*fT*(1.0f-fT);
    StandardQuaternion kSlerpP = Slerp(fT,rkP,rkQ);
    StandardQuaternion kSlerpQ = Slerp(fT,rkA,rkB);
    return Slerp(fSlerpT,kSlerpP,kSlerpQ);
}
//----------------------------------------------------------------------------


StandardQuaternion	StandardQuaternion::MakeQFromEulerAngles(float x, float y, float z)
{
	StandardQuaternion	q;
	double	roll = DegreesToRadians(x);
	double	pitch = DegreesToRadians(y);
	double	yaw = DegreesToRadians(z);
	
	double	cyaw, cpitch, croll, syaw, spitch, sroll;
	double	cyawcpitch, syawspitch, cyawspitch, syawcpitch;

	cyaw = cos(0.5f * yaw);
	cpitch = cos(0.5f * pitch);
	croll = cos(0.5f * roll);
	syaw = sin(0.5f * yaw);
	spitch = sin(0.5f * pitch);
	sroll = sin(0.5f * roll);

	cyawcpitch = cyaw*cpitch;
	syawspitch = syaw*spitch;
	cyawspitch = cyaw*spitch;
	syawcpitch = syaw*cpitch;

	q.w = (float) (cyawcpitch * croll + syawspitch * sroll);
	q.x = (float) (cyawcpitch * sroll - syawspitch * croll); 
	q.y = (float) (cyawspitch * croll + syawcpitch * sroll);
	q.z = (float) (syawcpitch * croll - cyawspitch * sroll);

	return q;
}

StandardVector3	StandardQuaternion::MakeEulerAnglesFromQ(StandardQuaternion q)
{
	double	r11, r21, r31, r32, r33, r12, r13;
	double	q00, q11, q22, q33;
	double	tmp;
	StandardVector3	u;

	q00 = q.w * q.w;
	q11 = q.x * q.x;
	q22 = q.y * q.y;
	q33 = q.z * q.z;

	r11 = q00 + q11 - q22 - q33;
	r21 = 2 * (q.x*q.y + q.w*q.z);
	r31 = 2 * (q.x*q.z - q.w*q.y);
	r32 = 2 * (q.y*q.z + q.w*q.x);
	r33 = q00 - q11 - q22 + q33;

	tmp = fabs(r31);
	if(tmp > 0.999999)
	{
		r12 = 2 * (q.x*q.y - q.w*q.z);
		r13 = 2 * (q.x*q.z + q.w*q.y);

		u.x = RadiansToDegrees(0.0f); //roll
		u.y = RadiansToDegrees((float) (-(PI/2) * r31/tmp)); // pitch
		u.z = RadiansToDegrees((float) atan2(-r12, -r31*r13)); // yaw
		return u;
	}

	u.x = RadiansToDegrees((float) atan2(r32, r33)); // roll
	u.y = RadiansToDegrees((float) asin(-r31));		 // pitch
	u.z = RadiansToDegrees((float) atan2(r21, r11)); // yaw
	return u;
	

}

StandardQuaternion	& StandardQuaternion::operator+=(const StandardQuaternion & q)
{
	w += q.w;
	x += q.x;
	y += q.y;
	z += q.z;
	return *this;
}


StandardQuaternion operator*(StandardQuaternion q, StandardVector3 v)
{
	return	StandardQuaternion(	-(q.x*v.x + q.y*v.y + q.z*v.z),
							q.w*v.x + q.y*v.z - q.z*v.y,
							q.w*v.y + q.z*v.x - q.x*v.z,
							q.w*v.z + q.x*v.y - q.y*v.x);
}

StandardQuaternion operator*(StandardVector3 v, StandardQuaternion q)
{
	return	StandardQuaternion(	-(q.x*v.x + q.y*v.y + q.z*v.z),
							q.w*v.x + q.z*v.y - q.y*v.z,
							q.w*v.y + q.x*v.z - q.z*v.x,
							q.w*v.z + q.y*v.x - q.x*v.y);
}

float	StandardQuaternion::Magnitude(void)
{
	return (float) sqrt(w*w + x*x + y*y + z*z);
}

StandardVector3	QVRotate(StandardQuaternion q, StandardVector3 v)
{
	StandardQuaternion t;


	t = q*v*(~q);

	return	t.GetVector();
}

StandardQuaternion QRotate(StandardQuaternion q1, StandardQuaternion q2)
{
	return	q1*q2*(~q1);
}



StandardVector3	StandardQuaternion::GetVector(void)
{
	return StandardVector3(x, y, z);
}

float	StandardQuaternion::GetScalar(void)
{
	return w;
}


StandardQuaternion & StandardQuaternion::operator/=(float s)
{
	w /= s;
	x /= s;
	y /= s;
	z /= s;
	return *this;
}


ostream & operator<<(ostream & os, const StandardQuaternion & m)
{
  os << "[" <<  setw(8) << m.w
     << ", " << setw(8) << m.x 
     << ", " << setw(8) << m.y
     << ", " << setw(8) << m.z
     << "]";
    return os;

}
