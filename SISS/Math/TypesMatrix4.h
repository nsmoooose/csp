#ifndef __STANDARDMATRIX4_H_
#define __STANDARDMATRIX4_H_


// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.


class StandardMatrix4
{
public:

    //            ROW  COL
   float rowcol[4][4];

    // construction
    StandardMatrix4 ();
    StandardMatrix4 (const float rowcol[4][4]);
    StandardMatrix4 (const StandardMatrix4& rkMatrix);
    StandardMatrix4 (const StandardMatrix3& rkMatrix);
    StandardMatrix4 (const float entries[16]);
    StandardMatrix4(float etr1, float etr2, float etr3, float etr4,
                                 float etr5, float etr6, float etr7, float etr8,
                                 float etr9, float etr10, float etr11, float etr12, 
		    float etr13, float etr14, float etr15, float etr16);


    // member access, allows use of construct mat[r][c]
    float* operator[] (int iRow) const;
    StandardVector4 GetColumn (int iCol) const;
    operator float* ();

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
    StandardMatrix4 operator* (float fScalar) const;

    // scalar * matrix
    friend StandardMatrix4 operator* (float fScalar, const StandardMatrix4& rkMatrix);

    // utilities
    StandardMatrix4 Transpose () const;
    bool Inverse (StandardMatrix4& rkInverse, float fTolerance = 1e-06) const;
    StandardMatrix4 Inverse (float fTolerance = 1e-06) const;
    StandardMatrix4 * Inverse ();

    float Determinant () const;
	

    static void TensorProduct (const StandardVector4& rkU, 
			       const StandardVector4& rkV,
			       StandardMatrix4& rkProduct);


    friend StandardMatrix4 TranslationMatrix4( float, float, float );
    friend StandardMatrix4 TranslationMatrix4( StandardVector3 & position);
    friend StandardMatrix4 RotationYMatrix4(  float );
    friend StandardMatrix4 RotationXMatrix4(  float );
    friend StandardMatrix4 RotationZMatrix4(  float );
	friend StandardMatrix4 RotationAxisMatrix4(float angle, float x, float y, float z);
	friend StandardMatrix4 RotationQuatMatrix4( float, float, float, float );
	friend StandardMatrix4 ScaleMatrix4( float, float, float );

    friend StandardMatrix4 OrthoMatrix4(float width, float height, 
                        float nearview, float farview);
	  friend StandardMatrix4 PerspectiveFOVMatrix4(float angle, float aspect, 
		float nearview, float farview);
	  friend StandardMatrix4 FrustumMatrix4(float left, float right, 
		float bottom, float top, float nearview, float farview) ;
    friend StandardMatrix4 PerspectiveMatrix4(float fov, float aspect, float nearz, float farz);

	  friend StandardMatrix4 LookAtMatrix4(StandardVector3 & pLocate,
		                          StandardVector3 & pTarget,
								  StandardVector3 & pUP);


    static const float EPSILON;
    static const StandardMatrix4 ZERO;
    static const StandardMatrix4 IDENTITY;

    void Print(FILE * stream);
    void Print(char *String);

};


#endif
