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


#ifndef __SIMDATA_VECTOR3_H__
#define __SIMDATA_VECTOR3_H__

#include <SimData/BaseType.h>
#include <SimData/Math.h>

#include <cmath>
#include <vector>
#include <iostream>


NAMESPACE_SIMDATA


class Matrix3;

/**
 * @brief A three-dimensional vector class using double-precision.
 *
 * @ingroup BaseTypes
 */
class SIMDATA_EXPORT Vector3: public BaseType
{
protected:
	double _x, _y, _z;

public:

	static const Vector3 ZERO;
	static const Vector3 XAXIS;
	static const Vector3 YAXIS;
	static const Vector3 ZAXIS;

        Vector3(): _x(0.0), _y(0.0), _z(0.0) {}
        Vector3(double x, double y, double z): _x(x), _y(y), _z(z) {}
        Vector3(const Vector3& v): BaseType(v), _x(v._x), _y(v._y), _z(v._z) {}

#ifndef SWIG
	inline const Vector3& operator = (const Vector3& v) { 
		_x=v._x; _y=v._y; _z=v._z; 
		return *this;
	}
#endif // SWIG

	inline bool operator == (const Vector3& v) const { return _x==v._x && _y==v._y && _z==v._z; }
        
        inline bool operator != (const Vector3& v) const { return _x!=v._x || _y!=v._y || _z!=v._z; }

        inline void set(double x, double y, double z) { _x=x; _y=y; _z=z; }

#ifndef SWIG
        inline double& operator [] (int i) { 
		switch (i) {
			case 0: return _x;
			case 1: return _y;
			case 2: return _z;
			default:
				throw ""; // FIXME
		}
	}
        inline double operator [] (int i) const { 
		switch (i) {
			case 0: return _x;
			case 1: return _y;
			case 2: return _z;
			default:
				throw ""; // FIXME
		}
	}
#endif // SWIG


#ifndef SWIG
        inline double& x() { return _x; }
        inline double& y() { return _y; }
        inline double& z() { return _z; }
#endif // SWIG

        inline double x() const { return _x; }
        inline double y() const { return _y; }
        inline double z() const { return _z; }

        inline bool valid() const { return !isNaN(); }
        inline bool isNaN() const { return simdata::isNaN(_x) || simdata::isNaN(_y) || simdata::isNaN(_z); }
	inline bool isZero() const { return *this == ZERO; }

        /// dot product
        inline double operator * (const Vector3& rhs) const {
		return _x*rhs._x+_y*rhs._y+_z*rhs._z;
        }

        /// cross product
        inline const Vector3 operator ^ (const Vector3& rhs) const {
		return Vector3(_y*rhs._z-_z*rhs._y, _z*rhs._x-_x*rhs._z, _x*rhs._y-_y*rhs._x);
        }

        /// multiply by scalar
        inline const Vector3 operator * (double rhs) const {
		return Vector3(_x*rhs, _y*rhs, _z*rhs);
        }

#ifndef SWIG
        /// unary multiply by scalar
        inline Vector3& operator *= (double rhs) {
		_x*=rhs;
		_y*=rhs;
		_z*=rhs;
		return *this;
        }
#endif // SWIG

        /// divide by scalar
        inline const Vector3 operator / (double rhs) const {
		return (*this)*(1.0/rhs);
        }

#ifndef SWIG
        /// unary divide by scalar
        inline Vector3& operator /= (double rhs) {
		return *this *= (1.0/rhs);
        }
#endif // SWIG

        /// binary vector add
        inline const Vector3 operator + (const Vector3& rhs) const {
		return Vector3(_x+rhs._x, _y+rhs._y, _z+rhs._z);
        }

#ifndef SWIG
        /** unary vector add.  Slightly more efficient because no temporary
            intermediate object*/
        inline Vector3& operator += (const Vector3& rhs) {
		_x += rhs._x;
		_y += rhs._y;
		_z += rhs._z;
		return *this;
        }
#endif // SWIG

        /// binary vector subtract
        inline const Vector3 operator - (const Vector3& rhs) const {
		return Vector3(_x-rhs._x, _y-rhs._y, _z-rhs._z);
        }

#ifndef SWIG
        /// unary vector subtract
        inline Vector3& operator -= (const Vector3& rhs) {
		_x-=rhs._x;
		_y-=rhs._y;
		_z-=rhs._z;
		return *this;
        }
#endif // SWIG

