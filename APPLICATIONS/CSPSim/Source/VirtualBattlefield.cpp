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

#define NOSHADOW


#include <sstream>


#include "LogStream.h"

#include "VirtualBattlefield.h"
#include "VirtualScene.h"
#include "TerrainObject.h"
#include "DynamicObject.h"

#include <SimData/Types.h>
#include <SimData/Math.h>


extern int g_ScreenWidth;
extern int g_ScreenHeight;

extern double g_LatticeXDist;
extern double g_LatticeYDist;


unsigned int VirtualBattlefield::latest_object_id = 0;

using simdata::Pointer;



VirtualBattlefield::VirtualBattlefield()
{
}

VirtualBattlefield::~VirtualBattlefield()
{

}

int VirtualBattlefield::create()
{
	return 1;
}

void VirtualBattlefield::cleanup()
{
}

void VirtualBattlefield::setScene(simdata::Pointer<VirtualScene> scene) 
{
	m_Scene = scene;
}


float VirtualBattlefield::getElevation( float x,float y ) const
{
	if (!m_Terrain) {
		return 0.0;
	} else {
		return m_Terrain->getElevation(x,y);
	}
}

void VirtualBattlefield::getNormal(float x, float y, float & normalX, 
                                   float & normalY, float & normalZ ) const
{
	normalX = 0; normalY = 0; normalZ = 1;
	if (!m_Terrain.isNull()) {
		m_Terrain->getNormal(x,y, normalX, normalY, normalZ);
	}
}




#if 0   
// NOW MOVED TO VirtualBattleFieldScene
int VirtualBattlefield::drawScene()
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::drawScene()...");
	
	ObjectList::iterator i = objectList.begin();
	ObjectList::const_iterator iEnd = objectList.end();
	for (; i != iEnd; ++i) {
		(*i)->updateScene();
	}
    
	osgUtil::CullVisitor * pCullVisitor = m_pView->getCullVisitor();
	//pCullVisitor->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	//pCullVisitor->setCullingMode(osgUtil::CullVisitor::ENABLE_ALL_CULLING);
	m_pView->setCullVisitor(pCullVisitor);
  
	m_pView->update();
	// segfaults... why?
	//assert(pCullVisitor);
	//m_TerrainNode->accept(*pCullVisitor);
	m_pView->cull();
	m_pView->draw();

/*
osg::Matrix _matrix = getCM(CM_intensity);
glMatrixMode( GL_COLOR );
glLoadMatrixf( _matrix.ptr() );
glMatrixMode( GL_MODELVIEW );
*/

	glFinish();

	Demeter::Settings::GetInstance()->LogStats();

	return 1;
}
#endif


void VirtualBattlefield::onUpdate(float dt)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::onUpdate - entering" );

	//  std::for_each( objectList.begin(), objectList.end(), &SimObject::onUpdate  );

	ObjectList::iterator i;

	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		(*i)->onUpdate(dt);
	}
	CSP_LOG(CSP_APP, CSP_DEBUG, "VirtualBattlefield::onUpdate - leaving" );
}



void VirtualBattlefield::addObject(Pointer<DynamicObject> obj)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectManager::addObject, object_id: " << obj->getObjectID() );
	assert(obj.valid());
	objectList.push_front(obj);
	std::ostringstream strg;
	strg << "Adding object " << obj->getObjectID() ;
	objectListHistory.push_back(strg.str());
	if (m_Scene.valid()) {
		m_Scene->addObject(obj);
	}
}

void VirtualBattlefield::addController(BaseController * controller)
{

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
	ObjectList::iterator i = objectList.begin();
	ObjectList::const_iterator iEnd = objectList.end();
	for (;i != iEnd; ++i) {
		if (ID == (*i)->getObjectID()) {
			(*i)->setDeleteFlag(true);
		}
	} 
}

