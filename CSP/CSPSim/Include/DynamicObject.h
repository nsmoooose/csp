// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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

#include "Bus.h"
#include "InputInterface.h"
#include "Stores/StoresDynamics.h"
#include "TerrainObject.h"

#include <SimCore/Battlefield/SimObject.h>
#include <SimData/Link.h>
#include <SimData/Matrix3.h>
#include <SimData/Quat.h>


class DataRecorder;
class DynamicModel;
class HUD;
class LocalController;
class ObjectModel;
class PhysicsModel;
class RemoteController;
class SceneModel;
class SceneModelChild;
class Station;
class StoresManagementSystem;
class SystemsModel;

namespace simdata { class Quat; }

namespace osgParticle {
	class Geode;
	class ModularEmitter;
	class ParticleSystemUpdater;
}


/**
 * class DynamicObject - Base class for all mobile objects in the simulation.
 *
 */
class DynamicObject: public SimObject, public InputInterface
{
	struct SystemsModelStore {
		ObjectId id;
		simdata::Ref<SystemsModel> model;
		SystemsModelStore();
		~SystemsModelStore();
		SystemsModelStore(ObjectId id_, simdata::Ref<SystemsModel> model_);
	};

	static std::list<SystemsModelStore> SystemsModelCache;

	enum { MODELCACHESIZE = 5 };

private:

	virtual void onHuman();
	virtual void onAgent();
	virtual void onLocal();
	virtual void onRemote();

	void cacheSystemsModel();
	simdata::Ref<SystemsModel> getCachedSystemsModel();
	void selectVehicleCore();
	void setVehicleCore(simdata::Ref<SystemsModel>);


protected:
	Bus* getBus();

public:
	SIMDATA_DECLARE_ABSTRACT_OBJECT(DynamicObject)

	DECLARE_INPUT_INTERFACE(DynamicObject, InputInterface)
		BIND_ACTION("MARKS_TOGGLE", toggleMarkers);
	END_INPUT_INTERFACE  // protected:

public:
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

	void setVelocity(simdata::Vector3 const & velocity);
	void setVelocity(double Vx, double Vy, double Vz);
	void setAngularVelocity(simdata::Vector3 const & angular_velocity);
	simdata::Vector3 const & getAngularVelocity() const { return b_AngularVelocity->value(); }
	simdata::Vector3 const & getAccelerationBody() const { return b_AccelerationBody->value(); }
	simdata::Vector3 const & getVelocity() const { return b_LinearVelocity->value(); }
	double getSpeed() const { return b_LinearVelocity->value().length(); }
	virtual double getAltitude() const { return (b_ModelPosition->value().z() - b_GroundZ->value()); }

	// Get the current offset from the model origin to the center of mass (body origin).
	simdata::Vector3 const & getCenterOfMassOffset() const { return b_CenterOfMassOffset->value(); }

	// Get the nominal offset from the model origin to the center of mass (body origin).  This
	// is the nominal value for the base configuration of the vehicle.  The actual center of
	// mass offset may vary with conditions (e.g., fuel and loadout).  See getCenterOfMassOffset().
	simdata::Vector3 const & getReferenceCenterOfMassOffset() const { return m_ReferenceCenterOfMassOffset; }

	virtual simdata::Vector3 getNominalViewPointBody() const;
	virtual void setViewPointBody(simdata::Vector3 const &point);

	virtual void onAggregate() { CSP_LOG(APP, INFO, "aggregate @ " << *this); }
	virtual void onDeaggregate() { CSP_LOG(APP, INFO, "deaggregate @ " << *this); }

	bool isNearGround();

	// The object name holds an identifier string for in-game display.  It is not
	// intended for unique object identification. (e.g. "Callsign", Cowboy11, T-62)
	void setObjectName(const std::string name) { m_ObjectName = name; }
	std::string getObjectName() const { return m_ObjectName; }

	simdata::Vector3 getDirection() const;
	simdata::Vector3 getUpDirection() const;
	simdata::Quat const & getAttitude() const { return b_Attitude->value(); }
	void setAttitude(simdata::Quat const & attitude);

