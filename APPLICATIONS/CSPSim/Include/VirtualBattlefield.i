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


%{
#include "VirtualBattlefield.h"
%}


class VirtualBattlefield
{
public:
	typedef std::list< simdata::Pointer<DynamicObject> > ObjectList;

	void addObject(simdata::Pointer<DynamicObject> object);
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

	void addController(BaseController * controller);
	unsigned int getNewObjectID() { return ++latest_object_id; }
	VirtualBattlefield::ObjectList getObjectList() { return objectList; }
	
	float getElevation(float x,float y) const;
	void getNormal(float x, float y, float & normalX, float & normalY, float & normalZ) const;

	void setTerrain(simdata::Pointer<TerrainObject>);
	simdata::Pointer<TerrainObject> getTerrain() const { return m_Terrain; }

	void setScene(simdata::Pointer<VirtualScene>);
	simdata::Pointer<VirtualScene> getScene() const { return m_Scene; }

};


