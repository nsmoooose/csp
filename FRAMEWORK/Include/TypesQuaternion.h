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

#include "TypesMatrix3.h"


class StandardQuaternion
{
public:
    float w, x, y, z;

    // construction and destruction
    StandardQuaternion (float fW = 1.0f, float fX = 0.0f, float fY = 0.0f,
        float fZ = 0.0f);
    StandardQuaternion (const StandardQuaternion& rkQ);

    // conversion between StandardQuaternions, matrices, and angle-axes
    void FromRotationMatrix (const StandardMatrix3& kRot);
    void ToRotationMatrix (StandardMatrix3& kRot) const;
    void FromAngleAxis (const float& rfAngle, const StandardVector3& rkAxis);
    void ToAngleAxis (float& rfAngle, StandardVector3& rkAxis) const;
    void FromAxes (const StandardVector3* akAxis);
    void ToAxes (StandardVector3* akAxis) const;

	StandardQuaternion & operator+=(const StandardQuaternion & q);
	StandardQuaternion & operator/=(float s);

    // arithmetic operations
    StandardQuaternion& operator= (const StandardQuaternion& rkQ);
    StandardQuaternion operator+ (const StandardQuaternion& rkQ) const;
    StandardQuaternion operator- (const StandardQuaternion& rkQ) const;
    StandardQuaternion operator* (const StandardQuaternion& rkQ) const;
    StandardQuaternion operator* (float fScalar) const;
    friend StandardQuaternion operator* (float fScalar,
        const StandardQuaternion& rkQ);
    StandardQuaternion operator- () const;
	StandardQuaternion operator~(void) const { return StandardQuaternion(w, -x, -y, -z);}

    friend StandardQuaternion operator*(StandardQuaternion q, StandardVector3 v);
    friend StandardQuaternion operator*(StandardVector3 v, StandardQuaternion q);


    // functions of a StandardQuaternion
    float Dot (const StandardQuaternion& rkQ) const;  // dot product
    float Norm () const;  // squared-length
    float Magnitude(void);
	StandardVector3	GetVector(void);
	float	GetScalar(void);


    StandardQuaternion Inverse () const;  // apply to non-zero StandardQuaternion
    StandardQuaternion UnitInverse () const;  // apply to unit-length StandardQuaternion
    StandardQuaternion Exp () const;
    StandardQuaternion Log () const;

    // rotation of a vector by a StandardQuaternion
    StandardVector3 operator* (const StandardVector3& rkVector) const;

    // spherical linear interpolation
    static StandardQuaternion Slerp (float fT, const StandardQuaternion& rkP,
        const StandardQuaternion& rkQ);

    static StandardQuaternion SlerpExtraSpins (float fT,
        const StandardQuaternion& rkP, const StandardQuaternion& rkQ,
        int iExtraSpins);

    // setup for spherical quadratic interpolation
    static void Intermediate (const StandardQuaternion& rkQ0,
        const StandardQuaternion& rkQ1, const StandardQuaternion& rkQ2,
        StandardQuaternion& rka, StandardQuaternion& rkB);

    // spherical quadratic interpolation
    static StandardQuaternion Squad (float fT, const StandardQuaternion& rkP,
        const StandardQuaternion& rkA, const StandardQuaternion& rkB,
        const StandardQuaternion& rkQ);

    // special values
    static StandardQuaternion ZERO;
    static StandardQuaternion IDENTITY;

    static StandardVector3	MakeEulerAnglesFromQ(StandardQuaternion q);
    static StandardQuaternion	MakeQFromEulerAngles(float x, float y, float z);

};


StandardVector3	QVRotate(StandardQuaternion q, StandardVector3 v);


#endif



