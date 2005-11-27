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
 * @file RandomBillboardModel.cpp
 *
 **/


#include <csp/cspsim/theater/RandomBillboardModel.h>
#include <csp/cspsim/theater/IsoContour.h>
#include <csp/cspsim/theater/FeatureSceneModel.h>
#include <csp/cspsim/theater/FeatureSceneGroup.h>
#include <csp/cspsim/theater/FeatureQuad.h>
#include <csp/cspsim/theater/LayoutTransform.h>
#include <csp/cspsim/theater/ElevationCorrection.h>

#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/util/Random.h>

#include <vector>

#include <osg/Billboard>
#include <osg/Geometry>
#include <osgUtil/Optimizer>

CSP_NAMESPACE

CSP_XML_BEGIN(RandomBillboardModel)
	CSP_DEF("models", m_Models, true)
	CSP_DEF("density", m_Density, true)
	CSP_DEF("minimum_spacing", m_MinimumSpacing, true)
	CSP_DEF("seed", m_Seed, false)
	CSP_DEF("isocontour", m_IsoContour, false)
CSP_XML_END


int RandomBillboardModel::getFeatureCount() const {
	return 0; // TODO
}

void RandomBillboardModel::makeFeatures(std::vector<Feature> &, int) const {
}

RandomBillboardModel::RandomBillboardModel():
	m_Seed(0),
	m_IsoContour(new RectangularCurve()) {
}

RandomBillboardModel::~RandomBillboardModel() {
}

void RandomBillboardModel::postCreate() {
	assert(m_Density.size() == m_Models.size());
	float area = m_IsoContour->getArea();
	int i, n = m_Models.size();
	random::Taus2 rand;
	rand.setSeed(m_Seed);
	m_Offsets.resize(n);
	for (i = 0; i < n; i++) {
		int count = int(m_Density[i] * area);
		m_Offsets[i].reserve(count);
		while (count > 0) {
			float x = rand.uniform(-1.0, 1.0);
			float y = rand.uniform(-1.0, 1.0);
			if (m_IsoContour->in(x, y)) {
				m_Offsets[i].push_back(m_IsoContour->getPoint(x,y));
				count--;
			}
		}
	}
}

void RandomBillboardModel::addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {
	int i, n = m_Models.size();
	std::vector<Vector3>::iterator ofs, end;
	
	// the drawables in the billboard are already offset to the correct positions
	// relative to the root scene group (ie. including all parent transformations),
	// so the scene model should not reapply the same transformations.
	// alternatively, we could set the transform in the scene model and set the
	// x,y offsets of the drawables to be the local offsets.  since each drawable
	// will generally have a unique z position, the latter doesn't offer any
	// obvious advantage.
	//FeatureSceneModel *scene_model = new FeatureSceneModel(transform);
	FeatureSceneModel *scene_model = new FeatureSceneModel(LayoutTransform());

	for (i = 0; i < n; i++) {
		osg::Billboard *bb = new osg::Billboard();
		bb->setStateSet(m_Models[i]->getStateSet());
		ofs = m_Offsets[i].begin();
		end = m_Offsets[i].end();
		osg::Geometry *model = m_Models[i]->getGeometry();
		for (; ofs != end; ofs++) {
			// pos has the x, y coordinates of the drawable relative to the root
			// FeatureSceneGroup.
			osg::Vec3 pos = correction(transform(*ofs));
			bb->addDrawable(new osg::Geometry(*model), pos);
			//bb->addDrawable(model, pos);
		}
		scene_model->addChild(bb);
	}
	group->addChild(scene_model);
	// FIXME this should be done at a higher level when the full FSG is complete
	//osgUtil::Optimizer optimizer;
	//optimizer.optimize(group);
}

CSP_NAMESPACE_END

