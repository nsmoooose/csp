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
 * @file VirtualBattlefield.cpp
 *
 **/

#include <sstream>

#include <osg/Fog>
#include <osg/Notify>
#include <osg/PolygonMode>
#include <osg/ColorMatrix>

#include <osgDB/FileUtils>

#include <osgUtil/CullVisitor>

#include "LogStream.h"
#include "VirtualBattlefield.h"

#include <SimData/Types.h>
#include <SimData/Math.h>


extern int g_ScreenWidth;
extern int g_ScreenHeight;

extern double g_LatticeXDist;
extern double g_LatticeYDist;

extern osg::Node *makeSky( void );
extern osg::Node *makeBase( void );
extern osg::Node *makeTreesPatch( float xcen, float ycen, float spacing, float width, 
					 float height, VirtualBattlefield * pBattlefield);

const float SKY_RED = 0.1f;
const float SKY_GREEN = 0.1f;
const float SKY_BLUE = 0.6f;
const float SKY_ALPHA = 1.0f;

const float FOG_RED = 0.4f;
const float FOG_GREEN = 0.4f;
const float FOG_BLUE = 0.5f;
const float FOG_ALPHA = 1.0f;

const float MAX_VIEW_DISTANCE = 10000.0f;


unsigned int VirtualBattlefield::latest_object_id = 0;

using simdata::Pointer;


#include <osgParticle/LinearInterpolator>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/SegmentPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>

class SmokeExpander: public osgParticle::Operator
{
public:
	META_Object(csp, SmokeExpander);
	SmokeExpander() {}
	SmokeExpander(const SmokeExpander &copy, const osg::CopyOp &copyop): Operator(copy, copyop) {}
	virtual void operate(osgParticle::Particle *p, double dt)
	{
		static int x = 0;
		if (p && isEnabled()) {
			float age = p->getAge();
			if (age > 1.0 && age < 2.0) {
				float dt = age-1.0;
				float r = 0.25+dt*dt;
				p->setSizeRange(osgParticle::rangef(0.0, r * 5.0 / age));
			} else
			if (age > 2.0) {
				float dt = age-2.0;
				float r = 1.25 + sqrt(dt);
				p->setSizeRange(osgParticle::rangef(0.0, r * 5.0 / age));
			}
			x++;
			if (age > 1.0 && (x % 300)==0) p->setLifeTime(age);
		}
	}
};

osgParticle::ParticleSystem *setupParticleSystem(osg::MatrixTransform *base, 
                                                 const string & p_textureFile,
                                                 const osg::Vec4 &colorMin,
                                                 const osg::Vec4 &colorMax,
                                                 const osg::Vec3 &center, 
						 osgParticle::SegmentPlacer *&placer,
                                                 float lifetime)
{
	osgParticle::Particle ptemp;
	ptemp.setShape(osgParticle::Particle::QUAD);
	ptemp.setLifeTime(lifetime);
	ptemp.setAlphaRange(osgParticle::rangef(1,0));
	ptemp.setSizeRange(osgParticle::rangef(0.25, 5.0));
	//ptemp.setSizeRange(osgParticle::rangef(0.25, 0.25));
	ptemp.setPosition(osg::Vec3(0,0,0));
	ptemp.setVelocity(osg::Vec3(0,0,0));
	ptemp.setColorRange(osgParticle::rangev4(colorMin, colorMax));

	osgParticle::ParticleSystem *ps = osgNew osgParticle::ParticleSystem;
	//setDefaultAttributes(const std::string &texturefile = "", 
	//                     bool emissive_particles = true, bool lighting = false, int texture_unit = 0) 

	ps->setDefaultAttributes(p_textureFile, false, false,0);
	ps->setDefaultParticleTemplate(ptemp);
	ps->setFreezeOnCull(false);

	osgParticle::SegmentPlacer *sp = osgNew osgParticle::SegmentPlacer;
	sp->setVertexA(center);
	sp->setVertexB(center + osg::Vec3(0, 0, 10));
	placer = sp;

	osgParticle::RandomRateCounter *rrc = osgNew osgParticle::RandomRateCounter;
	rrc->setRateRange(1000, 1200);

	osgParticle::RadialShooter *rs = osgNew osgParticle::RadialShooter;
	rs->setPhiRange(-1, 1);
	rs->setThetaRange(-0.5, 0.5);
	rs->setInitialSpeedRange(0, 8.0);

	osgParticle::ModularEmitter *me = osgNew osgParticle::ModularEmitter;
	me->setParticleSystem(ps);
	me->setPlacer(sp);
	me->setCounter(rrc);
	me->setShooter(rs);
	base->addChild(me);

	osgParticle::AccelOperator *aop = osgNew osgParticle::AccelOperator;
	aop->setToGravity();
	SmokeExpander *se = osgNew SmokeExpander;

	osgParticle::ModularProgram *mp = osgNew osgParticle::ModularProgram;
	mp->setParticleSystem(ps);
	//mp->addOperator(aop);
	//mp->addOperator(se);
	base->addChild(mp);

	return ps;
}

