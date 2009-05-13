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

// implement horizon colors
// move orbital body creation and image loading to Sky

#include <csp/cspsim/VirtualScene.h>
#include <csp/cspsim/Animation.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/glDiagnostics.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/Projection.h>
#include <csp/cspsim/SceneConstants.h>
#include <csp/cspsim/ScreenInfoNode.h>
#include <csp/cspsim/Shader.h>

#include <csp/cspsim/sky/Sky.h>
#include <csp/cspsim/sky/SkyDome.h>
#include <csp/cspsim/sky/StarDome.h>
#include <csp/cspsim/sky/SolarSystem.h>
#include <csp/cspsim/sky/OrbitalBodyModel.h>
#include <csp/cspsim/sky/OrbitalBodyImposter.h>
#include <osgDB/ReadFile> // TODO move orbital body creation and image loading to Sky

#include <csp/cspsim/sound/SoundEngine.h>
#include <csp/cspsim/TerrainObject.h>
#include <csp/cspsim/theater/FeatureGroup.h>
#include <csp/cspsim/theater/FeatureSceneGroup.h>

#include <csp/cspwf/Serialization.h>
#include <csp/cspwf/WindowManager.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/data/Types.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/osg.h>

#include <osg/AlphaFunc>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/ColorMatrix>
#include <osg/Fog>
#include <osg/LightModel>
#include <osg/LightSource>
#include <osg/Material>
#include <osg/Node>
#include <osg/Notify>
#include <osg/PolygonMode>
#include <osg/PositionAttitudeTransform>
#include <osg/StateSet>
#include <osg/TexEnv>
#include <osg/Texture2D>
#include <osg/Uniform>
#include <osgAL/SoundRoot>
#include <osgUtil/CullVisitor>
#include <osgUtil/IntersectVisitor>
#include <osgUtil/Optimizer>
#include <osgUtil/SceneView>

#include <cmath>
#include <cassert>
#include <iostream>

// SHADOW is an *extremely* experimental feature.  It is based on the
// osgShadow demo, and does (did) work to some extent, but only for a
// single localized object.  A more robust approach needs to be taken
// but the code remains for reference.
#define NOSHADOW
#ifdef SHADOW
#include "shadow.h"
#endif