	virtual simdata::Vector3 getCenterOfMassPosition() const { return b_Position->value(); }
	virtual simdata::Vector3 getGlobalPosition() const { return b_ModelPosition->value(); }
	virtual void setGlobalPosition(simdata::Vector3 const & position);
	virtual void setGlobalPosition(double x, double y, double z);

	virtual void updateScene(simdata::Vector3 const &origin);
	
	bool isSmoke();
	void disableSmoke();
	void enableSmoke();
	void internalView(bool internal);

	virtual void setDataRecorder(DataRecorder *recorder);

	// Allow the reference mass and inertia to be overridden.  This is used by StoresManagementSystem when
	// creating a objects for detached stores (for example, a half empty fuel tank).
	void setReferenceMass(double mass);
	void setReferenceInertia(simdata::Matrix3 const &inertia);
	void setReferenceCgOffset(simdata::Vector3 const &offset);

	void toggleMarkers();

protected:

	virtual void postCreate();

	virtual void registerUpdate(UpdateMaster *master);
	virtual double onUpdate(double dt);
	virtual void postUpdate(double dt);
	virtual void onRender() {}

	virtual void registerChannels(Bus* bus);
	virtual void bindChannels(Bus* bus);
	virtual void bindAnimations(Bus* bus);

	virtual simdata::Ref<simnet::NetworkMessage> getState(simcore::TimeStamp current_timestamp, simdata::SimTime interval, int detail) const;
	virtual void setState(simdata::Ref<simnet::NetworkMessage> const &msg, simcore::TimeStamp now);

	TerrainObject::IntersectionHint m_GroundHint;

	void updateDynamics(StoresManagementSystem *sms);

	// dynamic properties

	// previous cm position in global coordinates
	simdata::Vector3 m_PrevPosition;

	DataChannel<simdata::Vector3>::Ref b_ModelPosition;
	DataChannel<simdata::Vector3>::Ref b_Position;  // cm position
	DataChannel<double>::Ref b_Mass;
	DataChannel<double>::Ref b_GroundZ;
	DataChannel<simdata::Vector3>::Ref b_GroundN;
	DataChannel<bool>::Ref b_NearGround;
	DataChannel<simdata::Matrix3>::Ref b_Inertia;
	DataChannel<simdata::Matrix3>::Ref b_InertiaInv;
	DataChannel<simdata::Vector3>::Ref b_AngularVelocity;
	DataChannel<simdata::Vector3>::Ref b_AngularVelocityBody;
	DataChannel<simdata::Vector3>::Ref b_LinearVelocity;
	DataChannel<simdata::Vector3>::Ref b_AccelerationBody;
	DataChannel<simdata::Vector3>::Ref b_CenterOfMassOffset;
	DataChannel<StoresDynamics>::Ref b_StoresDynamics;
	DataChannel<simdata::Quat>::Ref b_Attitude;
	DataChannel<DynamicModel*>::Ref b_DynamicModel;
	DataChannel<HUD*>::CRef b_Hud;

	std::string m_ObjectName;

	simdata::Link<ObjectModel> m_Model;
	simdata::Ref<DynamicModel> m_DynamicModel;
	simdata::Ref<SceneModel> m_SceneModel;
	simdata::Ref<SceneModelChild> m_StationSceneModel;
	simdata::Ref<SystemsModel> m_SystemsModel;
	simdata::Ref<PhysicsModel> m_PhysicsModel;
	simdata::Ref<LocalController> m_LocalController;
	simdata::Ref<RemoteController> m_RemoteController;

	void createStationSceneModel();
	void activateStation(int index);
	void deactivateStation();
	Station const *activeStation();
	int m_ActiveStation;
	int m_PreviousStation;

	virtual void doPhysics(double dt);
	virtual void doControl(double dt);

	virtual bool onMapEvent(MapEvent const &event);

private:
	double m_ReferenceMass;
	simdata::Vector3 m_ReferenceCenterOfMassOffset;
	simdata::Matrix3 m_ReferenceInertia;
	simdata::Path m_HumanModel;
	simdata::Path m_AgentModel;
	simdata::Path m_RemoteModel;
};


#endif // __DYNAMICOBJECT_H__

