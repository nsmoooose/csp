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
 * @file Matrix3.h
 *
 * A three-by-three matrix class.
 *
 * This source code was originally based on the Matrix class of 
 * the OpenSceneGraph library, Copyright 1998-2003 Robert Osfield.
 * Source code from OpenSceneGraph is used here under the GNU General
 * Public License version 2 or later, as permitted under the 
 * OpenSceneGraph Public License version 0.0 (exception 3) and the GNU 
 * Lesser Public  License version 2 (clause 3).
 **/

#ifndef __SIMDATA_MATRIX3_H__
#define __SIMDATA_MATRIX3_H__


#include <SimData/BaseType.h>
#include <SimData/Vector3.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cmath>


NAMESPACE_SIMDATA


class Quat;


/**
 * @brief A three-by-three matrix class using double-precision.
 *
 * @ingroup BaseTypes
 */
class SIMDATA_EXPORT Matrix3: public BaseType
{
public:
	virtual std::string asString() const;
	virtual std::string typeString() const { return "Matrix3"; }
	virtual void pack(Packer &p) const;
	virtual void unpack(UnPacker &p);
	virtual void parseXML(const char* cdata);

	static const Matrix3 ZERO;
	static const Matrix3 IDENTITY;

        Matrix3() { } // for speed, do not initialize
        Matrix3(const Matrix3& other): BaseType(other) { set(other); }
        explicit Matrix3(double const * const def) { set(def); }
        Matrix3(double a00, double a01, double a02,
                double a10, double a11, double a12,
                double a20, double a21, double a22) { 
		set(a00, a01, a02, 
		    a10, a11, a12, 
		    a20, a21, a22); 
	}
	Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2);

        ~Matrix3() {}

        int compare(const Matrix3& m) const { return memcmp(_mat, m._mat, sizeof(_mat)); }

        bool operator < (const Matrix3& m) const { return compare(m)<0; }
        bool operator == (const Matrix3& m) const { return compare(m)==0; }
        bool operator != (const Matrix3& m) const { return compare(m)!=0; }

#ifndef SWIG
        inline double& operator()(int row, int col) { return _mat[row][col]; }
        inline double operator()(int row, int col) const { return _mat[row][col]; }
#endif // SWIG

	inline double getElement(int row, int col) { return _mat[row][col]; }
	inline void setElement(int row, int col, double value) { _mat[row][col]=value; }

        inline bool valid() const { return !isNaN(); }
        bool isNaN() const;

#ifndef SWIG
        inline Matrix3& operator = (const Matrix3& other) {
		if (&other == this) return *this;
		std::copy((double*)other._mat, (double*)other._mat+9, (double*)(_mat));
		return *this;
        }
