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
 * @file FeatureLayout.cpp
 *
 **/


#include "Theater/FeatureLayout.h"
#include "Theater/Feature.h"
#include "Theater/FeatureModel.h"
#include "Theater/FeatureSceneGroup.h"
#include "Theater/LayoutTransform.h"
#include "Theater/ElevationCorrection.h"


SIMDATA_REGISTER_INTERFACE(FeatureLayout)


FeatureLayout::FeatureLayout() { 
	m_X = 0.0;
	m_Y = 0.0;
	m_Orientation = 0.0;
	m_ValueModifier = char(0);
}

FeatureLayout::~FeatureLayout() {
}

void FeatureLayout::addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {
	LayoutTransform newTransform = transform * LayoutTransform(m_X, m_Y, m_Orientation); 
	m_FeatureModel->addSceneModel(group, newTransform, correction);
}

void FeatureLayout::makeFeatures(std::vector<Feature> &features, int value) const {
	assert(m_FeatureModel.valid());
	m_FeatureModel->makeFeatures(features, value + int(m_ValueModifier));
}


simdata::Ref<FeatureModel const> FeatureLayout::getFeatureModel() const { 
	return m_FeatureModel; 
}

