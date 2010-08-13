#include <csp/cspsim/sky/OrbitalBodyImposter.h>
#include <csp/cspsim/sky/OrbitalBodyModel.h>
#include <csp/cspsim/sky/Sky.h>
#include <csp/cspsim/sky/SkyDome.h>
#include <csp/cspsim/sky/SkyGroup.h>
#include <csp/cspsim/sky/SolarSystem.h>
#include <csp/cspsim/sky/StarDome.h>
#include <osg/LightSource>
#include <osgDB/ReadFile>
#include <osgUtil/CullVisitor>

namespace csp {

SkyGroup::SkyGroup() {
	osg::ref_ptr<Sky> sky = new Sky(1e+6);

	Ref<SolarSystem> ss = sky->getSolarSystem();
	osg::ref_ptr<OrbitalBodyImposter> moon_imposter = new OrbitalBodyImposter(osgDB::readImageFile("moon.png"), 1.0);
	Ref<OrbitalBodyModel> moon = new OrbitalBodyModel(moon_imposter.get(), 0.0, ss->moon(), ss->earth(), 0.01);
	sky->addModel(moon.get());
	sky->addSunlight(0);
	sky->addMoonlight(1);
	
	addChild(sky->getSunlight());
	addChild(sky->getMoonlight());
	addChild(sky->group());
	
	m_sky = sky;
}

bool SkyGroup::computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const {
	osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
	if (cv) {
		osg::Vec3 eyePointLocal = cv->getEyeLocal();
		matrix.preMult(osg::Matrix::translate(eyePointLocal.x(), eyePointLocal.y(), eyePointLocal.z()));
	}
	return true;
}

bool SkyGroup::computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const {
	osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
	if (cv) {
		osg::Vec3 eyePointLocal = cv->getEyeLocal();
		matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(), -eyePointLocal.y(), -eyePointLocal.z()));
	}
	return true;
}

Sky* SkyGroup::sky() {
	return m_sky.get();
}

} // namespace csp