void VirtualBattlefield::removeObjectsMarkedForDelete()
{
	ObjectList::iterator i = objectList.begin();
	ObjectList::const_iterator iEnd = objectList.end();
	for (; i != iEnd ; ++i) {
		if ((*i)->getDeleteFlag()) {
			CSP_LOG(CSP_APP, CSP_INFO, "removeObjectsMarkedForDelete() - Removing object " << (*i)->getObjectID());
			std::ostringstream strg;
			strg << "Removing object " << (*i)->getObjectID();
			objectListHistory.push_back(strg.str());
			if (m_Scene.valid()) {
				m_Scene->removeObject(*i);
			}
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

		std::ostringstream strg;
		strg << "Removing object " << object->getObjectID();
		objectListHistory.push_back(strg.str());

		if (m_Scene.valid()) {
			m_Scene->removeObject(object);
		}
	}
}

void VirtualBattlefield::dumpAllObjects()
{
	std::cout << "dumpAllObjects() - Dumping List of Game Objects" << std::endl;
	ObjectList::const_iterator i = objectList.begin();
	ObjectList::const_iterator iEnd = objectList.end();
	for (; i != iEnd ; ++i) {
		Pointer<DynamicObject> object = *i;
		object->dump();
	}
	std::cout << "dumpAllObjects() - Finished Dumping List of Game Objects" << std::endl;
}

// find the controller for this ID. return a null if the
// object is not found.
BaseController * VirtualBattlefield::getControllerFromID(unsigned int ID)
{
	std::list<BaseController *>::iterator i;
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
	std::list<string>::iterator i;
	for (i = objectListHistory.begin(); i != objectListHistory.end(); ++i) {
		CSP_LOG(CSP_APP, CSP_INFO, *i);
	}
	CSP_LOG(CSP_APP, CSP_INFO, "Finished dumping object history" );
}

// process the onUpdate of all game objects.
void VirtualBattlefield::updateAllObjects(float dt) // before: unsigned int dt ?!!
{
	std::cout << "updateAllObjects - Entering" << std::endl;

	ObjectList::iterator i = objectList.begin();
	ObjectList::const_iterator iEnd = objectList.end();

	for (; i != iEnd ; ++i) {
		Pointer<DynamicObject> object = *i;
	  	object->onUpdate(dt);
	}
	std::cout << "updateAllObjects - Leaving" << std::endl;
}

void VirtualBattlefield::initializeAllObjects()
{
	std::cout << "ObjectManager::initializeAllObjects" << std::endl;
	ObjectList::iterator i;
	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		Pointer<DynamicObject> object = *i;
	  	object->initialize();
	}
}

simdata::Pointer<DynamicObject> VirtualBattlefield::getNextObject(simdata::Pointer<DynamicObject> object, int human, int local, int category)
{
	ObjectList::iterator i = std::find(objectList.begin(), objectList.end(), object);
	if (i == objectList.end()) {
		return object;
	}
	do {
		if (++i == objectList.end()) i = objectList.begin();
		if ((human >= 0) && (*i)->isHuman() != (human != 0)) continue;
		if ((local >= 0) && (*i)->isLocal() != (local != 0)) continue;
		break;
	} while (object.get() != (*i).get());
	return *i;
}


/*
// calculates a list of objects within range from a centralObject.
void VirtualBattlefield::getObjectsInRange(SimObject * fromObj, float range, float view_angle  , int army, std::list<ObjectRangeInfo*> & rangeList  )
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

			//	  std::cout << "Distance between objects " << fromTankObj->object_id << " and "
			//	       << toTankObj->object_id << " is " << dist << ", angle is " << angle*360.0/2.0/3.1415 << std::endl;
			if (dist < range && fabs(angle) < view_angle/360.0*2.0*3.1415) {
				//   std::cout << "Object " << toTankObj->object_id << " is in sight from object " << fromTankObj->object_id << std::endl;
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



void VirtualBattlefield::setTerrain(simdata::Pointer<TerrainObject> terrain) {
	m_Terrain = terrain;
}
