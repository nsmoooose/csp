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
 * @file FeatureObjectModel.cpp
 *
 **/


#include "Theater/FeatureObjectModel.h"
#include "Theater/FeatureSceneGroup.h"
#include "Theater/FeatureSceneModel.h"
#include "Theater/LayoutTransform.h"
#include "Theater/ElevationCorrection.h"
#include "ObjectModel.h"

#include <vector>



SIMDATA_REGISTER_INTERFACE(FeatureObjectModel)



void FeatureObjectModel::addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {
	assert(group != 0);
	FeatureSceneModel *model;
	if (m_ObjectModel->getElevationCorrection()) {
		model = new FeatureSceneModel(transform, correction);
	} else {
		model = new FeatureSceneModel(transform);
	}
	model->addChild(m_ObjectModel->getModel().get());
	group->addChild(model);
}

void FeatureObjectModel::makeFeatures(std::vector<Feature> &features, int value) const {
	features.push_back(Feature(this, value + int(m_Value)));
}

FeatureObjectModel::FeatureObjectModel(): m_HitPoints(10), m_Value(0) {
}

FeatureObjectModel::~FeatureObjectModel() {
}



