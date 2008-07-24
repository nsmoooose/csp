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
 * @file CustomLayoutModel.cpp
 *
 **/


#include <csp/cspsim/theater/CustomLayoutModel.h>
#include <csp/cspsim/theater/FeatureLayout.h>

#include <csp/csplib/data/ObjectInterface.h>

#include <vector>

namespace csp {

CSP_XML_BEGIN(CustomLayoutModel)
	CSP_DEF("layout", m_FeatureLayout, true)
CSP_XML_END


int CustomLayoutModel::getFeatureCount() const {
	if (m_FeatureCount == 0) {
		Link<FeatureLayout>::vector::const_iterator i = m_FeatureLayout.begin();
		Link<FeatureLayout>::vector::const_iterator j = m_FeatureLayout.end();
		for (; i != j; i++) m_FeatureCount += (*i)->getFeatureModel()->getFeatureCount();
	}
	return m_FeatureCount;
}

void CustomLayoutModel::makeFeatures(std::vector<Feature> &features, int value) const {
	Link<FeatureLayout>::vector::const_iterator i = m_FeatureLayout.begin();
	Link<FeatureLayout>::vector::const_iterator j = m_FeatureLayout.end();
	for (; i != j; i++) {
		(*i)->makeFeatures(features, value);
	}
}

void CustomLayoutModel::addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {
	Link<FeatureLayout>::vector::iterator i = m_FeatureLayout.begin();
	Link<FeatureLayout>::vector::iterator j = m_FeatureLayout.end();
	for (; i != j; i++) {
		(*i)->addSceneModel(group, transform, correction);
	}
}

CustomLayoutModel::CustomLayoutModel() { m_FeatureCount = 0; }

CustomLayoutModel::~CustomLayoutModel() {}

} // namespace csp

