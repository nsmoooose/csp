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
 * @file Matrix3.h
 *
 * Initial CSP version by Wolverine69
 * Code based on libraries from magic-software.com and Game Programming Gems.
 **/

#ifndef __SIMDATA_MATRIX3_H__
#define __SIMDATA_MATRIX3_H__


#include <string>
#include <vector>
#include <cstdio>

#include <SimData/BaseType.h>


//using namespace std;

NAMESPACE_SIMDATA


class Vector3;


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
 * class Matrix3
 */
class SIMDATA_EXPORT Matrix3: public BaseType
{
public:
	typedef double M_t;
	
	// math notation: rowcol[i][j] designates the coefficient on row i and column j
	M_t rowcol[3][3];

	// BaseType interface
	virtual void pack(Packer&) const; 
	virtual void unpack(UnPacker&);
	virtual void parseXML(const char* cdata);

	// construction
	Matrix3();
	Matrix3(const double rowcol[3][3]);
	Matrix3(const Matrix3& rkMatrix);
	Matrix3(double fEntry00, double fEntry01, double fEntry02,
	                double fEntry10, double fEntry11, double fEntry12,
	                double fEntry20, double fEntry21, double fEntry22);
	Matrix3(const Vector3& column1, // a matrix is given by 3 column vectors
	                const Vector3& column2, 
	                const Vector3& column3);

	void SetColumn(int iCol, const Vector3& column);
	Vector3 GetColumn(int iCol) const;
	Vector3 GetRow(int iRow) const;
	std::vector<double> GetElements() const;

	// comparison
	bool operator==(const Matrix3& rkMatrix) const;
	bool operator!=(const Matrix3& rkMatrix) const;

#ifndef SWIG
	// member access, allows use of construct mat[r][c]
	double* operator[](int iRow) const;
	operator double*();

	//operators with assignment
	Matrix3 & operator+=(const Matrix3 & rkMatrix);
	Matrix3 & operator-=(const Matrix3 & rkMatrix);
	Matrix3 & operator*=(const Matrix3 & rkMatrix);
#endif // SWIG

	// arithmetic operations
	Matrix3 operator-() const;
	Matrix3 operator+(const Matrix3& rkMatrix) const;
	Matrix3 operator-(const Matrix3& rkMatrix) const;
#ifndef SWIG
	Matrix3 operator*(const Matrix3& rkMatrix) const;
	Matrix3 operator*(double fScalar) const;
#endif // SWIG

#ifndef SWIG
	// matrix * vector [3x3 * 3x1 = 3x1]
	SIMDATA_EXPORT friend Vector3 operator*(const Matrix3& rkMatrix,
	                                 const Vector3& rkVector);

	// vector * matrix [1x3 * 3x3 = 1x3]
	friend Vector3 operator*(const Vector3& rkVector,
		                 const Matrix3& rkMatrix);
	
	// scalar * matrix
	SIMDATA_EXPORT friend Matrix3 operator*(double fScalar, const Matrix3& rkMatrix);
#endif // SWIG

	// utilities
	Matrix3 Transpose() const;

#ifndef SWIG
	bool Inverse(Matrix3& rkInverse, double fTolerance = 1e-06f) const;
#endif // SWIG

	Matrix3 Inverse(double fTolerance = 1e-06) const;
	double Determinant() const;

	// matrix must be orthonormal
	void ToAxisAngle(Vector3& rkAxis, double& rfRadians) const;
	void FromAxisAngle(const Vector3& rkAxis, double fRadians);

	// The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
	// where yaw is rotation about the Up vector, pitch is rotation about the
	// Right axis, and roll is rotation about the Direction axis.
	bool ToEulerAnglesXYZ(double& rfYAngle, double& rfPAngle,
	                      double& rfRAngle) const;
	bool ToEulerAnglesXZY(double& rfYAngle, double& rfPAngle,
	                      double& rfRAngle) const;
	bool ToEulerAnglesYXZ(double& rfYAngle, double& rfPAngle,
	                      double& rfRAngle) const;
	bool ToEulerAnglesYZX(double& rfYAngle, double& rfPAngle,
	                      double& rfRAngle) const;
	bool ToEulerAnglesZXY(double& rfYAngle, double& rfPAngle,
	                      double& rfRAngle) const;
	bool ToEulerAnglesZYX(double& rfYAngle, double& rfPAngle,
	                      double& rfRAngle) const;
	void FromEulerAnglesXYZ(double fYAngle, double fPAngle, double fRAngle);
	void FromEulerAnglesXZY(double fYAngle, double fPAngle, double fRAngle);
	void FromEulerAnglesYXZ(double fYAngle, double fPAngle, double fRAngle);
	void FromEulerAnglesYZX(double fYAngle, double fPAngle, double fRAngle);
	void FromEulerAnglesZXY(double fYAngle, double fPAngle, double fRAngle);
	void FromEulerAnglesZYX(double fYAngle, double fPAngle, double fRAngle);


	static void TensorProduct(const Vector3& rkU, const Vector3& rkV,
	                          Matrix3& rkProduct);

#ifndef SWIG
//	friend Matrix3 TranslationMatrix3( double x, double y, double z );
	friend Matrix3 RotationXMatrix3( double angle );
	friend Matrix3 RotationYMatrix3( double angle );
	friend Matrix3 RotationZMatrix3( double angle );
#endif // SWIG

#ifndef EPSILON
	static const double EPSILON;
#endif
	static const Matrix3 ZERO;
	static const Matrix3 IDENTITY;

	void Print(FILE * stream);
	void Print(char *String);

	virtual std::string asString() const;

#ifndef SWIG
	friend std::ostream & operator<< (std::ostream & os, const Matrix3 & m);
#endif // SWIG
	
	// explicit operators for Python
#ifdef SWIG
	%extend {
		bool __eq__(const Matrix3 & a) { return *self == a; }
		bool __ne__(const Matrix3 & a) { return *self != a; }
		Matrix3 __mul__(const Matrix3& m) const { return (*self)*m; } 
		Matrix3 __mul__(double v) const { return (*self)*v; }
		Matrix3 __rmul__(double v) const { return (*self)*v; }
		Vector3 __mul__(const Vector3& v) const { return (*self)*v; }
	}
#endif // SWIG 

#ifdef SWIG 
%insert("shadow") %{
%}
#endif // SWIG

};


NAMESPACE_END // namespace simdata

#endif // __SIMDATA_MATRIX3_H__

