#ifndef __STANDARDMATRIX4_H_
#define __STANDARDMATRIX4_H_


// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.


/**
 * Class StandardMatrix4
 *
 * @author Wolverine
 */
class StandardMatrix4
{
public:

    //            ROW  COL
   double rowcol[4][4];

    // construction
    StandardMatrix4 ();
    StandardMatrix4 (const double rowcol[4][4]);
    StandardMatrix4 (const StandardMatrix4& rkMatrix);
    StandardMatrix4 (const StandardMatrix3& rkMatrix);
    StandardMatrix4 (const double entries[16]);
    StandardMatrix4(double etr1, double etr2, double etr3, double etr4,
                                 double etr5, double etr6, double etr7, double etr8,
                                 double etr9, double etr10, double etr11, double etr12, 
		    double etr13, double etr14, double etr15, double etr16);


    // member access, allows use of construct mat[r][c]
    double* operator[] (int iRow) const;
    StandardVector4 GetColumn (int iCol) const;
    operator double* ();

    // assignment and comparison
    StandardMatrix4& operator= (const StandardMatrix4& rkMatrix);
    bool operator== (const StandardMatrix4& rkMatrix) const;
    bool operator!= (const StandardMatrix4& rkMatrix) const;
    bool operator== (const StandardMatrix3& rkMatrix) const;
    bool operator!= (const StandardMatrix3& rkMatrix) const;


    //operators with assignment
    StandardMatrix4 & operator+=(const StandardMatrix4 & rkMatrix);
    StandardMatrix4 & operator-=(const StandardMatrix4 & rkMatrix);
    StandardMatrix4 & operator*=(const StandardMatrix4 & rkMatrix);


    // arithmetic operations
    StandardMatrix4 operator+ (const StandardMatrix4& rkMatrix) const;
    StandardMatrix4 operator- (const StandardMatrix4& rkMatrix) const;
    StandardMatrix4 operator* (const StandardMatrix4& rkMatrix) const;
    StandardMatrix4 operator- () const;

    // matrix * vector [4x4 * 4x1 = 4x1]
    friend StandardVector4 operator* (const StandardMatrix4& rkMatrix,
				      const StandardVector4& rkVector);

    // vector * matrix [1x4 * 4x4 = 1x4]
    friend StandardVector4 operator* (const StandardVector4& rkVector,
				      const StandardMatrix4& rkMatrix);

    // matrix * scalar
    StandardMatrix4 operator* (double fScalar) const;

    // scalar * matrix
    friend StandardMatrix4 operator* (double fScalar, const StandardMatrix4& rkMatrix);

    // utilities
    StandardMatrix4 Transpose () const;
    bool Inverse (StandardMatrix4& rkInverse, double fTolerance = 1e-06) const;
    StandardMatrix4 Inverse (double fTolerance = 1e-06) const;
    StandardMatrix4 * Inverse ();

    double Determinant () const;
	

    static void TensorProduct (const StandardVector4& rkU, 
			       const StandardVector4& rkV,
			       StandardMatrix4& rkProduct);


    friend StandardMatrix4 TranslationMatrix4( double, double, double );
    friend StandardMatrix4 TranslationMatrix4( StandardVector3 & position);
    friend StandardMatrix4 RotationYMatrix4(  double );
    friend StandardMatrix4 RotationXMatrix4(  double );
    friend StandardMatrix4 RotationZMatrix4(  double );
	friend StandardMatrix4 RotationAxisMatrix4(double angle, double x, double y, double z);
	friend StandardMatrix4 RotationQuatMatrix4( double, double, double, double );
	friend StandardMatrix4 ScaleMatrix4( double, double, double );

    friend StandardMatrix4 OrthoMatrix4(double width, double height, 
                        double nearview, double farview);
	  friend StandardMatrix4 PerspectiveFOVMatrix4(double angle, double aspect, 
		double nearview, double farview);
	  friend StandardMatrix4 FrustumMatrix4(double left, double right, 
		double bottom, double top, double nearview, double farview) ;
    friend StandardMatrix4 PerspectiveMatrix4(double fov, double aspect, double nearz, double farz);

	  friend StandardMatrix4 LookAtMatrix4(StandardVector3 & pLocate,
		                          StandardVector3 & pTarget,
								  StandardVector3 & pUP);


#ifndef EPSILON
    static const double EPSILON;
#endif
    static const StandardMatrix4 ZERO;
    static const StandardMatrix4 IDENTITY;

    void Print(FILE * stream);
    void Print(char *String);

};


#endif
