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
 * @file RandomBillboardModel.cpp
 *
 **/


#include "Theater/RandomBillboardModel.h"
#include "Theater/FeatureSceneModel.h"
#include "Theater/FeatureSceneGroup.h"
#include "Theater/FeatureQuad.h"
#include "Theater/LayoutTransform.h"
#include "Theater/ElevationCorrection.h"

#include <SimData/Random.h>

#include <osg/Billboard>
#include <osg/Geometry>

#include <vector>


SIMDATA_REGISTER_INTERFACE(RandomBillboardModel)



int RandomBillboardModel::getFeatureCount() const { 
	return 0; // TODO
}

void RandomBillboardModel::makeFeatures(std::vector<Feature> &features, int value) const {
}

RandomBillboardModel::RandomBillboardModel() {
	m_Seed = 0;
}

RandomBillboardModel::~RandomBillboardModel() {
}

void RandomBillboardModel::postCreate() {
	assert(m_Density.size() == m_Models.size());
	float radius = 400.0;
	float area = radius * radius;
	int i, n = m_Models.size();
	simdata::Random rand(m_Seed);
	m_Offsets.resize(n);
	for (i = 0; i < n; i++) {
		int count = int(m_Density[i] * area);
		m_Offsets[i].reserve(count);
		while (count > 0) {
			float x = rand.newRand()*2.0-1.0;
			float y = rand.newRand()*2.0-1.0;
			if (x*x + y*y < 1.0) {
				m_Offsets[i].push_back(simdata::Vector3(x*radius, y*radius, 0.0));
				count--;
			}
		}
	}
}

void RandomBillboardModel::addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {
	int i, n = m_Models.size();
	std::vector<simdata::Vector3>::iterator ofs, end;
	FeatureSceneModel *scene_model = new FeatureSceneModel(transform);
	for (i = 0; i < n; i++) {
		osg::Billboard *bb = new osg::Billboard();
		bb->setStateSet(m_Models[i]->getStateSet());
		ofs = m_Offsets[i].begin();
		end = m_Offsets[i].end();
		osg::Geometry *model = m_Models[i]->getGeometry();
		for (; ofs != end; ofs++) {
			bb->addDrawable(new osg::Geometry(*model), correction(transform(*ofs)));
		}
		scene_model->addChild(bb);
	}
	group->addChild(scene_model);
}



