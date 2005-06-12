// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file VirtualScene.cpp
 *
 **/

#include "VirtualScene.h"
#include "TerrainObject.h"
#include "DynamicObject.h"
#include "Theater/FeatureGroup.h"
#include "Theater/FeatureSceneGroup.h"
#include "CSPSim.h"
#include "Config.h"
#include "Sky.h"
#include "Animation.h"
#include "ObjectModel.h"
#include "glDiagnostics.h"
#include "SceneConstants.h"

#include <SimCore/Util/Log.h>
#include <SimData/Types.h>
#include <SimData/Math.h>
#include <SimData/FileUtility.h>
#include <SimData/osg.h>

#include <osg/Fog>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/ColorMatrix>
#include <osg/LightSource>
#include <osg/Material>
#include <osg/Node>
#include <osg/Notify>
#include <osg/PolygonMode>
#include <osg/PositionAttitudeTransform>
#include <osg/StateSet>
#include <osgUtil/CullVisitor>
#include <osgUtil/IntersectVisitor>
#include <osgUtil/Optimizer>
#include <osgUtil/SceneView>

#include <cmath>
#include <cassert>

// SHADOW is an *extremely* experimental feature.  It is based on the
// osgShadow demo, and does (did) work to some extent, but only for a
// single localized object.  A more robust approach needs to be taken
// but the code remains for reference.
#define NOSHADOW
#ifdef SHADOW
#include "shadow.h"
#endif

using simdata::Ref;


///////////////////////////////////////////////////////////////////////
// testing
///////////////////////////////////////////////////////////////////////

float intensity_test = 0.0;

void setNVG(osg::ColorMatrix* cm) {
	assert(cm);
	// 0.212671*R + 0.71516*G + 0.072169*B;
	cm->setMatrix(osg::Matrix(
		0.4*0.213, 1.0*0.213, 0.4*0.213, 0.0,
		0.4*0.715, 1.0*0.715, 0.4*0.715, 0.0,
		0.4*0.072, 1.0*0.072, 0.4*0.072, 0.0,
		0.0, 0.0, 0.0, 1.0));
}

void setCM(osg::ColorMatrix* cm, float intensity) {
	assert(cm);
	float sat = intensity*intensity*10.0;
	if (sat < 0.0) sat = 0.0;
	if (sat > 1.0) sat = 1.0;
	float des = 1.0 - sat;
	// 0.212671*R + 0.71516*G + 0.072169*B;
	cm->setMatrix(osg::Matrix(
		sat + (1.0 - sat)*0.213, des*0.213, des*0.213, 0.0,
		0.715*des, sat + (1.0 - sat)*0.715, des*0.715, 0.0,
		des*0.072, des*0.072, sat + (1.0 - sat)*0.072, 0.0,
		0.0, 0.0, 0.0, 1.0));
}

osg::Matrix getCM(float intensity) {
	float sat = intensity*intensity*10.0;
	if (sat < 0.0) sat = 0.0;
	if (sat > 1.0) sat = 1.0;
	float des = 1.0 - sat;
	// 0.212671*R + 0.71516*G + 0.072169*B;
	return osg::Matrix(
		sat + (1.0 - sat)*0.213, des*0.213, des*0.213, 0.0,
		0.715*des, sat + (1.0 - sat)*0.715, des*0.715, 0.0,
		des*0.072, des*0.072, sat + (1.0 - sat)*0.072, 0.0,
		0.0, 0.0, 0.0, 1.0);
}


///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////

ContextIDFactory::ContextIDFactory():
	m_NextContextID(1) {
}

unsigned int ContextIDFactory::getOrCreateContextID(osgUtil::SceneView *scene_view) {
	ContextIDSet::const_iterator i = m_ContextIDSet.find(scene_view);
	if (i == m_ContextIDSet.end()) {
		osg::State *state = scene_view->getState();
		state->setContextID(m_NextContextID);
		m_ContextIDSet[scene_view] = m_NextContextID++;
	}
	return m_ContextIDSet[scene_view];
}