/**
 * struct MoveEarthySkyWithEyePointCallback
 *
 * @author unknown
 */
struct MoveEarthySkyWithEyePointCallback : public osg::Transform::ComputeTransformCallback
{
    /** Get the transformation matrix which moves from local coords to world coords.*/
    virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,const osg::Transform*, osg::NodeVisitor* nv) const 
    {
        osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if (cv)
        {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
            matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),0.0f));
			//matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
        }
        return true;
    }

    /** Get the transformation matrix which moves from world coords to local coords.*/
    virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,const osg::Transform*, osg::NodeVisitor* nv) const
    {
        osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if (cv)
        {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),0.0));
            //matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
        }
        return true;
    }
};

VirtualBattlefield::VirtualBattlefield()
{
	m_pView = NULL;
	m_ViewAngle = 60.0;
	m_ViewDistance = 30000.0;
}

VirtualBattlefield::~VirtualBattlefield()
{

}

int VirtualBattlefield::create()
{
	return 1;
}

void VirtualBattlefield::Cleanup()
{
}


float VirtualBattlefield::getElevation( float x,float y ) const
{
	if (!m_ActiveTerrainObject.isNull()) {
		return m_ActiveTerrainObject->getElevation(x,y);
	} else {
		return 0.0;
	}
}

void VirtualBattlefield::getNormal(float x, float y, float & normalX, 
                                   float & normalY, float & normalZ ) const
{
	normalX = 0; normalY = 0; normalZ = 1;
	if (!m_ActiveTerrainObject.isNull()) {
		m_ActiveTerrainObject->getNormal(x,y, normalX, normalY, normalZ);
	}
}


