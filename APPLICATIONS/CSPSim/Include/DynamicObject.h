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
 * @file DynamicObject.h
 *
 **/

#ifndef __DYNAMICOBJECT_H__
#define __DYNAMICOBJECT_H__


#include "SimObject.h"
#include "InputInterface.h"
#include "BaseController.h"
#include "SmokeEffects.h"
#include "TerrainObject.h"


namespace osgParticle {
	class Geode;
	class ParticleSystemUpdater;
	class ModularEmitter;
}


class VirtualBattlefield;


/**
 * class DynamicObject - Base class for all mobile objects in the simulation.
 *
 */
class DynamicObject: public SimObject, public InputInterface
{
	friend class VirtualBattlefield;

private:
	enum { 
	       // bits 8-15 reserved for DynamicObject
	       F_LOCAL    = 0x00000100,  // controlled internally
	       F_HUMAN    = 0x00000200,  // controlled by a human
	       F_AIR      = 0x00000400,  // air object (airplane or helo)
	       F_GROUNDED = 0x00000800,  // currently on the ground
	     };

	// managed by the battlefield
	void setHuman(bool flag) { setFlags(F_HUMAN, flag); }
	void setLocal(bool flag) { setFlags(F_LOCAL, flag); }

protected:
	void setAir(bool flag) { setFlags(F_AIR, flag); }
	void setGrounded(bool flag) { setFlags(F_GROUNDED, flag); }

public:
	EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(DynamicObject, SimObject)
		SIMDATA_XML("model", DynamicObject::m_Model, true)
		SIMDATA_XML("mass", DynamicObject::m_Mass, true)
		SIMDATA_XML("inertia", DynamicObject::m_Inertia, false)
	END_SIMDATA_XML_INTERFACE

	DynamicObject();
	virtual ~DynamicObject();

	// model and scene related functions
	simdata::Ref<SceneModel> getSceneModel() { return m_SceneModel; }
	simdata::Ref<ObjectModel> getModel() const { return m_Model; }
	virtual void createSceneModel();
	virtual void destroySceneModel();
	osg::Node* getOrCreateModelNode();
	osg::Node* getModelNode();
	virtual void showModel() { if (m_SceneModel.valid()) m_SceneModel->show(); }
	virtual void hideModel() { if (m_SceneModel.valid()) m_SceneModel->hide(); }

	virtual void getStats(std::vector<std::string> &stats) const {};

	void setController(BaseController * Controller) { 
		m_Controller = Controller;
	}

	void setVelocity(simdata::Vector3 const & velocity);
	void setVelocity(double Vx, double Vy, double Vz);
	simdata::Vector3 const & getVelocity() const { return m_LinearVelocity; }
	double getSpeed() const { return m_Speed; }

	virtual simdata::Vector3 getViewPoint() const;

	bool isHuman() const { return getFlags(F_HUMAN) != 0; }
	bool isLocal() const { return getFlags(F_LOCAL) != 0; }
	bool isAir() const { return getFlags(F_AIR) != 0; }
	bool isGrounded() const { return getFlags(F_GROUNDED) != 0; }

	void updateGlobalPosition();
	void updateGroundPosition();
	void updateOrientation();

	virtual void aggregate() { std::cout << "aggregate " << int(this) << std::endl; }
	virtual void deaggregate() { std::cout << "deaggregate " << int(this) << std::endl; }
	virtual void setVisible(bool visible) { std::cout << int(this) << ": visible = " << visible << std::endl; }
	
	virtual unsigned int onRender();
	virtual double onUpdate(double dt);

	bool isNearGround();

	// The object name holds an identifier string for in-game display.  It is not 
	// intended for unique object identification. (e.g. "Callsign", Cowboy11, T-62)
	void setObjectName(const std::string name) { m_ObjectName = name; }
	std::string getObjectName() const { return m_ObjectName; }

	simdata::Vector3 getDirection() const;
	simdata::Vector3 getUpDirection() const;
	simdata::Quaternion & getAttitude() { return m_Attitude; }
	void setAttitude(simdata::Quaternion const & attitude);

	virtual simdata::Vector3 getGlobalPosition() const { return m_GlobalPosition; }
	virtual void setGlobalPosition(simdata::Vector3 const & position);
	virtual void setGlobalPosition(double x, double y, double z);

	virtual void updateScene(simdata::Vector3 const &origin) { 
		if (m_SceneModel.valid()) m_SceneModel->setPositionAttitude(m_GlobalPosition - origin, m_Attitude); 
	}
	
	bool isSmoke();
	void disableSmoke();
	void enableSmoke();

protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);

	virtual void doMovement(double dt);
	virtual void postMotion(double dt);

	BaseController * m_Controller;

	// dynamic properties
	
	simdata::Vector3 m_GlobalPosition;
	simdata::Vector3 m_PrevPosition;

	double m_Mass;
	double m_Speed;
	
	TerrainObject::IntersectionHint m_GroundHint;
	double m_GroundZ;
	simdata::Vector3 m_GroundN;
	bool m_NearGround;

	simdata::Matrix3 m_Inertia;
	simdata::Matrix3 m_InertiaInv;

	simdata::Vector3 m_AngularVelocity;
	simdata::Vector3 m_LinearVelocity;

	simdata::Quaternion m_Attitude;

	std::string m_ObjectName;

	simdata::Link<ObjectModel> m_Model;
	simdata::Ref<SceneModel> m_SceneModel;

};


#endif // __DYNAMICOBJECT_H__