class FeatureTile: public osg::PositionAttitudeTransform {
	simdata::Vector3 m_GlobalPosition;
	/* experiment
	osg::ref_ptr<osg::BlendColor> m_BlendColor;
	*/
public:
	FeatureTile(simdata::Vector3 const &origin): m_GlobalPosition(origin) {
		/* experiment
		m_BlendColor = new osg::BlendColor;
		getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA), osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
		getOrCreateStateSet()->setAttributeAndModes(m_BlendColor.get(), osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
		*/
	}
	typedef osg::ref_ptr<FeatureTile> Ref;
	inline void updateOrigin(simdata::Vector3 const &origin) {
		simdata::Vector3 offset = m_GlobalPosition - origin;
		setPosition(osg::Vec3(offset.x(), offset.y(), offset.z()));
		/** experiment with feature fading.  fading full tiles is efficient but too coarse.  might work for
		 *  smaller tiles (say 10x10km) but that adds overhead.  could drop into tiles that are in the fade
		 *  zone and blend individual features.  can probably get away with updating the blendcolor every
		 *  few frames.
		double distance = origin.length();
		float alpha = std::min(static_cast<float>(distance - 30000.0) / 10000.0f, 1.0f);
		m_BlendColor->setConstantColor(osg::Vec4(alpha, alpha, alpha, alpha));
		*/
	}
};

void VirtualScene::_updateOrigin(simdata::Vector3 const &origin) {
	m_Origin = origin;
	FeatureTileMap::iterator titer = m_FeatureTiles.begin();
	for (; titer != m_FeatureTiles.end(); ++titer) {
		titer->second->updateOrigin(origin);
	}
	DynamicObjectList::iterator diter = m_DynamicObjects.begin();
	for (; diter != m_DynamicObjects.end(); ++diter) {
		(*diter)->updateScene(origin);
	}
}

simdata::Vector3 VirtualScene::getFeatureOrigin(simdata::Ref<FeatureGroup> const &feature) const {
	double x = floor(feature->getGlobalPosition().x() * m_FeatureTileScale) * m_FeatureTileSize;
	double y = floor(feature->getGlobalPosition().y() * m_FeatureTileScale) * m_FeatureTileSize;
	return simdata::Vector3(x, y, 0.0);
}

int VirtualScene::_getFeatureTileIndex(simdata::Ref<FeatureGroup> feature) const {
	int x = static_cast<int>(feature->getGlobalPosition().x() * m_FeatureTileScale);
	int y = static_cast<int>(feature->getGlobalPosition().y() * m_FeatureTileScale);
	// the multiplier below is somewhat arbitrary (needs to be big enough to
	// separate x and y, and small enough not to overflow).
	return y * 30000 + x;
}

/** Called by the scene manager to add a feature to the scene.
 *  The feature's scene group must be constructed *before* calling
 *  this method.
 */
void VirtualScene::addFeature(simdata::Ref<FeatureGroup> feature) {
	int index = _getFeatureTileIndex(feature);
	FeatureTileMap::iterator iter = m_FeatureTiles.find(index);
	FeatureTileRef tile;
	if (iter == m_FeatureTiles.end()) {
		simdata::Vector3 origin = getFeatureOrigin(feature);
		tile = new FeatureTile(origin);
		m_FeatureTiles[index] = tile;
		m_FeatureGroup->addChild(tile.get());
		CSP_LOG(SCENE, DEBUG, "adding new feature cell, index = " << index);
	} else {
		tile = iter->second;
	}
	CSP_LOG(SCENE, DEBUG, "adding feature " << *feature << " to scene");
	FeatureSceneGroup *scene_group = feature->getSceneGroup();
	assert(scene_group != 0);
	if (scene_group == 0) return;

	feature->enterScene();
	tile->addChild(scene_group);
}

/**
*/
void VirtualScene::removeFeature(simdata::Ref<FeatureGroup> feature) {
	int index = _getFeatureTileIndex(feature);
	FeatureTileMap::iterator iter = m_FeatureTiles.find(index);
	assert(iter != m_FeatureTiles.end());
	if (iter == m_FeatureTiles.end()) return;
	FeatureSceneGroup *scene_group = feature->getSceneGroup();
	assert(scene_group != 0);
	if (scene_group == 0) return;
	FeatureTile* tile = iter->second.get();
	tile->removeChild(scene_group);
	feature->leaveScene();
	CSP_LOG(SCENE, DEBUG, "removing feature " << *feature << " from scene");
	if (tile->getNumChildren() == 0) {
		m_FeatureGroup->removeChild(tile);
		m_FeatureTiles.erase(iter);
		CSP_LOG(SCENE, DEBUG, "removing empty feature cell, index = " << index);
	}
	// TODO feature->discardSceneGroup() ??
}


