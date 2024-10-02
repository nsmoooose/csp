#pragma once
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

#include <csp/cspsim/Bus.h>
#include <csp/cspsim/input/InputInterface.h>
#include <csp/cspsim/TerrainObject.h>
#include <csp/cspsim/stores/StoresDynamics.h>
#include <csp/cspsim/battlefield/SimObject.h>

#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Quat.h>

namespace osgParticle {
	class Geode;
	class ModularEmitter;
	class ParticleSystemUpdater;
}

namespace csp {

class DataRecorder;
class LocalController;
class ObjectModel;
class PhysicsModel;
class Quat;
class RemoteController;
class SceneModel;
class SceneModelChild;
class Station;
class StoresManagementSystem;
class SystemsModel;

namespace hud { class HUD; }

/**
 * class DynamicObject - Base class for all mobile objects in the simulation.
 *
 */
class CSPSIM_EXPORT DynamicObject: public SimObject, public input::InputInterface
{
	struct SystemsModelStore {
		ObjectId id;
		Ref<SystemsModel> model;
		SystemsModelStore();
		~SystemsModelStore();
		SystemsModelStore(ObjectId id_, Ref<SystemsModel> model_);
	};

	static std::list<SystemsModelStore> SystemsModelCache;

	enum { MODEL_CACHE_SIZE = 5 };
	enum { NO_STATION = -1 };

private:

	virtual void onHuman();
	virtual void onAgent();
	virtual void onLocal();
	virtual void onRemote();

	void cacheSystemsModel();
	Ref<SystemsModel> getCachedSystemsModel();
	void selectVehicleCore();
	void setVehicleCore(Ref<SystemsModel>);


protected:
	Bus* getBus();

public:
	CSP_DECLARE_ABSTRACT_OBJECT(DynamicObject)

	DECLARE_INPUT_INTERFACE(DynamicObject, input::InputInterface)
		BIND_ACTION("MARKS_TOGGLE", toggleMarkers);
	END_INPUT_INTERFACE

public:
	DynamicObject(TypeId type);
	virtual ~DynamicObject();

	// model and scene related functions
	Ref<SceneModel> getSceneModel(); /** get the scene model */
	Ref<ObjectModel> getModel() const; /** get the object model */
	Ref<SystemsModel> getSystemsModel() const; /** get the systems model */
	virtual void createSceneModel(); /** create the scene model */
	virtual void destroySceneModel(); /** destroy the scene model */
	osg::Node* getOrCreateModelNode(); /** get or create the model node */
	osg::Node* getModelNode(); /** get the model node */

	virtual void getInfo(std::vector<std::string> &info) const;

	void setVelocity(Vector3 const & velocity);
	void setVelocity(double Vx, double Vy, double Vz);
	void setAngularVelocity(Vector3 const & angular_velocity);
	Vector3 const & getAngularVelocity() const { return b_AngularVelocity->value(); }
	Vector3 const & getAccelerationBody() const { return b_AccelerationBody->value(); }
	Vector3 const & getVelocity() const { return b_LinearVelocity->value(); }
	double getSpeed() const { return b_LinearVelocity->value().length(); }
	virtual double getAltitude() const { return (b_ModelPosition->value().z() - b_GroundZ->value()); }

	/** Get the current offset from the model origin to the center of mass (body origin). */
	Vector3 const & getCenterOfMassOffset() const { return b_CenterOfMassOffset->value(); }

	/** 
	 * Get the nominal offset from the model origin to the center of mass (body origin).  This
	 * is the nominal value for the base configuration of the vehicle.  The actual center of
	 * mass offset may vary with conditions (e.g., fuel and loadout).  See getCenterOfMassOffset().
	 */
	Vector3 const & getReferenceCenterOfMassOffset() const { return m_ReferenceCenterOfMassOffset; }

	virtual Vector3 getNominalViewPointBody() const;
	virtual void setViewPointBody(Vector3 const &point);