#endif // SWIG
        
        inline void set(const Matrix3& other) {
		std::copy((double*)other._mat, (double*)other._mat+9, (double*)(_mat));
        }
        
        inline void set(double const * const ptr) {
		std::copy(ptr, ptr+9, (double*)(_mat));
        }
        
        void set(double a00, double a01, double a02,
                 double a10, double a11, double a12,
                 double a20, double a21, double a22) {
		_mat[0][0] = a00;
		_mat[0][1] = a01;
		_mat[0][2] = a02;
		_mat[1][0] = a10;
		_mat[1][1] = a11;
		_mat[1][2] = a12;
		_mat[2][0] = a20;
		_mat[2][1] = a21;
		_mat[2][2] = a22;
	}
                  
	std::vector<double> getElements() const;
	void setElements(std::vector<double> const &v) const;

	Vector3 getRow(int i) { 
		assert(i>=0 && i<3); 
		return Vector3(_mat[i][0], _mat[i][1], _mat[i][2]); 
	}

	Vector3 getCol(int i) { 
		assert(i>=0 && i<3); 
		return Vector3(_mat[0][i], _mat[1][i], _mat[2][i]); 
	}

	void setRow(int i, const Vector3& v) {
		assert(i>=0 && i<3);
		_mat[i][0] = v.x();
		_mat[i][1] = v.y();
		_mat[i][2] = v.z();
	}

	void setCol(int i, const Vector3& v) {
		assert(i>=0 && i<3);
		_mat[0][i] = v.x();
		_mat[1][i] = v.y();
		_mat[2][i] = v.z();
	}

	double * row(int i) { assert(i>=0 && i<3); return (double*)(_mat[i]); }
        double * ptr() { return (double *)_mat; }
        double * ptr() const { return (double *)_mat; }

        inline void makeIdentity() { set(IDENTITY); }
	inline void makeZero() { set(ZERO); }

        inline void makeScale(const Vector3& v) {
		makeScale(v.x(), v.y(), v.z());
	}

        void makeScale(double, double, double);
        
        void makeRotate(const Vector3& from, const Vector3& to);
        void makeRotate(double angle, const Vector3& axis);
        void makeRotate(double angle, double x, double y, double z);
        void makeRotate(const Quat&);
        void makeRotate(double roll, double pitch, double yaw);
        void makeRotate(double angle1, const Vector3& axis1, 
                        double angle2, const Vector3& axis2,
                        double angle3, const Vector3& axis3);

        bool invert(const Matrix3&, double tolerance=1e-12);
        inline bool invert(double tolerance=1e-12) { return invert(*this, tolerance); }

	void transpose(const Matrix3& other);

	inline void transpose() {
		swap(_mat[0][1], _mat[1][0]);
		swap(_mat[0][2], _mat[2][0]);
		swap(_mat[1][2], _mat[2][1]);
	}

	inline Matrix3 getInverse(double tolerance=1e-12) const { return inverse(*this, tolerance); }
	inline Matrix3 getTranspose() const { 
		return Matrix3(_mat[0][0], _mat[1][0], _mat[2][0],
		               _mat[0][1], _mat[1][1], _mat[2][1],
			       _mat[0][2], _mat[1][2], _mat[2][2]);
	}

	double determinant() const;

	inline static Matrix3 const &identity(void) { return IDENTITY; }
        inline static Matrix3 scale(const Vector3& sv);
        inline static Matrix3 scale(double sx, double sy, double sz);
        inline static Matrix3 rotate(const Vector3& from, const Vector3& to);
        inline static Matrix3 rotate(double angle, double x, double y, double z);
        inline static Matrix3 rotate(double angle, const Vector3& axis);
        inline static Matrix3 rotate(double angle1, const Vector3& axis1, 
                                     double angle2, const Vector3& axis2,
                                     double angle3, const Vector3& axis3);
	inline static Matrix3 rotate(double roll, double pitch, double yaw);
        inline static Matrix3 rotate(const Quat& quat);
        inline static Matrix3 inverse(const Matrix3& matrix, double tolerance=1e-12);
	inline static Matrix3 tensor(const Vector3&a, const Vector3 &b);
	
	void getRotate(double angle, Vector3& axis) const;
	void getEulerAngles(double &roll, double &pitch, double &yaw);
        
        inline Vector3 preMult(const Vector3& v) const;
        inline Vector3 postMult(const Vector3& v) const;
        inline Vector3 operator* (const Vector3& v) const { return postMult(v); }
#ifndef SWIG
	inline friend Vector3 operator* (const Vector3& v, const Matrix3& m) { return m.preMult(v); }
#endif // SWIG

        inline Vector3 getScale() const { return Vector3(_mat[0][0],_mat[1][1],_mat[2][2]); }
	inline double getTrace() const { return _mat[0][0] + _mat[1][1] + _mat[2][2]; }
        
        void mult(const Matrix3&, const Matrix3&);
        void preMult(const Matrix3&);
        void postMult(const Matrix3&);

        inline void operator *= (const Matrix3& other) {
		if (this == &other) {
			Matrix3 temp(other);
			postMult(temp);
		} else {
			postMult(other); 
		}
        }

        inline Matrix3 operator * (const Matrix3& m) const {
		Matrix3 r;
		r.mult(*this, m);
		return r;
	}

	inline const Matrix3& operator += (const Matrix3& rhs) {
		double *m0 = reinterpret_cast<double*>(_mat);
		double const *m1 = reinterpret_cast<double const*>(rhs._mat);
		for (int i=0; i<9; ++i) {
			m0[i] += m1[i];
		}
		return *this;
	}

	inline const Matrix3& operator -= (const Matrix3& rhs) {
		double *m0 = reinterpret_cast<double*>(_mat);
		double const *m1 = reinterpret_cast<double const*>(rhs._mat);
		for (int i=0; i<9; ++i) {
			m0[i] -= m1[i];
		}
		return *this;
	}

	inline const Matrix3& operator *= (const double rhs) {
		double *m0 = reinterpret_cast<double*>(_mat);
		for (int i=0; i<9; ++i) {
			m0[i] *= rhs;
		}
		return *this;
	}

	inline const Matrix3& operator /= (const double rhs) {
		return *this *= (1.0/rhs);
	}

	inline Matrix3 operator + (const Matrix3& rhs) const {
		Matrix3 result;
		double *m0 = reinterpret_cast<double*>(result._mat);
		double const *m1 = reinterpret_cast<double const*>(_mat);
		double const *m2 = reinterpret_cast<double const*>(rhs._mat);
		for (int i=0; i<9; ++i) {
			m0[i] = m1[i] + m2[i];
		}
		return result;
	}

	inline Matrix3 operator - (const Matrix3& rhs) const {
		Matrix3 result;
		double *m0 = reinterpret_cast<double*>(result._mat);
		double const *m1 = reinterpret_cast<double const*>(_mat);
		double const *m2 = reinterpret_cast<double const*>(rhs._mat);
		for (int i=0; i<9; ++i) {
			m0[i] = m1[i] - m2[i];
		}
		return result;
	}

	inline Matrix3 operator * (double rhs) const {
		Matrix3 result;
		double *m0 = reinterpret_cast<double*>(result._mat);
		double const *m1 = reinterpret_cast<double const*>(_mat);
		for (int i=0; i<9; ++i) {
			m0[i] = m1[i] * rhs;
		}
		return result;
	}

	inline Matrix3 operator / (double rhs) const {
		return *this * (1.0/rhs);
	}

	inline Matrix3 operator - () const {
		return *this * (-1.0);
	}