namespace csp {

///////////////////////////////////////////////////////////////////////
// testing
///////////////////////////////////////////////////////////////////////

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
	Vector3 m_GlobalPosition;
	/* experiment
	osg::ref_ptr<osg::BlendColor> m_BlendColor;
	*/
public:
	FeatureTile(Vector3 const &origin): m_GlobalPosition(origin) {
		/* experiment
		m_BlendColor = new osg::BlendColor;
		getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA), osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
		getOrCreateStateSet()->setAttributeAndModes(m_BlendColor.get(), osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
		*/
	}
	typedef osg::ref_ptr<FeatureTile> Ref;
	inline void updateOrigin(Vector3 const &origin) {
		Vector3 offset = m_GlobalPosition - origin;
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


class VirtualScene::SceneState: public Referenced {
public:
	void setOrigin(Vector3 const &origin) {
		m_Origin = origin;
	}
	void bindSky(Sky *sky) {
		m_Sky = sky;
	}
	Vector3 getOrigin() const { return m_Origin; }
	osg::Vec4 getFogColor(Vector3 const &dir) {
		if (!m_Sky) return osg::Vec4(1.0, 1.0, 1.0, 0.0);
		const float angle = atan2(dir.y(), dir.x());
		return m_Sky->getSkyDome()->getHorizonColor(angle);
	}
private:
	Vector3 m_Origin;
	osg::ref_ptr<Sky> m_Sky;
};

class VirtualScene::FeatureGroupCallback: public osg::NodeCallback {
public:
	//FeatureGroupCallback(FeatureGroup *object, SceneState *state):
	FeatureGroupCallback(SimObject *object, SceneState *state):
			m_Bound(false),
			m_Object(object),
			m_State(state),
			m_Fade(new osg::Uniform(osg::Uniform::FLOAT, "fade")),
			m_Fog(new osg::Uniform(osg::Uniform::FLOAT_VEC4, "fog")),
			m_Alpha(-1) {
		assert(object);
		assert(state);
		/*
		osg::Node *node = object->getSceneGroup();
		osg::StateSet *ss = node->getOrCreateStateSet();
		m_Center = object->getGlobalPosition();
		m_Center.z() = node->getBound().center().z();
		ss->addUniform(m_Fade.get());
		ss->addUniform(m_Fog.get());
		ss->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
		*/
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv && nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR) {
			if (!m_Bound) {
				osg::StateSet *ss = node->getOrCreateStateSet();
				m_Center = m_Object->getGlobalPosition();
				m_Center.z() = node->getBound().center().z();
				ss->addUniform(m_Fade.get());
				ss->addUniform(m_Fog.get());
				ss->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
			}
			if (!m_Object->isStatic()) {
				m_Center = m_Object->getGlobalPosition();
			}
			updateFade();
			updateFog();
		}
		traverse(node, nv);
	}

private:

	void updateFade() {
		const double distance = (m_Object->getGlobalPosition() - m_State->getOrigin()).length();
		float fade = clampTo(static_cast<float>(distance - 10000.0) / 10000.0f, 0.0f, 1.0f);
		const int alpha = static_cast<int>(fade * 100);
		if (alpha != m_Alpha) {
			m_Alpha = alpha;
			m_Fade->set(fade);
		}
	}

	void updateFog() {
		const double fog_depth = 2000.0;
		const double fog_attenuation = 10000.0;
		const double eye_z = m_State->getOrigin().z();
		const double i0 = exp(-eye_z / fog_depth);
		const double i1 = exp(-m_Center.z() / fog_depth);
		const double dz = eye_z - m_Center.z();
		const Vector3 direction = m_Center - m_State->getOrigin();
		double distance = direction.length();
		if (fabs(dz) < 10.0) {
			distance *= (i0 + i1) * 0.5;
		} else {
			distance *= std::max(fog_depth * (i1 - i0) / dz, 0.0);
		}
		const double fog_intensity = clampTo(1.0 - exp(-distance / fog_attenuation), 0.0, 1.0);
		osg::Vec4 fog = m_State->getFogColor(direction);
		fog.w() = fog_intensity;
		m_Fog->set(fog);
	}

	//FeatureGroup *m_Object;
	bool m_Bound;
	SimObject *m_Object;
	SceneState *m_State;
	osg::ref_ptr<osg::Uniform> m_Fade;
	osg::ref_ptr<osg::Uniform> m_Fog;
	Vector3 m_Center;
	int m_Alpha;
};


void VirtualScene::_updateOrigin(Vector3 const &origin) {
	m_Origin = origin;
	m_SceneState->setOrigin(origin);
	FeatureTileMap::iterator titer = m_FeatureTiles.begin();
	for (; titer != m_FeatureTiles.end(); ++titer) {
		titer->second->updateOrigin(origin);
	}
	DynamicObjectList::iterator diter = m_DynamicObjects.begin();
	for (; diter != m_DynamicObjects.end(); ++diter) {
		(*diter)->updateScene(origin);
	}
}

Vector3 VirtualScene::getFeatureOrigin(Ref<FeatureGroup> const &feature) const {
	double x = floor(feature->getGlobalPosition().x() * m_FeatureTileScale) * m_FeatureTileSize;
	double y = floor(feature->getGlobalPosition().y() * m_FeatureTileScale) * m_FeatureTileSize;
	return Vector3(x, y, 0.0);
}

int VirtualScene::_getFeatureTileIndex(Ref<FeatureGroup> feature) const {
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
void VirtualScene::addFeature(Ref<FeatureGroup> feature) {
	int index = _getFeatureTileIndex(feature);
	FeatureTileMap::iterator iter = m_FeatureTiles.find(index);
	FeatureTileRef tile;
	if (iter == m_FeatureTiles.end()) {
		Vector3 origin = getFeatureOrigin(feature);
		tile = new FeatureTile(origin);
		m_FeatureTiles[index] = tile;
		m_FeatureGroup->addChild(tile.get());
		CSPLOG(DEBUG, SCENE) << "adding new feature cell, index = " << index;
	} else {
		tile = iter->second;
	}
	CSPLOG(DEBUG, SCENE) << "adding feature " << *feature << " to scene";
	FeatureSceneGroup *scene_group = feature->getSceneGroup();
	assert(scene_group != 0);
	if (scene_group == 0) return;

	feature->enterScene();
	tile->addChild(scene_group);
	scene_group->setCullCallback(new FeatureGroupCallback(feature.get(), m_SceneState.get()));
}

/**
*/
void VirtualScene::removeFeature(Ref<FeatureGroup> feature) {
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
	CSPLOG(DEBUG, SCENE) << "removing feature " << *feature << " from scene";
	if (tile->getNumChildren() == 0) {
		m_FeatureGroup->removeChild(tile);
		m_FeatureTiles.erase(iter);
		CSPLOG(DEBUG, SCENE) << "removing empty feature cell, index = " << index;
	}
	// TODO feature->discardSceneGroup() ??
}


VirtualScene::VirtualScene(osg::Group *virtualSceneGroup, int width, int height):
	m_SceneState(new SceneState),
	m_VirtualSceneGroup(virtualSceneGroup),
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
	m_ScreenHeight(height) {
	}

VirtualScene::~VirtualScene() { 
	ScreenInfoNode::tearDown(CSPSim::theSim->getSceneData());
	m_VirtualSceneGroup->removeChild( 0, m_VirtualSceneGroup->getNumChildren() );
}

void VirtualScene::init() {
	// configure the default state for all scene graphs in the various scene views.
	osg::StateSet *globalStateSet = m_VirtualSceneGroup->getOrCreateStateSet();

	// custom default settings
	globalStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	globalStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

	// add default uniforms for shaders
	Shader::instance()->addDefaultUniforms(globalStateSet);

	// create a light model to eliminate the default ambient light.
	osg::LightModel *light_model = new osg::LightModel();
	light_model->setAmbientIntensity(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	globalStateSet->setAttributeAndModes(light_model, osg::StateAttribute::ON);

	// set up an alphafunc by default to speed up blending operations.
	osg::AlphaFunc* alphafunc = new osg::AlphaFunc;
	alphafunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
	globalStateSet->setAttributeAndModes(alphafunc, osg::StateAttribute::ON);

	// set up an texture environment by default to speed up blending operations.
	osg::TexEnv* texenv = new osg::TexEnv;
	texenv->setMode(osg::TexEnv::MODULATE);
	globalStateSet->setTextureAttributeAndModes(0, texenv, osg::StateAttribute::ON);

	m_FrameStamp = new osg::FrameStamp;
}

void VirtualScene::createSceneViews() {
	createVeryFarView();
	createFarView();
	createNearView();
	_updateProjectionMatrix();
}

osg::Camera *VirtualScene::makeSceneCamera(unsigned mask) {
	osg::Camera *camera = new osg::Camera;
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	camera->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING); // Why don't use the default ?
	camera->setCullMask(SceneMasks::CULL_ONLY | SceneMasks::NORMAL | mask);
	// TODO: this was done on the old osgUtil::SceneView: sv->getUpdateVisitor()->setTraversalMask(SceneMasks::UPDATE_ONLY | SceneMasks::NORMAL | mask);
	m_VirtualSceneGroup->addChild(camera);
	return camera;
}

void VirtualScene::createVeryFarView() {
	m_VeryFarCamera = makeSceneCamera(SceneMasks::FAR);
	m_VeryFarGroup = new osg::Group;
	m_VeryFarGroup->setName("very_far_group");
	m_VeryFarCamera->addChild(m_VeryFarGroup.get());
}

void VirtualScene::createFarView() {
	m_TerrainCamera = makeSceneCamera(SceneMasks::FAR); // TODO: remove this temporary hack (see below)
	m_TerrainCamera->setClearMask(GL_DEPTH_BUFFER_BIT);

	m_FarCamera = makeSceneCamera(SceneMasks::FAR);
	m_FarCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	m_FarGroup = new osg::Group;
	m_FarGroup->setName("far_group");
	m_FarCamera->addChild(m_FarGroup.get());
}

void VirtualScene::createNearView() {
	m_NearCamera = makeSceneCamera(SceneMasks::NEAR);
	m_NearCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	// TODO: this was done on the old osgUtil::SceneView: m_NearView->setImpostorsActive(false); // Inherited by the CullVisitor
	m_NearGroup = new osg::Group;
	m_NearGroup->setName("near_group");
	m_NearCamera->addChild(m_NearGroup.get());
}

void VirtualScene::buildScene() {
	CSPLOG(INFO, APP) << "VirtualScene::buildScene() ";

	/////////////////////////////////////
	//(Un)comment to (enable) disable debug info from osg
	//osg::setNotifyLevel(osg::DEBUG_INFO);
	/////////////////////////////////////

	init();
	createSceneViews();
	ScreenInfoNode::setUp(CSPSim::theSim->getSceneData());

	if(SoundEngine::getInstance().getSoundEnabled()) {
		m_SoundRoot = new osg::PositionAttitudeTransform;
		m_SoundRoot->addChild(SoundEngine::getInstance().getSoundRoot());

		// place the sound root in the very far group to ensure that the listener is
		// traversed before any other sound sources.
		m_VeryFarGroup->addChild(m_SoundRoot.get());
	}

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
	background->addChild(m_Sky->group());

	m_VeryFarGroup->addChild(background);

	m_TerrainGroup = new osg::PositionAttitudeTransform;
	m_TerrainGroup->setName("terrain_group");
	// the terrain is placed in bin -3 to ensure that it is drawn first.
	// (bin -2 is used for planar objects and bin -1 is used for planar
	// shadows; see ObjectModel and SceneModel for details.)
	m_TerrainGroup->getOrCreateStateSet()->setRenderBinDetails(-3, "RenderBin");
	m_TerrainGroup->getOrCreateStateSet()->setTextureAttributeAndModes(2, m_Sky->getSkyDome()->getHorizonTexture(), osg::StateAttribute::ON);

	m_FogGroup = new osg::Group;
	m_FogGroup->setName("fog_group");
	//m_FogGroup->addChild(m_TerrainGroup.get()); // TODO: Temporary moved to a dedicated camera to avoid a depth buffer bug in the cockpit. But we can now see through the terrain.
	m_FogGroup->addChild(m_ObjectGroup.get());

	osg::Group* fogTerrainGroup = new osg::Group;
	m_TerrainCamera->addChild(fogTerrainGroup);
	fogTerrainGroup->addChild(m_SkyLights.get());
	fogTerrainGroup->addChild(m_TerrainGroup.get());

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
	fogTerrainGroup->setStateSet(pFogState);
	osg::Fog* fog = new osg::Fog;
	fog->setMode(osg::Fog::LINEAR);
	fog->setFogCoordinateSource(osg::Fog::FRAGMENT_DEPTH);
	fog->setDensity(0.3f);
	pFogState->setAttributeAndModes(fog, m_FogEnabled ? osg::StateAttribute::ON : osg::StateAttribute::OFF);

	m_NearObjectGroup = new osg::Group;
	m_NearGroup->addChild(m_SkyLights.get());
	m_NearGroup->addChild(m_NearObjectGroup.get());
	
	// TODO: Remove these calls and place them into some kind of weather object.
	m_CloudGroup = new osg::Group;
	m_NearGroup->addChild(m_CloudGroup.get());

	//FIXME: why doesn't ALL_OPTIMIZATIONS work as expected?
	//osgUtil::Optimizer opt;
	//opt.optimize(m_FarGroup.get(), osgUtil::Optimizer::COMBINE_ADJACENT_LODS);
	//opt.optimize(m_FarGroup.get(), osgUtil::Optimizer::SHARE_DUPLICATE_STATE);
}

void VirtualScene::buildSky() {
	osg::StateSet* globalStateSet = m_VirtualSceneGroup->getOrCreateStateSet();
	assert(globalStateSet);

	m_Sky = new Sky(1e+6);
	m_SceneState->bindSky(m_Sky.get());

	Ref<SolarSystem> ss = m_Sky->getSolarSystem();
	OrbitalBodyImposter *moon_imposter = new OrbitalBodyImposter(osgDB::readImageFile("moon.png"), 1.0);
	OrbitalBodyModel *moon = new OrbitalBodyModel(moon_imposter, 0.0, ss->moon(), ss->earth(), 0.01);
	m_Sky->addModel(moon);

	m_Sky->addSunlight(0);
	m_Sky->addMoonlight(1);

	m_SkyLights = new osg::Group;

	osg::LightSource *sunlight = m_Sky->getSunlight();
	sunlight->setStateSetModes(*globalStateSet, osg::StateAttribute::ON);
	m_SkyLights->addChild(sunlight);

	osg::LightSource *moonlight = m_Sky->getMoonlight();
	moonlight->setStateSetModes(*globalStateSet, osg::StateAttribute::ON);
	m_SkyLights->addChild(moonlight);
}

int VirtualScene::drawScene() {
	// The is no longuer needed, all the drawing is handled by osgViewer::Viewer::frame()
	CSPLOG(DEBUG, APP) << "VirtualScene::drawScene()...";
	//if (m_Terrain.valid()) m_Terrain->endDraw(); // TODO: for Demeter stats
	return 1;
}

void VirtualScene::onUpdate(float dt) {
	// TODO remove static
	static float t = 1.0; t += dt;
	if (m_Terrain.valid() || t >= 1.0) {
		LLA m = m_Terrain->getProjection()->convert(m_Origin);
		m_Sky->setPosition(m.latitude(), m.longitude());
		t = 0.0;
	}

	// TODO remove static
	static const double day0 = SimDate(2000, 1, 1, 0, 0, 0).getJulianDate();
	double day = CSPSim::theSim->getCurrentTime().getJulianDate() - day0;
	static double spin = 0.0;
	if (m_SpinTheWorld) spin += dt / 300.0;
	if (m_ResetTheWorld) {
		spin = 0.0;
		m_ResetTheWorld = false;
	}

	// TODO remove static
	static float view_angle = 0.0;
	if (view_angle != getViewAngle()) {
		view_angle = getViewAngle();
		m_Sky->getStarDome()->setViewAngle(view_angle);
	}

	m_Sky->update(day + spin);

	CSPLOG(DEBUG, APP) << "VirtualScene::onUpdate - entering" ;

	m_FrameStamp->setReferenceTime(m_FrameStamp->getReferenceTime() + dt);
	m_FrameStamp->setFrameNumber(m_FrameStamp->getFrameNumber() + 1);

	CSPLOG(DEBUG, APP) << "VirtualScene::onUpdate - leaving" ;
}


void VirtualScene::setCameraNode(osg::Node *) {
}

void VirtualScene::_setLookAt(const Vector3& eyePos, const Vector3& lookPos, const Vector3& upVec) {
	CSPLOG(DEBUG, APP) << "VirtualScene::setLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec;

	_updateOrigin(eyePos);
	osg::Vec3 _up(upVec.x(), upVec.y(), upVec.z());

	osg::Matrix view_matrix;
	view_matrix.makeLookAt(osg::Vec3(0.0, 0.0, 0.0), toOSG(lookPos - eyePos), _up);

	m_VeryFarCamera->setViewMatrix(view_matrix);
	m_TerrainCamera->setViewMatrix(view_matrix);
	m_FarCamera->setViewMatrix(view_matrix);
	m_NearCamera->setViewMatrix(view_matrix);

	osg::Quat listener_attitude;
	listener_attitude.set(view_matrix);
	m_SoundRoot->setAttitude(listener_attitude);

	m_GlobalFrame->setPosition(toOSG(-eyePos));

	_updateFog(lookPos, eyePos);

	if (m_Terrain.valid()) {
		m_Terrain->setCameraPosition(eyePos.x(), eyePos.y(), eyePos.z());
		Vector3 tpos = m_Terrain->getOrigin(eyePos) - eyePos;
		m_TerrainGroup->setPosition(toOSG(tpos));
	}
}

void VirtualScene::_updateProjectionMatrix() {
	m_VeryFarCamera->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, 0.8 * m_ViewDistance, 1.2e+6);
	m_TerrainCamera->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, m_NearPlane, m_ViewDistance);
	m_FarCamera->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, m_NearPlane, m_ViewDistance);
	m_NearCamera->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, 0.01f, 100.0);
}


