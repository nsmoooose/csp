/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002, 2003 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimData.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


/**
 * @file Matrix3.cpp
 *
 * This source code was originally based on the Matrix class of 
 * the OpenSceneGraph library, Copyright 1998-2003 Robert Osfield.
 * Source code from OpenSceneGraph is used here under the GNU General
 * Public License Version 2 or later, as permitted under the 
 * OpenSceneGraph Public License Version 0.0 (exception 3) and the GNU 
 * Lesser Public  License Version 2 (clause 3).
 **/


#include <SimData/Matrix3.h>
#include <SimData/Vector3.h>
#include <SimData/Quat.h>
#include <SimData/Pack.h>

#include <iomanip>
#include <sstream>
#include <cmath>


NAMESPACE_SIMDATA


const Matrix3 Matrix3::ZERO(0,0,0,0,0,0,0,0,0);
const Matrix3 Matrix3::IDENTITY(1,0,0,0,1,0,0,0,1);
        

Matrix3::Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2) {
	set(col0.x(), col1.x(), col2.x(),
	    col0.y(), col1.y(), col2.y(),
	    col0.z(), col1.z(), col2.z());
}

bool Matrix3::isNaN() const { 
	return simdata::isNaN(_mat[0][0]) || 
	       simdata::isNaN(_mat[0][1]) || 
	       simdata::isNaN(_mat[0][2]) || 
	       simdata::isNaN(_mat[1][0]) || 
	       simdata::isNaN(_mat[1][1]) || 
	       simdata::isNaN(_mat[1][2]) || 
	       simdata::isNaN(_mat[2][0]) || 
	       simdata::isNaN(_mat[2][1]) || 
	       simdata::isNaN(_mat[2][2]);
}

std::vector<double> Matrix3::getElements() const {
	std::vector<double> v(9);
	std::copy((double*)_mat, (double*)_mat+9, v.begin());
	return v;
}

void Matrix3::setElements(std::vector<double> const &v) const {
	assert(v.size() == 9);
	std::copy(v.begin(), v.end(), (double*)_mat);
}

#ifdef INNER_PRODUCT
#undef INNER_PRODUCT
#endif
#define INNER_PRODUCT(a, b, r, c) \
	((a)._mat[r][0] * (b)._mat[0][c] + \
	 (a)._mat[r][1] * (b)._mat[1][c] + \
	 (a)._mat[r][2] * (b)._mat[2][c]);

void Matrix3::mult(const Matrix3& lhs, const Matrix3& rhs) {
	if (&lhs == this) {
		postMult(rhs);
		return;
	} else
	if (&rhs == this) {
		preMult(lhs);
		return;
	}
	_mat[0][0] = INNER_PRODUCT(lhs, rhs, 0, 0);
	_mat[0][1] = INNER_PRODUCT(lhs, rhs, 0, 1);
	_mat[0][2] = INNER_PRODUCT(lhs, rhs, 0, 2);
	_mat[1][0] = INNER_PRODUCT(lhs, rhs, 1, 0);
	_mat[1][1] = INNER_PRODUCT(lhs, rhs, 1, 1);
	_mat[1][2] = INNER_PRODUCT(lhs, rhs, 1, 2);
	_mat[2][0] = INNER_PRODUCT(lhs, rhs, 2, 0);
	_mat[2][1] = INNER_PRODUCT(lhs, rhs, 2, 1);
	_mat[2][2] = INNER_PRODUCT(lhs, rhs, 2, 2);
}

void Matrix3::preMult(const Matrix3& other) {
	double t[3];
	for (int col=0; col<3; col++) {
		t[0] = INNER_PRODUCT(other, *this, 0, col);
		t[1] = INNER_PRODUCT(other, *this, 1, col);
		t[2] = INNER_PRODUCT(other, *this, 2, col);
		_mat[0][col] = t[0];
		_mat[1][col] = t[1];
		_mat[2][col] = t[2];
	}
}