#ifndef SWIG
	inline friend Matrix3 operator * (double lhs, const Matrix3& rhs) {
		return rhs * lhs;
	}

	friend SIMDATA_EXPORT std::ostream& operator<< (std::ostream& os, const Matrix3& m);
#endif // SWIG

protected:
        double _mat[3][3];

};


inline Matrix3 Matrix3::scale(double sx, double sy, double sz) {
	return Matrix3(sx, 0.0, 0.0, 0.0, sy, 0.0, 0.0, 0.0, sz);
}

inline Matrix3 Matrix3::scale(const Vector3& v) {
	return Matrix3(v.x(), 0.0, 0.0, 0.0, v.y(), 0.0, 0.0, 0.0, v.z());
}

inline Matrix3 Matrix3::rotate(const Quat& q) {
	Matrix3 m;
	m.makeRotate(q);
	return m;
}

inline Matrix3 Matrix3::rotate(double roll, double pitch, double yaw) {
	Matrix3 m;
	m.makeRotate(roll, pitch, yaw);
	return m;
}

inline Matrix3 Matrix3::rotate(double angle, double x, double y, double z) {
	Matrix3 m;
	m.makeRotate(angle, x, y, z);
	return m;
}

inline Matrix3 Matrix3::rotate(double angle, const Vector3& axis) {
	Matrix3 m;
	m.makeRotate(angle, axis);
	return m;
}

inline Matrix3 Matrix3::rotate(double angle1, const Vector3& axis1, 
                               double angle2, const Vector3& axis2,
                               double angle3, const Vector3& axis3)
{
	Matrix3 m;
	m.makeRotate(angle1, axis1, angle2, axis2, angle3, axis3);
	return m;
}

inline Matrix3 Matrix3::rotate(const Vector3& from, const Vector3& to) {
	Matrix3 m;
	m.makeRotate(from, to);
	return m;
}

inline Matrix3 Matrix3::inverse(const Matrix3& matrix, double tolerance) {
	Matrix3 m;
	m.invert(matrix, tolerance);
	return m;
}

inline Matrix3 Matrix3::tensor(const Vector3&a, const Vector3 &b) {
	return Matrix3(a.x()*b.x(),
	               a.x()*b.y(),
	               a.x()*b.z(),
	               a.y()*b.x(),
	               a.y()*b.y(),
	               a.y()*b.z(),
	               a.z()*b.x(),
	               a.z()*b.y(),
	               a.z()*b.z());
}

inline Vector3 Matrix3::postMult(const Vector3& v) const {
	return Vector3((_mat[0][0]*v.x() + _mat[0][1]*v.y() + _mat[0][2]*v.z()),
	               (_mat[1][0]*v.x() + _mat[1][1]*v.y() + _mat[1][2]*v.z()),
	               (_mat[2][0]*v.x() + _mat[2][1]*v.y() + _mat[2][2]*v.z()));
}

inline Vector3 Matrix3::preMult(const Vector3& v) const {
	return Vector3((_mat[0][0]*v.x() + _mat[1][0]*v.y() + _mat[2][0]*v.z()),
	               (_mat[0][1]*v.x() + _mat[1][1]*v.y() + _mat[2][1]*v.z()),
	               (_mat[0][2]*v.x() + _mat[1][2]*v.y() + _mat[2][2]*v.z()));
}



NAMESPACE_SIMDATA_END


#endif // __SIMDATA_MATRIX3_H__

