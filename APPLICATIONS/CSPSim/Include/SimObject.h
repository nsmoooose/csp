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
private:
	enum { 
	       // bits 0-7 reserved for SimObject
	       F_FREEZE    = 0x00000001,
	     };

protected:

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
	virtual void initialize() = 0;
	virtual double onUpdate(double dt) = 0;
	virtual unsigned int onRender() = 0;

	/*
	unsigned int getObjectID() const { return m_ObjectID; }
	unsigned int getObjectType() const { return m_ObjectType; }
	*/
	
	void setFreezeFlag(bool flag) { setFlags(F_FREEZE, flag); }
	bool getFreezeFlag() const { return getFlags(F_FREEZE) != 0; }

	// model and scene related functions
	SceneModel * getSceneModel() { return m_SceneModel; }
	simdata::Ref<ObjectModel> getModel() const { return m_Model; }
	virtual void createSceneModel();
	virtual void destroySceneModel();
	osg::Node* getOrCreateModelNode();
	osg::Node* getModelNode();
	virtual void showModel() { if (m_SceneModel) m_SceneModel->show(); }
	virtual void hideModel() { if (m_SceneModel) m_SceneModel->hide(); }

	virtual void updateScene(simdata::Vector3 const &origin) = 0;

	// position accessor methods
	virtual simdata::Vector3 const & getGlobalPosition() const = 0;
	
protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);

	simdata::Link<ObjectModel> m_Model;
	SceneModel *m_SceneModel;

	//unsigned int m_ObjectID;
	//unsigned int m_ObjectType;

	unsigned int m_Flags;
};


#endif // __SIMOBJECT_H__