// TODO externalize a couple fixed parameters
void VirtualScene::_updateFog(Vector3 const &lookPos, Vector3 const &eyePos) {
	if (!m_FogEnabled) return;
	osg::Light *sun = m_Sky->getSunlight()->getLight();
	osg::Vec3 sdir_ = sun->getDirection();
	Vector3 sdir(sdir_.x(), sdir_.y(), sdir_.z());
	Vector3 dir = lookPos - eyePos;
	dir.normalize();
	sdir.normalize();
	float sunz = (1.0 - sdir.z());
	if (sunz > 1.0) sunz = 2.0 - sunz;
	float clearSky = 0.0; //std::max(0.0, 0.5 * eyePos.z() - 2500.0);
	double a = dot(dir, sdir) * sunz;
	osg::StateSet *pStateSet = m_FogGroup->getStateSet();
	osg::Fog * pFogAttr = (osg::Fog*)pStateSet->getAttribute(osg::StateAttribute::FOG);
	// 0.8 brings some relief to distant mountain profiles at the clip plane, but
	// is not an ideal solution (better to push out the clip plane)
	//XXX--float angle = toDegrees(atan2(dir.y(), dir.x()));
	//osg::Vec4 color = m_Sky->getHorizonColor(angle) * 0.8;
	//XXX--m_FogColor = m_Sky->getHorizonColor(angle);
	float angle = atan2(dir.y(), dir.x());
	m_FogColor = m_Sky->getSkyDome()->getHorizonColor(angle);
	if (pFogAttr != NULL) {  // TODO why is pFogAttr NULL at startup?
		pFogAttr->setColor(m_FogColor);
		pFogAttr->setStart(m_FogStart * (1.0 + a) + clearSky);
		pFogAttr->setEnd(m_FogEnd);
		pStateSet->setAttributeAndModes(pFogAttr, osg::StateAttribute::ON);
	}
	//XXX--m_Sky->updateHorizon(m_FogColor, eyePos.z(), m_ViewDistance);
}

