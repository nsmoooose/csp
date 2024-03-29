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
 * @file FeatureGroupList.cpp
 *
 **/


#include <csp/cspsim/theater/FeatureGroupList.h>
#include <csp/cspsim/Projection.h>

#include <csp/csplib/data/ObjectInterface.h>


namespace csp {

CSP_XML_BEGIN(FeatureGroupList)
	CSP_DEF("feature_groups", m_FeatureGroups, true)
CSP_XML_END


FeatureGroupList::FeatureGroupList() {
}

FeatureGroupList::~FeatureGroupList() {
}

void FeatureGroupList::projectFeatureGroups(Projection const &map) {
	Link<FeatureGroup>::vector::iterator i = m_FeatureGroups.begin();
	for (; i != m_FeatureGroups.end(); i++) {
		(*i)->project(map);
	}
}

} // namespace csp

