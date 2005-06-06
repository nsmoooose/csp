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

#include <SimCore/Util/Log.h>
#include <SimData/Types.h>
#include <SimData/Math.h>
#include <SimData/FileUtility.h>
#include <SimData/osg.h>

#include <osg/Fog>
#include <osg/Node>
#include <osg/Notify>
#include <osg/PolygonMode>
#include <osg/ColorMatrix>
#include <osg/LightSource>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/SceneView>
#include <osgUtil/Optimizer>
#include <osgUtil/CullVisitor>
#include <osgUtil/IntersectVisitor>

#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/StateSet>

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
	//cout << sat << endl;
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
	//cout << sat << endl;
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

/**
 * struct MoveEarthySkyWithEyePointCallback
 *
 * @author unknown
 */
struct MoveEarthySkyWithEyePointCallback : public osg::Transform
{
	/** Get the transformation matrix which moves from local coords to world coords.*/
	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const {
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv) {
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),0.0f));
			//matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
		}
		return true;
	}

	/** Get the transformation matrix which moves from world coords to local coords.*/
	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const {
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv) {
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),0.0));
			//matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
		}
		return true;
	}
};


class FeatureTile: public osg::PositionAttitudeTransform {
	simdata::Vector3 m_GlobalPosition;
	public:
	FeatureTile(simdata::Vector3 const &origin): m_GlobalPosition(origin) { }
	typedef osg::ref_ptr<FeatureTile> Ref;
	inline void updateOrigin(simdata::Vector3 const &origin) {
		simdata::Vector3 offset = m_GlobalPosition - origin;
		setPosition(osg::Vec3(offset.x(), offset.y(), offset.z()));
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


VirtualScene::VirtualScene()
{
	m_FarView = NULL;
	m_NearView = NULL;
	m_ViewAngle = 60.0;
	m_NearPlane = 2.0;
	m_Aspect =  static_cast<float>(CSPSim::theSim->getSDLScreen()->w)/CSPSim::theSim->getSDLScreen()->h;
	m_ViewDistance = 30000.0;
	m_SpinTheWorld = false;
	m_ResetTheWorld = false;
	m_Wireframe = false;
	m_FeatureTileSize = 40000.0; // 40 km
	m_FeatureTileScale = 1.0 / m_FeatureTileSize;
}

VirtualScene::~VirtualScene()
{
}

int VirtualScene::buildScene()
{
	CSP_LOG(APP, INFO, "VirtualScene::buildScene() ");

	int ScreenWidth = CSPSim::theSim->getSDLScreen()->w;
	int ScreenHeight = CSPSim::theSim->getSDLScreen()->h;

	/////////////////////////////////////
	//
	//(Un)comment to (enable) disable debug info from osg
	//
	//osg::setNotifyLevel(osg::DEBUG_INFO);
	//
	/////////////////////////////////////

	// construct the views
	m_FarView = new osgUtil::SceneView();

	osg::DisplaySettings* ds;
	ds = m_FarView->getDisplaySettings();
	if (!ds) {
		ds = new osg::DisplaySettings;
		m_FarView->setDisplaySettings(ds);
	}
	m_FarView->setDefaults();
	ds->setDepthBuffer(true);
	m_FarView->setViewport(0, 0, ScreenWidth, ScreenHeight);
	m_FarView->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	m_FarView->getCullVisitor()->setImpostorsActive(true);
	// override default HEADLIGHT mode, we provide our own lights.
	m_FarView->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);

	m_ContextIDFactory = new ContextIDFactory;
	//m_ContextIDFactory->getOrCreateContextID(m_FarView.get());

	m_NearView = new osgUtil::SceneView();
	ds = m_NearView->getDisplaySettings();
	if (!ds) {
		ds = new osg::DisplaySettings;
		m_NearView->setDisplaySettings(ds);
	}
	m_NearView->setDefaults();
	ds->setDepthBuffer(true);
	m_NearView->setViewport(0, 0, ScreenWidth, ScreenHeight);
	m_NearView->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	m_NearView->getCullVisitor()->setImpostorsActive(false);
	// override default HEADLIGHT mode, we provide our own lights.
	m_NearView->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);

	//m_ContextIDFactory->getOrCreateContextID(m_NearView.get());

	m_FreeObjectGroup = new osg::Group;
	m_FreeObjectGroup->setName("SG_FreeObjects");

	m_FeatureGroup = new osg::PositionAttitudeTransform;
	m_FeatureGroup->setName("SG_Features");

	m_ObjectGroup = new osg::Group;
	m_ObjectGroup->setName("SG_Objects");
	m_ObjectGroup->addChild(m_FeatureGroup.get());
	m_ObjectGroup->addChild(m_FreeObjectGroup.get());

