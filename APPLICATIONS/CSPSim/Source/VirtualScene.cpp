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
#include <osgDB/FileUtils>
#include <osgUtil/SceneView>
#include <osgUtil/CullVisitor>
#include <osgUtil/DisplayListVisitor>

#include "Terrain.h"


// SHADOW is an *extremely* experimental feature.  It is based on the
// osgShadow demo, nad does (did) work to some extent, but only for a 
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
	m_View = NULL;
	m_ViewAngle = 60.0;
	m_ViewDistance = 30000.0;
	m_SpinTheWorld = false;
	m_ResetTheWorld = false;
	// FIXME these values are tied to the demeter's terrain lattice block size.
	// in xml, the vertex spacing is 250 m, which multiplied by 256 x 256 vectex
	// tiles gives 64000 m.  these values cannot be adjusted independently!
	// if we switch from demeter to osgCLOD the situation will likely change, so
	// for now we'll just stick with this hard-coded hack:
	m_LatticeXDist = 64000.0;
	m_LatticeYDist = 64000.0;
	m_LatticeX = 0;
	m_LatticeY = 0;
}

VirtualScene::~VirtualScene()
{
}

int VirtualScene::buildScene()
{
	CSP_LOG(APP, INFO, "VirtualScene::buildScene() ");

	int ScreenWidth = g_ScreenWidth;
	int ScreenHeight = g_ScreenHeight;

	// setup osg search path for external data files
	std::string image_path = g_Config.getPath("Paths", "ImagePath", ".", true);
	std::string model_path = g_Config.getPath("Paths", "ModelPath", ".", true);
	std::string font_path = g_Config.getPath("Paths", "FontPath", ".", true);
	std::string search_path;
	simdata::ospath::addpath(search_path, image_path);
	simdata::ospath::addpath(search_path, model_path);
	simdata::ospath::addpath(search_path, font_path);
	osgDB::setDataFilePathList(search_path);

	// we don't need this on Linux since libs are usually
	// installed in /usr/local/lib/osgPlugins or /usr/lib/osgPlugins.
	// OSG can find itself the plugins.
#ifdef _WIN32
	osgDB::setLibraryFilePathList(".");
#endif

	/////////////////////////////////////
	//
	//(Un)comment to (enable) disable debug info from osg
	//
	//osg::setNotifyLevel(osg::DEBUG_INFO);
	//
	/////////////////////////////////////

	// construct the view
	m_View = new osgUtil::SceneView();
	
	osg::DisplaySettings* ds = m_View->getDisplaySettings();
	if (!ds)
		ds = new osg::DisplaySettings;
	m_View->setDefaults();
	ds->setDepthBuffer(true);
	m_View->setViewport(0, 0, ScreenWidth, ScreenHeight);
	m_View->setBackgroundColor(osg::Vec4(SKY_RED, SKY_GREEN, SKY_BLUE, SKY_ALPHA));
	m_View->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	m_View->getCullVisitor()->setImpostorsActive(true);
	// override default HEADLIGHT mode, we provide our own lights.
	m_View->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);

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
	m_EyeTransform = new osg::Transform;

	// transform's value isn't knowm until in the cull traversal so its 
	// bounding volume can't be determined, therefore culling will be 
	// invalid, so switch it off.  this will cause all our parents to 
	// switch culling off as well, but culling will be remain on 
	// underneath this node or any other branch above this transform.
	m_EyeTransform->setCullingActive(false);

	// set the compute transform callback to do all the work of
	// determining the transform according to the current eye point.
	//0 m_EyeTransform->setComputeTransformCallback(new MoveEarthySkyWithEyePointCallback);
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
	fog->setColor(fogColor);
	fog->setStart(20000.0);
	fog->setEnd(35000.0);
	pFogState->setAttributeAndModes(fog, osg::StateAttribute::ON);
	m_FogGroup->setStateSet(pFogState);

	// add the scene graph to the view
	m_View->setSceneData(m_RootNode.get());

	m_FrameStamp = new osg::FrameStamp;
	m_View->setFrameStamp(m_FrameStamp.get());

	osgDB::Registry::instance();

	return 1;
}

