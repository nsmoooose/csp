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

#include <SimData/Ref.h>
#include <SimData/Path.h>

#include "TerrainObject.h"
#include "VirtualScene.h"
#include "DynamicObject.h"

class BaseController;


/**
 * class VirtualBattlefield
 *
 * @author unknown
 */
class VirtualBattlefield: public simdata::Referenced
{
public:
	typedef std::list< simdata::Ref<DynamicObject> > ObjectList;

	VirtualBattlefield();
	virtual ~VirtualBattlefield();

	int create();
	void cleanup();

	void addObject(simdata::Ref<DynamicObject> object);
	void onUpdate(float dt);

	simdata::Ref<DynamicObject> getObjectFromID(unsigned int ID);
	simdata::Ref<DynamicObject> getObjectFromName(std::string name);
	BaseController* getControllerFromID(unsigned int ID);
	void removeObjectWithID( unsigned int ID);
	void removeObjectsMarkedForDelete();
	void removeAllObjects();
	void dumpAllObjects();
	void dumpObjectHistory();
	void updateAllObjects(float dt);
	void initializeAllObjects();
	simdata::Ref<DynamicObject> getNextObject(simdata::Ref<DynamicObject> object, int human, int local, int category);

	void addController(BaseController * controller);
	unsigned int getNewObjectID() { return ++latest_object_id; }
	ObjectList getObjectList() { return objectList; }
	
	//bool doSphereTest(SimObject * pObj1, SimObject * pObj2);
	//void getObjectDistance(SimObject * fromObject, SimObject * toObject, float & distance, simdata::Vector3 & direction);
	//void getObjectsInRange(SimObject * fromObject, float range, float view_angle, int army, std::list<ObjectRangeInfo*> & rangeList  );

	float getElevation(float x,float y) const;
	void getNormal(float x, float y, float & normalX, float & normalY, float & normalZ) const;

	void setTerrain(simdata::Ref<TerrainObject>);
	simdata::Ref<TerrainObject> getTerrain() const { return m_Terrain; }

	void setScene(simdata::Ref<VirtualScene>);
	simdata::Ref<VirtualScene> getScene() const { return m_Scene; }

protected:

	ObjectList objectList;
	static unsigned int latest_object_id;
	std::list<std::string> objectListHistory;
	std::list<BaseController *> controllerList;

	simdata::Ref<TerrainObject> m_Terrain;
	simdata::Ref<VirtualScene> m_Scene;
};

#endif // __VIRTUALBATTLEFIELD_H__


