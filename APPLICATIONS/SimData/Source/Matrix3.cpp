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
 * @file Matrix3.cpp
 *
 * Initial CSP version by Wolverine69
 * Code based on libraries from magic-software.com and Game Programming Gems.
 **/



#include <iomanip>
#include <sstream>
#include <cmath>

#define HALF_PI (G_PI*0.5)

#include <SimData/Matrix3.h>
#include <SimData/Vector3.h>
#include <SimData/Pack.h>


NAMESPACE_SIMDATA


#ifndef EPSILON
   const double Matrix3::EPSILON = 1e-06;
#endif


const Matrix3 Matrix3::ZERO(0,0,0,0,0,0,0,0,0);
const Matrix3 Matrix3::IDENTITY(1,0,0,0,1,0,0,0,1);

//const double Matrix3::ms_fSvdEpsilon = 1e-04;
//const int Matrix3::ms_iSvdMaxIterations = 32;
  

Matrix3::Matrix3()
{
	// For efficiency reasons, do not initialize matrix.
}

Matrix3::Matrix3(const double aafEntry[3][3])
{
	memcpy(rowcol,aafEntry,9*sizeof(double));
}

Matrix3::Matrix3(const Matrix3& rkMatrix)
{
	memcpy(rowcol,rkMatrix.rowcol,9*sizeof(double));
}

Matrix3::Matrix3(double fEntry00, double fEntry01, double fEntry02,
                 double fEntry10, double fEntry11, double fEntry12, 
                 double fEntry20, double fEntry21, double fEntry22)
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

Matrix3::Matrix3(const Vector3& column1, // a matrix is given by 3 column vectors
                 const Vector3& column2, 
                 const Vector3& column3)
{
	SetColumn(0, column1);
	SetColumn(1, column2);
	SetColumn(2, column3);
}

double* Matrix3::operator[](int iRow) const
{
	return (double*)&rowcol[iRow][0];
}

Matrix3::operator double*()
{
	return &rowcol[0][0];
}

Vector3 Matrix3::GetRow(int iRow) const // Checked
{
	assert( 0 <= iRow && iRow < 3 );
	return Vector3(rowcol[iRow][0], rowcol[iRow][1], rowcol[iRow][2]);
}

Vector3 Matrix3::GetColumn(int iCol) const // Checked
{
	assert( 0 <= iCol && iCol < 3 );
	return Vector3(rowcol[0][iCol], rowcol[1][iCol], rowcol[2][iCol]);
}

std::vector<double> Matrix3::GetElements() const // Checkd
{
	std::vector<double> m;
	m.reserve(9);
	for (unsigned short iRow = 0; iRow < 3; ++iRow) {
		for (unsigned short iCol = 0; iCol < 3; ++iCol) {
	    		m.push_back(rowcol[iRow][iCol]);
		}
	}
	return m;
}

void Matrix3::SetColumn(int iCol, const Vector3& column) // Checked
{
	assert( 0 <= iCol && iCol < 3 );
	for (unsigned short iRow = 0; iRow < 3; ++iRow) {
		rowcol[iRow][iCol] = column[iRow];
	}
}

bool Matrix3::operator==(const Matrix3& rkMatrix) const // Checked
{
	for (unsigned short iRow = 0; iRow < 3; ++iRow) {
		for (unsigned short iCol = 0; iCol < 3; ++iCol) {
	    		if ( rowcol[iRow][iCol] != rkMatrix.rowcol[iRow][iCol] ) {
				return false;
			}
		}
	}
	return true;
}

bool Matrix3::operator!=(const Matrix3& rkMatrix) const // Checked
{
	return !operator==(rkMatrix);
}

//operators with assignment

Matrix3 & Matrix3::operator+=(const Matrix3 & rkMatrix) // Checked
{
	for (unsigned short iRow = 0; iRow < 3; ++iRow) {
		for (int iCol = 0; iCol < 3; ++iCol) {
			rowcol[iRow][iCol] += rkMatrix.rowcol[iRow][iCol];
		}
	}
	return *this;
}

Matrix3 & Matrix3::operator-=(const Matrix3 & rkMatrix)
{
	for (int iRow = 0; iRow < 3; iRow++) {
		for (int iCol = 0; iCol < 3; iCol++) {
			rowcol[iRow][iCol] -= rkMatrix.rowcol[iRow][iCol];
		}
	}
	return *this;
}