        /// negation operator.  Returns the negative of the Vector3
        inline const Vector3 operator - () const {
		return Vector3(-_x, -_y, -_z);
        }

        /// Length of the vector = sqrt( vec . vec )
        inline double length() const {
		return ::sqrt( _x*_x + _y*_y + _z*_z );
        }

        /// Length squared of the vector = vec . vec
        inline double length2() const {
		return _x*_x + _y*_y + _z*_z;
        }

        /** normalize the vector so that it has length unity
            returns the previous length of the vector*/
        inline double normalize() {
		double norm = length();
		if (norm > 0.0) {
			_x /= norm;
			_y /= norm;
			_z /= norm;
		}                
		return norm;
        }

	inline const Vector3 normalized() const {
		double norm = length();
		if (norm > 0.0) {
			return (*this / norm);
		}                
		return *this;
	}

	/**
	 * Compute the star matrix of this vector.
	 */
	Matrix3 starMatrix() const;

	inline void bound(Vector3& min, Vector3& max) {
		if (_x < min._x) min._x = _x;
		if (_y < min._y) min._y = _y;
		if (_z < min._z) min._z = _z;
		if (_x > max._x) max._x = _x;
		if (_y > max._y) max._y = _y;
		if (_z > max._z) max._z = _z;
	}
		
#ifndef SWIG
	friend double dot(const Vector3& a, const Vector3& b); // inline

	friend const Vector3 cross(const Vector3& a, const Vector3& b); // inline

	friend SIMDATA_EXPORT std::ostream& operator << (std::ostream& output, const Vector3& vec);

	friend Vector3 operator * (double lhs, const Vector3 &rhs); // inline
#endif // SWIG

	std::vector<double> getElements() const {
		std::vector<double> v(3);
		v[0] = _x;
		v[1] = _y;
		v[2] = _z;
		return v;
	}

	void setElements(std::vector<double> const &v) {
		if (v.size() == 3) {
			_x = v[0];
			_y = v[1];
			_z = v[2];
		} else {
			// FIXME (throw)
		}
	}

	/**
	 * String representation.
	 */
	virtual std::string asString() const;

	virtual std::string typeString() const { return "Vector3"; }

	/**
	 * Serialize
	 */
	virtual void pack(Packer&) const;

	/**
	 * Deserialize
	 */
	virtual void unpack(UnPacker&);

	/**
	 * Parse cdata from XML tag <Vector>
	 */
	virtual void parseXML(const char*);

#ifdef SWIG
	// setup accessors for x, y, and z (ugly hack)
%extend {
	void set_x(double x) { self->x()=x; }
	void set_y(double y) { self->y()=y; }
	void set_z(double z) { self->z()=z; }
	double get_x() { return self->x(); }
	double get_y() { return self->y(); }
	double get_z() { return self->z(); }
}
%insert("shadow") %{
	if _newclass:
		x = property(_cSimData.Vector3_get_x, _cSimData.Vector3_set_x)
		y = property(_cSimData.Vector3_get_y, _cSimData.Vector3_set_y)
		z = property(_cSimData.Vector3_get_z, _cSimData.Vector3_set_z)
	__swig_setmethods__["x"] = _cSimData.Vector3_set_x
	__swig_getmethods__["x"] = _cSimData.Vector3_get_x
	__swig_setmethods__["y"] = _cSimData.Vector3_set_y
	__swig_getmethods__["y"] = _cSimData.Vector3_get_y
	__swig_setmethods__["z"] = _cSimData.Vector3_set_z
	__swig_getmethods__["z"] = _cSimData.Vector3_get_z
%}
#endif // SWIG

};	// end of class Vector3

inline double dot(const Vector3& a, const Vector3& b) { return a*b; }

inline const Vector3 cross(const Vector3& a, const Vector3& b) { return a^b; }

inline Vector3 operator * (double lhs, const Vector3 &rhs) { return rhs*lhs; }

NAMESPACE_SIMDATA_END // simdata


#endif // __SIMDATA_VECTOR3_H__