VirtualScene::VirtualScene(int width, int height):
	m_ViewDistance(30000.0),
	m_ViewAngle(60.0),
	m_NearPlane(2.0),
	m_Aspect(1.0f * width / height),
	m_Wireframe(false),
	m_SpinTheWorld(false),
	m_ResetTheWorld(false),
	m_FeatureTileSize(40000.0), // 40 km
	m_FeatureTileScale(1.0 / m_FeatureTileSize),
	m_ScreenWidth(width),
	m_ScreenHeight(height),
	m_NodeMask(0) {
}

VirtualScene::~VirtualScene() { }

void VirtualScene::init() {
	// set up the display settings that will be shared by all sceneviews.
	m_DisplaySettings = new osg::DisplaySettings();
	m_DisplaySettings->setDefaults();

	// create the global state shared by all scene views (which access a single gl context).
	m_GlobalState = new osg::State();

	// configure the default state for all scene graphs in the various scene views.
	m_GlobalStateSet = new osg::StateSet();
	m_GlobalStateSet->setGlobalDefaults();
	// custom default settings
	m_GlobalStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	m_GlobalStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	// set up an alphafunc by default to speed up blending operations.
	osg::AlphaFunc* alphafunc = new osg::AlphaFunc;
	alphafunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
	m_GlobalStateSet->setAttributeAndModes(alphafunc, osg::StateAttribute::ON);
	// set up an texture environment by default to speed up blending operations.
	osg::TexEnv* texenv = new osg::TexEnv;
	texenv->setMode(osg::TexEnv::MODULATE);
	m_GlobalStateSet->setTextureAttributeAndModes(0, texenv, osg::StateAttribute::ON);

	m_FrameStamp = new osg::FrameStamp;
}

void VirtualScene::createSceneViews() {
	createVeryFarView();
	createFarView();
	createNearView();
	createInfoView();
}

osgUtil::SceneView *VirtualScene::makeSceneView() {
	osgUtil::SceneView *sv = new osgUtil::SceneView(m_DisplaySettings.get());
	sv->setDefaults(osgUtil::SceneView::COMPILE_GLOBJECTS_AT_INIT);
	sv->setViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
	sv->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	// override default HEADLIGHT mode, we provide our own lights.
	sv->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);
	// all scene views share a common gl context
	sv->setState(m_GlobalState.get());
	sv->setGlobalStateSet(m_GlobalStateSet.get());
	sv->setFrameStamp(m_FrameStamp.get());
	// default cull settings
	sv->getCullVisitor()->setImpostorsActive(true);
	sv->getCullVisitor()->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	sv->getCullVisitor()->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
	sv->setCullMask(SceneMasks::CULL_ONLY | SceneMasks::NORMAL);
	// default update settings
	sv->getUpdateVisitor()->setTraversalMask(SceneMasks::UPDATE_ONLY | SceneMasks::NORMAL);
	return sv;
}

void VirtualScene::createVeryFarView() {
	m_VeryFarView = makeSceneView();
	m_VeryFarGroup = new osg::Group;
	m_VeryFarGroup->setName("very_far_group");
	m_VeryFarView->setSceneData(m_VeryFarGroup.get());
}

void VirtualScene::createFarView() {
	m_FarView = makeSceneView();
	// clear the depth buffer (but not the color buffer)
	m_FarView->getRenderStage()->setClearMask(GL_DEPTH_BUFFER_BIT);
	m_FarGroup = new osg::Group;
	m_FarGroup->setName("far_group");
	m_FarView->setSceneData(m_FarGroup.get());
}

void VirtualScene::createNearView() {
	m_NearView = makeSceneView();
	// clear the depth buffer (but not the color buffer)
	m_NearView->getRenderStage()->setClearMask(GL_DEPTH_BUFFER_BIT);
	m_NearView->getCullVisitor()->setImpostorsActive(false);
	m_NearGroup = new osg::Group;
	m_NearGroup->setName("near_group");
	m_NearView->setSceneData(m_NearGroup.get());
}

void VirtualScene::createInfoView() {
	m_InfoView = makeSceneView();

	// clear the depth buffer (but not the color buffer)
	m_InfoView->getRenderStage()->setClearMask(GL_DEPTH_BUFFER_BIT);

	m_InfoGroup = new osg::Group;
	m_InfoGroup->setName("info_group");
	m_InfoView->setSceneData(m_InfoGroup.get());
}