// this = this * rkMatrix
Matrix3 & Matrix3::operator*=(const Matrix3 & rkMatrix)
{
	Matrix3 t;
	for (int iRow = 0; iRow < 3; iRow++) {
		for (int iCol = 0; iCol < 3; iCol++) {
			t[iRow][iCol] =
				rowcol[iRow][0]*rkMatrix.rowcol[0][iCol] +
				rowcol[iRow][1]*rkMatrix.rowcol[1][iCol] +
				rowcol[iRow][2]*rkMatrix.rowcol[2][iCol];
		}
	}
	*this = t;
	return *this;
}

Matrix3 Matrix3::operator+(const Matrix3& rkMatrix) const
{
	Matrix3 kSum;
	for (int iRow = 0; iRow < 3; iRow++) {
		for (int iCol = 0; iCol < 3; iCol++) {
			kSum.rowcol[iRow][iCol] = rowcol[iRow][iCol] +
			                          rkMatrix.rowcol[iRow][iCol];
		}
	}
	return kSum;
}

Matrix3 Matrix3::operator-(const Matrix3& rkMatrix) const
{
	Matrix3 kDiff;
	for (int iRow = 0; iRow < 3; iRow++) {
		for (int iCol = 0; iCol < 3; iCol++) {
			kDiff.rowcol[iRow][iCol] = rowcol[iRow][iCol] -
			                           rkMatrix.rowcol[iRow][iCol];
		}
	}
	return kDiff;
}

Matrix3 Matrix3::operator*(const Matrix3& rkMatrix) const
{
	Matrix3 kProd;
	for (int iRow = 0; iRow < 3; iRow++) {
		for (int iCol = 0; iCol < 3; iCol++) {
			kProd.rowcol[iRow][iCol] =
				rowcol[iRow][0]*rkMatrix.rowcol[0][iCol] +
				rowcol[iRow][1]*rkMatrix.rowcol[1][iCol] +
				rowcol[iRow][2]*rkMatrix.rowcol[2][iCol];
		}
	}
	return kProd;
}

// multiplication (proxy) operators for Python

/*
Matrix3 Matrix3::__mul__(const Matrix3& m) const {
	return (*this)*m;
}

Matrix3 Matrix3::__mul__(double v) const {
	return (*this)*v;
}

Matrix3 Matrix3::__rmul__(double v) const {
	return (*this)*v;
}

Vector3 Matrix3::__mul__(const Vector3& v) const {
	return (*this)*v;
}
*/


/*
Vector3 operator*(const Matrix3& rkMatrix,
                          const Vector3& rkPoint) // Checked
{
	Vector3 kProd;
	for (int iRow = 0; iRow < 3; ++iRow) {
		kProd[iRow] =
			rkMatrix.rowcol[iRow][0]*rkPoint.x +
			rkMatrix.rowcol[iRow][1]*rkPoint.y +
			rkMatrix.rowcol[iRow][2]*rkPoint.z;
	}
	return kProd;
}


Vector3 operator* (const Vector3& rkPoint, const Matrix3& rkMatrix)
{
	Vector3 kProd;
	for (int iCol = 0; iCol < 3; iCol++)
	{
		kProd[iCol] =
			rkPoint.x*rkMatrix.rowcol[0][iCol] +
			rkPoint.y*rkMatrix.rowcol[1][iCol] +
			rkPoint.z*rkMatrix.rowcol[2][iCol];
	}
	return kProd;
}
*/

Vector3 operator*(Matrix3 const & rkMatrix, Vector3 const & rkPoint) 
{
	double const *row0 = &(rkMatrix.rowcol[0][0]);
	double const *row1 = &(rkMatrix.rowcol[1][0]);
	double const *row2 = &(rkMatrix.rowcol[2][0]);
	double x, y, z;
	x  = rkPoint.x * *row0++;
	x += rkPoint.y * *row0++;
	x += rkPoint.z * *row0++;
	y  = rkPoint.x * *row1++; 
	y += rkPoint.y * *row1++;
	y += rkPoint.z * *row1++;
	z  = rkPoint.x * *row2++; 
	z += rkPoint.y * *row2++;
	z += rkPoint.z * *row2++;
	return Vector3(x, y, z);
}


