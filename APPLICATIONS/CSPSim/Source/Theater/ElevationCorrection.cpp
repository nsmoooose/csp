// Combat Simulator Project - FlightSim Demo
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
 * @file ElevationCorrection.cpp
 *
 **/


#include "Theater/ElevationCorrection.h"
#include "TerrainObject.h"


ElevationCorrection::ElevationCorrection(TerrainObject *terrain, float x, float y, float angle): LayoutTransform(x, y, angle) {
	m_Terrain = terrain;
}

osg::Vec3 ElevationCorrection::operator()(osg::Vec3 const &offset) const {
	osg::Vec3 absolute = LayoutTransform::operator()(offset);
	float elevation = 0.0;
	if (m_Terrain != 0) {
		TerrainObject::IntersectionHint hint;
		elevation = m_Terrain->getGroundElevation(absolute.x(), absolute.y(), hint);
	}
	std::cout << "PLACING FEATURE @ " << absolute << "\n";
	std::cout << "ELEVATION = " << elevation << ", offset = " << offset << "\n";
	return offset + osg::Z_AXIS * elevation;
}


