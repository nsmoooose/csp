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
 * @file VirtualScene.cpp
 *
 **/


#include "VirtualScene.h"
#include "VirtualBattlefield.h"
#include "TerrainObject.h"
#include "DynamicObject.h"
#include "CSPSim.h"
#include "Config.h"
#include "Log.h"
#include "Sky.h"

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
#include <osgUtil/SceneView>
#include <osgUtil/Optimizer>
#include <osgUtil/CullVisitor>
#include <osgUtil/DisplayListVisitor>

//#include "Terrain.h"


// SHADOW is an *extremely* experimental feature.  It is based on the
// osgShadow demo, and does (did) work to some extent, but only for a 
// single localized object.  A more robust approach needs to be taken
// but the code remains for reference.
#define NOSHADOW
#ifdef SHADOW
#include "shadow.h"
#endif

extern int g_ScreenWidth;
extern int g_ScreenHeight;


extern osg::Node *makeBase( void );
extern osg::Node *makeTreesPatch( float xcen, float ycen, float spacing, float width, 
					 float height, VirtualBattlefield * pBattlefield);

// XXX these values are no longer used

const float SKY_RED = 0.1f;
const float SKY_GREEN = 0.1f;
const float SKY_BLUE = 0.6f;
const float SKY_ALPHA = 1.0f;

// XXX these values are no longer used

const float FOG_RED = 0.4f;
const float FOG_GREEN = 0.4f;
const float FOG_BLUE = 0.5f;
const float FOG_ALPHA = 1.0f;


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



/**
 * struct MoveEarthySkyWithEyePointCallback
 *
 * @author unknown
 */
struct MoveEarthySkyWithEyePointCallback : public osg::Transform::ComputeTransformCallback
{
	/** Get the transformation matrix which moves from local coords to world coords.*/
	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,const osg::Transform*, osg::NodeVisitor* nv) const {
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv) {
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),0.0f));
			//matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
		}
		return true;
	}

	/** Get the transformation matrix which moves from world coords to local coords.*/
	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,const osg::Transform*, osg::NodeVisitor* nv) const {
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv) {
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),0.0));
			//matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
		}
		return true;
	}
};



VirtualScene::VirtualScene()
{
	m_FarView = NULL;
	m_NearView = NULL;
	m_ViewAngle = 60.0;
	m_NearPlane = 2.0;
	m_Aspect =  static_cast<float>(g_ScreenWidth)/g_ScreenHeight;
	m_ViewDistance = 30000.0;
	m_SpinTheWorld = false;
	m_ResetTheWorld = false;
	m_Wireframe = false;
}

VirtualScene::~VirtualScene()
{
}

int VirtualScene::buildScene()
{
	CSP_LOG(APP, INFO, "VirtualScene::buildScene() ");

	int ScreenWidth = g_ScreenWidth;
	int ScreenHeight = g_ScreenHeight;

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
	m_FarView->setBackgroundColor(osg::Vec4(SKY_RED, SKY_GREEN, SKY_BLUE, SKY_ALPHA));
	m_FarView->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	m_FarView->getCullVisitor()->setImpostorsActive(true);
	// override default HEADLIGHT mode, we provide our own lights.
	m_FarView->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);

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
	//  m_EyeTransform->setComputeTransformCallback(new MoveEarthySkyWithEyePointCallback);
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
	fog->setDensity(0.3f);
	osg::Vec4 fogColor;
	fogColor[0] = FOG_RED;
	fogColor[1] = FOG_GREEN; 
	fogColor[2] = FOG_BLUE; 
	fogColor[3] = FOG_ALPHA;
	m_FogEnabled = true;
	m_FogStart = 20000.0;
	m_FogEnd = 40000.0;
	fog->setColor(fogColor);
	fog->setStart(m_FogStart);
	fog->setEnd(m_FogEnd);
	if (m_FogEnabled) {
		pFogState->setAttributeAndModes(fog, osg::StateAttribute::ON);
	} else {
		pFogState->setAttributeAndModes(fog, osg::StateAttribute::OFF);
	}
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
#ifdef CSP_OSG_094
	m_NearView->setCamera(m_FarView->getCamera());
#else
	//m_NearView->setProjectionMatrix(m_FarView->getProjectionMatrix());
	//m_NearView->setViewMatrix(m_FarView->getViewMatrix());
#endif
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
    
	osgUtil::CullVisitor * CullVisitor;

#ifdef CSP_OSG_094
	osg::Camera * camera = m_FarView->getCamera();
	camera->setPerspective(m_ViewAngle, 1.0, 2.0f, m_ViewDistance);
#else
	m_FarView->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, m_NearPlane, m_ViewDistance);
#endif
	CullVisitor = m_FarView->getCullVisitor();
	CullVisitor->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	CullVisitor->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
	m_FarView->setCullVisitor(CullVisitor);
	m_FarView->update();
	m_FarView->cull();
	m_FarView->draw();

	if (m_NearObjectGroup->getNumChildren() > 0) {
#ifdef CSP_OSG_094
		camera->setPerspective(m_ViewAngle, 1.0, 0.01f, 100.0);
#else
		m_NearView->setProjectionMatrixAsPerspective(m_ViewAngle, m_Aspect, 0.01f, 100.0);
#endif
		CullVisitor = m_NearView->getCullVisitor();
		CullVisitor->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
		CullVisitor->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
		m_NearView->setCullVisitor(CullVisitor);
		m_NearView->update();
		m_NearView->cull();
		m_NearView->draw();
	}

	glFinish();

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
			simdata::LLA m = m_Terrain->getProjection().convert(m_Origin);
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


