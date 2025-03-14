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
 * @file Vector3.h
 *
 * A three-dimensional vector class.
 *
 * This source code was originally based on the Vec3 class of
 * the OpenSceneGraph library, Copyright 1998-2003 Robert Osfield.
 * Source code from OpenSceneGraph is used here under the GNU General
 * Public License Version 2 or later, as permitted under the
 * OpenSceneGraph Public License Version 0.0 (exception 3) and the GNU
 * Lesser Public  License Version 2 (clause 3).
 **/

#include <csp/csplib/data/BaseType.h>
#include <csp/csplib/util/Math.h>

#include <cmath>
#include <cstdlib>
#include <vector>


namespace csp {


class Matrix3;

/** A three-dimensional vector class using double-precision.
 *
 *  @ingroup BaseTypes
 */
class CSPLIB_EXPORT Vector3
{
	double _x, _y, _z;

public: // BaseType

	/// String representation.
	std::string asString() const;

	/// Type representation.
	std::string typeString() const { return "type::Vector3"; }

	/// Serialize from a Reader.
	void serialize(Reader&);

	/// Serialize to a Writer.
	void serialize(Writer&) const;

	/** Parse the character data from an XML \<Vector\> tag.
	 *  The three components must be separated by whitespace.
	 */
	void parseXML(const char*);

	/// XML post processing.
	void convertXML() {}

public:

	/// Null vector.
	static const Vector3 ZERO;
	/// Unit vector along the x-axis.
	static const Vector3 XAXIS;
	/// Unit vector along the y-axis.
	static const Vector3 YAXIS;
	/// Unit vector along the z-axis.
	static const Vector3 ZAXIS;

	/// Construct a new (null) vector.
	Vector3(): _x(0.0), _y(0.0), _z(0.0) {}
	/// Construct and initialize a new vector.
	Vector3(double x_, double y_, double z_): _x(x_), _y(y_), _z(z_) {}
	/// Copy constructor.
	Vector3(const Vector3& v): _x(v._x), _y(v._y), _z(v._z) {}

#ifndef SWIG
	/// Copy operator.
	const Vector3& operator = (const Vector3& v) {
		_x=v._x; _y=v._y; _z=v._z;
		return *this;
	}
#endif // SWIG

	/// Test for equality with another vectors.
	bool operator == (const Vector3& v) const { return _x==v._x && _y==v._y && _z==v._z; }

	/// Test for inequality with another vectors.
	bool operator != (const Vector3& v) const { return _x!=v._x || _y!=v._y || _z!=v._z; }

	/// Set the vector components.
	void set(double x_, double y_, double z_) { _x=x_; _y=y_; _z=z_; }

#ifndef SWIG
	/** Get a reference to vector component by numeric index.
	 *
	 *  The index starts at zero (=x), and an IndexException
	 *  is thrown for invalid indices.
	 *
	 *  @return A reference to the specified component.
	 */
	double& operator [] (int i) {
		switch (i) {
			case 0: return _x;
			case 1: return _y;
			case 2: return _z;
			default: abort(); return _x;
		}
	}

	/** Get the value of a vector component by numeric index.
	 *
	 *  The index starts at zero (=x), and an IndexException
	 *  is thrown for invalid indices.
	 *
	 *  @return The value of the specified component.
	 */
	double operator [] (int i) const {
		switch (i) {
			case 0: return _x;
			case 1: return _y;
			case 2: return _z;
			default: abort(); return _x;
		}
	}
#endif // SWIG


#ifndef SWIG
	/// Get a reference to the x-component.
	double& x() { return _x; }
	/// Get a reference to the y-component.
	double& y() { return _y; }
	/// Get a reference to the z-component.
	double& z() { return _z; }
#endif // SWIG

	/// Get the value of the x-component.
	double x() const { return _x; }
	/// Get the value of the y-component.
	double y() const { return _y; }
	/// Get the value of the z-component.
	double z() const { return _z; }

	/// Test if all components are valid floating point values.
	bool valid() const { return !isNaN(); }
	
	/// Test if any components are NaN (not-a-number).
	bool isNaN() const { return std::isnan(_x) || std::isnan(_y) || std::isnan(_z); }
	bool isZero() const { return *this == ZERO; }

	/// Compute the dot product with another vector.
	double operator * (const Vector3& rhs) const {
		return _x * rhs._x + _y * rhs._y + _z * rhs._z;
	}

	/// Compute the cross product with another vector.
	/// CAUTION: operator^ has low precedence, so cross products
	/// should always be enclosed in parentheses.  Consider using
	/// the cross() function instead.
	const Vector3 operator ^ (const Vector3& rhs) const {
		return Vector3(_y * rhs._z - _z * rhs._y, _z * rhs._x - _x * rhs._z, _x * rhs._y - _y*rhs._x);
	}

