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
 * @file TypesQuaternion.h
 *
 **/

#ifndef __TYPESQUATERNION_H__
#define __TYPESQUATERNION_H__

// Modified for CSP by Wolverine


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

#include <iostream>

#include "TypesMatrix3.h"


/**
 * class StandardQuaternion
 *
 * @author unknown
 */
class StandardQuaternion
{
public:
    double w, x, y, z;

    // construction and destruction
    StandardQuaternion (double fW = 1.0f, double fX = 0.0f, double fY = 0.0f,
        double fZ = 0.0f);
    StandardQuaternion (const StandardQuaternion& rkQ);

    // conversion between StandardQuaternions, matrices, and angle-axes
    void FromRotationMatrix (const StandardMatrix3& kRot);
    void ToRotationMatrix (StandardMatrix3& kRot) const;
    void FromAngleAxis (const double& rfAngle, const StandardVector3& rkAxis);
    void ToAngleAxis (double& rfAngle, StandardVector3& rkAxis) const;
    void FromAxes (const StandardVector3* akAxis);
    void ToAxes (StandardVector3* akAxis) const;

	StandardQuaternion & operator+=(const StandardQuaternion & q);
	StandardQuaternion & operator/=(double s);

    // arithmetic operations
    StandardQuaternion& operator= (const StandardQuaternion& rkQ);
    
	inline StandardQuaternion operator+ (const StandardQuaternion& rhs) const
	{
	    return StandardQuaternion(w+rhs.w,x+rhs.x,y+rhs.y,z+rhs.z);
	}

	inline StandardQuaternion operator- (const StandardQuaternion& rhs) const
	{
	    return StandardQuaternion(w-rhs.w,x-rhs.x,y-rhs.y,z-rhs.z);
	}

    StandardQuaternion operator* (const StandardQuaternion& rkQ) const;

    inline const StandardQuaternion operator* (const double fScalar) const
	{
	    return StandardQuaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
	}
    
	friend StandardQuaternion operator* (double fScalar, const StandardQuaternion& rkQ);

    StandardQuaternion operator- () const;
	StandardQuaternion operator~ (void) const { return StandardQuaternion(w, -x, -y, -z);}

	// Multiplication of a Quaternion and with a Vector, yielding a quaternion.
    friend StandardQuaternion operator*(StandardQuaternion q, StandardVector3 v);
    friend StandardQuaternion operator*(StandardVector3 v, StandardQuaternion q);


    // functions of a StandardQuaternion
    double Dot (const StandardQuaternion& rkQ) const;  // dot product
    double Norm () const;  // squared-length
    double Magnitude(void) const;
	StandardVector3	GetVector(void);
	double	GetScalar(void);

	StandardQuaternion Bar() const;
    StandardQuaternion Inverse () const;       // apply to non-zero StandardQuaternion
    StandardQuaternion UnitInverse () const;  // apply to unit-length StandardQuaternion
    StandardQuaternion Exp () const;
    StandardQuaternion Log () const;
	


	// Use QVRotate instead.
    // rotation of a vector by a StandardQuaternion
//    StandardVector3 operator* (const StandardVector3& rkVector) const;

    // spherical linear interpolation
    static StandardQuaternion Slerp (double fT, const StandardQuaternion& rkP,
        const StandardQuaternion& rkQ);

    static StandardQuaternion SlerpExtraSpins (double fT,
        const StandardQuaternion& rkP, const StandardQuaternion& rkQ,
        int iExtraSpins);

    // setup for spherical quadratic interpolation
    static void Intermediate (const StandardQuaternion& rkQ0,
        const StandardQuaternion& rkQ1, const StandardQuaternion& rkQ2,
        StandardQuaternion& rka, StandardQuaternion& rkB);

    // spherical quadratic interpolation
    static StandardQuaternion Squad (double fT, const StandardQuaternion& rkP,
        const StandardQuaternion& rkA, const StandardQuaternion& rkB,
        const StandardQuaternion& rkQ);

    // special values
    static const StandardQuaternion ZERO;
    static const StandardQuaternion IDENTITY;

    static StandardVector3	MakeEulerAnglesFromQ(StandardQuaternion q);
    static StandardQuaternion	MakeQFromEulerAngles(double x, double y, double z);

    friend std::ostream & operator<< (std::ostream & os, const StandardQuaternion & m);

};



StandardVector3	QVRotate(StandardQuaternion q, StandardVector3 v);
StandardQuaternion QRotate(StandardQuaternion q1, StandardQuaternion q2);


#endif // __TYPESQUATERNION_H__