Vector3 operator* (Vector3 const & rkPoint, Matrix3 const & rkMatrix)
{
	double const *row0 = &(rkMatrix.rowcol[0][0]);
	double const *row1 = &(rkMatrix.rowcol[1][0]);
	double const *row2 = &(rkMatrix.rowcol[2][0]);
	double x, y, z;
	x = rkPoint.x * *row0++ + rkPoint.y * *row1++ + rkPoint.z * *row2++;
	y = rkPoint.x * *row0++ + rkPoint.y * *row1++ + rkPoint.z * *row2++;
	z = rkPoint.x * *row0++ + rkPoint.y * *row1++ + rkPoint.z * *row2++;
	return Vector3(x, y, z);
}


Matrix3 Matrix3::operator-() const
{
	Matrix3 kNeg;
	for (int iRow = 0; iRow < 3; iRow++) {
		for (int iCol = 0; iCol < 3; iCol++) {
			kNeg[iRow][iCol] = -rowcol[iRow][iCol];
		}
	}
	return kNeg;
}

Matrix3 Matrix3::operator*(double fScalar) const
{
	Matrix3 kProd;
	for (int iRow = 0; iRow < 3; iRow++) {
		for (int iCol = 0; iCol < 3; iCol++) {
			kProd[iRow][iCol] = fScalar*rowcol[iRow][iCol];
		}
	}
	return kProd;
}

Matrix3 operator*(double fScalar, const Matrix3& rkMatrix)
{
	Matrix3 kProd;
	for (int iRow = 0; iRow < 3; iRow++) {
		for (int iCol = 0; iCol < 3; iCol++) {
			kProd[iRow][iCol] = fScalar*rkMatrix.rowcol[iRow][iCol];
		}
	}
	return kProd;
}

Matrix3 Matrix3::Transpose() const 
{
	Matrix3 kTranspose;
	for (int iRow = 0; iRow < 3; ++iRow) {
		for (int iCol = 0; iCol < 3; ++iCol) {
			kTranspose[iRow][iCol] = rowcol[iCol][iRow];
		}
	}
	return kTranspose;
}

bool Matrix3::Inverse(Matrix3& rkInverse, double fTolerance) const // Checked
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
	double fDet =
		rowcol[0][0]*rkInverse[0][0] +
		rowcol[0][1]*rkInverse[1][0] +
		rowcol[0][2]*rkInverse[2][0];
	if ( fabs(fDet) <= fTolerance ) {
		return false;
	}
	double fInvDet = 1.0f/fDet;
	for (int iRow = 0; iRow < 3; iRow++) {
		for (int iCol = 0; iCol < 3; iCol++) {
			rkInverse[iRow][iCol] *= fInvDet;
		}
	}
	return true;
}

Matrix3 Matrix3::Inverse(double fTolerance) const
{
	Matrix3 kInverse = Matrix3::ZERO;
	Inverse(kInverse,fTolerance);
	return kInverse;
}

double Matrix3::Determinant() const // Checked
{
	double fCofactor00 = rowcol[1][1]*rowcol[2][2] -
	                     rowcol[1][2]*rowcol[2][1];
	double fCofactor01 = rowcol[1][2]*rowcol[2][0] -
	                     rowcol[1][0]*rowcol[2][2];
	double fCofactor02 = rowcol[1][0]*rowcol[2][1] -
	                     rowcol[1][1]*rowcol[2][0];
	double fDet =
		rowcol[0][0]*fCofactor00 +
		rowcol[0][1]*fCofactor01 +
		rowcol[0][2]*fCofactor02;
	return fDet;
}