void Matrix3::postMult(const Matrix3& other) {
	double t[3];
	for (int row=0; row<3; row++) {
		t[0] = INNER_PRODUCT(*this, other, row, 0);
		t[1] = INNER_PRODUCT(*this, other, row, 1);
		t[2] = INNER_PRODUCT(*this, other, row, 2);
		_mat[row][0] = t[0];
		_mat[row][1] = t[1];
		_mat[row][2] = t[2];
	}
}

#undef INNER_PRODUCT

#ifdef DET2
#undef DET2
#endif
#define DET2(m, a, b, c, d)	((m)[a][b]*(m)[c][d] - (m)[a][d]*(m)[c][b])

double Matrix3::determinant() const {
	return _mat[0][0]*DET2(_mat,1,1,2,2) + _mat[0][1]*DET2(_mat,2,0,1,2) + _mat[0][2]*DET2(_mat,1,0,2,1);
}



bool Matrix3::invert(const Matrix3& m, double tolerance) {
	_mat[0][0] = DET2(m._mat,1,1,2,2);
	_mat[0][1] = DET2(m._mat,0,2,2,1);
	_mat[0][2] = DET2(m._mat,0,1,1,2);

	double d = m._mat[0][0]*_mat[0][0] + m._mat[1][0]*_mat[0][1] + m._mat[2][0]*_mat[0][2];
	if (d < tolerance) {
		*this = m;
		return false;
	}

	_mat[1][0] = DET2(m._mat,2,0,1,2);
	_mat[1][1] = DET2(m._mat,0,0,2,2);
	_mat[1][2] = DET2(m._mat,0,2,1,0);
	_mat[2][0] = DET2(m._mat,1,0,2,1);
	_mat[2][1] = DET2(m._mat,0,1,2,0);
	_mat[2][2] = DET2(m._mat,0,0,1,1);
	*this *= (1.0/d);
	return true;
}

#undef DET2

#if 0 
// osg inversion approach -- todo: test speed against cofactors
bool Matrix3::invert(const Matrix3& mat)
{
	if (&mat==this) {
		Matrix3 tm(mat);
		return invert(tm);
	}

	unsigned int indxc[3], indxr[3], ipiv[3];
	unsigned int i,j,k,l,ll;
	unsigned int icol = 0;
	unsigned int irow = 0;
	double temp, pivinv, dum, big;

	// copy in place this may be unnecessary
	*this = mat;

	for (j=0; j<3; j++) {
		ipiv[j]=0;
	}

	for(i=0; i<3; i++) {
		big=0.0;
		for (j=0; j<3; j++) {
			if (ipiv[j] != 1) {
				for (k=0; k<3; k++) {
					if (ipiv[k] == 0) {
						if (absolute(operator()(j,k)) >= big) {
							big = absolute(operator()(j,k));
							irow=j;
							icol=k;
						}
					} else 
					if (ipiv[k] > 1) {
						return false;
					}
				}
			}
		}	
		++(ipiv[icol]);
		if (irow != icol) {
			for (l=0; l<3; l++) {
				swap(operator()(irow,l), operator()(icol,l), temp);
			}
		}

		indxr[i]=irow;
		indxc[i]=icol;
		if (operator()(icol,icol) == 0) {
			return false;
		}

		pivinv = 1.0/operator()(icol,icol);
		operator()(icol,icol) = 1;
		for (l=0; l<3; l++) {
			operator()(icol,l) *= pivinv;
		}
		for (ll=0; ll<3; ll++) {
			if (ll != icol) {
				dum=operator()(ll,icol);
				operator()(ll,icol) = 0;
				for (l=0; l<3; l++) operator()(ll,l) -= operator()(icol,l)*dum;
			}
		}
	}
	for (int lx=3; lx>0; --lx) {
		if (indxr[lx-1] != indxc[lx-1]) {
			for (k=0; k<3; k++) {
				swap(operator()(k,indxr[lx-1]), operator()(k,indxc[lx-1]),temp);
			}
		}
	}

	return true;
}
#endif