void VirtualScene::buildScene() {
	CSP_LOG(APP, INFO, "VirtualScene::buildScene() ");

	/////////////////////////////////////
	//(Un)comment to (enable) disable debug info from osg
	//osg::setNotifyLevel(osg::DEBUG_INFO);
	/////////////////////////////////////

	assert(!m_GlobalState.valid());  // only call once!

	init();
	createSceneViews();

	m_FreeObjectGroup = new osg::Group;
	m_FreeObjectGroup->setName("free_object_group");

	m_FeatureGroup = new osg::PositionAttitudeTransform;
	m_FeatureGroup->setName("feature_group");

	m_ObjectGroup = new osg::Group;
	m_ObjectGroup->setName("object_group");
	m_ObjectGroup->addChild(m_FeatureGroup.get());
	m_ObjectGroup->addChild(m_FreeObjectGroup.get());

	// construct the skydome, stars, moon, sunlight, and moonlight
	buildSky();

	osg::ClearNode* background = new osg::ClearNode;
	// the horizon fan and sky now completely enclose the camera so there
	// is no longer any need to clear the background for normal rendering.
	// blanking is only needed for wireframe mode.
	background->setRequiresClear(true);
	background->setClearColor(osg::Vec4(0.6, 0.3, 0.4, 1.0));
	background->addChild(m_Sky.get());

	m_VeryFarGroup->addChild(background);

	m_TerrainGroup = new osg::PositionAttitudeTransform;
	m_TerrainGroup->setName("terrain_group");

	m_FogGroup = new osg::Group;
	m_FogGroup->setName("fog_group");
	m_FogGroup->addChild(m_TerrainGroup.get());
	m_FogGroup->addChild(m_ObjectGroup.get());

	m_GlobalFrame = new osg::PositionAttitudeTransform;
	m_GlobalFrame->setName("global_frame");
	//m_GlobalFrame->setCullingActive(false);
	//m_GlobalFrame->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);

	m_ParticleEmitterGroup = new osg::Group;
	m_ParticleEmitterGroup->setName("particle_emitter_group");
	//m_ParticleEmitterGroup->setCullingActive(false);
	m_ParticleUpdaterGroup = new osg::Group;
	m_ParticleUpdaterGroup->setName("particle_updater_group");
	//m_ParticleUpdaterGroup->setCullingActive(false);

	m_FarGroup->addChild(m_SkyLights.get());
	m_FarGroup->addChild(m_FogGroup.get());
	m_FarGroup->addChild(m_ParticleEmitterGroup.get());
	m_FarGroup->addChild(m_GlobalFrame.get());
	m_FarGroup->addChild(m_ParticleUpdaterGroup.get());

	// fog properties: start and end distances are read from CSPSim.ini
	osg::StateSet * pFogState = m_FogGroup->getOrCreateStateSet();
	osg::Fog* fog = new osg::Fog;
	fog->setMode(osg::Fog::LINEAR);
	fog->setFogCoordinateSource(osg::Fog::FRAGMENT_DEPTH);
	fog->setDensity(0.3f);
	pFogState->setAttributeAndModes(fog, m_FogEnabled ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
	m_FogGroup->setStateSet(pFogState);

	m_NearObjectGroup = new osg::Group;
	m_NearGroup->addChild(m_SkyLights.get());
	m_NearGroup->addChild(m_NearObjectGroup.get());

	//FIXME: why doesn't ALL_OPTIMIZATIONS work as expected?
	//osgUtil::Optimizer opt;
	//opt.optimize(m_FarGroup.get(), osgUtil::Optimizer::COMBINE_ADJACENT_LODS);
	//opt.optimize(m_FarGroup.get(), osgUtil::Optimizer::SHARE_DUPLICATE_STATE);
}


void VirtualScene::buildSky() {
	osg::StateSet* globalStateSet = m_FarView->getGlobalStateSet();
	assert(globalStateSet);

	// build the skydome, stars, and moon
	m_Sky = new Sky;

	// sunlight
	osg::Light *sun_light = m_Sky->getSunLight();
	osg::LightSource *sun_light_source = new osg::LightSource;
	sun_light_source->setLight(sun_light);
	sun_light_source->setLocalStateSetModes(osg::StateAttribute::ON);
	sun_light_source->setStateSetModes(*globalStateSet, osg::StateAttribute::ON);

	// moonlight
	osg::Light *moon_light = m_Sky->getMoonLight();
	osg::LightSource *moon_light_source = new osg::LightSource;
	moon_light_source->setLight(moon_light);
	moon_light_source->setLocalStateSetModes(osg::StateAttribute::ON);
	moon_light_source->setStateSetModes(*globalStateSet, osg::StateAttribute::ON);

	// light group under EyeTransform for celestial bodies (sun, moon, etc)
	m_SkyLights = new osg::Group;
	m_SkyLights->addChild(sun_light_source);
	m_SkyLights->addChild(moon_light_source);
}

void VirtualScene::drawVeryFarView() {
	m_VeryFarView->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, 0.8 * m_ViewDistance, 1.2e+6);
	m_VeryFarView->update();
	m_VeryFarView->cull();
	m_VeryFarView->draw();
}

