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

#include <vector>

#include <osg/Billboard>
#include <osg/Geometry>
#include <osgUtil/Optimizer>

#include <SimData/Random.h>

#include "Log.h"

#include "Theater/FeatureSceneModel.h"
#include "Theater/FeatureSceneGroup.h"
#include "Theater/FeatureQuad.h"
#include "Theater/LayoutTransform.h"
#include "Theater/ElevationCorrection.h"


SIMDATA_REGISTER_INTERFACE(RandomBillboardModel)

class IsoContour: public simdata::Object {
protected:
	virtual float f(float x, float y) const = 0;
public:
	virtual bool in(float x, float y) const {
		return f(x,y) < 1.0f;
	}
	virtual simdata::Vector3 getPoint(float x,float y) const = 0;
	virtual float getArea() const = 0;
	virtual ~IsoContour() {}
};

class Circle: public IsoContour {
	float m_Radius;
protected:
	virtual float f(float x, float y) const {
		return x*x+y*y;
	}
public:
	SIMDATA_OBJECT(Circle, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(Circle)
		SIMDATA_XML("radius", Circle::m_Radius, true)
	END_SIMDATA_XML_INTERFACE

	Circle(float radius = 20.0f):
	  m_Radius(radius) {
	}
	virtual simdata::Vector3 getPoint(float x, float y) const {
		return simdata::Vector3(m_Radius * x, m_Radius * y, 0.0f);
	}
	virtual float getArea() const {
		return simdata::PI * m_Radius * m_Radius;
	}
	virtual ~Circle() {}
};

class RectangularCurve: public IsoContour {
	float m_Width, m_Height;
protected:
	virtual float f(float x, float y) const {
		return 0.0f;
	}
public:
	SIMDATA_OBJECT(RectangularCurve, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(RectangularCurve)
		SIMDATA_XML("width", RectangularCurve::m_Width, true)
		SIMDATA_XML("height", RectangularCurve::m_Height, true)
	END_SIMDATA_XML_INTERFACE

	RectangularCurve(float width = 20.0f, float height = 50.0f):
		m_Width(width != 0.0f ? abs(width) : 20.0f),
		m_Height(height != 0.0f ? abs(height) : 50.0f) {
			if (width * height == 0.0f) {
				CSP_LOG(APP, WARNING, "Rectangle: [" << width << "," << height << "corrected to 20x50]");  
			}
	}
	virtual simdata::Vector3 getPoint(float x, float y) const {
		return simdata::Vector3(m_Width * x, m_Height * y, 0.0f);
	}
	virtual float getArea() const {
		return m_Width * m_Height;
	}
	virtual ~RectangularCurve(){}
};

SIMDATA_REGISTER_INTERFACE(Circle)
SIMDATA_REGISTER_INTERFACE(RectangularCurve)

int RandomBillboardModel::getFeatureCount() const { 
	return 0; // TODO
}

void RandomBillboardModel::makeFeatures(std::vector<Feature> &features, int value) const {
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
	simdata::random::Taus2 rand;
	rand.setSeed(m_Seed);
	m_Offsets.resize(n);
	for (i = 0; i < n; i++) {
		int count = int(m_Density[i] * area);
		m_Offsets[i].reserve(count);
		while (count > 0) {
			float x = rand.uniform(-1.0, 1.0);
			float y = rand.uniform(-1.0, 1.0);
			if (m_IsoContour->in(x,y)) {
				m_Offsets[i].push_back(m_IsoContour->getPoint(x,y));
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
	osgUtil::Optimizer optimizer;
	optimizer.optimize(group);
}



