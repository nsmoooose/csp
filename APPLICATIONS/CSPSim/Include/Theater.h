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



#ifndef __THEATER_H__
#define __THEATER_H__


#include <SimData/Ref.h>
#include <SimData/Path.h>
#include <SimData/Object.h>
#include <SimData/InterfaceRegistry.h>


class TerrainObject;
class FeatureGroup;

#include "Theater/FeatureGroupList.h"


/**
 * class Theater
 *
 * A Theater represents an area of military operations.  It consists of static
 * data representing the terrain, natural features, and man-made structures.
 */
class Theater: public simdata::Object
{
	simdata::Link<FeatureGroupList> m_FeatureGroupList;
	simdata::Link<TerrainObject> m_Terrain;
public:

	SIMDATA_OBJECT(Theater, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(Theater)
		SIMDATA_XML("feature_group_list", Theater::m_FeatureGroupList, true)
		SIMDATA_XML("terrain", Theater::m_Terrain, true)
	END_SIMDATA_XML_INTERFACE

	/**
	 * Return a list of all FeatureGroups in the theater.  Currently only
	 * Objectives are returned.
	 */
	simdata::Ref<FeatureGroup>::list getAllFeatureGroups();

	Theater();

	virtual ~Theater();

	virtual void pack(simdata::Packer& p) const {
		Object::pack(p);
		p.pack(m_FeatureGroupList);
		p.pack(m_Terrain);
	}

	virtual void unpack(simdata::UnPacker& p) {
		Object::unpack(p);
		p.unpack(m_FeatureGroupList);
		p.unpack(m_Terrain);
	}

	/**
	 * Get the TerrainObject used by the theater.
	 */
	simdata::Ref<TerrainObject> getTerrain();

	virtual void postCreate();

};


#endif // __THEATER_H__


