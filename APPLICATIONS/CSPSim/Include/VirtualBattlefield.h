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
 * @file VirtualBattlefield.h
 *
 **/

#ifndef __VIRTUALBATTLEFIELD_H__
#define __VIRTUALBATTLEFIELD_H__

#include <list>
#include <string>

#include <osg/FrameStamp>
#include <osg/Group>

#include <osgUtil/SceneView>

#include "BaseController.h"
#include "DynamicObject.h"
#include "ObjectRangeInfo.h"
#include "TerrainObject.h"

#include <SimData/Types.h>


const int MAX_NUM_VISIBLE_TRIANGLES = 30000; // Chosen based on the expected number of triangles that will be visible on-screen at any one time (the terrain mesh will typically have far more triangles than are seen at one time, especially with dynamic tessellation)


/**
 * class VirtualBattlefield
 *
 * @author unknown
 */
class VirtualBattlefield
{
public:
	typedef std::list< simdata::Pointer<DynamicObject> > ObjectList;

	VirtualBattlefield();
	virtual ~VirtualBattlefield();

	int create();
	void Cleanup();

	int buildScene();
	int drawScene();

	void addObject(simdata::Pointer<DynamicObject> object);
	void onUpdate(float dt);

	simdata::Pointer<DynamicObject> getObjectFromID( unsigned int ID);
	simdata::Pointer<DynamicObject> getObjectFromName( std::string name );
	BaseController * getControllerFromID(unsigned int ID);
	void removeObjectWithID( unsigned int ID);
	void removeObjectsMarkedForDelete();
	void removeAllObjects();
	void dumpAllObjects();
	void dumpObjectHistory();
	void updateAllObjects(float dt);
	void initializeAllObjects();
	simdata::Pointer<DynamicObject> getNextObject(simdata::Pointer<DynamicObject> object, int human, int local, int category);

	//void getObjectsInRange(SimObject * fromObject, float range, float view_angle, int army, std::list<ObjectRangeInfo*> & rangeList  );
	void addController(BaseController * controller);
	unsigned int getNewObjectID() { return ++latest_object_id; }
	ObjectList getObjectList() { return objectList; }
	
	//bool doSphereTest(SimObject * pObj1, SimObject * pObj2);
	//void getObjectDistance(SimObject * fromObject, SimObject * toObject, float & distance, simdata::Vector3 & direction);

	void addNodeToScene( osg::Node * pNode);
	void removeNodeFromScene( osg::Node * pNode);


	void setLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec);
	void getLookAt(simdata::Vector3 & eyePos, simdata::Vector3 & lookPos, simdata::Vector3 & upVec) const;
	void setWireframeMode(bool flag);
	void setFogMode(bool flag);
	void setFogStart(float value);
	void setFogEnd(float value);

	float getViewDistance() const { return m_ViewDistance; }
	void setViewDistance(float value);


	float getElevation(float x,float y) const;
	void getNormal(float x, float y, float & normalX, float & normalY, float & normalZ) const;

	void drawCockpit();

	void setCameraNode(osg::Node * pNode);
	void setActiveTerrain(TerrainObject *pTerrainObject);

	int getTerrainPolygonsRendered();

	float getViewAngle() const { return m_ViewAngle; }
	void setViewAngle(float);

public:



protected:

	osgUtil::SceneView* m_pView;
	osg::ref_ptr<osg::Group> m_rpRootNode;
	osg::ref_ptr<osg::Group> m_rpObjectRootNode;
	osg::ref_ptr<osg::FrameStamp> m_rpFrameStamp;

	osg::Node * m_pTerrainNode;

	ObjectList objectList;
	static unsigned int latest_object_id;
	std::list<string> objectListHistory;
	list<BaseController *> controllerList;

	float m_ViewDistance;
	float m_ViewAngle;

	simdata::Pointer<TerrainObject> m_ActiveTerrainObject;
};

#endif // __VIRTUALBATTLEFIELD_H__