void VirtualScene::setCameraNode( osg::Node * pNode)
{
}

void VirtualScene::setLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec)
{
	CSP_LOG(APP, DEBUG, "VirtualScene::setLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);

	assert(m_FarView.valid());
	m_Origin = eyePos;
	osg::Vec3 _up (upVec.x(), upVec.y(), upVec.z() );
#ifdef CSP_OSG_094
	osg::Camera * camera = m_FarView->getCamera();
	camera->setLookAt(osg::Vec3(0.0, 0.0, 0.0), simdata::toOSG(lookPos - eyePos), _up);
	camera->ensureOrthogonalUpVector();
#else
	m_FarView->setViewMatrixAsLookAt(osg::Vec3(0.0, 0.0, 0.0), simdata::toOSG(lookPos - eyePos), _up);
	m_NearView->setViewMatrixAsLookAt(osg::Vec3(0.0, 0.0, 0.0), simdata::toOSG(lookPos - eyePos), _up);
#endif

	m_GlobalFrame->setPosition(simdata::toOSG(-eyePos));

	// move sky with camera in x and y only
	m_EyeTransform->asPositionAttitudeTransform()->setPosition(osg::Vec3(0.0, 0.0, -eyePos.z()));

	_updateFog(lookPos, eyePos);
    
	if (m_Terrain.valid()) {
		m_Terrain->setCameraPosition(eyePos.x(), eyePos.y(), eyePos.z());
		simdata::Vector3 tpos = m_Terrain->getOrigin(eyePos) - eyePos;
		m_TerrainGroup->setPosition(simdata::toOSG(tpos));
	}

#ifdef CSP_OSG_094
	CSP_LOG(APP, DEBUG, 
		"VirtualScene::setLookAt - eye: " << camera->getEyePoint()  
		<< ", look: " << camera->getCenterPoint()  
		<< ", up: " << camera->getUpVector()  
		<< ", near: " << camera->zNear() 
		<< ", far: " << camera->zFar() 
	);
#else
	// FIXME this shouldn't run in debug builds
	osg::Vec3 _camEyePos;
	osg::Vec3 _camLookPos;
	osg::Vec3 _camUpVec;
	m_FarView->getViewMatrixAsLookAt(_camEyePos, _camLookPos, _camUpVec);
	CSP_LOG(APP, DEBUG, 
		"VirtualScene::setLookAt - eye: " << _camEyePos  
		<< ", look: " << _camLookPos  
		<< ", up: " << _camUpVec 
	);
#endif

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

#ifdef CSP_OSG_094
void VirtualScene::getLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec) const
{
	assert(m_FarView.valid());
	osg::Camera const * camera = m_FarView->getCamera();

	osg::Vec3 _eye = camera->getEyePoint();
	eyePos = simdata::Vector3(_eye.x(), _eye.y(),_eye.z());
	osg::Vec3 _center = camera->getCenterPoint();
	lookPos = simdata::Vector3(_center.x(), _center.y(),_center.z());
	osg::Vec3 _up = camera->getUpVector();
	upVec = simdata::Vector3(_up.x(), _up.y(),_up.z());

	CSP_LOG(APP, DEBUG, "VirtualScene::getLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);
}
#else
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
#endif


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
	assert(!object->getSceneFlag());
	object->setSceneFlag(true);
	osg::Node *node = object->getOrCreateModelNode();
	if (object->getNearFlag()) {
		m_NearObjectGroup->addChild(node);
	} else {
		m_FreeObjectGroup->addChild(node);
	}
}

void VirtualScene::removeObject(simdata::Ref<DynamicObject> object) {
	assert(object.valid());
	assert(object->getSceneFlag());
	object->setSceneFlag(false);
	osg::Node *node = object->getOrCreateModelNode();
	if (object->getNearFlag()) {
		m_NearObjectGroup->removeChild(node);
	} else {
		m_FreeObjectGroup->removeChild(node);
	}
}

void VirtualScene::setNearObject(simdata::Ref<DynamicObject> object, bool isNear) {
	assert(object.valid());
	if (object->getNearFlag() == isNear) return;
	object->setNearFlag(isNear);
	if (object->getSceneFlag()) {
		osg::Node *node = object->getOrCreateModelNode();
		if (isNear) {
			m_FreeObjectGroup->removeChild(node);
			m_NearObjectGroup->addChild(node);
		} else {
			m_NearObjectGroup->removeChild(node);
			m_FreeObjectGroup->addChild(node);
		}
	}
}

void VirtualScene::addFeatureCell(osg::Node *cell) {
	assert(cell);
	m_FeatureGroup->addChild(cell);
	CSP_LOG(SCENE, DEBUG, "Adding feature cell to scene.");
}

void VirtualScene::removeFeatureCell(osg::Node *cell) {
	assert(cell);
	CSP_LOG(SCENE, DEBUG, "Removing feature cell from scene.");
	m_FeatureGroup->removeChild(cell);
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

