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
 * @file Theater.h
 *
 * A theater is a large geographic area it which military operations are
 * conducted (dictionary.com).  In CSPSim this include the terrain,
 * natural features, and static man-made objects.  The classes here are
 * used to organize and manage this data.
 *
 **/

#include <csp/cspsim/theater/FeatureGroupList.h>

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Object.h>

class TerrainObject;
class FeatureGroup;

namespace csp {

/**
 * class Theater
 *
 * A Theater represents an area of military operations.  It consists of static
 * data representing the terrain, natural features, and man-made structures.
 */
class CSPSIM_EXPORT Theater: public Object
{
	Link<FeatureGroupList> m_FeatureGroupList;
	Link<TerrainObject> m_Terrain;
public:

	CSP_DECLARE_OBJECT(Theater)

	/**
	 * Return a list of all FeatureGroups in the theater.  Currently only
	 * Objectives are returned.
	 */
	Ref<FeatureGroup>::list getAllFeatureGroups();

	Theater();

	virtual ~Theater();

	/**
	 * Get the TerrainObject used by the theater.
	 */
	Ref<TerrainObject> getTerrain();

	virtual void postCreate();

};

} // namespace csp
