// Combat Simulator Project - CSPSim
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file MessageTypes.h
 * @brief Classes for compact representation of common types in NetworkMessages.
 *
 * To use these classes in .net message definition files, include the header
 * and declare the types.  For example:
 *
 * @code
 *   // test.net
 *   #header <test.h>
 *
 *   #include <SimCore/MessageTypes.h>
 *   type GlobalPosition;
 *   type Vector4f;
 *
 *   message ObjectUpdate : simnet::NetworkMessage {
 *     GlobalPosition position;
 *     Vector4f attitude;
 *   }
 * @endcode
 *
 */

#ifndef __SIMCORE_UTIL_MESSAGETYPES_H__
#define __SIMCORE_UTIL_MESSAGETYPES_H__

#include <ostream>
#include <cmath>
#include <SimData/Archive.h>
#include <SimData/Quat.h>
#include <SimData/Vector3.h>

namespace simcore {

class GlobalPosition {
	float x;
	float y;
	float z;
	simdata::uint8 x0;
	simdata::uint8 y0;

	friend simdata::Reader & operator >> (simdata::Reader &r, GlobalPosition &p);
	friend simdata::Writer & operator << (simdata::Writer &w, GlobalPosition const &p);
public:

	GlobalPosition(): x(0), y(0), z(0), x0(0), y0(0) {}
	
	explicit GlobalPosition(simdata::Vector3 const &v) {
		*this = v;
	}

	inline GlobalPosition const &operator=(simdata::Vector3 const &v) {
		x0 = static_cast<simdata::uint8>(floor(v.x() / 32000.0) + 128);
		y0 = static_cast<simdata::uint8>(floor(v.y() / 32000.0) + 128);
		x = static_cast<float>(v.x() - (static_cast<int>(x0)-128) * 32000.0);
		y = static_cast<float>(v.y() - (static_cast<int>(y0)-128) * 32000.0);
		z = static_cast<float>(v.z());
		return *this;
	}

	inline simdata::Vector3 Vector3() const {
		return simdata::Vector3(x + (static_cast<int>(x0)-128) * 32000.0, y + (static_cast<int>(y0)-128) * 32000.0, z);
	}

};

inline simdata::Reader & operator >> (simdata::Reader &r, GlobalPosition &p) {
	return r >> p.x >> p.y >> p.z >> p.x0 >> p.y0;
}

inline simdata::Writer & operator << (simdata::Writer &w, GlobalPosition const &p) {
	return w << p.x << p.y << p.z << p.x0 << p.y0;
}

inline std::ostream & operator << (std::ostream &o, GlobalPosition const &p) {
	return o << p.Vector3();
}


class Vector2f {
	float x;
	float y;

	friend simdata::Reader & operator >> (simdata::Reader &r, Vector2f &p);
	friend simdata::Writer & operator << (simdata::Writer &w, Vector2f const &p);
public:

	Vector2f(): x(0), y(0) {}

	Vector2f(float x_, float y_): x(x_), y(y_) { }

	explicit Vector2f(simdata::Vector3 const &v) {
		*this = v;
	}

	inline Vector2f const &operator=(Vector2f const &v) {
		x = v.x;
		y = v.y;
		return *this;
	}

	inline Vector2f const &operator=(simdata::Vector3 const &v) {
		x = static_cast<float>(v.x());
		y = static_cast<float>(v.y());
		return *this;
	}

	inline simdata::Vector3 Vector3() const {
		return simdata::Vector3(x, y, 0.0);
	}

};

inline simdata::Reader & operator >> (simdata::Reader &r, Vector2f &p) {
	return r >> p.x >> p.y;
}

inline simdata::Writer & operator << (simdata::Writer &w, Vector2f const &p) {
	return w << p.x << p.y;
}

inline std::ostream & operator << (std::ostream &o, Vector2f const &p) {
	return o << p.Vector3();
}


class Vector3f {
	float x;
	float y;
	float z;

	friend simdata::Reader & operator >> (simdata::Reader &r, Vector3f &p);
	friend simdata::Writer & operator << (simdata::Writer &w, Vector3f const &p);
public:

	Vector3f(): x(0), y(0), z(0) { }
	Vector3f(float x_, float y_, float z_): x(x_), y(y_), z(z_) { }
	explicit Vector3f(simdata::Vector3 const &v) { *this = v; }

	inline Vector3f const &operator=(Vector3f const &v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	inline Vector3f const &operator=(simdata::Vector3 const &v) {
		x = static_cast<float>(v.x());
		y = static_cast<float>(v.y());
		z = static_cast<float>(v.z());
		return *this;
	}

	inline simdata::Vector3 Vector3() const {
		return simdata::Vector3(x, y, z);
	}

};

inline simdata::Reader & operator >> (simdata::Reader &r, Vector3f &p) {
	return r >> p.x >> p.y >> p.z;
}

inline simdata::Writer & operator << (simdata::Writer &w, Vector3f const &p) {
	return w << p.x << p.y << p.z;
}

inline std::ostream & operator << (std::ostream &o, Vector3f const &p) {
	return o << p.Vector3();
}


class Vector4f {
	float x;
	float y;
	float z;
	float w;

	friend simdata::Reader & operator >> (simdata::Reader &r, Vector4f &p);
	friend simdata::Writer & operator << (simdata::Writer &w, Vector4f const &p);
public:

	Vector4f(): x(0), y(0), z(0), w(0) { }
	Vector4f(float x_, float y_, float z_, float w_): x(x_), y(y_), z(z_), w(w_) { }
	explicit Vector4f(simdata::Quat const &q) { *this = q; }

	inline Vector4f const &operator=(Vector4f const &v) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	inline Vector4f const &operator=(simdata::Quat const &q) {
		x = static_cast<float>(q.x());
		y = static_cast<float>(q.y());
		z = static_cast<float>(q.z());
		w = static_cast<float>(q.w());
		return *this;
	}

	inline simdata::Quat Quat() const {
		return simdata::Quat(x, y, z, w);
	}

};

inline simdata::Reader & operator >> (simdata::Reader &r, Vector4f &p) {
	return r >> p.x >> p.y >> p.z >> p.w;
}

inline simdata::Writer & operator << (simdata::Writer &w, Vector4f const &p) {
	return w << p.x << p.y << p.z << p.w;
}

inline std::ostream & operator << (std::ostream &o, Vector4f const &p) {
	return o << p.Quat();
}

} // namespace simcore

#endif  // __SIMCORE_UTIL_MESSAGETYPES_H__

