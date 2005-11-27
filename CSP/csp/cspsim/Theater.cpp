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
 * @file Theater.cpp
 *
 **/


#include "Theater.h"
#include "Theater/FeatureGroup.h"
#include "Theater/FeatureGroupList.h"
#include "TerrainObject.h"

#include <csp/csplib/util/Log.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Path.h>
#include <csp/csplib/util/Ref.h>


SIMDATA_XML_BEGIN(Theater)
	SIMDATA_DEF("feature_group_list", m_FeatureGroupList, true)
	SIMDATA_DEF("terrain", m_Terrain, true)
SIMDATA_XML_END



simdata::Ref<FeatureGroup>::list Theater::getAllFeatureGroups() {
	simdata::Link<FeatureGroup>::vector const &groups = m_FeatureGroupList->getFeatureGroups();
	simdata::Ref<FeatureGroup>::list result;
	// translate from Link<> list to Ref<> list.  XXX should be a simdata function?
	simdata::Link<FeatureGroup>::vector::const_iterator i = groups.begin();
	for (; i != groups.end(); i++) {
		result.push_back(*i);
	}
	return result;
}

Theater::Theater() {
}

Theater::~Theater() {
}

void Theater::postCreate() {
	CSP_LOG(TERRAIN, INFO, "Projecting feature groups.");
	m_FeatureGroupList->projectFeatureGroups(*(m_Terrain->getProjection()));
	CSP_LOG(TERRAIN, INFO, "Projecting feature groups done.");
}

simdata::Ref<TerrainObject> Theater::getTerrain() {
	return m_Terrain;
}

