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
 * @file ElevationCorrection.h
 *
 **/

#ifndef __CSPSIM_THEATER_ELEVATIONCORRECTION_H__
#define __CSPSIM_THEATER_ELEVATIONCORRECTION_H__

#include <csp/cspsim/Export.h>
#include <csp/cspsim/theater/LayoutTransform.h>
#include <osg/Vec3>

CSP_NAMESPACE

class TerrainObject;

/**
 * class ElevationCorrection
 *
 * A helper class for correcting the elevation of static objects to match
 * the local terrain.
 */
class CSP_EXPORT ElevationCorrection: public LayoutTransform {
	TerrainObject *m_Terrain;
	ElevationCorrection() {}
public:
	ElevationCorrection(TerrainObject *terrain, float x=0.0, float y=0.0, float angle=0.0);
	osg::Vec3 operator()(osg::Vec3 const &offset) const;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_THEATER_ELEVATIONCORRECTION_H__


