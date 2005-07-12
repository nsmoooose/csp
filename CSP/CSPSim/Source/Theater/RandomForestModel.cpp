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
 * @file RandomForestModel.cpp
 *
 **/


#include "Theater/RandomForestModel.h"
#include "Theater/FeatureSceneModel.h"
#include "Theater/FeatureSceneGroup.h"
#include "Theater/LayoutTransform.h"
#include "Theater/ElevationCorrection.h"
#include "Theater/IsoContour.h"

#include <SimData/Random.h>

#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/StateSet>


SIMDATA_XML_BEGIN(RandomForestModel)
	SIMDATA_DEF("models", m_Models, true)
	SIMDATA_DEF("density", m_Density, true)
	SIMDATA_DEF("minimum_spacing", m_MinimumSpacing, true)
	SIMDATA_DEF("seed", m_Seed, false)
	SIMDATA_DEF("isocontour", m_IsoContour, false)
SIMDATA_XML_END


osg::Geometry *RandomForestModel::construct(simdata::Ref<FeatureQuad> quad, std::vector<osg::Vec3> const &position) const {
	int i, j, k, n = position.size();

	osg::Vec3Array &vex = *(new osg::Vec3Array(n*16));
	osg::Vec3Array &nrm = *(new osg::Vec3Array(4));
	osg::Vec2Array &tex = *(new osg::Vec2Array(n*16));
	osg::Vec4Array &col = *(new osg::Vec4Array(1));
	std::vector<osg::Vec3> proto(16);

	float width = quad->getWidth();
	float height = quad->getHeight();
	float offsetX = quad->getOffsetX();
	float offsetY = quad->getOffsetY();

	proto[ 0].set(width*(offsetX-0.5), 0.0, height*(offsetY-0.5));
	proto[ 1].set(width*(offsetX+0.5), 0.0, height*(offsetY-0.5));
	proto[ 2].set(width*(offsetX+0.5), 0.0, height*(offsetY+0.5));
	proto[ 3].set(width*(offsetX-0.5), 0.0, height*(offsetY+0.5));
	proto[ 4].set(width*(offsetX-0.5), 0.1, height*(offsetY-0.5));
	proto[ 7].set(width*(offsetX+0.5), 0.1, height*(offsetY-0.5));
	proto[ 6].set(width*(offsetX+0.5), 0.1, height*(offsetY+0.5));
	proto[ 5].set(width*(offsetX-0.5), 0.1, height*(offsetY+0.5));
	proto[ 8].set(0.0, width*(offsetX-0.5), height*(offsetY-0.5));
	proto[ 9].set(0.0, width*(offsetX+0.5), height*(offsetY-0.5));
	proto[10].set(0.0, width*(offsetX+0.5), height*(offsetY+0.5));
	proto[11].set(0.0, width*(offsetX-0.5), height*(offsetY+0.5));
	proto[12].set(0.1, width*(offsetX-0.5), height*(offsetY-0.5));
	proto[15].set(0.1, width*(offsetX+0.5), height*(offsetY-0.5));
	proto[14].set(0.1, width*(offsetX+0.5), height*(offsetY+0.5));
	proto[13].set(0.1, width*(offsetX-0.5), height*(offsetY+0.5));

	col[0].set(1.0, 1.0, 1.0, 1.0);
	nrm[0].set(0.0, -1.0, 0.0);
	nrm[1].set(0.0, +1.0, 0.0);
	nrm[2].set(-1.0, 0.0, 0.0);
	nrm[3].set(+1.0, 0.0, 0.0);

	osg::Vec2Array::iterator t = tex.begin();
	osg::Vec3Array::iterator v = vex.begin();
	osg::Vec3Array::const_iterator pos;
	for (k = 0; k < 4; k++) {
		pos = position.begin();
		int proto_idx = k*4;
		for (i = 0; i < n; i++) {
			if (k & 1) {
				t->set(0.0, 0.0); t++;
				t->set(0.0, 1.0); t++;
				t->set(1.0, 1.0); t++;
				t->set(1.0, 0.0); t++;
			} else {
				t->set(0.0, 0.0); t++;
				t->set(1.0, 0.0); t++;
				t->set(1.0, 1.0); t++;
				t->set(0.0, 1.0); t++;
			}
			for (j = 0; j < 4; j++) {
				*v++ = proto[proto_idx + j] + *pos;
			}
			pos++;
		}
	}

	osg::Geometry *geom = new osg::Geometry;
	geom->setVertexArray(&vex);
	geom->setTexCoordArray(0, &tex);
	geom->setColorArray(&col);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	geom->setNormalArray(&nrm);
	geom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
	
	n *= 4;
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,   0, n));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,   n, n));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 2*n, n));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 3*n, n));

	return geom;
}

int RandomForestModel::getFeatureCount() const {
	return 0; // TODO
}

void RandomForestModel::makeFeatures(std::vector<Feature> &, int) const {
}

RandomForestModel::RandomForestModel():
	m_Seed(0),
	m_IsoContour(new RectangularCurve()) {
}

RandomForestModel::~RandomForestModel() {
}

void RandomForestModel::postCreate() {
	assert(m_Density.size() == m_Models.size());
	float area = m_IsoContour->getArea();
	int i, n = m_Models.size();
	simdata::random::Taus2 rand;
	rand.setSeed(m_Seed);
	m_Offsets.resize(n);
	for (i = 0; i < n; i++) {
		int count = int(m_Density[i] * area);
		m_Offsets[i].reserve(count);
		while (count > 0) {
			float x = rand.uniform(-1.0, 1.0);
			float y = rand.uniform(-1.0, 1.0);
			if (m_IsoContour->in(x, y)) {
				m_Offsets[i].push_back(m_IsoContour->getPoint(x, y));
				count--;
			}
		}
	}
}

void RandomForestModel::addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {
	int i, n = m_Models.size();
	std::vector<simdata::Vector3>::iterator ofs, end;
	FeatureSceneModel *scene_model = new FeatureSceneModel(transform);
	for (i = 0; i < n; i++) {
		ofs = m_Offsets[i].begin();
		end = m_Offsets[i].end();
		std::vector<osg::Vec3> pos(m_Offsets[i].size());
		int idx = 0;
		for (; ofs != end; ofs++) {
			pos[idx++] = correction(transform(*ofs));
		}
		osg::Geode *geode = new osg::Geode;
		osg::StateSet *state = m_Models[i]->getStateSet();
		osg::Drawable *forest = construct(m_Models[i], pos);
		geode->addDrawable(forest);
		geode->setStateSet(state);
		scene_model->addChild(geode);
	}
	group->addChild(scene_model);
}












