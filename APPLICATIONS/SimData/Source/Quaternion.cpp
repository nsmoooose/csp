// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file Quaternion.cpp
 *
 * Initial CSP version by Wolverine69
 * Modified for SimData by Onsight (Jan 2003)
 * Code based on libraries from magic-software.com and Game Programming Gems.
 **/

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


#include <iomanip>
#include <sstream>
#include <cmath>

#include <SimData/Pack.h>
#include <SimData/Math.h>
#include <SimData/Vector3.h>
#include <SimData/Matrix3.h>
#include <SimData/Quaternion.h>


using namespace std;


NAMESPACE_SIMDATA


static double gs_fEpsilon = 1e-03f;
const Quaternion Quaternion::ZERO(0.0f,0.0f,0.0f,0.0f);
const Quaternion Quaternion::IDENTITY(1.0f,0.0f,0.0f,0.0f);

//----------------------------------------------------------------------------

Quaternion::Quaternion (double fW, double fX, double fY, double fZ)
{
	w = fW;
	x = fX;
	y = fY;
	z = fZ;
}

//----------------------------------------------------------------------------

Quaternion::Quaternion (const Quaternion& rkQ)
{
	w = rkQ.w;
	x = rkQ.x;
	y = rkQ.y;
	z = rkQ.z;
}

//----------------------------------------------------------------------------

void Quaternion::FromRotationMatrix (const Matrix3& kRot)
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".

	double fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
	double fRoot;

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
		double* apkQuat[3] = { &x, &y, &z };
		*apkQuat[i] = 0.5f*fRoot;
		fRoot = 0.5f/fRoot;
		w = (kRot[k][j]-kRot[j][k])*fRoot;
		*apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
		*apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
	}
}

//----------------------------------------------------------------------------

