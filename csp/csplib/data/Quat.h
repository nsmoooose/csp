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
 * @file Quat.h
 *
 * A quaternion class.
 *
 * This source code was originally based on the Quat class of
 * the OpenSceneGraph library, Copyright 1998-2003 Robert Osfield.
 * Source code from OpenSceneGraph is used here under the GNU General
 * Public License version 2 or later, as permitted under the
 * OpenSceneGraph Public License version 0.0 (exception 3) and the
 * GNU Lesser Public  License version 2 (clause 3).
 **/

#ifndef __CSPLIB_DATA_QUAT_H__
#define __CSPLIB_DATA_QUAT_H__


#include <csp/csplib/data/BaseType.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/Matrix3.h>

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

namespace csp {

/**
 * @brief Quaternion class using double-precision.
 *
 * Quaternions are four dimensional objects that form a compact
 * representation for rotations.  Many thorough treatments of
 * quaternions and their use in simulations can be readily found
 * on the web.
 *
 * Good introductions to Quaternions at:
 * http://www.gamasutra.com/features/programming/19980703/quaternions_01.htm
 * http://mathworld.wolfram.com/Quaternion.html
 *
 * @ingroup BaseTypes
 */
class CSPLIB_EXPORT Quat
{
	double _x, _y, _z, _w;

public:  // BaseType

	/// String representation.
	std::string asString() const;

	/// Type representation.
	std::string typeString() const { return "type::Quat"; }

	/// Serialize from a Reader.
	void serialize(Reader&);

	/// Serialize to a Writer.
	void serialize(Writer&) const;

	/** Internal method used by the XML parser.
	 *  The format for Quats is "X Y Z W"
	 */
	void parseXML(const char* cdata) throw(ParseException);

	/// XML post processing.
	void convertXML() {}

public:

	/// The identity Quat = (0,0,0,1)
	static const Quat IDENTITY;
	/// The zero Quat = (0,0,0,0)
	static const Quat ZERO;

	/** Construct a new quaternion.
	 *
	 *  Initialize to (0,0,0,0)
	 */
	inline Quat(): _x(0.0), _y(0.0), _z(0.0), _w(0.0) {}

	/** Construct a new quaternion.
	 *
	 *  Specifiy the four real-valued components.
	 */
	inline Quat(double x_, double y_, double z_, double w_):
		_x(x_), _y(y_), _z(z_), _w(w_) { }

	/** Construct a new quaternion representing a rotation.
	 *
	 *  @param angle the rotation angle (right-handed) in radians.
	 *  @param axis the rotation axis.
	 */
	inline Quat(double angle, const Vector3& axis) { makeRotate(angle, axis); }

	/** Construct a new quaternion representing a rotation.
	 *
	 *  @param m a matrix specifying the rotation.
	 */
	inline Quat(const Matrix3 &m) { set(m); }

	/** Construct a new quaternion representing a product of three rotations.
	 *
	 *  @param angle1 the first rotation angle (right-handed) in radians.
	 *  @param axis1 the first rotation axis.
	 *  @param angle2 the second rotation angle (right-handed) in radians.
	 *  @param axis2 the second rotation axis.
	 *  @param angle3 the third rotation angle (right-handed) in radians.
	 *  @param axis3 the third rotation axis.
	 */
	inline Quat(double angle1, const Vector3& axis1,
	            double angle2, const Vector3& axis2,
	            double angle3, const Vector3& axis3) {
		makeRotate(angle1, axis1, angle2, axis2, angle3, axis3);
	}

	/** Return the 3-vector component of the quaternion (x,y,z)
	 */
	inline const Vector3 asVector3() const {
		return Vector3(_x, _y, _z);
	}

	/** Set the components.
	 */
	inline void set(double x_, double y_, double z_, double w_) {
		_x = x_; _y = y_; _z = z_; _w = w_;
	}

#ifndef SWIG
	/** Access a component by index (0,1,2,3 = x,y,z,w)
	 */
	inline double& operator [] (int i) {
		switch (i) {
			case 0: return _x;
			case 1: return _y;
			case 2: return _z;
			case 3: return _w;
			default: abort(); return _x;
		}
	}