float CM_intensity=0.0;


void VirtualScene::buildSky() 
{
	osg::StateSet* globalStateSet = m_View->getGlobalStateSet();
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
    
	osgUtil::CullVisitor * CullVisitor = m_View->getCullVisitor();
	CullVisitor->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	CullVisitor->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
	m_View->setCullVisitor(CullVisitor);
  
	m_View->update();
	m_View->cull();
	m_View->draw();

	glFinish();

	Demeter::Settings::GetInstance()->LogStats();

	return 1;
}

void VirtualScene::onUpdate(float dt)
{
	static float t = 0.0;
	static bool init = false;
	static float lat = 0.0;
	static float lon = 0.0;

	// temporary testing purposes only
	if (!init) {
		lat = simdata::DegreesToRadians(g_Config.getFloat("Testing", "Latitude", 0, true));
		lon = simdata::DegreesToRadians(g_Config.getFloat("Testing", "Longitude", 0, true));
		init = true;
	}
	if (m_SpinTheWorld || m_ResetTheWorld || (int(t) % 10) == 0) {
		if (m_ResetTheWorld) {
			m_Sky->spinTheWorld(false);
			m_ResetTheWorld = false;
		}
		if (m_SpinTheWorld) m_Sky->spinTheWorld();
		m_Sky->update(lat, lon, CSPSim::theSim->getCurrentTime());
		// greenwich, england (for testing)
		//m_Sky->update(0.8985, 0.0, CSPSim::theSim->getCurrentTime());
		t+=1.0;
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

	assert(m_View.valid());
	osg::Camera * camera = m_View->getCamera();
    
	m_Origin = eyePos;

	int XLatticePos = (int) (eyePos.x / m_LatticeXDist);
	int YLatticePos = (int) (eyePos.y / m_LatticeYDist);

	if (m_LatticeX != XLatticePos || m_LatticeY != YLatticePos) {
		// TODO update battlefield components
		m_LatticeX = XLatticePos;
		m_LatticeY = YLatticePos;
		m_TerrainOrigin = simdata::Vector3(XLatticePos * m_LatticeXDist, YLatticePos * m_LatticeYDist, 0.0);
		std::cout << "repositioning terrain origin to " << (-m_TerrainOrigin) << std::endl;
	}

	//m_FeatureGroup->setPosition(osg::Vec3(-eyePos.x, -eyePos.y, -eyePos.z));

	osg::Vec3 _up (upVec.x, upVec.y, upVec.z );
	camera->setLookAt(osg::Vec3(0.0, 0.0, 0.0), simdata::toOSG(lookPos - eyePos), _up);
	camera->ensureOrthogonalUpVector();

	m_GlobalFrame->setPosition(simdata::toOSG(-eyePos));

	//AdjustCM(m_Sky->getSkyIntensity());
	intensity_test = m_Sky->getSkyIntensity();
	osg::Light *sun = m_Sky->getSunLight();
	osg::Vec3 sdir_ = sun->getDirection();
	simdata::Vector3 sdir(sdir_.x(), sdir_.y(), sdir_.z());
	simdata::Vector3 dir = lookPos - eyePos;
	dir.Normalize();
	sdir.Normalize();
	float sunz = (1.0 - sdir.z);
	if (sunz > 1.0) sunz = 2.0 - sunz;
	float eyez = eyePos.z;
	double a = simdata::Dot(dir, sdir) * sunz;
	//setFogStart(15000.0 + eyez + 15000.0*a);
	{
		osg::StateSet *pStateSet = m_FogGroup->getStateSet();
		osg::Fog * pFogAttr = (osg::Fog*)pStateSet->getAttribute(osg::StateAttribute::FOG);
		float angle = atan2(dir.y, dir.x) * 180.0 / 3.14;
		osg::Vec4 color = m_Sky->getHorizonColor(angle) * 0.8;
		pFogAttr->setColor(color);
		pFogAttr->setStart(15000.0 + eyez + 15000.0*a);
		pStateSet->setAttributeAndModes(pFogAttr ,osg::StateAttribute::ON);
		m_FogGroup->setStateSet(pStateSet);
	}
    
	if (m_Terrain.valid()) {
		m_Terrain->setCameraPosition(eyePos.x, eyePos.y, eyePos.z);
		osg::Vec3 tpos = osg::Vec3(m_TerrainOrigin.x - eyePos.x, m_TerrainOrigin.y - eyePos.y, -eyePos.z);
		m_TerrainGroup->setPosition(tpos);
	}


	CSP_LOG(APP, DEBUG, "VirtualScene::setLookAt - eye: " << camera->getEyePoint()  << 
	  ", look: " << camera->getCenterPoint()  << ", up: " << camera->getUpVector()  <<
	  ", near: " << camera->zNear() << ", far: " << camera->zFar() );

}

#if 0
void VirtualScene::setLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec)
{
	CSP_LOG(APP, DEBUG, "VirtualScene::setLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);

	assert(m_View.valid());
	osg::Camera * camera = m_View->getCamera();
    
	int XLatticePos = (int) (eyePos.x / m_LatticeXDist);
	int YLatticePos = (int) (eyePos.y / m_LatticeYDist);

	if (m_LatticeX != XLatticePos || m_LatticeY != YLatticePos) {
		// TODO update battlefield components
		m_LatticeX = XLatticePos;
		m_LatticeY = YLatticePos;
		m_Origin = simdata::Vector3(XLatticePos * m_LatticeXDist, YLatticePos * m_LatticeYDist, 0.0);
		std::cout << "repositioning feature origin to " << (-m_Origin) << std::endl;
		m_FeatureGroup->setPosition(simdata::toOSG(-m_Origin));
	}



	double localEyePosition_x = eyePos.x - m_Origin.x;
	double localEyePosition_y = eyePos.y - m_Origin.y;

	double localLookPosition_x = lookPos.x - m_Origin.x;
	double localLookPosition_y = lookPos.y - m_Origin.y;
    
	osg::Vec3 _localEye( localEyePosition_x,  localEyePosition_y,  eyePos.z) ;
	osg::Vec3 _localLook(localLookPosition_x, localLookPosition_y, lookPos.z ) ;
	osg::Vec3 _up (upVec.x, upVec.y, upVec.z );
    
	camera->setLookAt(_localEye, _localLook , _up);
	camera->ensureOrthogonalUpVector();

	//AdjustCM(m_Sky->getSkyIntensity());
	intensity_test = m_Sky->getSkyIntensity();
	osg::Light *sun = m_Sky->getSunLight();
	osg::Vec3 sdir_ = sun->getDirection();
	simdata::Vector3 sdir(sdir_.x(), sdir_.y(), sdir_.z());
	simdata::Vector3 dir = lookPos - eyePos;
	dir.Normalize();
	sdir.Normalize();
	float sunz = (1.0 - sdir.z);
	if (sunz > 1.0) sunz = 2.0 - sunz;
	float eyez = eyePos.z;
	double a = simdata::Dot(dir, sdir) * sunz;
	//setFogStart(15000.0 + eyez + 15000.0*a);
	{
		osg::StateSet *pStateSet = m_ObjectGroup->getStateSet();
		osg::Fog * pFogAttr = (osg::Fog*)pStateSet->getAttribute(osg::StateAttribute::FOG);
		float angle = atan2(dir.y, dir.x) * 180.0 / 3.14;
		osg::Vec4 color = m_Sky->getHorizonColor(angle) * 0.8;
		pFogAttr->setColor(color);
		pFogAttr->setStart(15000.0 + eyez + 15000.0*a);
		pStateSet->setAttributeAndModes(pFogAttr ,osg::StateAttribute::ON);
		//osg::ColorMatrix* cm = (osg::ColorMatrix *) pStateSet->getAttribute(osg::StateAttribute::COLORMATRIX);
		//setNVG(cm);
		//pStateSet->setAttributeAndModes(cm ,osg::StateAttribute::OVERRIDE);
		m_ObjectGroup->setStateSet(pStateSet);
	}
    
    	if (0)
    	{
		static bool up = false;
		static float I = 0.0;
		if (up) I = I + 0.01; else I = I - 0.01;
		if (!up && I < 0.05) up = true;
		if (up && I > 0.95) up = false;
		CM_intensity = I*0.4;
		
		osg::StateSet* pStateSet = m_View->getGlobalStateSet();
		osg::ColorMatrix* cm = (osg::ColorMatrix *) pStateSet->getAttribute(osg::StateAttribute::COLORMATRIX);
		//setCM(cm, I*0.4);
		setNVG(cm);
		pStateSet->setAttributeAndModes(cm ,osg::StateAttribute::OVERRIDE);
		m_View->setGlobalStateSet(pStateSet);
	
	}


	if (m_Terrain.valid())
		m_Terrain->setCameraPosition( eyePos.x, eyePos.y, eyePos.z );


	CSP_LOG(APP, DEBUG, "VirtualScene::setLookAt - eye: " << camera->getEyePoint()  << 
	  ", look: " << camera->getCenterPoint()  << ", up: " << camera->getUpVector()  <<
	  ", near: " << camera->zNear() << ", far: " << camera->zFar() );

}
#endif

void VirtualScene::getLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec) const
{
	assert(m_View.valid());
	osg::Camera const * camera = m_View->getCamera();

	osg::Vec3 _eye = camera->getEyePoint();
	eyePos = simdata::Vector3(_eye.x(), _eye.y(),_eye.z());
	osg::Vec3 _center = camera->getCenterPoint();
	lookPos = simdata::Vector3(_center.x(), _center.y(),_center.z());
	osg::Vec3 _up = camera->getUpVector();
	upVec = simdata::Vector3(_up.x(), _up.y(),_up.z());

	CSP_LOG(APP, DEBUG, "VirtualScene::getLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);
}



void VirtualScene::addParticleSystem(osg::Node *system, osg::Node *program) {
// XXX temporary, for testing (should go under a separate branch of the sg)
	std::cout << "ADD PS\n";
	if (program != 0) m_GlobalFrame->addChild(program);
	m_GlobalFrame->addChild(system);
	//m_RootNode->addChild(system);
	//m_RootNode->addChild(program);
}
void VirtualScene::removeParticleSystem(osg::Node *system, osg::Node *program) {
// XXX temporary, for testing (should go under a separate branch of the sg)
	std::cout << "REM PS\n";
	if (program != 0) m_GlobalFrame->removeChild(program);
	m_GlobalFrame->removeChild(system);
	//m_RootNode->removeChild(system);
	//m_RootNode->removeChild(program);
}

void VirtualScene::addEffectUpdater(osg::Node *updater) {
	std::cout << "ADD EU\n";
	m_ParticleUpdaterGroup->addChild(updater);
}

void VirtualScene::removeEffectUpdater(osg::Node *updater) {
	std::cout << "REM EU\n";
	m_ParticleUpdaterGroup->removeChild(updater);
}

void VirtualScene::addParticleEmitter(osg::Node *emitter) {
	std::cout << "ADD PE\n";
	m_ParticleEmitterGroup->addChild(emitter);
}

void VirtualScene::removeParticleEmitter(osg::Node *emitter) {
	std::cout << "REM PE\n";
	m_ParticleEmitterGroup->removeChild(emitter);
}

void VirtualScene::addObject(simdata::Ref<SimObject> object) {
	assert(object.valid());
	assert(m_FreeObjectGroup.valid());
	osg::Node *node = object->getOrCreateModelNode();
	m_FreeObjectGroup->addChild(node);
}

void VirtualScene::removeObject(simdata::Ref<SimObject> object) {
	assert(object.valid());
	assert(m_FreeObjectGroup.valid());
	osg::Node *node = object->getOrCreateModelNode();
	m_FreeObjectGroup->removeChild(node);
}

void VirtualScene::addFeatureCell(osg::Node *cell) {
	assert(cell);
	m_FeatureGroup->addChild(cell);
	std::cout << "adding feature to scene";
	osg::PositionAttitudeTransform *p;
	p = dynamic_cast<osg::PositionAttitudeTransform *>(cell);
	if (p) {
		//p->setPosition(p->getPosition() + osg::Vec3(0.0, 0.0, 90.5));
		std::cout << " @ position: " << p->getPosition();
		/*
		osg::Node *n = p->getChild(0);
		if (n) {
			p = dynamic_cast<osg::PositionAttitudeTransform *>(n);
			if (p) {
				std::cout << p->getPosition() << std::endl;
			}
		}
		*/
	}
	std::cout << std::endl;
}

void VirtualScene::removeFeatureCell(osg::Node *cell) {
	assert(cell);
	std::cout << "removing feature from scene\n";
	osg::PositionAttitudeTransform *p;
	p = dynamic_cast<osg::PositionAttitudeTransform *>(cell);
	if (p) {
		p->setPosition(p->getPosition() + osg::Vec3(0.0, 0.0, 87.5));
		std::cout << " @ position: " << p->getPosition();
	}
	std::cout << std::endl;
	m_FeatureGroup->removeChild(cell);
}


void VirtualScene::setWireframeMode(bool flag)
{
	osg::StateSet* globalStateSet = m_View->getGlobalStateSet();
	if (!globalStateSet) {
		globalStateSet = new osg::StateSet;
		m_View->setGlobalStateSet(globalStateSet);
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
	osg::StateSet * pStateSet = m_FogGroup->getStateSet();
	osg::Fog * pFogAttr = (osg::Fog*)pStateSet->getAttribute(osg::StateAttribute::FOG);

	pFogAttr->setStart(value);

	pStateSet->setAttributeAndModes(pFogAttr ,osg::StateAttribute::ON);    
	m_FogGroup->setStateSet(pStateSet);
}

void VirtualScene::setFogEnd(float value)
{
	osg::StateSet * pStateSet = m_FogGroup->getStateSet();
	osg::Fog * pFogAttr = (osg::Fog*)pStateSet->getAttribute(osg::StateAttribute::FOG);

	pFogAttr->setEnd(value);

	pStateSet->setAttributeAndModes(pFogAttr ,osg::StateAttribute::ON);    
	m_FogGroup->setStateSet(pStateSet);
}

void VirtualScene::setViewDistance(float value)
{
	osg::Camera * camera = m_View->getCamera();
	m_ViewDistance = value;
	camera->setPerspective(m_ViewAngle, 1.0, 0.3f, m_ViewDistance);
}

void VirtualScene::setViewAngle(float value)
{
	osg::Camera * camera = m_View->getCamera();
	m_ViewAngle = value;
	camera->setPerspective(m_ViewAngle, 1.0, 0.3f, m_ViewDistance);
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

	float xPatch = 35000 + 448000;
	float yPatch = 6000 + 500000;

	if (m_Terrain.valid()) {
		m_Terrain->setCameraPosition(xPatch, yPatch, 1000);
		m_TerrainNode = m_Terrain->getNode();
		if (m_TerrainNode.valid()) {
			m_TerrainGroup->addChild(m_TerrainNode.get());
		}
	}

}