int VirtualBattlefield::buildScene()
{
	CSP_LOG(CSP_APP, CSP_INFO, "VirtualBattlefield::buildScene() ");

	int ScreenWidth = g_ScreenWidth;
	int ScreenHeight = g_ScreenHeight;

	// must be updated to match your file path configuration
#ifdef _WIN32
	std::string sep = ";";
#else
	std::string sep = ":";
#endif

	osgDB::setDataFilePathList("../Data" + sep + "../Data/Images" + sep + "../Data/Models" + sep + "../Data/Fonts");

	// we don't need this on Linux since libs are usually
	// installed in /usr/local/lib/osgPlugins or /usr/lib/osgPlugins.
	// OSG can find itself the plugins.
#ifdef _WIN32
	osgDB::setLibraryFilePathList("../Bin");
#endif

	/////////////////////////////////////
	//
	//(Un)comment to (enable) disable debug info from osg
	//
	//osg::setNotifyLevel(osg::DEBUG_INFO);
	//
	/////////////////////////////////////

	m_rpRootNode = osgNew osg::Group;
	m_rpRootNode->setName( "RootSceneGroup" );

	m_rpObjectRootNode = osgNew osg::Group;
	m_rpObjectRootNode->setName("ObjectRootSceneGraph.");

	osg::ClearNode* earthSky = osgNew osg::ClearNode;
	earthSky->setRequiresClear(false); // we've got base and sky to do it.
	//earthSky->setRequiresClear(true); // we've got base and sky to do it.

	// use a transform to make the sky and base around with the eye point.
	osg::Transform* transform = osgNew osg::Transform;

	// transform's value isn't knowm until in the cull traversal so its bounding
	// volume can't be determined, therefore culling will be invalid,
	// so switch it off, this cause all our parents to switch culling
	// off as well. But don't worry culling will be back on once underneath
	// this node or any other branch above this transform.
	transform->setCullingActive(false);

	// set the compute transform callback to do all the work of
	// determining the transform according to the current eye point.
	transform->setComputeTransformCallback(osgNew MoveEarthySkyWithEyePointCallback);

	// add the sky and base layer.
	transform->addChild(makeSky());  // bin number -2 so drawn first.
	transform->addChild(makeBase()); // bin number -1 so draw second.      

	// add the transform to the earth sky.
	earthSky->addChild(transform);

	// add to earth sky to the scene.
	m_rpRootNode->addChild(earthSky);

	m_pView = osgNew osgUtil::SceneView();
	m_pView->setDefaults();
	m_pView->setViewport(0,0,ScreenWidth,ScreenHeight);

	osg::Vec4 bkgColor;
	bkgColor[0] = SKY_RED;
	bkgColor[1] = SKY_GREEN;
	bkgColor[2] = SKY_BLUE;
	bkgColor[3] = SKY_ALPHA;
	m_pView->setBackgroundColor(bkgColor);
	m_pView->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	m_pView->getCullVisitor()->setImpostorsActive(true);

	// Fog properties: start and end distances are read from Start.csp
	osg::StateSet * pFogState = osgNew osg::StateSet;
	osg::Fog* fog = osgNew osg::Fog;
	fog->setMode(osg::Fog::LINEAR);
	fog->setDensity(0.3f);

	osg::Vec4 fogColor;
	fogColor[0] = FOG_RED;
	fogColor[1] = FOG_GREEN; 
	fogColor[2] = FOG_BLUE; 
	fogColor[3] = FOG_ALPHA;
	fog->setColor(fogColor);
	pFogState->setAttributeAndModes(fog ,osg::StateAttribute::ON);

	// light properties
	osg::Light * pLight = osgNew osg::Light;

#ifndef DARK_TEST
	pLight->setDirection( osg::Vec3(0,0,-1) );
	pLight->setAmbient( osg::Vec4(0.3f,0.3f,0.3f,1.0f) );
	pLight->setDiffuse( osg::Vec4(0.8f,0.8f,0.8f,1.0f) );
	pLight->setSpecular( osg::Vec4(0.75f,0.75f,0.75f,1.0f) );
#else
	osg::ref_ptr<osg::ColorMatrix> cm = osgNew osg::ColorMatrix;
	cm->setMatrix(osg::Matrix(0.5,0.2,0.2,0.0,0.3,0.6,0.3,0.0,0.2,0.2,0.5,0,0,0,0,1));
	osg::State state;
	cm->apply(state);
	pLight->setDirection( osg::Vec3(0,0,-1) );
	pLight->setAmbient( osg::Vec4(0.1f,0.1f,0.1f,1.0f) );
	pLight->setDiffuse( osg::Vec4(0.3f,0.3f,0.3f,1.0f) );
	pLight->setSpecular( osg::Vec4(0.25f,0.25f,0.25f,1.0f) );
#endif

	osg::StateSet * pLightSet = osgNew osg::StateSet;

	pFogState->setAttributeAndModes(pLight, osg::StateAttribute::ON);

	m_rpObjectRootNode->setStateSet(pLightSet);
	m_rpObjectRootNode->setStateSet(pFogState);

	//m_pView->setLightingMode(osgUtil::SceneView::SKY_LIGHT); // HEADLIGHT is default

	m_rpRootNode->addChild(m_rpObjectRootNode.get());

	m_pView->setSceneData(m_rpRootNode.get() );

	m_rpFrameStamp = osgNew osg::FrameStamp;

	m_pView->setFrameStamp(m_rpFrameStamp.get());

	osgDB::Registry::instance();

	osg::Camera * pCamera = m_pView->getCamera();
	pCamera->setNearFar(1.0f,20000);
	float aspect = (float)g_ScreenWidth/(float)g_ScreenHeight;
	pCamera->setPerspective(90.0f,aspect,1.0f,20000.0f);

	osg::Vec3  posVec(0, 0, 1000);
	osg::Vec3  lookVec(15, 15, 1000);
	osg::Vec3  upVec(0, 0, 1);

	pCamera->setLookAt(posVec, lookVec, upVec);

	return 1;
}


