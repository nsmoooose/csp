#ifndef STANDARDMATRIX3_H_
#define STANDARDMATRIX3_H_

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

// This code is based on free libraries from Magic Software

class StandardVector3;

//using namespace std;

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


/** 
 * class StandardMatrix3
 *
 * @author Wolverine
 */
class StandardMatrix3
{
public:
    // math notation: rowcol[i][j] designs the coefficient on row i and column j
    double rowcol[3][3];

    // construction
    StandardMatrix3 ();
    StandardMatrix3 (const double rowcol[3][3]);
    StandardMatrix3 (const StandardMatrix3& rkMatrix);
    StandardMatrix3 (double fEntry00, double fEntry01, double fEntry02,
                     double fEntry10, double fEntry11, double fEntry12,
                     double fEntry20, double fEntry21, double fEntry22);
    StandardMatrix3 (const StandardVector3& column1, // a matrix is given by 3 column vectors
		             const StandardVector3& column2, 
					 const StandardVector3& column3);

    // member access, allows use of construct mat[r][c]
    double* operator[] (int iRow) const;
    operator double* ();
	void SetColumn (int iCol, const StandardVector3& column);
    StandardVector3 GetColumn (int iCol) const;


    // comparison
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
   // friend StandardVector3 operator*(const StandardVector3& rkVector,
   //				     const StandardMatrix3& rkMatrix);



    // matrix * scalar
    StandardMatrix3 operator* (double fScalar) const;

    // scalar * matrix
    friend StandardMatrix3 operator* (double fScalar, const StandardMatrix3& rkMatrix);

    // utilities
    StandardMatrix3 Transpose () const;
    bool Inverse (StandardMatrix3& rkInverse, double fTolerance = 1e-06f) const;
    StandardMatrix3 Inverse (double fTolerance = 1e-06) const;
    double Determinant () const;


    // matrix must be orthonormal
    void ToAxisAngle (StandardVector3& rkAxis, double& rfRadians) const;
    void FromAxisAngle (const StandardVector3& rkAxis, double fRadians);

    // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
    // where yaw is rotation about the Up vector, pitch is rotation about the
    // Right axis, and roll is rotation about the Direction axis.
    bool ToEulerAnglesXYZ (double& rfYAngle, double& rfPAngle,
        double& rfRAngle) const;
    bool ToEulerAnglesXZY (double& rfYAngle, double& rfPAngle,
        double& rfRAngle) const;
    bool ToEulerAnglesYXZ (double& rfYAngle, double& rfPAngle,
        double& rfRAngle) const;
    bool ToEulerAnglesYZX (double& rfYAngle, double& rfPAngle,
        double& rfRAngle) const;
    bool ToEulerAnglesZXY (double& rfYAngle, double& rfPAngle,
        double& rfRAngle) const;
    bool ToEulerAnglesZYX (double& rfYAngle, double& rfPAngle,
        double& rfRAngle) const;
    void FromEulerAnglesXYZ (double fYAngle, double fPAngle, double fRAngle);
    void FromEulerAnglesXZY (double fYAngle, double fPAngle, double fRAngle);
    void FromEulerAnglesYXZ (double fYAngle, double fPAngle, double fRAngle);
    void FromEulerAnglesYZX (double fYAngle, double fPAngle, double fRAngle);
    void FromEulerAnglesZXY (double fYAngle, double fPAngle, double fRAngle);
    void FromEulerAnglesZYX (double fYAngle, double fPAngle, double fRAngle);


    static void TensorProduct (const StandardVector3& rkU, const StandardVector3& rkV,
        StandardMatrix3& rkProduct);

    //friend StandardMatrix3 TranslationMatrix3( double x, double y, double z );
    friend StandardMatrix3 RotationXMatrix3( double angle );
    friend StandardMatrix3 RotationYMatrix3( double angle );
    friend StandardMatrix3 RotationZMatrix3( double angle );



#ifndef EPSILON
    static const double EPSILON;
#endif
    static const StandardMatrix3 ZERO;
    static const StandardMatrix3 IDENTITY;

    void Print(FILE * stream);
    void Print(char *String);

	friend std::ostream & operator<< (std::ostream & os, const StandardMatrix3 & m);

};

#endif
