/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
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
 * @file osg.h
 *
 * OpenSceneGraph (OSG) math adapters
 */


#ifndef __SIMDATA_OSG_H__
#define __SIMDATA_OSG_H__

#include <SimData/Namespace.h>
#include <SimData/Vector3.h>
#include <SimData/Matrix3.h>
#include <SimData/Quat.h>

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Quat>

NAMESPACE_SIMDATA


inline osg::Vec3 toOSG(Vector3 const &v) {
	return osg::Vec3(v.x(), v.y(), v.z());
}

inline Vector3 fromOSG(osg::Vec3 const &v) {
	return Vector3(v.x(), v.y(), v.z());
}

// transpose: osg matrix convension is v * M
inline osg::Matrix toOSG(Matrix3 const &m) {
	return osg::Matrix(m(0, 0), m(1, 0), m(2, 0), 0.0,
	                   m(0, 1), m(1, 1), m(2, 1), 0.0,
			   m(0, 2), m(1, 2), m(2, 2), 0.0,
			   0.0, 0.0, 0.0, 1.0);
}

// transpose: osg matrix convension is v * M
inline Matrix3 fromOSG(osg::Matrix const &m) {
	return Matrix3(m(0, 0), m(1, 0), m(2, 0),
	               m(0, 1), m(1, 1), m(2, 1),
	               m(0, 2), m(1, 2), m(2, 2));
}

inline osg::Quat toOSG(Quat const &q) {
	return osg::Quat(q.x(), q.y(), q.z(), q.w());
}

inline Quat fromOSG(osg::Quat const &q) {
	return Quat(q.x(), q.y(), q.z(), q.w());
}


NAMESPACE_SIMDATA_END


#endif //__SIMDATA_OSG_H__