int VirtualBattlefield::drawScene()
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::drawScene()...");
    
	ObjectList::iterator i;
	for (i = objectList.begin(); i != objectList.end(); i++) {
		(*i)->updateScene();
	}

	osgUtil::CullVisitor * pCullVisitor = m_pView->getCullVisitor();
	pCullVisitor->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	pCullVisitor->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
	m_pView->setCullVisitor(pCullVisitor);
  
	m_pView->app();
	m_pView->cull();
	m_pView->draw();

	glFinish();

	Demeter::Settings::GetInstance()->LogStats();

	return 1;
}

void VirtualBattlefield::onUpdate(float dt)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::onUpdate - entering" );

	//  std::for_each( objectList.begin(), objectList.end(), &SimObject::onUpdate  );

	ObjectList::iterator i;

	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		(*i)->onUpdate(dt);
	}

	m_rpFrameStamp->setReferenceTime(m_rpFrameStamp->getReferenceTime() + dt);
	m_rpFrameStamp->setFrameNumber(m_rpFrameStamp->getFrameNumber() + 1);

	CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::onUpdate - leaving" );
}

void VirtualBattlefield::setCameraNode( osg::Node * pNode)
{

}

void VirtualBattlefield::setLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::setLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);

	osg::Camera * pCamera = m_pView->getCamera();

	int XLatticePos = (int) (eyePos.x / g_LatticeXDist);
	int YLatticePos = (int) (eyePos.y / g_LatticeYDist);

	float localEyePosition_x = eyePos.x - g_LatticeXDist*XLatticePos;
	float localEyePosition_y = eyePos.y - g_LatticeYDist*YLatticePos;

	float localLookPosition_x = lookPos.x - g_LatticeXDist*XLatticePos;
	float localLookPosition_y = lookPos.y - g_LatticeYDist*YLatticePos;

	osg::Vec3 _localEye( localEyePosition_x,  localEyePosition_y,  eyePos.z) ;
	osg::Vec3 _localLook(localLookPosition_x, localLookPosition_y, lookPos.z ) ;
	osg::Vec3 _up (upVec.x, upVec.y, upVec.z );

	pCamera->setLookAt(_localEye, _localLook, _up);
	pCamera->ensureOrthogonalUpVector();

	if (!m_ActiveTerrainObject.isNull())
		m_ActiveTerrainObject->setCameraPosition( eyePos.x, eyePos.y, eyePos.z );


	CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::setLookAt - eye: " << pCamera->getEyePoint()  << 
	  ", look: " << pCamera->getCenterPoint()  << ", up: " << pCamera->getUpVector()  <<
	  ", near: " << pCamera->zNear() << ", far: " << pCamera->zFar() );

}

void VirtualBattlefield::getLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec) const
{
	assert(m_pView);
	osg::Camera * pCamera = m_pView->getCamera();

	osg::Vec3 _eye = pCamera->getEyePoint();
	eyePos = simdata::Vector3(_eye.x(), _eye.y(),_eye.z());
	osg::Vec3 _center = pCamera->getCenterPoint();
	lookPos = simdata::Vector3(_center.x(), _center.y(),_center.z());
	osg::Vec3 _up = pCamera->getUpVector();
	upVec = simdata::Vector3(_up.x(), _up.y(),_up.z());

	CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::getLookAt - eye: " << eyePos << ", look: " << lookPos << ", up: " << upVec);
}

