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

#include "DataRecorder.h"
#include "Controller.h"
#include "InputInterface.h"
#include "TerrainObject.h"

#include <SimCore/Battlefield/OldSimObject.h>
#include <SimData/Quat.h>


class NetworkMessage;
class PhysicsModel;
class SystemsModel;
class ObjectModel;
class SceneModel;


namespace osgParticle {
	class Geode;
	class ParticleSystemUpdater;
	class ModularEmitter;
};


/**
 * class DynamicObject - Base class for all mobile objects in the simulation.
 *
 */
class DynamicObject: public SimObject, public InputInterface
{
	friend class VirtualBattlefield;

	struct SystemsModelStore {
		unsigned int id;
		simdata::Ref<SystemsModel> model;
		SystemsModelStore();
		~SystemsModelStore();
		SystemsModelStore(unsigned int id_, simdata::Ref<SystemsModel> model_);
	};

	static std::list<SystemsModelStore> SystemsModelCache;

	enum { MODELCACHESIZE = 5 };

private:

	virtual void onHuman() {
		if (isLocal()) {
			selectVehicleCore();
		}
	}

	virtual void onAgent() {
		if (isLocal()) {
			cacheSystemsModel();
			selectVehicleCore();
		}
	}

	virtual void onLocal() {
		selectVehicleCore();
	}

	virtual void onRemote() {
		if (isHuman()) {
			cacheSystemsModel();
		}
		selectVehicleCore();
	}

	void cacheSystemsModel();
	simdata::Ref<SystemsModel> getCachedSystemsModel();
	void selectVehicleCore();
	void setVehicleCore(simdata::Ref<SystemsModel>);


protected:
	Bus::Ref getBus();

public:
	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(DynamicObject)
		SIMDATA_XML("model", DynamicObject::m_Model, true)
		SIMDATA_XML("mass", DynamicObject::m_ReferenceMass, true)
		SIMDATA_XML("inertia", DynamicObject::m_ReferenceInertia, false)
		SIMDATA_XML("human_systems", DynamicObject::m_HumanModel, false)
		SIMDATA_XML("agent_systems", DynamicObject::m_AgentModel, false)
	END_SIMDATA_XML_INTERFACE

	DynamicObject(TypeId type);
	virtual ~DynamicObject();

	// model and scene related functions
	simdata::Ref<SceneModel> getSceneModel();
	simdata::Ref<ObjectModel> getModel() const;
	simdata::Ref<SystemsModel> getSystemsModel() const;
	virtual void createSceneModel();
	virtual void destroySceneModel();
	osg::Node* getOrCreateModelNode();
	osg::Node* getModelNode();

	virtual void getInfo(std::vector<std::string> &info) const;

	void setController(Controller * Controller) {
		m_Controller = Controller;
	}

	void setVelocity(simdata::Vector3 const & velocity);
	void setVelocity(double Vx, double Vy, double Vz);
	simdata::Vector3 const & getVelocity() const { return b_LinearVelocity->value(); }
	double getSpeed() const { return b_LinearVelocity->value().length(); }
	virtual double getAltitude() const { return (b_GlobalPosition->value().z() - b_GroundZ->value()); }

	virtual simdata::Vector3 getViewPoint() const;

	void updateGlobalPosition();
	void updateOrientation();

	virtual void onAggregate() { CSP_LOG(APP, INFO, "aggregate @ " << int(this)); }
	virtual void onDeaggregate() { CSP_LOG(APP, INFO, "deaggregate @ " << int(this)); }

	bool isNearGround();

	// The object name holds an identifier string for in-game display.  It is not
	// intended for unique object identification. (e.g. "Callsign", Cowboy11, T-62)
	void setObjectName(const std::string name) { m_ObjectName = name; }
	std::string getObjectName() const { return m_ObjectName; }

	simdata::Vector3 getDirection() const;
	simdata::Vector3 getUpDirection() const;
	simdata::Quat & getAttitude() { return b_Attitude->value(); }
	void setAttitude(simdata::Quat const & attitude);

	virtual simdata::Vector3 getGlobalPosition() const { return b_GlobalPosition->value(); }
	virtual void setGlobalPosition(simdata::Vector3 const & position);
	virtual void setGlobalPosition(double x, double y, double z);

	virtual void updateScene(simdata::Vector3 const &origin);
	
	bool isSmoke();
	void disableSmoke();
	void enableSmoke();

	virtual void setDataRecorder(DataRecorder *recorder);
	virtual NetworkMessage* getUpdateMessage();
	virtual void putUpdateMessage(NetworkMessage* message);

protected:

	virtual void postCreate();

	virtual void registerUpdate(UpdateMaster *master);
	virtual double onUpdate(double dt);
	virtual void postUpdate(double dt);
	virtual void onRender() {}

	virtual void registerChannels(Bus::Ref bus);
	virtual void bindChannels(Bus::Ref bus);
	virtual void bindAnimations(Bus::Ref bus);

	TerrainObject::IntersectionHint m_GroundHint;

	// dynamic properties
	
	simdata::Vector3 m_PrevPosition;

	DataChannel<simdata::Vector3>::Ref b_GlobalPosition;
	DataChannel<double>::Ref b_Mass;
	DataChannel<double>::Ref b_GroundZ;
	DataChannel<simdata::Vector3>::Ref b_GroundN;
	DataChannel<bool>::Ref b_NearGround;
	DataChannel<simdata::Matrix3>::Ref b_Inertia;
	DataChannel<simdata::Matrix3>::Ref b_InertiaInv;
	DataChannel<simdata::Vector3>::Ref b_AngularVelocity;
	DataChannel<simdata::Vector3>::Ref b_LinearVelocity;
	DataChannel<simdata::Quat>::Ref b_Attitude;

	std::string m_ObjectName;

	simdata::Link<ObjectModel> m_Model;
	simdata::Ref<SceneModel> m_SceneModel;
	simdata::Ref<SystemsModel> m_SystemsModel;
	simdata::Ref<PhysicsModel> m_PhysicsModel;
	simdata::Ref<Controller> m_Controller;

	virtual void doPhysics(double dt);
	virtual void doControl(double dt);

	virtual bool onMapEvent(MapEvent const &event);

private:
	double m_ReferenceMass;
	simdata::Matrix3 m_ReferenceInertia;
	simdata::Path m_HumanModel;
	simdata::Path m_AgentModel;
};


#endif // __DYNAMICOBJECT_H__