void Quaternion::ToRotationMatrix (Matrix3& kRot) const
{
	double fTx  = 2.0f*x;
	double fTy  = 2.0f*y;
	double fTz  = 2.0f*z;
	double fTwx = fTx*w;
	double fTwy = fTy*w;
	double fTwz = fTz*w;
	double fTxx = fTx*x;
	double fTxy = fTy*x;
	double fTxz = fTz*x;
	double fTyy = fTy*y;
	double fTyz = fTz*y;
	double fTzz = fTz*z;

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

void Quaternion::FromAngleAxis (const double& rfAngle, const Vector3& rkAxis)
{
	// assert:  axis[] is unit length
	//
	// The Quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	double fHalfAngle = 0.5f*rfAngle;
	double fSin = sin(fHalfAngle);
	w = cos(fHalfAngle);
	x = fSin*rkAxis.x;
	y = fSin*rkAxis.y;
	z = fSin*rkAxis.z;
}

//----------------------------------------------------------------------------

void Quaternion::ToAngleAxis (double& rfAngle, Vector3& rkAxis) const
{
	// The Quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	double fSqrLength = x*x+y*y+z*z;
	if ( fSqrLength > 0.0f )
	{
		rfAngle = 2.0f*acos(w);
		double fInvLength = 1.0/sqrt(fSqrLength);
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

void Quaternion::FromAxes (const Vector3* akAxis)
{
	Matrix3 kRot;

	for (int iCol = 0; iCol < 3; iCol++)
	{
		kRot[0][iCol] = akAxis[iCol].x;
		kRot[1][iCol] = akAxis[iCol].y;
		kRot[2][iCol] = akAxis[iCol].z;
	}

	FromRotationMatrix(kRot);
}

//----------------------------------------------------------------------------

void Quaternion::ToAxes (Vector3* akAxis) const
{
	Matrix3 kRot;

	ToRotationMatrix(kRot);

	for (int iCol = 0; iCol < 3; iCol++)
	{
		akAxis[iCol].x = kRot[0][iCol];
		akAxis[iCol].y = kRot[1][iCol];
		akAxis[iCol].z = kRot[2][iCol];
	}
}

//----------------------------------------------------------------------------

Quaternion& Quaternion::operator= (const Quaternion& rkQ)
{
	w = rkQ.w;
	x = rkQ.x;
	y = rkQ.y;
	z = rkQ.z;
	return *this;
}

//----------------------------------------------------------------------------

//Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
//{
//    return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
//}

//----------------------------------------------------------------------------

//Quaternion Quaternion::operator- (const Quaternion& rkQ) const
//{
//    return Quaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
//}

//----------------------------------------------------------------------------

Quaternion Quaternion::operator* (const Quaternion& rkQ) const //checked
{
	// NOTE:  Multiplication is not generally commutative, so in most
	// cases p*q != q*p.

	return Quaternion
	(
		w*rkQ.w-x*rkQ.x-y*rkQ.y-z*rkQ.z,
		w*rkQ.x+x*rkQ.w+y*rkQ.z-z*rkQ.y,
		w*rkQ.y+y*rkQ.w+z*rkQ.x-x*rkQ.z,
		w*rkQ.z+z*rkQ.w+x*rkQ.y-y*rkQ.x
	);
}

//----------------------------------------------------------------------------

//Quaternion Quaternion::operator* (double fScalar) const
//{
//    return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
//}

//----------------------------------------------------------------------------

//Quaternion operator* (double fScalar, const Quaternion& rkQ)
//{
//    return Quaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y,
//        fScalar*rkQ.z);
//}

//----------------------------------------------------------------------------

Quaternion Quaternion::operator- () const
{
	return Quaternion(-w,-x,-y,-z);
}

//----------------------------------------------------------------------------

double Quaternion::Dot (const Quaternion& rkQ) const
{
	return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
}

//----------------------------------------------------------------------------

double Quaternion::Norm () const
{
	return w*w+x*x+y*y+z*z;
}

//----------------------------------------------------------------------------

Quaternion Quaternion::Inverse () const //checked (delta)
{
	double fNorm = w*w+x*x+y*y+z*z;
	if ( fNorm > 0.0f )
	{
		double fInvNorm = 1.0f/fNorm;
		return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
	}
	else
	{
		// return an invalid result to flag the error
		return ZERO;
	}
}

//----------------------------------------------------------------------------

Quaternion Quaternion::UnitInverse () const //checked (delta)
{
	// assert:  'this' is unit length
	return Quaternion(w,-x,-y,-z);
}

//----------------------------------------------------------------------------

Quaternion Quaternion::Exp () const
{
	// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
	// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

	double fAngle = sqrt(x*x+y*y+z*z);
	double fSin = sin(fAngle);

	Quaternion kResult;
	kResult.w = cos(fAngle);

	if ( fabs(fSin) >= gs_fEpsilon )
	{
		double fCoeff = fSin/fAngle;
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

Quaternion Quaternion::Log () const
{
	// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
	// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

	Quaternion kResult;
	kResult.w = 0.0f;

	if ( fabs(w) < 1.0f )
	{
		double fAngle = acos(w);
		double fSin = sin(fAngle);
		if ( fabs(fSin) >= gs_fEpsilon )
		{
			double fCoeff = fAngle/fSin;
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
//Vector3 Quaternion::operator* (const Vector3& rkVector) const
//{
    // Given a vector u = (x0,y0,z0) and a unit length Quaternion
    // q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
    // rotation of u by q is v = q*u*q^{-1} where * indicates Quaternion
    // multiplication and where u is treated as the Quaternion <0,x0,y0,z0>.
    // Note that q^{-1} = <w,-x,-y,-z>, so no double work is required to
    // invert q.  Now
    //
    //   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
    //     = q*(x0*i+y0*j+z0*k)*q^{-1}
    //     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
    //
    // As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
    // of the rotation matrix computed in Quaternion::ToRotationMatrix.
    // The vector v is obtained as the product of that rotation matrix with
    // vector u.  As such, the Quaternion representation of a rotation
    // matrix requires less space than the matrix and more time to compute
    // the rotated vector.  Typical space-time tradeoff...

//    Matrix3 kRot;
//    ToRotationMatrix(kRot);
//    return kRot*rkVector;
//}



//----------------------------------------------------------------------------

Quaternion Quaternion::Slerp (double fT, const Quaternion& rkP, const Quaternion& rkQ)
{
	double fCos = rkP.Dot(rkQ);
	double fAngle = acos(fCos);

	if ( fabs(fAngle) < gs_fEpsilon )
		return rkP;

	double fSin = sin(fAngle);
	double fInvSin = 1.0f/fSin;
	double fCoeff0 = sin((1.0f-fT)*fAngle)*fInvSin;
	double fCoeff1 = sin(fT*fAngle)*fInvSin;
	return fCoeff0*rkP + fCoeff1*rkQ;
}

//----------------------------------------------------------------------------

Quaternion Quaternion::SlerpExtraSpins (double fT, const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
{
	double fCos = rkP.Dot(rkQ);
	double fAngle = acos(fCos);

	if ( fabs(fAngle) < gs_fEpsilon )
		return rkP;

	double fSin = sin(fAngle);
	double fPhase = G_PI*iExtraSpins*fT;
	double fInvSin = 1.0f/fSin;
	double fCoeff0 = sin((1.0f-fT)*fAngle - fPhase)*fInvSin;
	double fCoeff1 = sin(fT*fAngle + fPhase)*fInvSin;
	return fCoeff0*rkP + fCoeff1*rkQ;
}

//----------------------------------------------------------------------------

void Quaternion::Intermediate (const Quaternion& rkQ0,
                               const Quaternion& rkQ1,
                               const Quaternion& rkQ2, 
                               Quaternion& rkA,
                               Quaternion& rkB)
{
	// assert:  q0, q1, q2 are unit Quaternions

	Quaternion kQ0inv = rkQ0.UnitInverse();
	Quaternion kQ1inv = rkQ1.UnitInverse();
	Quaternion rkP0 = kQ0inv*rkQ1;
	Quaternion rkP1 = kQ1inv*rkQ2;
	Quaternion kArg = 0.25f*(rkP0.Log()-rkP1.Log());
	Quaternion kMinusArg = -kArg;

	rkA = rkQ1*kArg.Exp();
	rkB = rkQ1*kMinusArg.Exp();
}

//----------------------------------------------------------------------------

Quaternion Quaternion::Squad (double fT, const Quaternion& rkP,
    const Quaternion& rkA, const Quaternion& rkB, const Quaternion& rkQ)
{
    double fSlerpT = 2.0f*fT*(1.0f-fT);
    Quaternion kSlerpP = Slerp(fT,rkP,rkQ);
    Quaternion kSlerpQ = Slerp(fT,rkA,rkB);
    return Slerp(fSlerpT,kSlerpP,kSlerpQ);
}
//----------------------------------------------------------------------------


Quaternion Quaternion::MakeQFromEulerAngles(double x, double y, double z)
{
	Quaternion q;
	double roll = DegreesToRadians(x);
	double pitch = DegreesToRadians(y);
	double yaw = DegreesToRadians(z);
	
	double cyaw, cpitch, croll, syaw, spitch, sroll;
	double cyawcpitch, syawspitch, cyawspitch, syawcpitch;

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

	q.w = (double) (cyawcpitch * croll + syawspitch * sroll);
	q.x = (double) (cyawcpitch * sroll - syawspitch * croll); 
	q.y = (double) (cyawspitch * croll + syawcpitch * sroll);
	q.z = (double) (syawcpitch * croll - cyawspitch * sroll);

	return q;
}

Vector3	Quaternion::MakeEulerAnglesFromQ(Quaternion const& q)
{
	double	r11, r21, r31, r32, r33, r12, r13;
	double	q00, q11, q22, q33;
	double	tmp;
	Vector3	u;

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
		u.y = RadiansToDegrees((double) (-(G_PI/2) * r31/tmp)); // pitch
		u.z = RadiansToDegrees((double) atan2(-r12, -r31*r13)); // yaw
		return u;
	}

	u.x = RadiansToDegrees((double) atan2(r32, r33)); // roll
	u.y = RadiansToDegrees((double) asin(-r31)); // pitch
	u.z = RadiansToDegrees((double) atan2(r21, r11)); // yaw
	return u;
	

}

Quaternion & Quaternion::operator+=(const Quaternion& q)
{
	w += q.w;
	x += q.x;
	y += q.y;
	z += q.z;
	return *this;
}


Quaternion operator*(Quaternion const& q, Vector3 const& v)
{
	return Quaternion( -(q.x*v.x + q.y*v.y + q.z*v.z),
	                     q.w*v.x + q.y*v.z - q.z*v.y,
	                     q.w*v.y + q.z*v.x - q.x*v.z,
	                     q.w*v.z + q.x*v.y - q.y*v.x);
}

Quaternion operator*(Vector3 const& v, Quaternion const& q)
{
	return Quaternion( -(q.x*v.x + q.y*v.y + q.z*v.z),
	                     q.w*v.x + q.z*v.y - q.y*v.z,
	                     q.w*v.y + q.x*v.z - q.z*v.x,
	                     q.w*v.z + q.y*v.x - q.x*v.y);
}

double	Quaternion::Magnitude(void) const
{
	return sqrt(w*w + x*x + y*y + z*z);
}

Vector3	QVRotate(Quaternion const& q, Vector3 const& v)
{
	Quaternion t;
	t = (q*v)*q.Bar();
	return t.GetVector();
}

Quaternion Quaternion::Bar() const
{
	return Quaternion(w,-x,-y,-z);
}

Quaternion QRotate(Quaternion const& q1, Quaternion const& q2)
{
	return q1*q2*(~q1);
}

Vector3	Quaternion::GetVector(void)
{
	return Vector3(x, y, z);
}

double	Quaternion::GetScalar(void)
{
	return w;
}


Quaternion& Quaternion::operator/=(double s) // Checked (delta)
{
	w /= s;
	x /= s;
	y /= s;
	z /= s;
	return *this;
}


ostream& operator<<(ostream& os, Quaternion const& m)
{
	os << "[" <<  setw(8) << m.w << ", " << setw(8) << m.x 
	   << ", " << setw(8) << m.y << ", " << setw(8) << m.z << "]";
	return os;
}

Quaternion operator* (double fScalar, const Quaternion& rhs)
{
	return Quaternion(fScalar*rhs.w,fScalar*rhs.x,fScalar*rhs.y,fScalar*rhs.z);
}


void Quaternion::pack(Packer &p) const {
	p.pack(w);
	p.pack(x);
	p.pack(y);
	p.pack(z);
}

void Quaternion::unpack(UnPacker &p) {
	p.unpack(w);
	p.unpack(x);
	p.unpack(y);
	p.unpack(z);
}

void Quaternion::parseXML(const char* cdata) {
	std::stringstream ss(cdata); 
	std::string token; 
	double v[9];
	for (int i = 0; i < 9; i++) {
		if (!(ss >> token)) {
			if (i == 4) {
				w = v[0];
				x = v[1];
				y = v[2];
				z = v[3];
				return;
			} else {
				throw ParseException("Expect either four (4) or  nine (9) elements in quaternion");
			}
		}
    		v[i] = atof(token.c_str());
	}
	if (ss >> token) {
		throw ParseException("Expect exactly four (4) or nine (9) elements in quaternion");
	}
	FromRotationMatrix(Matrix3(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8]));
}


std::string Quaternion::asString() const {
	std::stringstream repr;
	repr << *this;
	return repr.str();
}

bool operator==(const Quaternion &a, const Quaternion &b) {
	return a.w == b.w && a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator!=(const Quaternion &a, const Quaternion &b) {
	return a.w != b.w || a.x != b.x || a.y != b.y || a.z != b.z;
}


NAMESPACE_END // namespace simdata