void VirtualBattlefield::addObject(Pointer<DynamicObject> obj)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectManager::addObject, object_id: " << obj->getObjectID() );
	assert(obj.valid());
	objectList.push_front(obj);
	ostringstream strg;
	strg << "Adding object " << obj->getObjectID() ;
	objectListHistory.push_back(strg.str());
}

void VirtualBattlefield::addController(BaseController * controller)
{

}

void VirtualBattlefield::addNodeToScene( osg::Node * pNode)
{
	CSP_LOG(CSP_APP, CSP_INFO, "VirtualBattlefield::addNodeToScene() - NodeName: " << pNode->getName() );
	//  osgUtil::Optimizer optimzer;
	//  optimzer.optimize(pNode, osgUtil::Optimizer::OptimizationOptions::ALL_OPTIMIZATIONS);

	m_rpObjectRootNode->addChild(pNode);
}

void VirtualBattlefield::removeNodeFromScene( osg::Node * pNode)
{
	m_rpRootNode->removeChild(pNode);
}

void VirtualBattlefield::setWireframeMode(bool flag)
{
	osg::StateSet* globalStateSet = m_pView->getGlobalStateSet();
	if (!globalStateSet) {
		globalStateSet = osgNew osg::StateSet;
		m_pView->setGlobalStateSet(globalStateSet);
	}

	osg::PolygonMode* polyModeObj = osgNew osg::PolygonMode;

	if (flag) {
		polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	} else {
		polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
	}
	globalStateSet->setAttribute(polyModeObj);
}

void VirtualBattlefield::setFogMode(bool flag)
{
	// osg::StateSet * pStateSet = m_rpRootNode->getStateSet();
	osg::StateSet * pStateSet = m_rpObjectRootNode->getStateSet();
	osg::StateAttribute * pStateAttr = pStateSet->getAttribute(osg::StateAttribute::FOG);

	if (flag) {
		pStateSet->setAttributeAndModes(pStateAttr ,osg::StateAttribute::ON);
	} else {
		pStateSet->setAttributeAndModes(pStateAttr ,osg::StateAttribute::OFF);
	}

	// m_rpRootNode->setStateSet(pStateSet);
	m_rpObjectRootNode->setStateSet(pStateSet);
}

void VirtualBattlefield::setFogStart(float value)
{
	// osg::StateSet * pStateSet = m_rpRootNode->getStateSet();
	osg::StateSet * pStateSet = m_rpObjectRootNode->getStateSet();
	osg::Fog * pFogAttr = (osg::Fog*)pStateSet->getAttribute(osg::StateAttribute::FOG);

	pFogAttr->setStart(value);

	pStateSet->setAttributeAndModes(pFogAttr ,osg::StateAttribute::ON);    
	// m_rpRootNode->setStateSet(pStateSet);
	m_rpObjectRootNode->setStateSet(pStateSet);
}

void VirtualBattlefield::setFogEnd(float value)
{
	//    osg::StateSet * pStateSet = m_rpRootNode->getStateSet();
	osg::StateSet * pStateSet = m_rpObjectRootNode->getStateSet();
	osg::Fog * pFogAttr = (osg::Fog*)pStateSet->getAttribute(osg::StateAttribute::FOG);

	pFogAttr->setEnd(value);

	pStateSet->setAttributeAndModes(pFogAttr ,osg::StateAttribute::ON);    
	m_rpObjectRootNode->setStateSet(pStateSet);
}

void VirtualBattlefield::setViewDistance(float value)
{
	osg::Camera * pCamera = m_pView->getCamera();
	m_ViewDistance = value;
	pCamera->setNearFar(1.0f, m_ViewDistance);
	pCamera->setPerspective(m_ViewAngle, 1.0, 1.0f, m_ViewDistance);
}

void VirtualBattlefield::setViewAngle(float value)
{
	osg::Camera * pCamera = m_pView->getCamera();
	m_ViewAngle = value;
	pCamera->setPerspective(m_ViewAngle, 1.0, 1.0f, m_ViewDistance);
}

