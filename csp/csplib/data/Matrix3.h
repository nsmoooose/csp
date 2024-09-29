#pragma once
/* Combat Simulator Project
 * Copyright (C) 2002, 2003, 2005 Mark Rose <mkrose@users.sf.net>
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

#include <csp/csplib/data/BaseType.h>
#include <csp/csplib/data/Vector3.h>

#include <vector>
#include <cassert>
#include <cmath>
#include <string.h>

namespace csp {


class Quat;


/**
 * @brief A three-by-three matrix class using double-precision.
 *
 * @ingroup BaseTypes
 */
class CSPLIB_EXPORT Matrix3
{
public: // BaseType

	/// String representation.
	std::string asString() const;

	/// Type representation.
	std::string typeString() const { return "type::Matrix3"; }

	/// Serialize from a Reader.
	void serialize(Reader&);

	/// Serialize to a Writer.
	void serialize(Writer&) const;
	
	/** Extract the matrix values from XML character data.
	 *
	 *  The nine values should be separated by white-space
	 *  and arranged in order (0,0), (0,1), (0,2), (1,0), etc.
	 */
	void parseXML(const char* cdata);

	/// XML post processing.
	void convertXML() {}

public:

	/** Null matrix.
	 */
	static const Matrix3 ZERO;

	/** Identity matrix.
	 */
	static const Matrix3 IDENTITY;

	/** Default constructor.
	 *
	 *  @note The default constructor does @b not initialize the
	 *  matrix for the sake of efficiency.  If you do not explicitly
	 *  set the matrix, the elements will generally assume random
	 *  values.
	 */
	Matrix3() { } // for speed, do not initialize

	/** Copy constructor.
	 */
	Matrix3(const Matrix3& other) { set(other); }

	/** Construct and initialize a matrix from a double[9] array.
	 */
	explicit Matrix3(double const * const def) { set(def); }
	Matrix3(double a00, double a01, double a02,
	        double a10, double a11, double a12,
	        double a20, double a21, double a22) {
		set(a00, a01, a02,
		    a10, a11, a12,
		    a20, a21, a22);
	}

	/** Construct and initialize a matrix from three column vectors.
	 */
	Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2);

	/** Destructor.
	 */
	~Matrix3() {}

	/** Compare two matrices.
	 *
	 *  Compares two matrices byte-by-byte.  The sign of the return value is
	 *  useless, since the byte comparisons are inequivalent to floating point
	 *  comparisions.  Used only to test for equality.
	 *
	 *  @returns 0 if equal, non-zero if unequal.
	 */
	int compare(const Matrix3& m) const { return memcmp(_mat, m._mat, sizeof(_mat)); }

	/** Compare two matrices for (byte) equality.
	 */
	bool operator == (const Matrix3& m) const { return compare(m)==0; }

	/** Compare two matrices for (byte) inequality.
	 */
	bool operator != (const Matrix3& m) const { return compare(m)!=0; }

#ifndef SWIG
	/** Get the value (reference) of a matrix element.
	 */
	inline double& operator()(int row_, int col_) { return _mat[row_][col_]; }

	/** Get the (const) value of a matrix element.
	 */
	inline double operator()(int row_, int col_) const { return _mat[row_][col_]; }
#endif // SWIG

	/** Get the value of a matrix element.
	 */
	inline double getElement(int row_, int col_) { return _mat[row_][col_]; }

	/** Set the value of a matrix element.
	 */
	inline void setElement(int row_, int col_, double value) { _mat[row_][col_]=value; }

	/** Return true if all elements are valid floating point numbers.
	 */
	inline bool valid() const { return !isNaN(); }

	/** Return true if any elements are NaN (not-a-number).
	 */
	bool isNaN() const;

#ifndef SWIG
	/** Copy operator.
	 */
	inline Matrix3& operator = (const Matrix3& other) {
		if (&other == this) return *this;
		set(reinterpret_cast<double const *>(other._mat));
		return *this;
	}