	// construct the skydome, stars, moon, sunlight, and moonlight
	buildSky();

	// use a transform to make the sky and base around with the eye point.
	m_EyeTransform = new osg::PositionAttitudeTransform;

	// transform's value isn't knowm until in the cull traversal so its
	// bounding volume can't be determined, therefore culling will be
	// invalid, so switch it off.  this will cause all our parents to
	// switch culling off as well, but culling will be remain on
	// underneath this node or any other branch above this transform.
	m_EyeTransform->setCullingActive(false);

	// set the compute transform callback to do all the work of
	// determining the transform according to the current eye point.
	// m_EyeTransform = new MoveEarthySkyWithEyePointCallback;
	m_EyeTransform->addChild(m_Sky.get());  // bin number -2 so drawn first.
	m_EyeTransform->addChild(m_SkyLights.get());

	osg::ClearNode* background = new osg::ClearNode;
	// in general the earth and sky should cover the entire field of
	// view, but at high altitude it is still possible to see around the
	// terrain (either past the clipping plane or beyond the edge of the
	// terrain lattice).  when low detail terrain is eventually extended
	// without clipping then this can be set to false.
	background->setRequiresClear(true);
	background->addChild(m_EyeTransform.get());

	m_TerrainGroup = new osg::PositionAttitudeTransform;
	m_TerrainGroup->setName("SG_Terrain");

	m_FogGroup = new osg::Group;
	m_FogGroup->setName("SG_Fog");
	m_FogGroup->addChild(m_TerrainGroup.get());
	m_FogGroup->addChild(m_ObjectGroup.get());

	m_GlobalFrame = new osg::PositionAttitudeTransform;
	m_GlobalFrame->setName("SG_Global");
	//m_GlobalFrame->setCullingActive(false);
	//m_GlobalFrame->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);

	m_ParticleEmitterGroup = new osg::Group;
	m_ParticleEmitterGroup->setName("SG_ParticleEmitter");
	//m_ParticleEmitterGroup->setCullingActive(false);
	m_ParticleUpdaterGroup = new osg::Group;
	m_ParticleUpdaterGroup->setName("SG_ParticleUpdater");
	//m_ParticleUpdaterGroup->setCullingActive(false);

	// top node
	m_RootNode = new osg::Group;
	m_RootNode->setName( "SG_Root" );
	m_RootNode->addChild(background);
	m_RootNode->addChild(m_FogGroup.get());
	m_RootNode->addChild(m_ParticleEmitterGroup.get());
	m_RootNode->addChild(m_GlobalFrame.get());
	m_RootNode->addChild(m_ParticleUpdaterGroup.get());

	// fog properties: start and end distances are read from CSPSim.ini
	osg::StateSet * pFogState = m_FogGroup->getOrCreateStateSet();
	osg::Fog* fog = new osg::Fog;
	fog->setMode(osg::Fog::LINEAR);
	fog->setFogCoordinateSource(osg::Fog::FRAGMENT_DEPTH);
	fog->setDensity(0.3f);
	pFogState->setAttributeAndModes(fog, m_FogEnabled ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
	m_FogGroup->setStateSet(pFogState);

	m_FrameStamp = new osg::FrameStamp;

	// add the scene graph to the view
	m_FarView->setSceneData(m_RootNode.get());
	m_FarView->setFrameStamp(m_FrameStamp.get());

	m_NearGroup = new osg::Group;
	m_NearObjectGroup = new osg::Group;
	m_NearGroup->addChild(m_SkyLights.get());
	m_NearGroup->addChild(m_NearObjectGroup.get());
	m_NearView->setSceneData(m_NearGroup.get());
	m_NearView->setFrameStamp(m_FrameStamp.get());
	m_NearView->getRenderStage()->setClearMask(GL_DEPTH_BUFFER_BIT);

	//FIXME: why ALL_OPTIMIZATIONS don t work as expected?
	osgUtil::Optimizer opt;
	opt.optimize(m_RootNode.get(), osgUtil::Optimizer::COMBINE_ADJACENT_LODS);
	opt.optimize(m_RootNode.get(), osgUtil::Optimizer::SHARE_DUPLICATE_STATE);

	return 1;
}

float CM_intensity=0.0;


void VirtualScene::buildSky()
{
	osg::StateSet* globalStateSet = m_FarView->getGlobalStateSet();
	assert(globalStateSet);

	// build the skydome, stars, and moon
	m_Sky = new Sky;

	// sunlight
	osg::Light *pSunLight = m_Sky->getSunLight();
	osg::LightSource *pSunLightSource = new osg::LightSource;
	pSunLightSource->setLight(pSunLight);
	pSunLightSource->setLocalStateSetModes(osg::StateAttribute::ON);
	pSunLightSource->setStateSetModes(*globalStateSet,osg::StateAttribute::ON);

	// moonlight
	osg::Light *pMoonLight = m_Sky->getMoonLight();
	osg::LightSource *pMoonLightSource = new osg::LightSource;
	pMoonLightSource->setLight(pMoonLight);
	pMoonLightSource->setLocalStateSetModes(osg::StateAttribute::ON);
	pMoonLightSource->setStateSetModes(*globalStateSet,osg::StateAttribute::ON);

	// light group under EyeTransform for celestial bodies (sun, moon, etc)
	m_SkyLights = new osg::Group;
	m_SkyLights->addChild(pSunLightSource);
	m_SkyLights->addChild(pMoonLightSource);
}

int VirtualScene::drawScene()
{
	CSP_LOG(APP, DEBUG, "VirtualScene::drawScene()...");

	m_FarView->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, m_NearPlane, m_ViewDistance);