	/// Multiply by a scalar.
	const Vector3 operator * (double rhs) const {
		return Vector3(_x * rhs, _y * rhs, _z * rhs);
	}

#ifndef SWIG
	/// Unary multiply by a scalar.
	Vector3& operator *= (double rhs) {
		_x *= rhs;
		_y *= rhs;
		_z *= rhs;
		return *this;
	}
#endif // SWIG

	/// Divide by a scalar.
	const Vector3 operator / (double rhs) const {
		return (*this) * (1.0 / rhs);
	}

#ifndef SWIG
	/// Unary divide by a scalar.
	Vector3& operator /= (double rhs) {
		return *this *= (1.0 / rhs);
	}
#endif // SWIG

	/// Binary vector addition.
	const Vector3 operator + (const Vector3& rhs) const {
		return Vector3(_x + rhs._x, _y + rhs._y, _z + rhs._z);
	}

#ifndef SWIG
	/// Unary vector addition.
	Vector3& operator += (const Vector3& rhs) {
		_x += rhs._x;
		_y += rhs._y;
		_z += rhs._z;
		return *this;
	}
#endif // SWIG

	/// Binary vector subtraction.
	const Vector3 operator - (const Vector3& rhs) const {
		return Vector3(_x - rhs._x, _y - rhs._y, _z - rhs._z);
	}

#ifndef SWIG
	/// Unary vector subtraction.
	Vector3& operator -= (const Vector3& rhs) {
		_x -= rhs._x;
		_y -= rhs._y;
		_z -= rhs._z;
		return *this;
	}
#endif // SWIG

	/// Negation operator; returns the negative of the vector.
	const Vector3 operator - () const {
		return Vector3(-_x, -_y, -_z);
	}

	/// Get the length of the vector = sqrt(v*v)
	double length() const {
		return ::sqrt(_x*_x + _y*_y + _z*_z);
	}

	/// Get the length squared of the vector = v*v
	double length2() const {
		return _x*_x + _y*_y + _z*_z;
	}

	/** Normalize the vector so that it has length unity.
	 *
	 *  @returns The original length of the vector.
	 */
	double normalize() {
		double norm = length();
		if (norm > 0.0) *this /= norm;
		return norm;
	}

	/// Get a unit vector in the direction of this vector.
	const Vector3 normalized() const {
		double norm = length();
		return (norm > 0.0) ? (*this / norm) : *this;
	}

	/// Compute the star matrix of this vector.
	Matrix3 starMatrix() const;

	/** Update bounding vectors.
	 *
	 *  The components of @c min and @c max are updated to
	 *  contain the corresponding components of this vector.
	 */
	void bound(Vector3& min, Vector3& max) const {
		if (_x < min._x) min._x = _x;
		if (_y < min._y) min._y = _y;
		if (_z < min._z) min._z = _z;
		if (_x > max._x) max._x = _x;
		if (_y > max._y) max._y = _y;
		if (_z > max._z) max._z = _z;
	}

#ifndef SWIG
	/// Compute the dot product of two vectors.
	friend double dot(const Vector3& a, const Vector3& b); // inline

	/// Compute the cross product of two vectors.
	friend const Vector3 cross(const Vector3& a, const Vector3& b); // inline

	/// Multiply a vector by a scalar on the left.
	friend Vector3 operator * (double lhs, const Vector3 &rhs); // inline
#endif // SWIG

	/// Get the components of this vector as a std::vector<double>.
	std::vector<double> getElements() const {
		std::vector<double> v(3);
		v[0] = _x;
		v[1] = _y;
		v[2] = _z;
		return v;
	}

	/// Set the components of this vector from a std::vector<double>.
	void setElements(std::vector<double> const &v) {
		if (v.size() == 3) {
			_x = v[0];
			_y = v[1];
			_z = v[2];
		} else {
			abort();
		}
	}

#ifdef SWIG
// clang-format off
	// setup accessors for x, y, and z (ugly hack)
%extend {
	void set_x(double x) { self->x()=x; }
	void set_y(double y) { self->y()=y; }
	void set_z(double z) { self->z()=z; }
	double get_x() { return self->x(); }
	double get_y() { return self->y(); }
	double get_z() { return self->z(); }
}
// clang-format on
#endif // SWIG

}; // end of class Vector3

inline double dot(const Vector3& a, const Vector3& b) { return a*b; }

inline const Vector3 cross(const Vector3& a, const Vector3& b) { return a^b; }

inline Vector3 operator * (double lhs, const Vector3 &rhs) { return rhs*lhs; }

CSPLIB_EXPORT std::ostream &operator <<(std::ostream &o, Vector3 const &v);

} // namespace csp
