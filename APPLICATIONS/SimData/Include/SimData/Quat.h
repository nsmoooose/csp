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
 * @file Quat.h
 *
 * This source code was originally based on the Quat class of 
 * the OpenSceneGraph library, Copyright 1998-2003 Robert Osfield.
 * Source code from OpenSceneGraph is used here under the GNU General
 * Public License version 2 or later, as permitted under the 
 * OpenSceneGraph Public License version 0.0 (exception 3) and the 
 * GNU Lesser Public  License version 2 (clause 3).
 **/

#ifndef __SIMDATA_QUAT_H__
#define __SIMDATA_QUAT_H__


#include <SimData/BaseType.h>
#include <SimData/Vector3.h>
#include <SimData/Matrix3.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>


NAMESPACE_SIMDATA


/** 
 * A quaternion class. It can be used to represent an orientation in 3D space.
 */
class SIMDATA_EXPORT Quat: public BaseType
{
	double _x, _y, _z, _w;
public:

	static const Quat IDENTITY;
	static const Quat ZERO;

	// BaseType interface
	virtual void pack(Packer&) const; 
	virtual void unpack(UnPacker&);
	virtual void parseXML(const char* cdata);

	/**
	 * String representation.
	 */
	std::string asString() const;

	inline Quat(): _x(0.0), _y(0.0), _z(0.0), _w(0.0) {}
	inline Quat(double x, double y, double z, double w):  _x(x), _y(y), _z(z), _w(w) {}
	inline Quat(double angle, const Vector3& axis) { makeRotate(angle, axis); }
	inline Quat(const Matrix3 &m) { set(m); }
	inline Quat(double angle1, const Vector3& axis1, 
		    double angle2, const Vector3& axis2,
		    double angle3, const Vector3& axis3)
	{
		makeRotate(angle1, axis1, angle2, axis2, angle3, axis3);
	}

	inline const Vector3 asVector3() const {
		return Vector3(_x, _y, _z);
	}

	inline void set(double x, double y, double z, double w) {
		_x = x; _y = y; _z = z; _w = w;
	}

#ifndef SWIG
	inline double& operator [] (int i) { 
		switch (i) {
			case 0: return _x;
			case 1: return _y;
			case 2: return _z;
			case 3: return _w;
			default:
				throw ""; // FIXME
		}
	}
	inline double  operator [] (int i) const { 
		switch (i) {
			case 0: return _x;
			case 1: return _y;
			case 2: return _z;
			case 3: return _w;
			default:
				throw ""; // FIXME
		}
	}

	inline double& x() { return _x; }
	inline double& y() { return _y; }
	inline double& z() { return _z; }
	inline double& w() { return _w; }

	inline double x() const { return _x; }
	inline double y() const { return _y; }
	inline double z() const { return _z; }
	inline double w() const { return _w; }
#endif // SWIG

