#pragma once
// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file LayoutTransform.h
 *
 **/

#include <csp/cspsim/Export.h>
#include <osg/Vec3>

namespace csp {

class Vector3;

/**
 * class LayoutTransform
 *
 * A helper class for accumulationg and applying 2D displacements and rotations.
 */
class CSPSIM_EXPORT LayoutTransform {
	float m_X, m_Y, m_Angle;
	float c, s;
public:
	LayoutTransform();
	LayoutTransform(float x, float y, float angle);
	LayoutTransform operator *(LayoutTransform const &t) const;
	osg::Vec3 operator()() const;
	osg::Vec3 operator()(Vector3 const &offset) const;
	osg::Vec3 operator()(osg::Vec3 const &offset) const;
	float getAngle() const { return m_Angle; }
};

} // namespace csp