	/** Get a component value by index (0,1,2,3 = x,y,z,w)
	 */
	inline double operator [] (int i) const {
		switch (i) {
			case 0: return _x;
			case 1: return _y;
			case 2: return _z;
			case 3: return _w;
			default: abort(); return _x;
		}
	}

	/// X component accessor
	inline double& x() { return _x; }
	/// Y component accessor
	inline double& y() { return _y; }
	/// Z component accessor
	inline double& z() { return _z; }
	/// W component accessor
	inline double& w() { return _w; }

	/// X component const accessor
	inline double x() const { return _x; }
	/// Y component const accessor
	inline double y() const { return _y; }
	/// Z component const accessor
	inline double z() const { return _z; }
	/// W component const accessor
	inline double w() const { return _w; }
#endif // SWIG

	/// Test equality.
	bool operator==(Quat const &rhs) const {
		return _x==rhs._x && _y==rhs._y && _z==rhs._z && _w==rhs._w;
	}

	/// Test inequality.
	bool operator!=(Quat const &rhs) const {
		return !(*this == rhs);
	}

	/** Shorthand to test if the quaternion is the identity element.
	 */
	bool isIdentity() const {
		return _x==0.0 && _y==0.0 && _z==0.0 && _w==1.0;
	}

	/// Multiply by scalar
	inline const Quat operator * (double rhs) const {
		return Quat(_x*rhs, _y*rhs, _z*rhs, _w*rhs);
	}

	/// Unary multiply by scalar
	inline Quat& operator *= (double rhs) {
		_x *= rhs; _y *= rhs; _z *= rhs; _w *= rhs;
		return *this;
	}

	/// Binary multiply  --- adjusted relative to osg for active transformations!
	inline const Quat operator*(const Quat& rhs) const {
		return Quat(rhs._w*_x + rhs._x*_w - rhs._y*_z + rhs._z*_y,
		            rhs._w*_y + rhs._x*_z + rhs._y*_w - rhs._z*_x,
		            rhs._w*_z - rhs._x*_y + rhs._y*_x + rhs._z*_w,
		            rhs._w*_w - rhs._x*_x - rhs._y*_y - rhs._z*_z);
	}

	/// Unary multiply  --- adjusted relative to osg for active transformations!
	inline Quat& operator*=(const Quat& rhs) {
		double x_ = rhs._w*_x + rhs._x*_w - rhs._y*_z + rhs._z*_y;
		double y_ = rhs._w*_y + rhs._x*_z + rhs._y*_w - rhs._z*_x;
		double z_ = rhs._w*_z - rhs._x*_y + rhs._y*_x + rhs._z*_w;
		_w = rhs._w*_w - rhs._x*_x - rhs._y*_y - rhs._z*_z;
		_z = z_;
		_y = y_;
		_x = x_;
		return *this;
	}

	/// Divide by scalar
	inline const Quat operator / (double rhs) const {
		return *this * (1.0 / rhs);
	}

	/// Unary divide by scalar
	inline Quat& operator /= (double rhs) {
		return *this *= (1.0 / rhs);
	}

	/// Binary divide
	inline const Quat operator/(const Quat& denom) const {
		return *this * denom.inverse();
	}

	/// Unary divide
	inline Quat& operator/=(const Quat& denom) {
		return *this *= denom.inverse();
	}

	/// Binary addition
	inline const Quat operator + (const Quat& rhs) const {
		return Quat(_x+rhs._x, _y+rhs._y, _z+rhs._z, _w+rhs._w);
	}

	/// Unary addition
	inline Quat& operator += (const Quat& rhs) {
		_x += rhs._x; _y += rhs._y; _z += rhs._z; _w += rhs._w;
		return *this;
	}

	/// Binary subtraction
	inline const Quat operator - (const Quat& rhs) const {
		return Quat(_x-rhs._x, _y-rhs._y, _z-rhs._z, _w-rhs._w);
	}

	/// Unary subtraction
	inline Quat& operator -= (const Quat& rhs) {
		_x -= rhs._x; _y -= rhs._y; _z -= rhs._z; _w -= rhs._w;
		return *this;
	}

