#pragma once
/* Combat Simulator Project
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
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
 * @file osg.h
 *
 * OpenSceneGraph (OSG) math adapters
 *
 * See http://www.openscenegraph.org
 */

#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Quat.h>

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Quat>

namespace csp {

/**
 * @defgroup Adapters Class adapter utilities
 */
//@{

/** convert csp::Vector3 to osg::Vec3 */
inline osg::Vec3 toOSG(Vector3 const &v) {
	return osg::Vec3(static_cast<float>(v.x()),
	                 static_cast<float>(v.y()),
	                 static_cast<float>(v.z()));
}

/** convert osg::Vec3 to csp::Vector3 */
inline Vector3 fromOSG(osg::Vec3 const &v) {
	return Vector3(v.x(), v.y(), v.z());
}

/** convert csp::Matrix3 to osg::Matrix */
inline osg::Matrix toOSG(Matrix3 const &m) {
	// transpose: osg matrix convension is v * M
	return osg::Matrix(m(0, 0), m(1, 0), m(2, 0), 0.0,
	                   m(0, 1), m(1, 1), m(2, 1), 0.0,
	                   m(0, 2), m(1, 2), m(2, 2), 0.0,
	                   0.0, 0.0, 0.0, 1.0);
}

/** convert osg::Matrix to csp::Matrix3 */
inline Matrix3 fromOSG(osg::Matrix const &m) {
	// transpose: osg matrix convension is v * M
	return Matrix3(m(0, 0), m(1, 0), m(2, 0),
	               m(0, 1), m(1, 1), m(2, 1),
	               m(0, 2), m(1, 2), m(2, 2));
}

/** convert osg::Quat to csp::Quat */
inline osg::Quat toOSG(Quat const &q) {
	return osg::Quat(static_cast<float>(q.x()),
	                 static_cast<float>(q.y()),
	                 static_cast<float>(q.z()),
	                 static_cast<float>(q.w()));
}

/** convert csp::Quat to osg::Quat */
inline Quat fromOSG(osg::Quat const &q) {
	return Quat(q.x(), q.y(), q.z(), q.w());
}


//@}


} // namespace csp