void VirtualScene::drawFarView() {
	m_FarView->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, m_NearPlane, m_ViewDistance);
	m_FarView->update();
	m_FarView->cull();
	m_FarView->draw();
}

void VirtualScene::drawNearView() {
	if (m_NearObjectGroup->getNumChildren() > 0) {
		assert(m_NearObjectGroup->getNumChildren() == 1);
		m_NearView->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, 0.01f, 100.0);
		m_NearView->update();
		m_NearView->cull();
		m_NearView->draw();
	}
}

osg::Group *VirtualScene::getInfoGroup() {
	return m_InfoGroup.get();
}

void VirtualScene::drawInfoView() {
	static int i = 0;
	if ((++i % 3) == 1) m_InfoView->update();
	m_InfoView->cull();
	m_InfoView->draw();
}

int VirtualScene::drawScene() {
	CSP_LOG(APP, DEBUG, "VirtualScene::drawScene()...");
	drawVeryFarView();
	drawFarView();
	drawNearView();
	drawInfoView();
	if (m_Terrain.valid()) m_Terrain->endDraw();
	return 1;
}

void VirtualScene::onUpdate(float dt) {
	static float t = 0.0;

	if (m_SpinTheWorld || m_ResetTheWorld || (int(t) % 10) == 0 ||
			(m_SkyPoint - m_Origin).length2() > 25.0e+6) {
		m_SkyPoint = m_Origin;
		if (m_ResetTheWorld) {
			m_Sky->spinTheWorld(false);
			m_ResetTheWorld = false;
		}
		if (m_SpinTheWorld) m_Sky->spinTheWorld();
		if (m_Terrain.valid()) {
			simdata::LLA m = m_Terrain->getProjection()->convert(m_Origin);
			m_Sky->update(m.latitude(), m.longitude(), CSPSim::theSim->getCurrentTime());
		} else {
			m_Sky->update(0.0, 0.0, CSPSim::theSim->getCurrentTime());
		}
		// greenwich, england (for testing)
		//m_Sky->update(0.8985, 0.0, CSPSim::theSim->getCurrentTime());
		t = 1.0;
	} else {
		t += dt;
	}

	CSP_LOG(APP, DEBUG, "VirtualScene::onUpdate - entering" );

	m_FrameStamp->setReferenceTime(m_FrameStamp->getReferenceTime() + dt);
	m_FrameStamp->setFrameNumber(m_FrameStamp->getFrameNumber() + 1);

	CSP_LOG(APP, DEBUG, "VirtualScene::onUpdate - leaving" );
}


void VirtualScene::setCameraNode(osg::Node *) {
}

std::string VirtualScene::logLookAt() {
	osg::Vec3 _camEyePos;
	osg::Vec3 _camLookPos;
	osg::Vec3 _camUpVec;
	m_FarView->getViewMatrixAsLookAt(_camEyePos, _camLookPos, _camUpVec);

	std::ostringstream os("VirtualScene::_setLookAt - eye: ");
	os << _camEyePos << ", look: " << _camLookPos << ", up: " << _camUpVec;
	return os.str();
}

