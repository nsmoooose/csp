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
 * @file DynamicObject.cpp
 *
 **/

#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/Animation.h>
#include <csp/cspsim/Controller.h>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/DataRecorder.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/PhysicsModel.h>
#include <csp/cspsim/SceneModel.h>
#include <csp/cspsim/Station.h>
#include <csp/cspsim/SystemsModel.h>
#include <csp/cspsim/TerrainObject.h>
#include <csp/cspsim/hud/HUD.h>
#include <csp/cspsim/stores/StoresManagementSystem.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/data/DataManager.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/util/osg.h>
#include <csp/csplib/data/Quat.h>

#include <osg/Group>

namespace csp {

CSP_XML_BEGIN(DynamicObject)
	CSP_DEF("model", m_Model, false)
	CSP_DEF("mass", m_ReferenceMass, true)
	CSP_DEF("inertia", m_ReferenceInertia, false)
	CSP_DEF("human_systems", m_HumanModel, false)
	CSP_DEF("agent_systems", m_AgentModel, false)
	CSP_DEF("remote_systems", m_RemoteModel, false)
	CSP_DEF("reference_center_of_mass_offset", m_ReferenceCenterOfMassOffset, false)
CSP_XML_END

DEFINE_INPUT_INTERFACE(DynamicObject)

using bus::Kinetics;


DynamicObject::DynamicObject(TypeId type): SimObject(type) {
	assert(!isStatic());

	b_ModelPosition = DataChannel<Vector3>::newLocal(Kinetics::ModelPosition, Vector3::ZERO);
	b_Position = DataChannel<Vector3>::newLocal(Kinetics::Position, Vector3::ZERO);
	b_Mass = DataChannel<double>::newLocal(Kinetics::Mass, 1.0);
	b_GroundZ = DataChannel<double>::newLocal(Kinetics::GroundZ, 0.0);
	b_GroundN = DataChannel<Vector3>::newLocal(Kinetics::GroundN, Vector3::ZAXIS);
	b_NearGround = DataChannel<bool>::newLocal(Kinetics::NearGround, false);
	b_Inertia = DataChannel<Matrix3>::newLocal(Kinetics::Inertia, Matrix3::IDENTITY);
	b_InertiaInv = DataChannel<Matrix3>::newLocal(Kinetics::InertiaInverse, Matrix3::IDENTITY);
	b_AngularVelocity = DataChannel<Vector3>::newLocal(Kinetics::AngularVelocity, Vector3::ZERO);
	b_AngularVelocityBody = DataChannel<Vector3>::newLocal(Kinetics::AngularVelocityBody, Vector3::ZERO);
	b_LinearVelocity = DataChannel<Vector3>::newLocal(Kinetics::Velocity, Vector3::ZERO);
	b_AccelerationBody = DataChannel<Vector3>::newLocal(Kinetics::AccelerationBody, Vector3::ZERO);
	b_Attitude = DataChannel<Quat>::newLocal(Kinetics::Attitude, Quat::IDENTITY);
	b_CenterOfMassOffset = DataChannel<Vector3>::newLocal(Kinetics::CenterOfMassOffset, Vector3::ZERO);
	b_StoresDynamics = DataChannel<StoresDynamics>::newLocal(Kinetics::StoresDynamics, StoresDynamics());

	m_InternalView = false;
	m_GroundHint = 0;
	m_ReferenceMass = 1.0;
	m_ReferenceInertia = Matrix3::IDENTITY;
	m_ReferenceCenterOfMassOffset = Vector3::ZERO;

	setGlobalPosition(Vector3::ZERO);
	m_PrevPosition = Vector3::ZERO;

	m_ActiveStation = NO_STATION;
	m_PreviousStation = 0;

	// XXX XXX hack for now.  these values should probably be externalized in the xml interface.
	setAggregationBubbles(60000, 40000);
}

DynamicObject::~DynamicObject() {
	destroySceneModel();
}

void DynamicObject::postCreate() {
	b_Mass->value() = m_ReferenceMass;
	b_Inertia->value() = m_ReferenceInertia;
	b_InertiaInv->value() = m_ReferenceInertia.getInverse();
	b_CenterOfMassOffset->value() = m_ReferenceCenterOfMassOffset;
}

void DynamicObject::setReferenceMass(double mass) {
	m_ReferenceMass = mass;
	b_Mass->value() = m_ReferenceMass;
}

void DynamicObject::setReferenceInertia(Matrix3 const &inertia) {
	m_ReferenceInertia = inertia;
	b_Inertia->value() = m_ReferenceInertia;
	b_InertiaInv->value() = m_ReferenceInertia.getInverse();
}

void DynamicObject::setReferenceCgOffset(Vector3 const &offset) {
	m_ReferenceCenterOfMassOffset = offset;
	b_CenterOfMassOffset->value() = m_ReferenceCenterOfMassOffset;
}

void DynamicObject::createSceneModel() {
	if (!m_SceneModel) {
		m_SceneModel = new SceneModel(m_Model);
		assert(m_SceneModel.valid());
		if (activeStation()) createStationSceneModel();
		if (getBus()) bindAnimations(getBus());
		if (m_SystemsModel.valid()) {
			m_SystemsModel->attachSceneModel(m_SceneModel.get());
			m_SystemsModel->setInternalView(m_InternalView);
		}
	}
}

void DynamicObject::destroySceneModel() {
	if (m_SystemsModel.valid()) {
		m_SystemsModel->detachSceneModel(m_SceneModel.get());
	}
	if (m_SceneModel.valid()) {
		m_SceneModel = 0;
	}
}

osg::Node* DynamicObject::getOrCreateModelNode() {
	if (!m_SceneModel) createSceneModel();
	return m_SceneModel->getRoot();
}

osg::Node* DynamicObject::getModelNode() {
	if (!m_SceneModel) return 0;
	return m_SceneModel->getRoot();
}

void DynamicObject::setGlobalPosition(Vector3 const & position) {
	b_ModelPosition->value() = position;
	b_Position->value() = position + b_Attitude->value().rotate(b_CenterOfMassOffset->value());
}

void DynamicObject::setGlobalPosition(double x, double y, double z) {
	setGlobalPosition(Vector3(x, y, z));
}

void DynamicObject::setVelocity(Vector3 const &velocity) {
	b_LinearVelocity->value() = velocity;
}

void DynamicObject::setVelocity(double Vx, double Vy, double Vz) {
	setVelocity(Vector3(Vx, Vy, Vz));
}

void DynamicObject::setAngularVelocity(Vector3 const & angular_velocity) {
	b_AngularVelocity->value() = angular_velocity;
}

void DynamicObject::registerUpdate(UpdateMaster *master) {
	SimObject::registerUpdate(master);
	if (m_SystemsModel.valid()) {
		m_SystemsModel->registerUpdate(master);
	}
}

Ref<NetworkMessage> DynamicObject::getState(TimeStamp current_time, SimTime interval, int detail) const {
	CSPLOG(INFO, OBJECT) << "get object state";
	if (m_RemoteController.valid()) {
		CSPLOG(INFO, OBJECT) << "get object state from remote controller";
		return m_RemoteController->getUpdate(current_time, interval, detail);
	}
	return 0;
}

void DynamicObject::setState(Ref<NetworkMessage> const &msg, TimeStamp now) {
	CSPLOG(INFO, OBJECT) << "set object state";
	if (m_LocalController.valid()) {
		CSPLOG(INFO, OBJECT) << "set object state (local controller)";
		m_LocalController->onUpdate(msg, now);
	}
}

void DynamicObject::onHuman() {
	if (isLocal()) {
		selectVehicleCore();
	}
}

void DynamicObject::onAgent() {
	if (isLocal()) {
		cacheSystemsModel();
		selectVehicleCore();
	}
}

void DynamicObject::onLocal() {
	selectVehicleCore();
}

void DynamicObject::onRemote() {
	if (isHuman()) {
		cacheSystemsModel();
	}
	selectVehicleCore();
}

// update
double DynamicObject::onUpdate(double dt) {
	// Save the objects old cm position
	m_PrevPosition = b_Position->value();
	// XXX don't move non-human aircraft for now (no ai yet)
	if (isHuman()) {
		doControl(dt);
		//doPhysics(dt);
	}
	doPhysics(dt);  // XXX temporary hack to play with released stores (moved from the isHuman block above)
	if (m_SystemsModel.valid()) {
		StoresManagementSystem *sms = m_SystemsModel->getStoresManagementSystem().get();
		if (sms) {
			if (sms->hasDirtyDynamics()) updateDynamics(sms);
			if (sms->hasStoresToRelease()) sms->releaseMarkedStores(this);
		}
	}
	postUpdate(dt);
	return 0.0;
}

void DynamicObject::updateDynamics(StoresManagementSystem *sms) {
	StoresDynamics &dynamics = b_StoresDynamics->value();
	sms->getDynamics(dynamics);
	b_Mass->value() = m_ReferenceMass + dynamics.getMass();
	b_Inertia->value() = m_ReferenceInertia + dynamics.getInertia();
	b_InertiaInv->value() = b_Inertia->value().getInverse();
	b_CenterOfMassOffset->value() = (m_ReferenceCenterOfMassOffset * m_ReferenceMass + dynamics.getCenterOfMass() * dynamics.getMass()) / b_Mass->value();
	sms->clearDirtyDynamics();
}

void DynamicObject::doControl(double dt) {
	if (m_LocalController.valid()) {
		m_LocalController->onUpdate(dt);
	}
}

void DynamicObject::doPhysics(double dt) {
	if (m_PhysicsModel.valid()) {
		m_PhysicsModel->doSimStep(dt);
	}
}

// update variables that depend on position
void DynamicObject::postUpdate(double dt) {
	const Vector3 center_of_mass_offset_world = b_Attitude->value().rotate(b_CenterOfMassOffset->value());
	const Vector3 model_position = b_Position->value() - center_of_mass_offset_world;
	b_ModelPosition->value() = model_position;
	if (m_SceneModel.valid() && isSmoke()) {
		m_SceneModel->updateSmoke(dt, b_ModelPosition->value(), b_Attitude->value());
	}
	CSPSim *sim = CSPSim::theSim;
	assert(sim);
	TerrainObject *terrain = sim->getTerrain();
	assert(terrain);
	b_GroundZ->value() = terrain->getGroundElevation(
		model_position.x(),
		model_position.y(),
		b_GroundN->value(),
		m_GroundHint
	);
	double height = model_position.z() - b_GroundZ->value();
	b_NearGround->value() = (height * b_GroundN->value().z() < std::max(10.0, m_Model->getBoundingSphereRadius()));
	if (m_SceneModel.valid() && m_SceneModel->hasGroundShadow()) {
		m_SceneModel->updateGroundShadow(Vector3(0.0, 0.0, height) + center_of_mass_offset_world, b_GroundN->value());
	}
}

Vector3 DynamicObject::getDirection() const {
	return b_Attitude->value().rotate(Vector3::YAXIS);
}

Vector3 DynamicObject::getUpDirection() const {
	return b_Attitude->value().rotate(Vector3::ZAXIS);
}

void DynamicObject::setAttitude(Quat const &attitude) {
	b_Attitude->value() = attitude;
	b_Position->value() = b_ModelPosition->value() + attitude.rotate(b_CenterOfMassOffset->value());
}

Vector3 DynamicObject::getNominalViewPointBody() const {
	return m_Model->getViewPoint();
}

void DynamicObject::setViewPointBody(Vector3 const &point) {
	if (m_SceneModel.valid() && b_Hud.valid()) {
		b_Hud->value()->setViewPoint(toOSG(point));
	}
}

bool DynamicObject::isSmoke() {
	return m_SceneModel.valid() && m_SceneModel->isSmoke();
}

void DynamicObject::disableSmoke() {
	if (m_SceneModel.valid() && isSmoke()) {
		m_SceneModel->disableSmoke();
	}
}

void DynamicObject::enableSmoke() {
	if (m_SceneModel.valid() && !isSmoke()) {
		m_SceneModel->enableSmoke();
	}
}

void DynamicObject::internalView(bool internal) {
	m_InternalView = internal;
	if (m_SceneModel.valid()) {
		m_SceneModel->onViewMode(internal);
		// the following is mostly for testing; we may want to keep the detailed pit
		// in the external view as well.  Calling [de]activateStation() also works;
		// it is less efficient but doesn't create a noticible delay.
		if (m_StationSceneModel.valid()) {
			m_SceneModel->setStation(internal ? m_ActiveStation : NO_STATION);
			m_StationSceneModel->getRoot()->setNodeMask(internal ? ~0 : 0);
		}
		// notify interested subsystems that the view has changed.
		if (m_SystemsModel.valid()) m_SystemsModel->setInternalView(internal);
	}
}

void DynamicObject::setDataRecorder(DataRecorder *recorder) {
	if (!recorder) return;
	if (m_SystemsModel.valid()) {
		m_SystemsModel->setDataRecorder(recorder);
	}
	recorder->addSource(b_ModelPosition);
	recorder->addSource(b_LinearVelocity);
	recorder->addSource(b_AngularVelocity);
	recorder->addSource(b_AngularVelocityBody);
}

std::list<DynamicObject::SystemsModelStore> DynamicObject::SystemsModelCache;

void DynamicObject::cacheSystemsModel() {
	if (!m_SystemsModel.valid()) return;
	std::list<SystemsModelStore>::iterator cached = SystemsModelCache.begin();
	for (; cached != SystemsModelCache.end(); ++cached) {
		if (cached->id == id()) {
			SystemsModelCache.erase(cached);
			break;
		}
	}
	if (SystemsModelCache.size() >= MODEL_CACHE_SIZE) {
		SystemsModelCache.pop_front();
	}
	SystemsModelCache.push_back(SystemsModelStore(id(), m_SystemsModel));
}

Ref<SystemsModel> DynamicObject::getCachedSystemsModel() {
	std::list<SystemsModelStore>::iterator iter;
	Ref<SystemsModel> model;
	for (iter = SystemsModelCache.begin(); iter != SystemsModelCache.end(); ++iter) {
		if (iter->id == id()) {
			model = iter->model;
			SystemsModelCache.erase(iter);
			break;
		}
	}
	return model;
}

void DynamicObject::registerChannels(Bus* bus) {
	if (!bus) return;
	bus->registerChannel(b_Position.get());
	bus->registerChannel(b_ModelPosition.get());
	bus->registerChannel(b_LinearVelocity.get());
	bus->registerChannel(b_AccelerationBody.get());
	bus->registerChannel(b_AngularVelocity.get());
	bus->registerChannel(b_AngularVelocityBody.get());
	bus->registerChannel(b_Attitude.get());
	bus->registerChannel(b_Mass.get());
	bus->registerChannel(b_Inertia.get());
	bus->registerChannel(b_InertiaInv.get());
	bus->registerChannel(b_CenterOfMassOffset.get());
	bus->registerChannel(b_GroundN.get());
	bus->registerChannel(b_GroundZ.get());
	bus->registerChannel(b_NearGround.get());
	bus->registerChannel(b_StoresDynamics.get());
	bus->registerLocalDataChannel< Ref<ObjectModel> >("Internal.ObjectModel", m_Model);
}

Bus* DynamicObject::getBus() {
	return (m_SystemsModel.valid() ? m_SystemsModel->getBus() : 0);
}

void DynamicObject::createStationSceneModel() {
	assert(activeStation() && !m_StationSceneModel);
	assert(m_SceneModel.valid());
	m_StationSceneModel = activeStation()->createDetailModel();
	if (m_StationSceneModel.valid()) {
		m_SceneModel->addChild(m_StationSceneModel);
		m_SceneModel->setStation(m_ActiveStation);
	}
}

void DynamicObject::activateStation(int index) {
	CSPLOG(INFO, OBJECT) << "Object " << *this << " selecting station " << index;
	if (m_ActiveStation != index) {
		if (m_SceneModel.valid() && m_StationSceneModel.valid()) {
			m_StationSceneModel->bindAnimationChannels(0);
			m_SceneModel->removeChild(m_StationSceneModel);
			m_SceneModel->setStation(NO_STATION);
			m_StationSceneModel = 0;
		}
		if (index < static_cast<int>(m_Model->numStations())) {
			m_ActiveStation = index;
		} else {
			m_ActiveStation = NO_STATION;
			CSPLOG(ERROR, OBJECT) << "Selecting invalid station " << index;
		}
		if (m_SceneModel.valid() && activeStation()) {
			createStationSceneModel();
			m_StationSceneModel->bindAnimationChannels(getBus());
		}
	}
}

void DynamicObject::deactivateStation() {
	if (m_ActiveStation >= 0) {
		m_PreviousStation = m_ActiveStation;
		activateStation(NO_STATION);
	}
}

Station const *DynamicObject::activeStation() {
	return m_ActiveStation >= 0 ? m_Model->station(m_ActiveStation) : 0;
}

void DynamicObject::bindAnimations(Bus* bus) {
	if (m_SceneModel.valid()) {
		m_SceneModel->bindAnimationChannels(bus);
		if (b_Hud.valid()) {
			m_SceneModel->bindHud(b_Hud->value());
		}
		if (m_StationSceneModel.valid()) {
			m_StationSceneModel->bindAnimationChannels(bus);
		}
	}
}

// called whenever the bus (ie systemsmodel) changes
void DynamicObject::bindChannels(Bus* bus) {
	if (bus != NULL) {
		b_Hud = bus->getChannel("HUD", false);
		bindAnimations(bus);
	} else {
		CSPLOG(DEBUG, OBJECT) << "DynamicObject::bindChannels() - bus is invalid";
	}
}

void DynamicObject::selectVehicleCore() {
	Path path;
	Ref<SystemsModel> systems;
	if (isLocal()) {
		if (isHuman()) {
			systems = getCachedSystemsModel();
			path = m_HumanModel;
			CSPLOG(INFO, OBJECT) << "selecting local human systems model for " << *this;
			activateStation(m_PreviousStation);
		} else {
			path = m_AgentModel;
			CSPLOG(INFO, OBJECT) << "selecting local agent systems model for " << *this;
			deactivateStation();
		}
	} else {
		path = m_RemoteModel;
		CSPLOG(INFO, OBJECT) << "selecting remote systems model for " << *this;
		deactivateStation();
	}
	if (!systems && !path.isNone()) {
		CSPSim *sim = CSPSim::theSim;
		if (sim) {
			DataManager &manager = sim->getDataManager();
			systems = manager.getObject(path);
			if (systems.valid()) {
				CSPLOG(INFO, OBJECT) << "registering channels and binding systems for " << *this << " " << this;
				registerChannels(systems->getBus());
				systems->bindSystems();
			}
		}
	}
	setVehicleCore(systems);
}

void DynamicObject::setVehicleCore(Ref<SystemsModel> systems) {
	if (systems.valid()) {
		systems->init(m_SystemsModel);
		m_PhysicsModel = systems->getPhysicsModel();
		m_LocalController = systems->getLocalController();
		m_RemoteController = systems->getRemoteController();
	} else {
		m_PhysicsModel = 0;
		m_LocalController = 0;
		m_RemoteController = 0;
	}
	if (m_SystemsModel.valid()) {
		m_SystemsModel->registerUpdate(0);
		m_SystemsModel->detachSceneModel(m_SceneModel.get());
	}
	m_SystemsModel = systems;
	if (m_SystemsModel.valid()) {
		bindChannels(getBus());
		m_SystemsModel->copyRegistration(this);
		m_SystemsModel->attachSceneModel(m_SceneModel.get());
		m_SystemsModel->setInternalView(m_InternalView);
	}
}

bool DynamicObject::onMapEvent(MapEvent const &event) {
	if (InputInterface::onMapEvent(event)) {
		return true;
	}
	if (m_SystemsModel.valid() && m_SystemsModel->onMapEvent(event)) {
		return true;
	}
	return false;
}

void DynamicObject::getInfo(std::vector<std::string> &info) const {
	if (m_SystemsModel.valid()) {
		m_SystemsModel->getInfo(info);
	}
}

void DynamicObject::updateScene(Vector3 const &origin) {
	if (m_SceneModel.valid()) {
		m_SceneModel->setPositionAttitude(b_Position->value() - origin, b_Attitude->value(), b_CenterOfMassOffset->value());
		onRender();
	}
}

Ref<SceneModel> DynamicObject::getSceneModel() {
	return m_SceneModel;
}

Ref<ObjectModel> DynamicObject::getModel() const {
	return m_Model;
}

Ref<SystemsModel> DynamicObject::getSystemsModel() const {
	return m_SystemsModel;
}

DynamicObject::SystemsModelStore::SystemsModelStore(): id(0) {
}

DynamicObject::SystemsModelStore::SystemsModelStore(ObjectId id_, Ref<SystemsModel> model_): id(id_), model(model_) {
}

DynamicObject::SystemsModelStore::~SystemsModelStore() {
}

void DynamicObject::toggleMarkers() {
	m_Model->showDebugMarkers(!m_Model->getDebugMarkersVisible());
}

} // namespace csp

