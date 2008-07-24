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
 * @file LayoutTransform.cpp
 *
 **/


#include <csp/cspsim/theater/LayoutTransform.h>
#include <csp/csplib/util/osg.h>

namespace csp {

LayoutTransform::LayoutTransform() {
	m_X = 0.0;
	m_Y = 0.0;
	m_Angle = 0.0;
	c = 1.0;
	s = 0.0;
}

LayoutTransform::LayoutTransform(float x, float y, float angle) {
	m_X = x;
	m_Y = y;
	m_Angle = angle;
	c = cos(angle);
	s = sin(angle);
}

LayoutTransform LayoutTransform::operator *(LayoutTransform const &t) const {
	double angle = m_Angle + t.m_Angle;
	double x = m_X + t.m_X * c - t.m_Y * s;
	double y = m_Y + t.m_X * s + t.m_Y * c;
	return LayoutTransform(x, y, angle);
}

osg::Vec3 LayoutTransform::operator()() const {
	return operator()(osg::Vec3(0.0, 0.0, 0.0));
}

osg::Vec3 LayoutTransform::operator()(Vector3 const &offset) const {
	return operator()(toOSG(offset));
}

osg::Vec3 LayoutTransform::operator()(osg::Vec3 const &offset) const {
	return osg::Vec3(m_X + offset.x() * c - offset.y() * s, m_Y + offset.x() * s + offset.y() * c, 0.0);
}

} // namespace csp

