#ifndef STANDARDMATRIX3_H_
#define STANDARDMATRIX3_H_

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

// This code is based on free libraries from Magic Software

class StandardVector3;

#include <stdio.h>
#include <iostream>

using namespace std;

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.


/** \class StandardMatrix3
 */


class StandardMatrix3
{
public:

    float rowcol[3][3];


    // construction
    StandardMatrix3 ();
    StandardMatrix3 (const float rowcol[3][3]);
    StandardMatrix3 (const StandardMatrix3& rkMatrix);
    StandardMatrix3 (float fEntry00, float fEntry01, float fEntry02,
                float fEntry10, float fEntry11, float fEntry12,
                float fEntry20, float fEntry21, float fEntry22);

    // member access, allows use of construct mat[r][c]
    float* operator[] (int iRow) const;
    operator float* ();
    StandardVector3 GetColumn (int iCol) const;

    // assignment and comparison
    StandardMatrix3& operator= (const StandardMatrix3& rkMatrix);
    bool operator== (const StandardMatrix3& rkMatrix) const;
    bool operator!= (const StandardMatrix3& rkMatrix) const;

    //operators with assignment
    StandardMatrix3 & operator+=(const StandardMatrix3 & rkMatrix);
    StandardMatrix3 & operator-=(const StandardMatrix3 & rkMatrix);
    StandardMatrix3 & operator*=(const StandardMatrix3 & rkMatrix);

    // arithmetic operations
    StandardMatrix3 operator+ (const StandardMatrix3& rkMatrix) const;
    StandardMatrix3 operator- (const StandardMatrix3& rkMatrix) const;
    StandardMatrix3 operator* (const StandardMatrix3& rkMatrix) const;
    StandardMatrix3 operator- () const;

    // matrix * vector [3x3 * 3x1 = 3x1]
    friend StandardVector3 operator*(const StandardMatrix3& rkMatrix,
				     const StandardVector3& rkVector);

    // vector * matrix [1x3 * 3x3 = 1x3]
    friend StandardVector3 operator*(const StandardVector3& rkVector,
				     const StandardMatrix3& rkMatrix);



    // matrix * scalar
    StandardMatrix3 operator* (float fScalar) const;

    // scalar * matrix
    friend StandardMatrix3 operator* (float fScalar, const StandardMatrix3& rkMatrix);

    // utilities
    StandardMatrix3 Transpose () const;
    bool Inverse (StandardMatrix3& rkInverse, float fTolerance = 1e-06f) const;
    StandardMatrix3 Inverse (float fTolerance = 1e-06) const;
    float Determinant () const;


    // matrix must be orthonormal
    void ToAxisAngle (StandardVector3& rkAxis, float& rfRadians) const;
    void FromAxisAngle (const StandardVector3& rkAxis, float fRadians);

    // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
    // where yaw is rotation about the Up vector, pitch is rotation about the
    // Right axis, and roll is rotation about the Direction axis.
    bool ToEulerAnglesXYZ (float& rfYAngle, float& rfPAngle,
        float& rfRAngle) const;
    bool ToEulerAnglesXZY (float& rfYAngle, float& rfPAngle,
        float& rfRAngle) const;
    bool ToEulerAnglesYXZ (float& rfYAngle, float& rfPAngle,
        float& rfRAngle) const;
    bool ToEulerAnglesYZX (float& rfYAngle, float& rfPAngle,
        float& rfRAngle) const;
    bool ToEulerAnglesZXY (float& rfYAngle, float& rfPAngle,
        float& rfRAngle) const;
    bool ToEulerAnglesZYX (float& rfYAngle, float& rfPAngle,
        float& rfRAngle) const;
    void FromEulerAnglesXYZ (float fYAngle, float fPAngle, float fRAngle);
    void FromEulerAnglesXZY (float fYAngle, float fPAngle, float fRAngle);
    void FromEulerAnglesYXZ (float fYAngle, float fPAngle, float fRAngle);
    void FromEulerAnglesYZX (float fYAngle, float fPAngle, float fRAngle);
    void FromEulerAnglesZXY (float fYAngle, float fPAngle, float fRAngle);
    void FromEulerAnglesZYX (float fYAngle, float fPAngle, float fRAngle);


    static void TensorProduct (const StandardVector3& rkU, const StandardVector3& rkV,
        StandardMatrix3& rkProduct);

    friend StandardMatrix3 TranslationMatrix3( float x, float y, float z );
    friend StandardMatrix3 RotationXMatrix3( float angle );
    friend StandardMatrix3 RotationYMatrix3( float angle );
    friend StandardMatrix3 RotationZMatrix3( float angle );


    static const float EPSILON;
    static const StandardMatrix3 ZERO;
    static const StandardMatrix3 IDENTITY;

    void Print(FILE * stream);
    void Print(char *String);

    friend ostream & operator<< (ostream & os, const StandardMatrix3 & m);

};

#endif