void Matrix3::ToAxisAngle(Vector3& rkAxis, double& rfRadians) const
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

	double fTrace = rowcol[0][0] + rowcol[1][1] + rowcol[2][2];
	double fCos = 0.5f*(fTrace-1.0f);
	rfRadians = (double)acos(fCos);  // in [0,PI]

	if ( rfRadians > 0.0 ) {
		if ( rfRadians < G_PI ) {
			rkAxis.x = rowcol[2][1]-rowcol[1][2];
			rkAxis.y = rowcol[0][2]-rowcol[2][0];
			rkAxis.z = rowcol[1][0]-rowcol[0][1];
			rkAxis.Unitize();
		}
		else {
		// angle is PI
			double fHalfInverse;
			if ( rowcol[0][0] >= rowcol[1][1] ) {
				// r00 >= r11
				if ( rowcol[0][0] >= rowcol[2][2] ) {
					// r00 is maximum diagonal term
					rkAxis.x = 0.5f*(double)sqrt(rowcol[0][0] -
					rowcol[1][1] - rowcol[2][2] + 1.0f);
					fHalfInverse = 0.5f/rkAxis.x;
					rkAxis.y = fHalfInverse*rowcol[0][1];
					rkAxis.z = fHalfInverse*rowcol[0][2];
				}
				else {
					// r22 is maximum diagonal term
					rkAxis.z = 0.5f*(double)sqrt(rowcol[2][2] -
					rowcol[0][0] - rowcol[1][1] + 1.0f);
					fHalfInverse = 0.5f/rkAxis.z;
					rkAxis.x = fHalfInverse*rowcol[0][2];
					rkAxis.y = fHalfInverse*rowcol[1][2];
				}
			}
			else {
				// r11 > r00
				if ( rowcol[1][1] >= rowcol[2][2] ) {
					// r11 is maximum diagonal term
					rkAxis.y = 0.5f*(double)sqrt(rowcol[1][1] -
					rowcol[0][0] - rowcol[2][2] + 1.0f);
					fHalfInverse  = 0.5f/rkAxis.y;
					rkAxis.x = fHalfInverse*rowcol[0][1];
					rkAxis.z = fHalfInverse*rowcol[1][2];
				}
				else {
					// r22 is maximum diagonal term
					rkAxis.z = 0.5f*(double)sqrt(rowcol[2][2] -
					rowcol[0][0] - rowcol[1][1] + 1.0f);
					fHalfInverse = 0.5f/rkAxis.z;
					rkAxis.x = fHalfInverse*rowcol[0][2];
					rkAxis.y = fHalfInverse*rowcol[1][2];
				}
			}
		}
	}
	else {
		// The angle is 0 and the matrix is the identity.  Any axis will
		// work, so just use the x-axis.
		rkAxis.x = 1.0;
		rkAxis.y = 0.0;
		rkAxis.z = 0.0;
	}
}