	osg::NodeVisitor* UpdateVisitor = m_FarView->getUpdateVisitor();
	UpdateVisitor->setTraversalMask(0x1);
	m_FarView->setUpdateVisitor(UpdateVisitor);
	m_FarView->update();

	osgUtil::CullVisitor* CullVisitor = m_FarView->getCullVisitor();
	CullVisitor->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	CullVisitor->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
	m_FarView->setCullMask(0x2);
	m_FarView->setCullVisitor(CullVisitor);
	m_FarView->cull();

	// FIXME pushing and popping state around the SceneView.draw call should
	// _not_ be necessary, yet all three of the SceneViews currently used by
	// CSP leak state.  It's not clear where the state leaks occur, and the
	// whole problem may go away when we upgrade to the next version of OSG.
	glPushAttrib(GL_ALL_ATTRIB_BITS);  // FIXME
	glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);  // FIXME
	//GlStateSnapshot snapshot;  // <-- uncomment this to log the state leak
	m_FarView->draw();
	//snapshot.logDiff("far view");  // <-- uncomment this to log the state leak
	glPopClientAttrib();  // FIXME
	glPopAttrib();  // FIXME

	if (m_NearObjectGroup->getNumChildren() > 0) {
		assert(m_NearObjectGroup->getNumChildren() == 1);
		m_NearView->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, 0.01f, 100.0);

		osg::NodeVisitor* UpdateVisitor = m_NearView->getUpdateVisitor();
		UpdateVisitor->setTraversalMask(0x1);
		m_NearView->setUpdateVisitor(UpdateVisitor);
		m_NearView->update();

		osgUtil::CullVisitor* CullVisitor = m_NearView->getCullVisitor();
		CullVisitor->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
		CullVisitor->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
		m_NearView->setCullMask(0x2);
		m_NearView->setCullVisitor(CullVisitor);
		m_NearView->cull();

		// FIXME pushing and popping state around the SceneView.draw call should
		// _not_ be necessary, yet all three of the SceneViews currently used by
		// CSP leak state.  It's not clear where the state leaks occur, and the
		// whole problem may go away when we upgrade to the next version of OSG.
		glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);  // FIXME
		glPushAttrib(GL_ALL_ATTRIB_BITS); // FIXME
		//GlStateSnapshot snapshot; // <-- uncomment this to log the state leak
		m_NearView->draw();
		//snapshot.logDiff("near view"); // <-- uncomment this to log the state leak
		glPopClientAttrib(); // FIXME
		glPopAttrib(); // FIXME
	}

	if (m_Terrain.valid()) m_Terrain->endDraw();

	return 1;
}