#endif // SWIG

	/** Set this matrix from another matrix.
	 */
	inline void set(const Matrix3& other) {
		set(reinterpret_cast<double const *>(other._mat));
	}

	/** Set this matrix from a double[9] array.
	 */
	inline void set(double const * src) {
		double *dst = reinterpret_cast<double*>(_mat);
		for (const double *end = src + 9; src != end; ) *dst++ = *src++;
	}

	/** Set this matrix from a list of element values.
	 */
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

	/** Get the matrix elements as a vector<double>.
	 */
	std::vector<double> getElements() const;

	/** Set this matrix from a vector<double>.
	 */
	void setElements(std::vector<double> const &v) const;

	/** Get a row vector of this matrix.
	 */
	Vector3 getRow(int i) {
		assert(i>=0 && i<3);
		return Vector3(_mat[i][0], _mat[i][1], _mat[i][2]);
	}

	/** Get a column vector of this matrix.
	 */
	Vector3 getCol(int i) {
		assert(i>=0 && i<3);
		return Vector3(_mat[0][i], _mat[1][i], _mat[2][i]);
	}

	/** Set a row of this matrix from a vector.
	 */
	void setRow(int i, const Vector3& v) {
		assert(i>=0 && i<3);
		_mat[i][0] = v.x();
		_mat[i][1] = v.y();
		_mat[i][2] = v.z();
	}

	/** Set a column of this matrix from a vector.
	 */
	void setCol(int i, const Vector3& v) {
		assert(i>=0 && i<3);
		_mat[0][i] = v.x();
		_mat[1][i] = v.y();
		_mat[2][i] = v.z();
	}

	/** Get a pointer to a row of this matrix.
	 */
	double * row(int i) { assert(i>=0 && i<3); return (double*)(_mat[i]); }
	
	/** Get a pointer to the first element of this matrix.
	 */
	double * ptr() { return (double *)_mat; }

	/** Get a const pointer to the first element of this matrix.
	 */
	double const * ptr() const { return (double const *)_mat; }

	/** Set this matrix equal to the identity matrix.
	 */
	inline void makeIdentity() { set(IDENTITY); }

	/** Set all matrix elements to zero.
	 */
	inline void makeZero() { set(ZERO); }

	/** Set this matrix to a scaling matrix.
	 *
	 *  The resulting matrix has the components
	 *  of the input vector along the diagonal,
	 *  with all off-diagonal elements equal to
	 *  zero.
	 *
	 *  @param v A vector specifying the scale factor for
	 *           each axis.
	 */
	inline void makeScale(const Vector3& v) {
		makeScale(v.x(), v.y(), v.z());
	}

	/** Set this matrix to a scaling matrix.
	 *
	 *  See makeScale(const Vector3&).
	 */
	void makeScale(double, double, double);

	/** Make a rotation matrix to transform one vector into another.
	 *
	 *  The resulting matrix will rotate the @c from vector
	 *  into the @c to vector.
	 */
	void makeRotate(const Vector3& from, const Vector3& to);

	/** Make a rotation matrix to rotate around a given axis.
	 *
	 *  @param angle The angle of rotation.
	 *  @param axis The axis of rotation.
	 */
	void makeRotate(double angle, const Vector3& axis);

	/** Make a rotation matrix to rotate around a given axis.
	 *
	 *  See makeRotate(double angle, const Vector3& axis).
	 */
	void makeRotate(double angle, double x, double y, double z);

	/** Make a rotation matrix from a quaternion.
	 */
	void makeRotate(const Quat&);

	/** Make a rotation matrix from euler angles.
	 *
	 *  @param roll the x-axis rotation.
	 *  @param pitch the y-axis rotation.
	 *  @param yaw the z-axis rotation.
	 */
	void makeRotate(double roll, double pitch, double yaw);

	/** Make a rotation from combining three rotations.
	 *
	 *  @param angle1 The angle of the first rotation.
	 *  @param axis1 The axis of the first rotation.
	 *  @param angle2 The angle of the second rotation.
	 *  @param axis2 The axis of the second rotation.
	 *  @param angle3 The angle of the third rotation.
	 *  @param axis3 The axis of the third rotation.
	 */
	void makeRotate(double angle1, const Vector3& axis1,
	                double angle2, const Vector3& axis2,
	                double angle3, const Vector3& axis3);

	/** Construct the inverse of a matrix.
	 *
	 *  @param m The input matrix.
	 *  @param tolerance The minimum value of the deteriminant.
	 *  @returns false if the deteriminant less than the tolerance,
	 *           true otherwise.  If false, the matrix elements are
	 *           copied directly from the input matrx.
	 */
	bool invert(const Matrix3 &m, double tolerance=1e-12);

	/** Invert this matrix.
	 *
	 *  @param tolerance The minimum value of the deteriminant.
	 *  @returns false if the deteriminant less than the tolerance,
	 *           true otherwise.  If false, the matrix elements are
	 *           unchanged.
	 */
	inline bool invert(double tolerance=1e-12) { return invert(*this, tolerance); }

	/** Construct the transpose of a matrix.
	 *
	 *  @param other The input matrix to transpose.
	 */
	void transpose(const Matrix3& other);

	/** Transpose this matrix.
	 */
	inline void transpose() {
		std::swap(_mat[0][1], _mat[1][0]);
		std::swap(_mat[0][2], _mat[2][0]);
		std::swap(_mat[1][2], _mat[2][1]);
	}

	/** Get the inverse of this matrix.
	 *
	 *  See invert(double).
	 */
	inline Matrix3 getInverse(double tolerance=1e-12) const { return inverse(*this, tolerance); }

	/** Get the transpose of this matrix.
	 */
	inline Matrix3 getTranspose() const {
		return Matrix3(_mat[0][0], _mat[1][0], _mat[2][0],
		               _mat[0][1], _mat[1][1], _mat[2][1],
		               _mat[0][2], _mat[1][2], _mat[2][2]);
	}

	/** Compute the determinant of this matrix.
	 */
	double determinant() const;

	/** Create a new identity matrix.
	 */
	inline static Matrix3 const &identity(void) { return IDENTITY; }

	/** Create a new scaling matrix.
	 */
	inline static Matrix3 scale(const Vector3& sv);

	/** Create a new scaling matrix.
	 */
	inline static Matrix3 scale(double sx, double sy, double sz);

	/** Create a new rotation matrix.
	 */
	inline static Matrix3 rotate(const Vector3& from, const Vector3& to);

	/** Create a new rotation matrix.
	 */
	inline static Matrix3 rotate(double angle, double x, double y, double z);
	/** Create a new rotation matrix.
	 */
	inline static Matrix3 rotate(double angle, const Vector3& axis);

	/** Create a new rotation matrix.
	 */
	inline static Matrix3 rotate(double angle1, const Vector3& axis1,
	                             double angle2, const Vector3& axis2,
	                             double angle3, const Vector3& axis3);

	/** Create a new rotation matrix.
	 */
	inline static Matrix3 rotate(double roll, double pitch, double yaw);

	/** Create a new rotation matrix.
	 */
	inline static Matrix3 rotate(const Quat& quat);

	/** Get the inverse of a matrix.
	 */
	inline static Matrix3 inverse(const Matrix3& matrix, double tolerance=1e-12);
	/** Get the tensor product of two vectors.
	 */
	inline static Matrix3 tensor(const Vector3&a, const Vector3 &b);
	
	/** Get the rotation angle and axis of this matrix.
	 */
	void getRotate(double angle, Vector3& axis) const;

	/** Get the Euler angles of this matrix.
	 */
	void getEulerAngles(double &roll, double &pitch, double &yaw);

	/** Multiply this matrix by a row vector (v*M).
	 */
	inline Vector3 preMult(const Vector3& v) const;

	/** Multiply this matrix by a column vector (M*v).
	 */
	inline Vector3 postMult(const Vector3& v) const;

	/** Multiply this matrix by a column vector (M*v).
	 */
	inline Vector3 operator* (const Vector3& v) const { return postMult(v); }