void Matrix3::FromAxisAngle(const Vector3& rkAxis, double fRadians)
{
	double fCos = (double)cos(fRadians);
	double fSin = (double)sin(fRadians);
	double fOneMinusCos = 1.0f-fCos;
	double fX2 = rkAxis.x*rkAxis.x;
	double fY2 = rkAxis.y*rkAxis.y;
	double fZ2 = rkAxis.z*rkAxis.z;
	double fXYM = rkAxis.x*rkAxis.y*fOneMinusCos;
	double fXZM = rkAxis.x*rkAxis.z*fOneMinusCos;
	double fYZM = rkAxis.y*rkAxis.z*fOneMinusCos;
	double fXSin = rkAxis.x*fSin;
	double fYSin = rkAxis.y*fSin;
	double fZSin = rkAxis.z*fSin;

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

bool Matrix3::ToEulerAnglesXYZ(double& rfXAngle, 
                                       double& rfYAngle,
                                       double& rfZAngle) const
{
	// rot =  cy*cz          -cy*sz           sy
	//        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
	//       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy
	if ( rowcol[0][2] < 1.0 ) {
		if ( rowcol[0][2] > -1.0 ) {
			rfXAngle = (double)atan2(-rowcol[1][2],rowcol[2][2]);
			rfYAngle = (double)asin(rowcol[0][2]);
			rfZAngle = (double)atan2(-rowcol[0][1],rowcol[0][0]);
			return true;
		}
		else {
			// WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
			rfXAngle = (double)-atan2(rowcol[1][0],rowcol[1][1]);
			rfYAngle = (double)-HALF_PI;
			rfZAngle = 0.0f;
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
		rfXAngle = (double)atan2(rowcol[1][0],rowcol[1][1]);
		rfYAngle = (double)HALF_PI;
		rfZAngle = 0.0f;
		return false;
	}
}

bool Matrix3::ToEulerAnglesXZY(double& rfXAngle, 
                                       double& rfZAngle,
                                       double& rfYAngle) const
{
	// rot =  cy*cz          -sz              cz*sy
	//        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
	//       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz
	if ( rowcol[0][1] < 1.0 ) {
		if ( rowcol[0][1] > -1.0 ) {
			rfXAngle = (double)atan2(rowcol[2][1],rowcol[1][1]);
			rfZAngle = (double)asin(-rowcol[0][1]);
			rfYAngle = (double)atan2(rowcol[0][2],rowcol[0][0]);
			return true;
		}
		else {
			// WARNING.  Not unique.  XA - YA = atan2(r20,r22)
			rfXAngle = (double)atan2(rowcol[2][0],rowcol[2][2]);
			rfZAngle = (double)HALF_PI;
			rfYAngle = 0.0f;
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
		rfXAngle = (double)atan2(-rowcol[2][0],rowcol[2][2]);
		rfZAngle = (double)-HALF_PI;
		rfYAngle = 0.0f;
		return false;
	}
}

bool Matrix3::ToEulerAnglesYXZ(double& rfYAngle, 
                                       double& rfXAngle,
                                       double& rfZAngle) const
{
	// rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
	//        cx*sz           cx*cz          -sx
	//       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy
	if ( rowcol[1][2] < 1.0 ) {
		if ( rowcol[1][2] > -1.0 ) {
			rfYAngle = (double)atan2(rowcol[0][2],rowcol[2][2]);
			rfXAngle = (double)asin(-rowcol[1][2]);
			rfZAngle = (double)atan2(rowcol[1][0],rowcol[1][1]);
			return true;
		}
		else {
			// WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
			rfYAngle = (double)atan2(rowcol[0][1],rowcol[0][0]);
			rfXAngle = (double)HALF_PI;
			rfZAngle = 0.0f;
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
		rfYAngle = (double)atan2(-rowcol[0][1],rowcol[0][0]);
		rfXAngle = (double)-HALF_PI;
		rfZAngle = 0.0f;
		return false;
	}
}


bool Matrix3::ToEulerAnglesYZX(double& rfYAngle, 
                                       double& rfZAngle,
                                       double& rfXAngle) const
{
	// rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
	//        sz              cx*cz          -cz*sx
	//       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz
	if ( rowcol[1][0] < 1.0 ) {
		if ( rowcol[1][0] > -1.0 ) {
			rfYAngle = (double)atan2(-rowcol[2][0],rowcol[0][0]);
			rfZAngle = (double) asin(rowcol[1][0]);
			rfXAngle = (double)atan2(-rowcol[1][2],rowcol[1][1]);
			return true;
		}
		else {
			// WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
			rfYAngle = (double)-atan2(rowcol[2][1],rowcol[2][2]);
			rfZAngle = (double)-HALF_PI;
			rfXAngle = 0.0f;
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  YA + XA = atan2(r21,r22)
		rfYAngle = (double)atan2(rowcol[2][1],rowcol[2][2]);
		rfZAngle = (double)HALF_PI;
		rfXAngle = 0.0f;
		return false;
	}
}

bool Matrix3::ToEulerAnglesZXY(double& rfZAngle, 
                                       double& rfXAngle,
                                       double& rfYAngle) const
{
	// rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
	//        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
	//       -cx*sy           sx              cx*cy
	if ( rowcol[2][1] < 1.0 ) {
		if ( rowcol[2][1] > -1.0 ) {
			rfZAngle = (double)atan2(-rowcol[0][1],rowcol[1][1]);
			rfXAngle = (double) asin(rowcol[2][1]);
			rfYAngle = (double)atan2(-rowcol[2][0],rowcol[2][2]);
			return true;
		}
		else {
			// WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
			rfZAngle = (double)-atan2(rowcol[0][2],rowcol[0][0]);
			rfXAngle = (double)-HALF_PI;
			rfYAngle = 0.0f;
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
		rfZAngle = (double)atan2(rowcol[0][2],rowcol[0][0]);
		rfXAngle = (double)HALF_PI;
		rfYAngle = 0.0f;
		return false;
	}
}

bool Matrix3::ToEulerAnglesZYX(double& rfZAngle, 
                                       double& rfYAngle,
                                       double& rfXAngle) const
{
	// rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
	//        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
	//       -sy              cy*sx           cx*cy
	if ( rowcol[2][0] < 1.0 ) {
		if ( rowcol[2][0] > -1.0 ) {
			rfZAngle = (double)atan2(rowcol[1][0],rowcol[0][0]);
			rfYAngle = (double) asin(-rowcol[2][0]);
			rfXAngle = (double)atan2(rowcol[2][1],rowcol[2][2]);
			return true;
		}
		else {
			// WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
			rfZAngle = (double)-atan2(rowcol[0][1],rowcol[0][2]);
			rfYAngle = (double)HALF_PI;
			rfXAngle = 0.0f;
			return false;
		}
	}
	else {
		// WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
		rfZAngle = (double)atan2(-rowcol[0][1],-rowcol[0][2]);
		rfYAngle = (double)-HALF_PI;
		rfXAngle = 0.0f;
		return false;
	}
}

void Matrix3::FromEulerAnglesXYZ(double fYAngle, 
                                         double fPAngle,
                                         double fRAngle)
{
	double fCos = (double)cos(fYAngle);
	double fSin = (double)sin(fYAngle);
	Matrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

	fCos = (double)cos(fPAngle);
	fSin = (double)sin(fPAngle);
	Matrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

	fCos = (double)cos(fRAngle);
	fSin = (double)sin(fRAngle);
	Matrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

	*this = kXMat*(kYMat*kZMat);
}

void Matrix3::FromEulerAnglesXZY(double fYAngle, 
                                         double fPAngle,
                                         double fRAngle)
{
	double fCos, fSin;

	fCos = (double)cos(fYAngle);
	fSin = (double)sin(fYAngle);
	Matrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

	fCos = (double)cos(fPAngle);
	fSin = (double)sin(fPAngle);
	Matrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

	fCos = (double)cos(fRAngle);
	fSin = (double)sin(fRAngle);
	Matrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

	*this = kXMat*(kZMat*kYMat);
}

void Matrix3::FromEulerAnglesYXZ(double fYAngle, 
                                         double fPAngle,
                                         double fRAngle)
{
	double fCos, fSin;

	fCos = (double)cos(fYAngle);
	fSin = (double)sin(fYAngle);
	Matrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

	fCos = (double)cos(fPAngle);
	fSin = (double)sin(fPAngle);
	Matrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

	fCos = (double)cos(fRAngle);
	fSin = (double)sin(fRAngle);
	Matrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

	*this = kYMat*(kXMat*kZMat);
}

void Matrix3::FromEulerAnglesYZX(double fYAngle, 
                                         double fPAngle,
                                         double fRAngle)
{
	double fCos, fSin;

	fCos = (double)cos(fYAngle);
	fSin = (double)sin(fYAngle);
	Matrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

	fCos = (double)cos(fPAngle);
	fSin = (double)sin(fPAngle);
	Matrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

	fCos = (double)cos(fRAngle);
	fSin = (double)sin(fRAngle);
	Matrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

	*this = kYMat*(kZMat*kXMat);
}

void Matrix3::FromEulerAnglesZXY(double fYAngle, 
                                         double fPAngle,
                                         double fRAngle)
{
	double fCos, fSin;

	fCos = (double)cos(fYAngle);
	fSin = (double)sin(fYAngle);
	Matrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

	fCos = (double)cos(fPAngle);
	fSin = (double)sin(fPAngle);
	Matrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

	fCos = (double)cos(fRAngle);
	fSin = (double)sin(fRAngle);
	Matrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

	*this = kZMat*(kXMat*kYMat);
}

void Matrix3::FromEulerAnglesZYX(double fYAngle, 
                                         double fPAngle,
                                         double fRAngle)
{
	double fCos, fSin;

	fCos = (double)cos(fYAngle);
	fSin = (double)sin(fYAngle);
	Matrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

	fCos = (double)cos(fPAngle);
	fSin = (double)sin(fPAngle);
	Matrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

	fCos = (double)cos(fRAngle);
	fSin = (double)sin(fRAngle);
	Matrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

	*this = kZMat*(kYMat*kXMat);
}

void Matrix3::TensorProduct(const Vector3& rkU, 
                                    const Vector3& rkV,
                                    Matrix3& rkProduct)
{
	for (int iRow = 0; iRow < 3; iRow++) {
		for (int iCol = 0; iCol < 3; iCol++) {
			rkProduct[iRow][iCol] = rkU[iRow]*rkV[iCol];
		}
	}
}

// These routines are using right handed transformations and are based
// on row-column based matrices. They will work with OpenGL or DirectX
// because the linear representation of the matrix will be the same.
/*
Matrix3 TranslationMatrix3(double x, double y, double z)
{
	Matrix3 mat = Matrix3::IDENTITY;
	mat[3][0] = x;
	mat[3][1] = y;
	mat[3][2] = z;
	return mat;
}
*/

Matrix3 RotationXMatrix3(double angle) // Checked
{
	Matrix3 mat = Matrix3::IDENTITY;
	double cy = (double)cos(angle);
	double sy = (double)sin(angle);
	mat[1][1] = cy;
	mat[2][1] = sy;
	mat[1][2] = -sy;
	mat[2][2] = cy;
	return mat;
}

Matrix3 RotationYMatrix3(double angle) // Checked
{
	Matrix3 mat = Matrix3::IDENTITY;
	double cy = (double)cos(angle);
	double sy = (double)sin(angle);
	mat[0][0] = cy;
	mat[2][0] = -sy;
	mat[0][2] = sy;
	mat[2][2] = cy;
	return mat;
}

Matrix3 RotationZMatrix3(double angle) // Ckecked
{
	Matrix3 mat = Matrix3::IDENTITY;
	mat[0][0] = (double)cos( angle );
	mat[1][0] = (double)sin(angle);
	mat[0][1] = -mat[1][0];
	mat[1][1] = mat[0][0];
	return mat;
}


void Matrix3::Print(FILE * stream)
{
	fprintf(stream, "[%f, %f, %f]\n", rowcol[0][0], rowcol[0][1], rowcol[0][2]);
	fprintf(stream, "[%f, %f, %f]\n", rowcol[1][0], rowcol[1][1], rowcol[1][2]);
	fprintf(stream, "[%f, %f, %f]\n", rowcol[2][0], rowcol[2][1], rowcol[2][2]);
}

/* 
 * FIXME
 * This is a rather dangerous routine... 
 * how about returning a std::string instead? 
 */
void Matrix3::Print(char *String)
{
	sprintf(String, "[%+f, %+f, %+f]\n[%+f, %+f, %+f]\n[%+f, %+f, %+f]\n",
	rowcol[0][0], rowcol[0][1], rowcol[0][2],
	rowcol[1][0], rowcol[1][1], rowcol[1][2],
	rowcol[2][0], rowcol[2][1], rowcol[2][2]);
}

std::string Matrix3::asString() const {
	std::stringstream repr;
	repr << *this;
	return repr.str();
}

std::ostream & operator<<(std::ostream & os, const Matrix3 & m)
{
	os << "[" <<  std::setw(8) << m[0][0]
	   << ", " << std::setw(8) << m[0][1] 
	   << ", " << std::setw(8) << m[0][2] 
	   << "]" << std::endl 
	   << "[" << std::setw(8) << m[1][0]
	   << ", " << std::setw(8) << m[1][1] 
	   << ", " << std::setw(8) << m[1][2] 
	   << "]" <<  std::endl
	   << "[" <<  std::setw(8) << m[2][0]
	   << ", " << std::setw(8) << m[2][1] 
	   << ", " << std::setw(8) << m[2][2] 
	   << "]"; // <<  std::endl;
	return os;
}



void Matrix3::pack(Packer &p) const {
	p.pack(rowcol[0][0]);
	p.pack(rowcol[0][1]);
	p.pack(rowcol[0][2]);
	p.pack(rowcol[1][0]);
	p.pack(rowcol[1][1]);
	p.pack(rowcol[1][2]);
	p.pack(rowcol[2][0]);
	p.pack(rowcol[2][1]);
	p.pack(rowcol[2][2]);
}

void Matrix3::unpack(UnPacker &p) {
	p.unpack(rowcol[0][0]);
	p.unpack(rowcol[0][1]);
	p.unpack(rowcol[0][2]);
	p.unpack(rowcol[1][0]);
	p.unpack(rowcol[1][1]);
	p.unpack(rowcol[1][2]);
	p.unpack(rowcol[2][0]);
	p.unpack(rowcol[2][1]);
	p.unpack(rowcol[2][2]);
}

void Matrix3::parseXML(const char* cdata) {
	std::stringstream ss(cdata); 
	std::string token; 
	for (unsigned short iRow = 0; iRow < 3; ++iRow) {
		for (unsigned short iCol = 0; iCol < 3; ++iCol) {
			if (!(ss >> token)) {
				throw ParseException("Expect exactly nine (9) elements in matrix");
			}
	    		rowcol[iRow][iCol] = atof(token.c_str());
		}
	}
	if (ss >> token) {
		throw ParseException("Expect exactly nine (9) elements in matrix");
	}
}

NAMESPACE_END // namespace simdata