void VirtualScene::_setLookAt(const simdata::Vector3& eyePos, const simdata::Vector3& lookPos, const simdata::Vector3& upVec) {
	CSP_LOG(APP, DEBUG, "VirtualScene::setLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);

	assert(m_FarView.valid());
	_updateOrigin(eyePos);
	osg::Vec3 _up (upVec.x(), upVec.y(), upVec.z() );

	m_VeryFarView->setViewMatrixAsLookAt(osg::Vec3(0.0, 0.0, 0.0), simdata::toOSG(lookPos - eyePos), _up);
	m_FarView->setViewMatrixAsLookAt(osg::Vec3(0.0, 0.0, 0.0), simdata::toOSG(lookPos - eyePos), _up);
	m_NearView->setViewMatrixAsLookAt(osg::Vec3(0.0, 0.0, 0.0), simdata::toOSG(lookPos - eyePos), _up);

	m_GlobalFrame->setPosition(simdata::toOSG(-eyePos));

	_updateFog(lookPos, eyePos);

	if (m_Terrain.valid()) {
		m_Terrain->setCameraPosition(eyePos.x(), eyePos.y(), eyePos.z());
		simdata::Vector3 tpos = m_Terrain->getOrigin(eyePos) - eyePos;
		m_TerrainGroup->setPosition(simdata::toOSG(tpos));
	}

	CSP_LOG(APP, DEBUG, logLookAt());
}

// TODO externalize a couple fixed parameters
void VirtualScene::_updateFog(simdata::Vector3 const &lookPos, simdata::Vector3 const &eyePos) {
	if (!m_FogEnabled) return;
	//AdjustCM(m_Sky->getSkyIntensity());
	intensity_test = m_Sky->getSkyIntensity();
	osg::Light *sun = m_Sky->getSunLight();
	osg::Vec3 sdir_ = sun->getDirection();
	simdata::Vector3 sdir(sdir_.x(), sdir_.y(), sdir_.z());
	simdata::Vector3 dir = lookPos - eyePos;
	dir.normalize();
	sdir.normalize();
	float sunz = (1.0 - sdir.z());
	if (sunz > 1.0) sunz = 2.0 - sunz;
	float clearSky = 0.0; //std::max(0.0, 0.5 * eyePos.z() - 2500.0);
	double a = simdata::dot(dir, sdir) * sunz;
	osg::StateSet *pStateSet = m_FogGroup->getStateSet();
	osg::Fog * pFogAttr = (osg::Fog*)pStateSet->getAttribute(osg::StateAttribute::FOG);
	float angle = simdata::toDegrees(atan2(dir.y(), dir.x()));
	// 0.8 brings some relief to distant mountain profiles at the clip plane, but
	// is not an ideal solution (better to push out the clip plane)
	//osg::Vec4 color = m_Sky->getHorizonColor(angle) * 0.8;
	m_FogColor = m_Sky->getHorizonColor(angle);
	pFogAttr->setColor(m_FogColor);
	pFogAttr->setStart(m_FogStart * (1.0 + a) + clearSky);
	pFogAttr->setEnd(m_FogEnd);
	pStateSet->setAttributeAndModes(pFogAttr, osg::StateAttribute::ON);
	m_FogGroup->setStateSet(pStateSet);
	m_Sky->updateHorizon(m_FogColor, eyePos.z(), m_ViewDistance);
}

void VirtualScene::getLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec) const {
	assert(m_FarView.valid());
	osg::Vec3 _eye;
	osg::Vec3 _center;
	osg::Vec3 _up;
	const_cast<osgUtil::SceneView*>(m_FarView.get())->getViewMatrixAsLookAt(_eye, _center, _up);
	eyePos = simdata::Vector3(_eye.x(), _eye.y(), _eye.z());
	lookPos = simdata::Vector3(_center.x(), _center.y(), _center.z());
	upVec = simdata::Vector3(_up.x(), _up.y(), _up.z());

	CSP_LOG(APP, DEBUG, "VirtualScene::getLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);
}


void VirtualScene::addParticleSystem(osg::Node *system, osg::Node *program) {
	if (program != 0) m_GlobalFrame->addChild(program);
	m_GlobalFrame->addChild(system);
}

void VirtualScene::removeParticleSystem(osg::Node *system, osg::Node *program) {
	if (program != 0) m_GlobalFrame->removeChild(program);
	m_GlobalFrame->removeChild(system);
}

void VirtualScene::addEffectUpdater(osg::Node *updater) {
	m_ParticleUpdaterGroup->addChild(updater);
}

void VirtualScene::removeEffectUpdater(osg::Node *updater) {
	m_ParticleUpdaterGroup->removeChild(updater);
}

void VirtualScene::addParticleEmitter(osg::Node *emitter) {
	m_ParticleEmitterGroup->addChild(emitter);
}

void VirtualScene::removeParticleEmitter(osg::Node *emitter) {
	m_ParticleEmitterGroup->removeChild(emitter);
}

void VirtualScene::addObject(simdata::Ref<DynamicObject> object) {
	assert(object.valid());
	assert(!object->isInScene());
	osg::Node *node = object->getOrCreateModelNode();
	assert(node != 0);
	object->enterScene();
	if (object->isNearField()) {
		bool ok = m_NearObjectGroup->addChild(node);
		assert(ok);
		CSP_LOG(SCENE, INFO, "adding object to near field " << *object);
	} else {
		bool ok = m_FreeObjectGroup->addChild(node);
		assert(ok);
		CSP_LOG(SCENE, INFO, "adding object to far field " << *object);
	}
	m_DynamicObjects.push_back(object);
}

void VirtualScene::removeObject(simdata::Ref<DynamicObject> object) {
	assert(object.valid());
	assert(object->isInScene());
	osg::Node *node = object->getOrCreateModelNode();
	assert(node != 0);
	if (object->isNearField()) {
		m_NearObjectGroup->removeChild(node);
		CSP_LOG(SCENE, INFO, "removing object from near field " << *object);
	} else {
		m_FreeObjectGroup->removeChild(node);
		CSP_LOG(SCENE, INFO, "removing object from far field " << *object);
	}
	object->leaveScene();
	// not very efficient, but object removal is rare compared to
	// traversing the visible object list for camera origin updates
	// every frame.
	for (unsigned i = 0; i < m_DynamicObjects.size(); ++i) {
		if (m_DynamicObjects[i] == object) {
			m_DynamicObjects.erase(m_DynamicObjects.begin() + i);
			break;
		}
	}
}

void VirtualScene::setNearObject(simdata::Ref<DynamicObject> object, bool isNear) {
	assert(object.valid());
	if (object->isNearField() == isNear) return;
	object->setNearFlag(isNear);
	CSP_LOG(SCENE, INFO, "setting near flag for " << *object << " to " << isNear);
	if (object->isInScene()) {
		osg::Node *node = object->getOrCreateModelNode();
		if (isNear) {
			m_FreeObjectGroup->removeChild(node);
			m_NearObjectGroup->addChild(node);
			CSP_LOG(SCENE, INFO, "moving object from far to near field " << *object);
		} else {
			bool ok;
			ok = m_NearObjectGroup->removeChild(node);
			assert(ok);
			ok = m_FreeObjectGroup->addChild(node);
			assert(ok);
			CSP_LOG(SCENE, INFO, "moving object from near to far field " << *object);
		}
	}
}

void VirtualScene::setWireframeMode(bool flag) {
	if (m_Wireframe == flag) return;
	m_Wireframe = flag;
	osg::StateSet* globalStateSet = m_FarView->getGlobalStateSet();
	if (!globalStateSet) {
		globalStateSet = new osg::StateSet;
		m_FarView->setGlobalStateSet(globalStateSet);
	}

	osg::PolygonMode* polyModeObj = new osg::PolygonMode;

	if (flag) {
		polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	} else {
		polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
	}
	globalStateSet->setAttribute(polyModeObj);
}

void VirtualScene::setFogMode(bool flag) {
	if (m_FogEnabled == flag) return;
	m_FogEnabled = flag;

	osg::StateSet * pStateSet = m_FogGroup->getStateSet();
	osg::StateAttribute * pStateAttr = pStateSet->getAttribute(osg::StateAttribute::FOG);

	if (flag) {
		pStateSet->setAttributeAndModes(pStateAttr, osg::StateAttribute::ON);
	} else {
		pStateSet->setAttributeAndModes(pStateAttr, osg::StateAttribute::OFF);
	}

	m_FogGroup->setStateSet(pStateSet);
}

void VirtualScene::setFogStart(float value) {
	m_FogStart = value;
}

void VirtualScene::setFogEnd(float value) {
	m_FogEnd = value;
}

// XXX XXX this needs to be integrated with SceneManager, and
// is probably not worth supporting.  SimpleSceneManager currently
// defines its own value, independent of this one, which is used
// by the battlefield to decide which objects should be added or
// removed from the scene.  it's probably not worth supporting a
// method to change the view distance of an existing scene, since
// the battlefield would then need to update which objects are visible.
void VirtualScene::setViewDistance(float value) {
	m_ViewDistance = value;
}

void VirtualScene::setViewAngle(float value) {
	m_ViewAngle = value;
}

void VirtualScene::setNearPlane(float value) {
	m_NearPlane = value;
}

void VirtualScene::setAspect(float value) {
	m_Aspect = value;
}

void VirtualScene::getViewport(int& x, int& y, int& width, int& height) {
	m_FarView->getViewport(x, y, width, height);
}

void VirtualScene::spinTheWorld(bool spin) {
	m_SpinTheWorld = spin;
}

void VirtualScene::resetSpin() {
	m_ResetTheWorld = true;
}

/**
 * Return the Sky time shift in seconds.
 *
 * Sky time shifting is for testing purposes only.   This shift is an artificial
 * advancement of the clock used to compute the position of the sun and moon.  It
 * does not directly effect normal "simtime".
 */
double VirtualScene::getSpin() {
	return m_Sky->getSpin();
}

void VirtualScene::drawPlayerInterface() { }

int VirtualScene::getTerrainPolygonsRendered() {
	if (!m_Terrain) {
		return 0;
	} else {
		return m_Terrain->getTerrainPolygonsRendered();
	}
}

void VirtualScene::setTerrain(simdata::Ref<TerrainObject> terrain) {
	if (!terrain) {
		if (m_TerrainNode.valid()) {
			m_TerrainGroup->removeChild(m_TerrainNode.get());
		}
		m_TerrainNode = NULL;
	}

	m_Terrain = terrain;

	if (m_Terrain.valid()) {
		m_TerrainNode = m_Terrain->getNode();
		if (m_TerrainNode.valid()) {
			m_TerrainGroup->addChild(m_TerrainNode.get());
#ifdef SHADOW
	osg::Vec4 ambientLightColor(0.9f, 0.1f, 0.1f, 1.0f);
	int texture_unit = 3;
	osg::NodeCallback* cb = createCullCallback(m_FreeObjectGroup.get(), osg::Vec3(6000.0, 1500.0, 30000), ambientLightColor, texture_unit);
	m_TerrainGroup->setCullCallback(cb);
	m_TerrainGroup->setCullingActive(true);
	m_FogGroup->setCullingActive(true);
	m_FarGroup->setCullingActive(true);
#endif

		}
	}
}

void VirtualScene::setLabels(bool show) {
	m_NodeMask = (m_NodeMask & ~SceneMasks::LABELS) | show ? SceneMasks::LABELS : 0;
	m_FarView->setCullMask(SceneMasks::CULL_ONLY | SceneMasks::NORMAL | m_NodeMask);
}

bool VirtualScene::getLabels() const {
	return (m_NodeMask & SceneMasks::LABELS) != 0;
}

bool VirtualScene::pick(int x, int y) {
	if (m_NearObjectGroup->getNumChildren() > 0) {
		assert(m_NearObjectGroup->getNumChildren() == 1);
		osg::Vec3 var_near;
		osg::Vec3 var_far;
		const int height = m_NearView->getViewport()->height();
		if (m_NearView->projectWindowXYIntoObject(x, height - y, var_near, var_far)) {
			osgUtil::IntersectVisitor iv;
			osg::ref_ptr<osg::LineSegment> line_segment = new osg::LineSegment(var_near, var_far);
			iv.addLineSegment(line_segment.get());
			m_NearView->getSceneData()->accept(iv);
			osgUtil::IntersectVisitor::HitList &hits = iv.getHitList(line_segment.get());
			if (!hits.empty()) {
				osg::NodePath const &nearest = hits[0]._nodePath;
				// TODO should we iterate in reverse?
				for (osg::NodePath::const_iterator iter = nearest.begin(); iter != nearest.end(); ++iter) {
					osg::Node *node = *iter;
					osg::NodeCallback *callback = node->getUpdateCallback();
					if (callback) {
						AnimationCallback *anim = dynamic_cast<AnimationCallback*>(callback);
						// TODO set flags for click type, possibly add position if we can determine a useful
						// coordinate system.
						if (anim && anim->pick(0)) break;
					}
				}
			}
		}
	}
	return false;
}