	/// Negation operator - returns the negative of the quaternion.
	inline const Quat operator - () const {
		return Quat(-_x, -_y, -_z, -_w);
	}

	/// Length of the quaternion = sqrt(vec . vec)
	double length() const {
		return sqrt(_x*_x + _y*_y + _z*_z + _w*_w);
	}

	/// Squared length of the quaternion = vec . vec
	double length2() const {
		return (_x*_x + _y*_y + _z*_z + _w*_w);
	}

	/// Get conjugate
	inline Quat conj() const {
		return Quat(-_x, -_y, -_z, _w);
	}

	/// Get conjugate
	inline Quat operator ~() const {
		return Quat(-_x, -_y, -_z, _w);
	}

	/// Normalize to length 1 (unless zero)
	inline void normalize() {
		const double len2 = length2();
		if (len2 > 0.0) *this /= sqrt(len2);
	}

	/// Get normalized quat (or zero)
	inline Quat normalized() {
		const double len2 = length2();
		return (len2 > 0.0) ? Quat(*this / sqrt(len2)) : *this;
	}

	/// Multiplicative inverse method: q^(-1) = q^*/(q.q^*)
	inline const Quat inverse() const {
		return conj() / length2();
	}

	/** Set this Quat to represent a rotation about an axis.
	 *
	 *  @param angle the angle of rotation (right-handed) in radians.
	 *  @param x the x component of the rotation axis.
	 *  @param y the y component of the rotation axis.
	 *  @param z the z component of the rotation axis.
	 */
	void makeRotate(double angle, double x, double y, double z);

	/** Set this Quat to represent a rotation about an axis.
	 *
	 *  @param angle the angle of rotation (right-handed) in radians.
	 *  @param vec the rotation axis.
	 */
	void makeRotate(double angle, const Vector3& vec);

	/** Set this Quat to represent a product of three rotations.
	 *
	 *  @param angle1 the first rotation angle (right-handed) in radians.
	 *  @param axis1 the first rotation axis.
	 *  @param angle2 the second rotation angle (right-handed) in radians.
	 *  @param axis2 the second rotation axis.
	 *  @param angle3 the third rotation angle (right-handed) in radians.
	 *  @param axis3 the third rotation axis.
	 */
	void makeRotate(double angle1, const Vector3& axis1,
	                double angle2, const Vector3& axis2,
	                double angle3, const Vector3& axis3);

	/** Set this Quat to represent a rotation that transforms vec1 to vec2.
	 */
	void makeRotate(const Vector3& vec1, const Vector3& vec2);

	/** Set this Quat to represent a rotation defined by Euler angles.
	 *  This system of Euler angles uses a right-handerd coordinate
	 *  system, with rotations applied in the order (Z, Y, X).
	 *
	 *  @param roll The x-axis rotation angle (right-handed) in radians.
	 *  @param pitch The y-axis rotation angle (right-handed) in radians.
	 *  @param yaw The z-axis rotation angle (right-handed) in radians.
	 */
	void makeRotate(double roll, double pitch, double yaw);

	/** Get the Euler angles corresponding to the rotation defined by
	 *  this Quat.  This method is the inverse of makeRotate(double,
	 *  double, double).  The Quat does not need to be normalized.
	 */
	void getEulerAngles(double &roll, double &pitch, double &yaw) const;

	/** Return the angle and vector components represented by the quaternion.
	 */
	void getRotate(double& angle, double& x, double& y, double& z) const;

	/** Return the angle and vector represented by the quaternion.
	 */
	void getRotate(double& angle, Vector3& vec) const;

	/** Spherical linear interpolation.
	 *  As t goes from 0 to 1, the Quat object goes from "from" to "to".
	 */
	void slerp(double t, const Quat& from, const Quat& to);

	/** Normalized linear interpolation.
	 *  As t goes from 0 to 1, the quat goes from "from" to "to".  At each
	 *  point, the resulting quat is normalized.
	 */
	void nlerp(double t, const Quat& from, const Quat& to);

	/** Set quaternion to be equivalent to specified matrix.
	 */
	void set(const Matrix3& m);

