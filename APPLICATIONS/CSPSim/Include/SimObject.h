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
 * @file SimObject.h
 *
 **/

#ifndef __SIMOBJECT_H__
#define __SIMOBJECT_H__


#include "ObjectModel.h"

#include <SimData/Object.h>
#include <SimData/Vector3.h>
#include <SimData/Quaternion.h>
#include <SimData/Link.h>
#include <SimData/InterfaceRegistry.h>



/**
 * class SimObject - Base class for all objects in the simulation.
 *
 */
class SimObject: public simdata::Object
{
	enum { F_DELETE    = 0x0001,
	       F_FREEZE    = 0x0002,
	       F_GROUND    = 0x0004,
	     };

	void setFlags(int flag, bool on) {
		if (on) m_Flags |= flag; else m_Flags &= ~flag;
	}

	int getFlags(int flags) const {
		return m_Flags & flags;
	}

public:

	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(SimObject)
		SIMDATA_XML("model", SimObject::m_Model, true)
	END_SIMDATA_XML_INTERFACE

	SimObject();
	virtual ~SimObject();

	virtual void dump() = 0;

	virtual void onUpdate(double dt) = 0;

	virtual void initialize() = 0;
	virtual unsigned int onRender() = 0;

	unsigned int getObjectID() const { return m_ObjectID; }
	unsigned int getObjectType() const { return m_ObjectType; }
	
	void setFreezeFlag(bool flag) { setFlags(F_FREEZE, flag); }
	bool getFreezeFlag() const { return getFlags(F_FREEZE) != 0; }

	void setDeleteFlag(bool flag) { setFlags(F_DELETE, flag); }
	bool getDeleteFlag() const { return getFlags(F_DELETE) != 0; }

	void setGroundFlag(bool flag) { setFlags(F_GROUND, flag); }
	bool getGroundFlag() const { return getFlags(F_GROUND) != 0; }

	virtual simdata::Vector3 getViewPoint() const;

	virtual void getStats(std::vector<std::string> &stats) const {}

	// model and scene related functions
	SceneModel * getSceneModel() { return m_SceneModel; }
	simdata::Ref<ObjectModel> getModel() const { return m_Model; }
	virtual void createSceneModel();
	virtual void destroySceneModel();
	osg::Node* getOrCreateModelNode();
	osg::Node* getModelNode();
	virtual void showModel() { if (m_SceneModel) m_SceneModel->show(); }
	virtual void hideModel() { if (m_SceneModel) m_SceneModel->hide(); }
	void updateTransform() { 
		if (m_SceneModel) m_SceneModel->setPositionAttitude(m_LocalPosition, m_Attitude); 
	}

	// position accessor methods
	simdata::Vector3 const & getLocalPosition() const { return m_LocalPosition; }
	simdata::Vector3 const & getGlobalPosition() const { return m_GlobalPosition; }
	void getLatticePosition(int & x, int & y) const;

	void setGlobalPosition(simdata::Vector3 const & position);
	void setGlobalPosition(double x, double y, double z);

	simdata::Quaternion & getAttitude() { return m_Attitude; }
	void setAttitude(simdata::Quaternion const & attitude);

	// The object name holds an identifier string for in-game display.  It is not 
	// intended for unique object identification. (e.g. "Callsign", Cowboy11, T-62)
	void setObjectName(const std::string name) { m_ObjectName = name; }
	std::string getObjectName() const { return m_ObjectName; }

	simdata::Vector3 getDirection() const;
	simdata::Vector3 getUpDirection() const;
	
protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);

	simdata::Link<ObjectModel> m_Model;
	SceneModel *m_SceneModel;

	simdata::Quaternion m_Attitude;

	simdata::Vector3 m_GlobalPosition;
	simdata::Vector3 m_LocalPosition;
	int m_XLatticePos;
	int m_YLatticePos;

	unsigned int m_ObjectID;
	unsigned int m_ObjectType;

	unsigned int m_Flags;

	std::string m_ObjectName;
};


#endif // __SIMOBJECT_H__