void VirtualScene::getLookAt(Vector3 & eyePos, Vector3 & lookPos, Vector3 & upVec) const {
	osg::Vec3 _eye;
	osg::Vec3 _center;
	osg::Vec3 _up;
	m_FarCamera->getViewMatrixAsLookAt(_eye, _center, _up);
	eyePos = Vector3(_eye.x(), _eye.y(), _eye.z());
	lookPos = Vector3(_center.x(), _center.y(), _center.z());
	upVec = Vector3(_up.x(), _up.y(), _up.z());

	CSPLOG(DEBUG, APP) << "VirtualScene::getLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec;
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

void VirtualScene::addObject(Ref<DynamicObject> object) {
	assert(object.valid());
	assert(!object->isInScene());
	osg::Node *node = object->getOrCreateModelNode();
	assert(node != 0);
	// simpler to just call the update directly in _updateOrigin  (remove this)
#if 0
	// FIXME breaks if the node already has a different update callback!
	if (!node->getUpdateCallback()) {
		node->setUpdateCallback(new ObjectUpdateCallback(object.get(), m_SceneState.get()));
	}
#endif

	node->setCullCallback(new FeatureGroupCallback(object.get(), m_SceneState.get()));

	object->enterScene();
	if (object->isNearField()) {
		bool ok = m_NearObjectGroup->addChild(node);
		assert(ok);
		CSPLOG(INFO, SCENE) << "adding object to near field " << *object;
	} else {
		bool ok = m_FreeObjectGroup->addChild(node);
		assert(ok);
		CSPLOG(INFO, SCENE) << "adding object to far field " << *object;
	}
	m_DynamicObjects.push_back(object);
}

void VirtualScene::removeObject(Ref<DynamicObject> object) {
	assert(object.valid());
	assert(object->isInScene());
	osg::Node *node = object->getOrCreateModelNode();
	assert(node != 0);
	if (object->isNearField()) {
		m_NearObjectGroup->removeChild(node);
		CSPLOG(INFO, SCENE) << "removing object from near field " << *object;
	} else {
		m_FreeObjectGroup->removeChild(node);
		CSPLOG(INFO, SCENE) << "removing object from far field " << *object;
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

void VirtualScene::setNearObject(Ref<DynamicObject> object, bool isNear) {
	assert(object.valid());
	if (object->isNearField() == isNear) return;
	object->setNearFlag(isNear);
	CSPLOG(INFO, SCENE) << "setting near flag for " << *object << " to " << isNear;
	if (object->isInScene()) {
		osg::Node *node = object->getOrCreateModelNode();
		if (isNear) {
			m_FreeObjectGroup->removeChild(node);
			m_NearObjectGroup->addChild(node);
			CSPLOG(INFO, SCENE) << "moving object from far to near field " << *object;
		} else {
			CSP_VERIFY(m_NearObjectGroup->removeChild(node));
			CSP_VERIFY(m_FreeObjectGroup->addChild(node));
			CSPLOG(INFO, SCENE) << "moving object from near to far field " << *object;
		}
	}
}

void VirtualScene::setWireframeMode(bool flag) {
	if (m_Wireframe == flag) return;
	m_Wireframe = flag;

	osg::PolygonMode* polyModeObj = new osg::PolygonMode;

	if (flag) {
		polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	} else {
		polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
	}
	m_VirtualSceneGroup->getOrCreateStateSet()->setAttribute(polyModeObj);
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
	_updateProjectionMatrix();
}

void VirtualScene::setNearPlane(float value) {
	m_NearPlane = value;
}

void VirtualScene::setAspect(float value) {
	m_Aspect = value;
	_updateProjectionMatrix();
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
	return 0.0;  //XXX--m_Sky->getSpin();
}

void VirtualScene::drawPlayerInterface() { }

int VirtualScene::getTerrainPolygonsRendered() {
	if (!m_Terrain) {
		return 0;
	} else {
		return m_Terrain->getTerrainPolygonsRendered();
	}
}

void VirtualScene::setTerrain(Ref<TerrainObject> terrain) {
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
			if (!terrain->getShader().empty()) {
				Shader::instance()->applyShader(terrain->getShader(), m_TerrainNode->getOrCreateStateSet());
			}
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
	unsigned int currentMask = m_FarCamera->getCullMask();
	unsigned int newMask = (currentMask & ~SceneMasks::LABELS) | (show ? SceneMasks::LABELS : 0);
	m_FarCamera->setCullMask(newMask);
}

bool VirtualScene::getLabels() const {
	return (m_FarCamera->getCullMask() & SceneMasks::LABELS) != 0;
}

bool VirtualScene::pick(int x, int y) {
	if(CSPSim::theSim->getWindowManager()->onClick(x, y))
		return true;

	// TODO: pick
	/*
	if (m_NearObjectGroup->getNumChildren() > 0) {
		assert(m_NearObjectGroup->getNumChildren() == 1);
		osg::Vec3 var_near;
		osg::Vec3 var_far;
		const int height = static_cast<int>(m_NearView->getViewport()->height());
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
	*/
	return false;
}

} // namespace csp