	/** Get the equivalent matrix for this quaternion.
	 */
	void get(Matrix3& m) const;

	/** Get the equivalent matrix for this quaternion.
	 */
	Matrix3 getMatrix3() const {
		Matrix3 matrix;
		get(matrix);
		return matrix;
	}
	
	/** Rotate a vector by this quaternion.
	 */
	Vector3 rotate(const Vector3 &v) const;

	/** Rotate a vector by the inverse of this quaternion.
	 */
	Vector3 invrotate(const Vector3 &v) const;

	/** Rotate another quaternion by this quaternion.
	 */
	inline Quat rotate(const Quat &q) const { return (*this)*q*conj(); }

	/** Rotate another quaternion by the inverse of this quaternion.
	 */
	inline Quat invrotate(const Quat &q) const { return conj()*q*(*this); }

#ifndef SWIG
	/** Format to an output stream.
	 */
	CSPLIB_EXPORT friend std::ostream& operator << (std::ostream& output, const Quat& q);

	/** Multiply a Quat by a scalar value on the left.
	 */
	friend inline Quat operator * (double lhs, const Quat& rhs) { return rhs*lhs; }

	/** Multiply a Quat by a vector on the right.
	 */
	friend inline Quat operator *(Quat const& lhs, Vector3 const& rhs) {
		return Quat( lhs.w()*rhs.x() + lhs.y()*rhs.z() - lhs.z()*rhs.y(),
		             lhs.w()*rhs.y() + lhs.z()*rhs.x() - lhs.x()*rhs.z(),
		             lhs.w()*rhs.z() + lhs.x()*rhs.y() - lhs.y()*rhs.x(),
		            -lhs.x()*rhs.x() - lhs.y()*rhs.y() - lhs.z()*rhs.z());
	}

	/** Multiply a Quat by a vector on the left.
	 */
	friend inline Quat operator *(Vector3 const& lhs, Quat const& rhs) {
		return Quat( rhs.w()*lhs.x() + rhs.z()*lhs.y() - rhs.y()*lhs.z(),
		             rhs.w()*lhs.y() + rhs.x()*lhs.z() - rhs.z()*lhs.x(),
		             rhs.w()*lhs.z() + rhs.y()*lhs.x() - rhs.x()*lhs.y(),
		            -rhs.x()*lhs.x() - rhs.y()*lhs.y() - rhs.z()*lhs.z());
	}
#endif // SWIG

#ifdef SWIG
	// setup accessors for w, x, y, and z (ugly hack)
%extend {
	void set_w(double w) { self->w()=w; }
	void set_x(double x) { self->x()=x; }
	void set_y(double y) { self->y()=y; }
	void set_z(double z) { self->z()=z; }
	double get_w() { return self->w(); }
	double get_x() { return self->x(); }
	double get_y() { return self->y(); }
	double get_z() { return self->z(); }
}
%insert("shadow") %{
	if _newclass:
		w = property(_csplib_module.Quat_get_w, _csplib_module.Quat_set_w)
		x = property(_csplib_module.Quat_get_x, _csplib_module.Quat_set_x)
		y = property(_csplib_module.Quat_get_y, _csplib_module.Quat_set_y)
		z = property(_csplib_module.Quat_get_z, _csplib_module.Quat_set_z)
	__swig_setmethods__["w"] = _csplib_module.Quat_set_w
	__swig_getmethods__["w"] = _csplib_module.Quat_get_w
	__swig_setmethods__["x"] = _csplib_module.Quat_set_x
	__swig_getmethods__["x"] = _csplib_module.Quat_get_x
	__swig_setmethods__["y"] = _csplib_module.Quat_set_y
	__swig_getmethods__["y"] = _csplib_module.Quat_get_y
	__swig_setmethods__["z"] = _csplib_module.Quat_set_z
	__swig_getmethods__["z"] = _csplib_module.Quat_get_z
%}
#endif // SWIG

};

CSPLIB_EXPORT std::ostream &operator <<(std::ostream &o, Quat const &q);

} // namespace csp

#endif  // __CSPLIB_DATA_QUAT_H__