// look for the object given an id, return null if object
// is not found, in the future to speed this up a map could
// be used.
Pointer<DynamicObject> VirtualBattlefield::getObjectFromID( unsigned int ID)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "Getting ObjectFromID, ID: " << ID );
	ObjectList::iterator i;
	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		if (ID == (*i)->getObjectID()) {
			return *i;
		}
	}
	return Pointer<DynamicObject>();
}

// look for an object given a name. return null if object is not found.
// this needs to be improved with a map.
Pointer<DynamicObject> VirtualBattlefield::getObjectFromName( string name )
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "Getting ObjectFromName, name: " << name );
	ObjectList::iterator i;
	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		if (name == (*i)->getObjectName()) {
			return *i;
		}
	}
	return Pointer<DynamicObject>();
}

// removing object with ID. 
// change the deleteFlag to true.
void VirtualBattlefield::removeObjectWithID( unsigned int ID)
{
	CSP_LOG( CSP_APP, CSP_INFO, "removeObjectWithID - ID: " << ID );
	ObjectList::iterator i;
	for (i = objectList.begin(); i != objectList.end(); ++i) {
		if (ID == (*i)->getObjectID()) {
			(*i)->setDeleteFlag(true);
		}
	} 
}

void VirtualBattlefield::removeObjectsMarkedForDelete()
{
	ObjectList::iterator i;
	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		if ((*i)->getDeleteFlag()) {
			CSP_LOG(CSP_APP, CSP_INFO, "removeObjectsMarkedForDelete() - Removing object " << (*i)->getObjectID());
			ostringstream strg;
			strg << "Removing object " << (*i)->getObjectID();
			objectListHistory.push_back(strg.str());
			removeNodeFromScene((*i)->getNode());
			i = objectList.erase(i);
		}
	}
}

void VirtualBattlefield::removeAllObjects()
{
	CSP_LOG(CSP_APP, CSP_INFO, "RemoveAllObjects()" );

	while (!objectList.empty()) {
		Pointer<DynamicObject> object = objectList.front();
		objectList.pop_front();

		ostringstream strg;
		strg << "Removing object " << object->getObjectID();
		objectListHistory.push_back(strg.str());

		removeNodeFromScene( object->getNode() );
	}
}

void VirtualBattlefield::dumpAllObjects()
{
	cout << "dumpAllObjects() - Dumping List of Game Objects" << endl;
	ObjectList::iterator i;
	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		Pointer<DynamicObject> object = *i;
		object->dump();
	}
	cout << "dumpAllObjects() - Finished Dumping List of Game Objects" << endl;
}

// find the controller for this ID. return a null if the
// object is not found.
BaseController * VirtualBattlefield::getControllerFromID(unsigned int ID)
{
	list<BaseController *>::iterator i;
	for (i = controllerList.begin() ; i != controllerList.end() ; ++i) {
		BaseController * controller = *i;
		if (ID == controller->controller_id) {
			return controller;
		}
	}
	return NULL;
}

void VirtualBattlefield::dumpObjectHistory()
{
	CSP_LOG(CSP_APP, CSP_INFO, "DumpingObject history" );
	list<string>::iterator i;
	for (i = objectListHistory.begin(); i != objectListHistory.end(); ++i) {
		CSP_LOG(CSP_APP, CSP_INFO, *i);
	}
	CSP_LOG(CSP_APP, CSP_INFO, "Finished dumping object history" );
}

// process the onUpdate of all game objects.
void VirtualBattlefield::updateAllObjects(float dt) // before: unsigned int dt ?!!
{
	cout << "updateAllObjects - Entering" << endl;

	ObjectList::iterator i;

	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		Pointer<DynamicObject> object = *i;
	  	object->onUpdate(dt);
	}
	cout << "updateAllObjects - Leaving" << endl;
}

void VirtualBattlefield::initializeAllObjects()
{
	cout << "ObjectManager::initializeAllObjects" << endl;
	ObjectList::iterator i;
	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		Pointer<DynamicObject> object = *i;
	  	object->initialize();
	}
}