void VirtualScene::onUpdate(float dt)
{
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


void VirtualScene::setCameraNode(osg::Node *)
{
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

void VirtualScene::_setLookAt(const simdata::Vector3& eyePos, const simdata::Vector3& lookPos, const simdata::Vector3& upVec)
{
	CSP_LOG(APP, DEBUG, "VirtualScene::setLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);

	assert(m_FarView.valid());
	_updateOrigin(eyePos);
	osg::Vec3 _up (upVec.x(), upVec.y(), upVec.z() );

	m_FarView->setViewMatrixAsLookAt(osg::Vec3(0.0, 0.0, 0.0), simdata::toOSG(lookPos - eyePos), _up);
	m_NearView->setViewMatrixAsLookAt(osg::Vec3(0.0, 0.0, 0.0), simdata::toOSG(lookPos - eyePos), _up);

	m_GlobalFrame->setPosition(simdata::toOSG(-eyePos));

	// move sky with camera in x and y only
	m_EyeTransform->asPositionAttitudeTransform()->setPosition(osg::Vec3(0.0, 0.0, -eyePos.z()));

	_updateFog(lookPos, eyePos);

	if (m_Terrain.valid()) {
		m_Terrain->setCameraPosition(eyePos.x(), eyePos.y(), eyePos.z());
		simdata::Vector3 tpos = m_Terrain->getOrigin(eyePos) - eyePos;
		m_TerrainGroup->setPosition(simdata::toOSG(tpos));
	}

	CSP_LOG(APP, DEBUG, logLookAt());
}

// TODO externalize a couple fixed parameters
void VirtualScene::_updateFog(simdata::Vector3 const &lookPos, simdata::Vector3 const &eyePos)
{
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
	pStateSet->setAttributeAndModes(pFogAttr ,osg::StateAttribute::ON);
	m_FogGroup->setStateSet(pStateSet);
	m_Sky->updateHorizon(m_FogColor, eyePos.z(), m_ViewDistance);
}

void VirtualScene::getLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec) const
{
	assert(m_FarView.valid());
	osg::Vec3 _eye;
	osg::Vec3 _center;
	osg::Vec3 _up;
	const_cast<osgUtil::SceneView*>(m_FarView.get())->getViewMatrixAsLookAt(_eye, _center, _up);
	eyePos = simdata::Vector3(_eye.x(), _eye.y(),_eye.z());
	lookPos = simdata::Vector3(_center.x(), _center.y(),_center.z());
	upVec = simdata::Vector3(_up.x(), _up.y(),_up.z());

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

void VirtualScene::setWireframeMode(bool flag)
{
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

void VirtualScene::setFogMode(bool flag)
{
	if (m_FogEnabled == flag) return;
	m_FogEnabled = flag;

	osg::StateSet * pStateSet = m_FogGroup->getStateSet();
	osg::StateAttribute * pStateAttr = pStateSet->getAttribute(osg::StateAttribute::FOG);

	if (flag) {
		pStateSet->setAttributeAndModes(pStateAttr ,osg::StateAttribute::ON);
	} else {
		pStateSet->setAttributeAndModes(pStateAttr ,osg::StateAttribute::OFF);
	}

	m_FogGroup->setStateSet(pStateSet);
}

void VirtualScene::setFogStart(float value)
{
	m_FogStart = value;
}

void VirtualScene::setFogEnd(float value)
{
	m_FogEnd = value;
}

// XXX XXX this needs to be integrated with SceneManager, and
// is probably not worth supporting.  SimpleSceneManager currently
// defines its own value, independent of this one, which is used
// by the battlefield to decide which objects should be added or
// removed from the scene.  it's probably not worth supporting a
// method to change the view distance of an existing scene, since
// the battlefield would then need to update which objects are visible.
void VirtualScene::setViewDistance(float value)
{
	m_ViewDistance = value;
}

void VirtualScene::setViewAngle(float value)
{
	m_ViewAngle = value;
}

void VirtualScene::setNearPlane(float value) {
	m_NearPlane = value;
}

void VirtualScene::setAspect(float value) {
	m_Aspect = value;
}

void VirtualScene::getViewport(int& x,int& y,int& width,int& height) {
	m_FarView->getViewport(x,y,width,height);
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

void VirtualScene::drawPlayerInterface()
{

}

int VirtualScene::getTerrainPolygonsRendered()
{
	if (!m_Terrain) {
		return 0;
	} else {
		return m_Terrain->getTerrainPolygonsRendered();
	}
}

void VirtualScene::setTerrain(simdata::Ref<TerrainObject> terrain)
{
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
	osg::Vec4 ambientLightColor(0.9f,0.1f,0.1f,1.0f);
	int texture_unit = 3;
	osg::NodeCallback* cb = createCullCallback(m_FreeObjectGroup.get(),osg::Vec3(6000.0,1500.0,30000),ambientLightColor,texture_unit);
	m_TerrainGroup->setCullCallback(cb);
	m_TerrainGroup->setCullingActive(true);
	m_FogGroup->setCullingActive(true);
	m_RootNode->setCullingActive(true);
#endif

		}
	}
}

bool VirtualScene::pick(int x, int y) {
	if (m_NearObjectGroup->getNumChildren() > 0) {
		assert(m_NearObjectGroup->getNumChildren() == 1);
		std::cout << "PICK\n";
		osg::Vec3 near;
		osg::Vec3 far;
		const int height = m_NearView->getViewport()->height();
		if (m_NearView->projectWindowXYIntoObject(x, height - y, near, far)) {
			std::cout << x << "," << y << " -> " << near << " " << far << "\n";
			osgUtil::IntersectVisitor iv;
			osg::ref_ptr<osg::LineSegment> line_segment = new osg::LineSegment(near, far);
			iv.addLineSegment(line_segment.get());
			m_NearView->getSceneData()->accept(iv);
			osgUtil::IntersectVisitor::HitList &hits = iv.getHitList(line_segment.get());
			if (!hits.empty()) {
				std::cout << hits.size() << " hits\n";
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