	virtual void onAggregate() { CSPLOG(INFO, APP) << "aggregate @ " << *this; }
	virtual void onDeaggregate() { CSPLOG(INFO, APP) << "deaggregate @ " << *this; }

	bool isNearGround();

	/**
	 * The object name holds an identifier string for in-game display.  It is not
	 * intended for unique object identification. (e.g. "Callsign", Cowboy11, T-62)
	 */
	void setObjectName(const std::string name) { m_ObjectName = name; }
	std::string getObjectName() const { return m_ObjectName; }

	Vector3 getDirection() const;
	Vector3 getUpDirection() const;
	Quat const & getAttitude() const { return b_Attitude->value(); }
	void setAttitude(Quat const & attitude);

	virtual Vector3 getCenterOfMassPosition() const { return b_Position->value(); }
	virtual Vector3 getGlobalPosition() const { return b_ModelPosition->value(); }
	virtual void setGlobalPosition(Vector3 const & position);
	virtual void setGlobalPosition(double x, double y, double z);

	virtual void updateScene(Vector3 const &origin);
	
	bool isSmoke();
	void disableSmoke();
	void enableSmoke();
	void internalView(bool internal);

	virtual void setDataRecorder(DataRecorder *recorder);

	/**
	 * Allow the reference mass and inertia to be overridden.  This is used by StoresManagementSystem when
	 * creating a objects for detached stores (for example, a half empty fuel tank).
	 */
	void setReferenceMass(double mass);
	void setReferenceInertia(Matrix3 const &inertia);
	void setReferenceCgOffset(Vector3 const &offset);

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

	virtual Ref<NetworkMessage> getState(TimeStamp current_timestamp, SimTime interval, int detail) const;
	virtual void setState(Ref<NetworkMessage> const &msg, TimeStamp now);

	TerrainObject::IntersectionHint m_GroundHint;

	void updateDynamics(StoresManagementSystem *sms);

	// dynamic properties

	/** previous cm position in global coordinates */
	Vector3 m_PrevPosition;

	DataChannel<Vector3>::RefT b_ModelPosition;
	DataChannel<Vector3>::RefT b_Position;  /** cm position */
	DataChannel<double>::RefT b_Mass;
	DataChannel<double>::RefT b_GroundZ;
	DataChannel<Vector3>::RefT b_GroundN;
	DataChannel<bool>::RefT b_NearGround;
	DataChannel<Matrix3>::RefT b_Inertia;
	DataChannel<Matrix3>::RefT b_InertiaInv;
	DataChannel<Vector3>::RefT b_AngularVelocity;
	DataChannel<Vector3>::RefT b_AngularVelocityBody;
	DataChannel<Vector3>::RefT b_LinearVelocity;
	DataChannel<Vector3>::RefT b_AccelerationBody;
	DataChannel<Vector3>::RefT b_CenterOfMassOffset;
	DataChannel<StoresDynamics>::RefT b_StoresDynamics;
	DataChannel<Quat>::RefT b_Attitude;
	DataChannel<hud::HUD*>::CRefT b_Hud;

	std::string m_ObjectName;

	Link<ObjectModel> m_Model;
	Ref<SceneModel> m_SceneModel;
	Ref<SceneModelChild> m_StationSceneModel;
	Ref<SystemsModel> m_SystemsModel;
	Ref<PhysicsModel> m_PhysicsModel;
	Ref<LocalController> m_LocalController;
	Ref<RemoteController> m_RemoteController;

	void createStationSceneModel();
	void activateStation(int index);
	void deactivateStation();
	Station const *activeStation();
	int m_ActiveStation;
	int m_PreviousStation;
	bool m_InternalView;

	virtual void doPhysics(double dt);
	virtual void doControl(double dt);

	virtual bool onMapEvent(input::MapEvent const &event);

private:
	double m_ReferenceMass;
	Vector3 m_ReferenceCenterOfMassOffset;
	Matrix3 m_ReferenceInertia;
	Path m_HumanModel;
	Path m_AgentModel;
	Path m_RemoteModel;
};

} // namespace csp