void Matrix3::makeScale(double x, double y, double z) {
	set(x, 0.0, 0.0, 0.0, y, 0.0, 0.0, 0.0, z);
}

void Matrix3::makeRotate(const Vector3& from, const Vector3& to) {
	Quat quat;
	quat.makeRotate(from, to);
	quat.get(*this);
}

void Matrix3::makeRotate(double angle, const Vector3& axis) {
	Quat quat;
	quat.makeRotate(angle, axis);
	quat.get(*this);
}

void Matrix3::makeRotate(double angle, double x, double y, double z) {
	Quat quat;
	quat.makeRotate(angle, x, y, z);
	quat.get(*this);
}

void Matrix3::makeRotate(const Quat& q) {
	q.get(*this);    
}

void Matrix3::makeRotate(double angle1, const Vector3& axis1, 
                         double angle2, const Vector3& axis2,
                         double angle3, const Vector3& axis3)
{
	Quat quat;
	quat.makeRotate(angle1, axis1, angle2, axis2, angle3, axis3);
	quat.get(*this);
}

void Matrix3::makeRotate(double roll, double pitch, double yaw) {
	Quat quat;
	quat.makeRotate(roll, pitch, yaw);
	quat.get(*this);
}

void Matrix3::getRotate(double angle, Vector3& axis) const {
	Quat quat(*this);
	quat.getRotate(angle, axis);
}

void Matrix3::getEulerAngles(double &roll, double &pitch, double &yaw) {
	Quat quat(*this);
	quat.getEulerAngles(roll, pitch, yaw);
}

void Matrix3::transpose(const Matrix3& other) {
	_mat[0][0] = other._mat[0][0];
	_mat[0][1] = other._mat[1][0];
	_mat[0][2] = other._mat[2][0];
	_mat[1][0] = other._mat[0][1];
	_mat[1][1] = other._mat[1][1];
	_mat[1][2] = other._mat[2][1];
	_mat[2][0] = other._mat[0][2];
	_mat[2][1] = other._mat[1][2];
	_mat[2][2] = other._mat[2][2];
}

std::string Matrix3::asString() const {
	std::stringstream repr;
	repr << *this;
	return repr.str();
}

void Matrix3::pack(Packer &p) const {
	p.pack(_mat[0][0]);
	p.pack(_mat[0][1]);
	p.pack(_mat[0][2]);
	p.pack(_mat[1][0]);
	p.pack(_mat[1][1]);
	p.pack(_mat[1][2]);
	p.pack(_mat[2][0]);
	p.pack(_mat[2][1]);
	p.pack(_mat[2][2]);
}

void Matrix3::unpack(UnPacker &p) {
	p.unpack(_mat[0][0]);
	p.unpack(_mat[0][1]);
	p.unpack(_mat[0][2]);
	p.unpack(_mat[1][0]);
	p.unpack(_mat[1][1]);
	p.unpack(_mat[1][2]);
	p.unpack(_mat[2][0]);
	p.unpack(_mat[2][1]);
	p.unpack(_mat[2][2]);
}

void Matrix3::parseXML(const char* cdata) {
	std::stringstream ss(cdata); 
	std::string token; 
	for (int irow = 0; irow < 3; ++irow) {
		for (int icol = 0; icol < 3; ++icol) {
			if (!(ss >> token)) {
				throw ParseException("Expect exactly nine (9) elements in matrix");
			}
	    		_mat[irow][icol] = atof(token.c_str());
		}
	}
	if (ss >> token) {
		throw ParseException("Expect exactly nine (9) elements in matrix");
	}
}

std::ostream& operator<< (std::ostream& os, const Matrix3& m)
{
	os << "["<<std::endl;
	for (int irow=0; irow<3; ++irow) {
		os << "\t";
		for (int icol=0; icol<3; ++icol) {
			os << std::setw(8) << m(irow, icol) << " ";
		}
		os << std::endl;
	}
	os << "]";
	return os;
}


NAMESPACE_SIMDATA_END