#ifndef SWIG
	/** Multiply a matrix by a row vector (v*M).
	 */
	inline friend Vector3 operator* (const Vector3& v, const Matrix3& m) { return m.preMult(v); }
#endif // SWIG

	/** Get the diagonal elements of this matrix as a vector.
	 */
	inline Vector3 getScale() const { return Vector3(_mat[0][0],_mat[1][1],_mat[2][2]); }

	/** Get the trace of this matrix.
	 */
	inline double getTrace() const { return _mat[0][0] + _mat[1][1] + _mat[2][2]; }

	/** Matrix multipliation (M*M)
	 */
	void mult(const Matrix3&, const Matrix3&);

	/** Multiply this matrix by another matrix on the left.
	 */
	void preMult(const Matrix3&);

	/** Multiply this matrix by another matrix on the right.
	 */
	void postMult(const Matrix3&);

	/** Multiply this matrix by another matrix on the right.
	 */
	inline void operator *= (const Matrix3& other) {
		if (this == &other) {
			Matrix3 temp(other);
			postMult(temp);
		} else {
			postMult(other);
		}
	}

	/** Get the product of this matrix and another matrix.
	 */
	inline Matrix3 operator * (const Matrix3& m) const {
		Matrix3 r;
		r.mult(*this, m);
		return r;
	}

	/** Add another matrix to this matrix.
	 */
	inline const Matrix3& operator += (const Matrix3& rhs) {
		double *m0 = reinterpret_cast<double*>(_mat);
		double const *m1 = reinterpret_cast<double const*>(rhs._mat);
		for (int i=0; i<9; ++i) {
			m0[i] += m1[i];
		}
		return *this;
	}

	/** Subtract another matrix from this matrix.
	 */
	inline const Matrix3& operator -= (const Matrix3& rhs) {
		double *m0 = reinterpret_cast<double*>(_mat);
		double const *m1 = reinterpret_cast<double const*>(rhs._mat);
		for (int i=0; i<9; ++i) {
			m0[i] -= m1[i];
		}
		return *this;
	}

	/** Multiply this matrix by a scalar.
	 */
	inline const Matrix3& operator *= (const double rhs) {
		double *m0 = reinterpret_cast<double*>(_mat);
		for (int i=0; i<9; ++i) {
			m0[i] *= rhs;
		}
		return *this;
	}

	/** Divide this matrix by a scalar.
	 */
	inline const Matrix3& operator /= (const double rhs) {
		return *this *= (1.0/rhs);
	}

	/** Get the sum of this matrix and another matrix.
	 */
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

	/** Get the difference of this matrix and another matrix.
	 */
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

	/** Get the product of this matrix and a scalar.
	 */
	inline Matrix3 operator * (double rhs) const {
		Matrix3 result;
		double *m0 = reinterpret_cast<double*>(result._mat);
		double const *m1 = reinterpret_cast<double const*>(_mat);
		for (int i=0; i<9; ++i) {
			m0[i] = m1[i] * rhs;
		}
		return result;
	}

	/** Get the quotient of this matrix and a scalar.
	 */
	inline Matrix3 operator / (double rhs) const {
		return *this * (1.0/rhs);
	}

	/** Get this matrix with each element negated.
	 */
	inline Matrix3 operator - () const {
		return *this * (-1.0);
	}

#ifndef SWIG
	/** Multiply a matrix by a scalar on the left (s*M).
	 */
	inline friend Matrix3 operator * (double lhs, const Matrix3& rhs) {
		return rhs * lhs;
	}
#endif // SWIG


protected:
	/** The matrix elements */
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

CSPLIB_EXPORT std::ostream &operator <<(std::ostream &o, Matrix3 const &m);


} // namespace csp
