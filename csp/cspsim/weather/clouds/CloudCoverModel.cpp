#include <osg/Group>
#include <osg/LOD>
#include <osgDB/WriteFile>

#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/util/osg.h>
#include <csp/csplib/util/Random.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/theater/FeatureSceneGroup.h>
#include <csp/cspsim/theater/FeatureSceneModel.h>
#include <csp/cspsim/theater/LayoutTransform.h>
#include <csp/cspsim/weather/clouds/CloudCoverModel.h>

namespace csp {
namespace weather {
namespace clouds {

CSP_XML_BEGIN(CloudCoverModel)
CSP_DEF("models", m_Models, false)
CSP_DEF("density", m_Density, false)
CSP_DEF("zmin", m_Zmin, false)
CSP_DEF("zmax", m_Zmax, false)
CSP_DEF("seed", m_Seed, false)
CSP_DEF("isocontour", m_IsoContour, false)
CSP_XML_END

void CloudCoverModel::postCreate() {
	assert(m_Density.size() == m_Models.size());
	float area = m_IsoContour->getArea();
	int i, n = m_Models.size();
	random::Taus2 rand;
	rand.setSeed(m_Seed);
	for (i = 0; i < n; i++) {
		int count = int(m_Density[i] * area);
		while (count > 0) {
			float x = rand.uniform(-1.0, 1.0);
			float y = rand.uniform(-1.0, 1.0);
			float z = rand.uniform(m_Zmin, m_Zmax);
			if (m_IsoContour->in(x, y)) {
				Vector3 point = m_IsoContour->getPoint(x,y);
				point.z() = z;
				m_Layout.push_back(CloudPos(point, m_Models[i]));
				count--;
			}
		}
	}
}

void CloudCoverModel::addSceneModel(
	FeatureSceneGroup *group, LayoutTransform const & /* transform */,
	ElevationCorrection const & /* correction */) {

	if(!getScreenSettings().clouds) {
		return;
	}

	FeatureSceneModel *scene_model = new FeatureSceneModel(LayoutTransform());
	LayoutVector::iterator it = m_Layout.begin();
	for(;it != m_Layout.end();++it) {
		osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform();
		transform->setPosition(toOSG(it->first));
		transform->addChild(it->second->getNode());
		osg::StateSet* stateset = transform->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		scene_model->addChild(transform.get());
	}
	group->addChild(scene_model);
}

}
}
}