simdata::Pointer<DynamicObject> VirtualBattlefield::getNextObject(simdata::Pointer<DynamicObject> object, int human, int local, int category)
{
	ObjectList::iterator i;
	i = std::find(objectList.begin(), objectList.end(), object);
	if (i == objectList.end()) {
		return object;
	}
	do {
		if (++i == objectList.end()) i = objectList.begin();
		if ((human >= 0) && (*i)->isHuman() != (human != 0)) continue;
		if ((local >= 0) && (*i)->isLocal() != (local != 0)) continue;
		break;
	} while (object.ptr() != (*i).ptr());
	return *i;
}


/*
// calculates a list of objects within range from a centralObject.
void VirtualBattlefield::getObjectsInRange(SimObject * fromObj, float range, float view_angle  , int army, list<ObjectRangeInfo*> & rangeList  )
{
	ObjectList::iterator i;

	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		SimObject *toObj = (SimObject*)*i;
		if (fromObj->getObjectID() != toObj->getObjectID() && toObj->getArmy() == army) {
			simdata::Vector3 distVect = toObj->getGlobalPosition() - fromObj->getGlobalPosition();
			float dist = distVect.Length();
			simdata::Vector3 distVectNormalized = Normalized(distVect);
			float angle = simdata::angleBetweenTwoVectors(fromObj->getDirection(), distVectNormalized);
			angle = angle*360.0/3.1415/2.0;

			//	  cout << "Distance between objects " << fromTankObj->object_id << " and "
			//	       << toTankObj->object_id << " is " << dist << ", angle is " << angle*360.0/2.0/3.1415 << endl;
			if (dist < range && fabs(angle) < view_angle/360.0*2.0*3.1415) {
				//   cout << "Object " << toTankObj->object_id << " is in sight from object " << fromTankObj->object_id << endl;
				// todo use a better memory scheme
				ObjectRangeInfo * objectRangeInfo = new ObjectRangeInfo;
				objectRangeInfo->setObjectID( toObj->getObjectID() );
				objectRangeInfo->setRange( dist );
				objectRangeInfo->setDirection( distVectNormalized);
				rangeList.push_front(objectRangeInfo);
			}
		}
	}
}


bool VirtualBattlefield::doSphereTest( SimObject * pObj1, SimObject * pObj2)
{
	// return true if the distance between the two objects is less then then 
	// or equal there bounding sphere radius squared sum.

	// Calculate the radius squared sum.
	float r2 = pObj1->getBoundingSphereRadius() * pObj1->getBoundingSphereRadius()
	           + pObj2->getBoundingSphereRadius() * pObj2->getBoundingSphereRadius();

	// Calculate the diff vector between the two
	simdata::Vector3 pDiff = pObj1->getGlobalPosition() - pObj2->getGlobalPosition();
	float p2 = Dot( pDiff, pDiff);

	if ( (p2 - r2) <= 0) {
		return true;
	}

	// Do other tests.
	return false;
}

void VirtualBattlefield::getObjectDistance(SimObject * fromObject, SimObject * toObject, float & distance, simdata::Vector3 & direction)
{
	simdata::Vector3 distVect = fromObject->getGlobalPosition() - toObject->getGlobalPosition();
	distance = distVect.Length();
	direction = Normalized(distVect);
}
*/

void VirtualBattlefield::drawCockpit()
{

}

int VirtualBattlefield::getTerrainPolygonsRendered()
{
	if (!m_ActiveTerrainObject.isNull()) {
		return m_ActiveTerrainObject->getTerrainPolygonsRendered();
	} else {
		return 0;
	}
}

void VirtualBattlefield::setActiveTerrain(TerrainObject *pTerrainObject)
{
	m_ActiveTerrainObject = pTerrainObject; 

	float xPatch = 35000 + 448000;
	float yPatch = 6000 + 500000;

	if (!m_ActiveTerrainObject.isNull()) {
		 m_ActiveTerrainObject->setCameraPosition( xPatch, yPatch, 1000);
	}
	
	/*
	for (unsigned short i=0; i < 15; ++i)
	{
		osg::Node * pTrees = makeTreesPatch( xPatch, yPatch + 256 * i, 100, 1000,
			100, this);
		pTrees->setName("Trees");
		addNodeToScene(pTrees);
	}
	*/

}