    	/** return true if the Quat represents a zero rotation, and therefore can be ignored in computations.*/
	bool zeroRotation() const { 
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

	/// Binary multiply  --- adjusted relative to osg
	inline const Quat operator*(const Quat& rhs) const {
		return Quat(rhs._w*_x + rhs._x*_w - rhs._y*_z + rhs._z*_y,
		            rhs._w*_y + rhs._x*_z + rhs._y*_w - rhs._z*_x,
		            rhs._w*_z - rhs._x*_y + rhs._y*_x + rhs._z*_w,
		            rhs._w*_w - rhs._x*_x - rhs._y*_y - rhs._z*_z);
	}

	/// Unary multiply  --- adjusted relative to osg
	inline Quat& operator*=(const Quat& rhs) {
		double x = rhs._w*_x + rhs._x*_w - rhs._y*_z + rhs._z*_y;
		double y = rhs._w*_y + rhs._x*_z + rhs._y*_w - rhs._z*_x;
		double z = rhs._w*_z - rhs._x*_y + rhs._y*_x + rhs._z*_w;
		_w = rhs._w*_w - rhs._x*_x - rhs._y*_y - rhs._z*_z;
		_z = z;
		_y = y;
		_x = x;
		return *this;	
	}

	/// Divide by scalar 
	inline const Quat operator / (double rhs) const {
		return *this * (1.0/rhs);
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

        /// Length of the quaternion = vec . vec
        double length2() const {
		return (_x*_x + _y*_y + _z*_z + _w*_w);
        }

        /// Conjugate 
        inline Quat conj () const { 
		return Quat(-_x, -_y, -_z, _w);
        }

	inline Quat operator ~() const {
		return Quat(-_x, -_y, -_z, _w);
	}

        /// Multiplicative inverse method: q^(-1) = q^*/(q.q^*)
        inline const Quat inverse () const {
		return conj() / length2();
        }

        void makeRotate(double angle, 
                        double x, double y, double z);

        void makeRotate(double angle, const Vector3& vec);

        void makeRotate(double angle1, const Vector3& axis1, 
                        double angle2, const Vector3& axis2,
                        double angle3, const Vector3& axis3);

        /** Make a rotation Quat which will rotate vec1 to vec2.
            Generally take a dot product to get the angle between these
            and then use a cross product to get the rotation axis
            Watch out for the two special cases of when the vectors
            are co-incident or opposite in direction.*/
        void makeRotate(const Vector3& vec1, const Vector3& vec2);

	void makeRotate(double roll, double pitch, double yaw);

	void getEulerAngles(double &roll, double &pitch, double &yaw) const;
    
        /** Return the angle and vector components represented by the quaternion.*/
        void getRotate(double& angle, double& x, double& y, double& z) const;

        /** Return the angle and vector represented by the quaternion.*/
        void getRotate(double& angle, Vector3& vec) const;

        /** Spherical Linear Interpolation.
	    As t goes from 0 to 1, the Quat object goes from "from" to "to". */
        void slerp(double t, const Quat& from, const Quat& to);
        
        /** Set quaternion to be equivalent to specified matrix.*/
        void set(const Matrix3& m);

        /** Get the equivalent matrix for this quaternion.*/
        void get(Matrix3& m) const;

        /** Get the equivalent matrix for this quaternion.*/
        Matrix3 getMatrix3() const {
		Matrix3 matrix;
		get(matrix);
		return matrix;
        }
	
	Vector3 rotate(const Vector3 &v) const;
	Vector3 invrotate(const Vector3 &v) const;
	inline Quat rotate(const Quat &q) const { return (*this)*q*conj(); }
	inline Quat invrotate(const Quat &q) const { return conj()*q*(*this); }

#ifndef SWIG
        friend inline std::ostream& operator << (std::ostream& output, const Quat& vec);
        friend inline Quat operator * (double lhs, const Quat& rhs) { return rhs*lhs; }
	friend inline Quat operator *(Quat const& lhs, Vector3 const& rhs) {
		return Quat( lhs.w()*rhs.x() + lhs.y()*rhs.z() - lhs.z()*rhs.y(),
		             lhs.w()*rhs.y() + lhs.z()*rhs.x() - lhs.x()*rhs.z(),
		             lhs.w()*rhs.z() + lhs.x()*rhs.y() - lhs.y()*rhs.x(),
		            -lhs.x()*rhs.x() - lhs.y()*rhs.y() - lhs.z()*rhs.z());
	}
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
		w = property(_cSimData.Quat_get_w, _cSimData.Quat_set_w)
		x = property(_cSimData.Quat_get_x, _cSimData.Quat_set_x)
		y = property(_cSimData.Quat_get_y, _cSimData.Quat_set_y)
		z = property(_cSimData.Quat_get_z, _cSimData.Quat_set_z)
	__swig_setmethods__["w"] = _cSimData.Quat_set_w
	__swig_getmethods__["w"] = _cSimData.Quat_get_w
	__swig_setmethods__["x"] = _cSimData.Quat_set_x
	__swig_getmethods__["x"] = _cSimData.Quat_get_x
	__swig_setmethods__["y"] = _cSimData.Quat_set_y
	__swig_getmethods__["y"] = _cSimData.Quat_get_y
	__swig_setmethods__["z"] = _cSimData.Quat_set_z
	__swig_getmethods__["z"] = _cSimData.Quat_get_z
%}
#endif // SWIG

};	// end of class prototype


inline std::ostream& operator << (std::ostream& output, const Quat& quat)
{
    output << "[" << quat._x
	   << " " << quat._y
	   << " " << quat._z
	   << " " << quat._w << "]";
    return output;
}


NAMESPACE_SIMDATA_END

#endif  // __SIMDATA_QUAT_H__

