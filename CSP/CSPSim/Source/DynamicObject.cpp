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


#include <DynamicObject.h>
#include <Animation.h>
#include <Controller.h>
#include <CSPSim.h>
#include <HUD/HUD.h>
#include <KineticsChannels.h>
#include <ObjectModel.h>
#include <PhysicsModel.h>
#include <Station.h>
#include <SystemsModel.h>
#include <TerrainObject.h>

#include <SimCore/Util/Log.h>
#include <SimData/DataManager.h>
#include <SimData/Quat.h>

#include <osg/Group>


SIMDATA_REGISTER_INTERFACE(DynamicObject)


using bus::Kinetics;


DynamicObject::DynamicObject(TypeId type): SimObject(type) {
	assert(!isStatic());

	b_ModelPosition = DataChannel<simdata::Vector3>::newLocal(Kinetics::ModelPosition, simdata::Vector3::ZERO);
	b_Position = DataChannel<simdata::Vector3>::newLocal(Kinetics::Position, simdata::Vector3::ZERO);
	b_Mass = DataChannel<double>::newLocal(Kinetics::Mass, 1.0);
	b_GroundZ = DataChannel<double>::newLocal(Kinetics::GroundZ, 0.0);
	b_GroundN = DataChannel<simdata::Vector3>::newLocal(Kinetics::GroundN, simdata::Vector3::ZAXIS);
	b_NearGround = DataChannel<bool>::newLocal(Kinetics::NearGround, false);
	b_Inertia = DataChannel<simdata::Matrix3>::newLocal(Kinetics::Inertia, simdata::Matrix3::IDENTITY);
	b_InertiaInv = DataChannel<simdata::Matrix3>::newLocal(Kinetics::InertiaInverse, simdata::Matrix3::IDENTITY);
	b_AngularVelocity = DataChannel<simdata::Vector3>::newLocal(Kinetics::AngularVelocity, simdata::Vector3::ZERO);
	b_AngularVelocityBody = DataChannel<simdata::Vector3>::newLocal(Kinetics::AngularVelocityBody, simdata::Vector3::ZERO);
	b_LinearVelocity = DataChannel<simdata::Vector3>::newLocal(Kinetics::Velocity, simdata::Vector3::ZERO);
	b_AccelerationBody = DataChannel<simdata::Vector3>::newLocal(Kinetics::AccelerationBody, simdata::Vector3::ZERO);
	b_Attitude = DataChannel<simdata::Quat>::newLocal(Kinetics::Attitude, simdata::Quat::IDENTITY);
	b_CenterOfMassOffset = DataChannel<simdata::Vector3>::newLocal(Kinetics::CenterOfMassOffset, simdata::Vector3::ZERO);

	m_GroundHint = 0;
	m_ReferenceMass = 1.0;
	m_ReferenceInertia = simdata::Matrix3::IDENTITY;
	m_ReferenceCenterOfMassOffset = simdata::Vector3::ZERO;

	setGlobalPosition(simdata::Vector3::ZERO);
	m_PrevPosition = simdata::Vector3::ZERO;

	m_ActiveStation = -1;
	m_PreviousStation = 0;

	// XXX XXX hack for now.  these values should probably be externalized in the xml interface.
	setAggregationBubbles(60000, 40000);
}

DynamicObject::~DynamicObject() {
}

void DynamicObject::postCreate() {
	b_Mass->value() = m_ReferenceMass;
	b_Inertia->value() = m_ReferenceInertia;
	b_InertiaInv->value() = m_ReferenceInertia.getInverse();
	b_CenterOfMassOffset->value() = m_ReferenceCenterOfMassOffset;
}

void DynamicObject::createSceneModel() {
	if (!m_SceneModel) {
		m_SceneModel = new SceneModel(m_Model);
		assert(m_SceneModel.valid());
		if (activeStation()) createStationSceneModel();
		if (getBus()) bindAnimations(getBus());
	}
}

void DynamicObject::destroySceneModel() {
	m_SceneModel = NULL;
}

osg::Node* DynamicObject::getOrCreateModelNode() {
	if (!m_SceneModel) createSceneModel();
	return m_SceneModel->getRoot();
}

osg::Node* DynamicObject::getModelNode() {
	if (!m_SceneModel) return 0;
	return m_SceneModel->getRoot();
}

void DynamicObject::setGlobalPosition(simdata::Vector3 const & position) {
	b_ModelPosition->value() = position;
	b_Position->value() = position + b_Attitude->value().rotate(b_CenterOfMassOffset->value());
}

void DynamicObject::setGlobalPosition(double x, double y, double z) {
	setGlobalPosition(simdata::Vector3(x, y, z));
}

void DynamicObject::setVelocity(simdata::Vector3 const &velocity) {
	b_LinearVelocity->value() = velocity;
}

void DynamicObject::setVelocity(double Vx, double Vy, double Vz) {
	setVelocity(simdata::Vector3(Vx, Vy, Vz));
}

void DynamicObject::registerUpdate(UpdateMaster *master) {
	SimObject::registerUpdate(master);
	if (m_SystemsModel.valid()) {
		m_SystemsModel->registerUpdate(master);
	}
}

simdata::Ref<simnet::NetworkMessage> DynamicObject::getState(simcore::TimeStamp current_time, simdata::SimTime interval, int detail) const {
	CSP_LOG(OBJECT, INFO, "get object state");
	if (m_RemoteController.valid()) {
		CSP_LOG(OBJECT, INFO, "get object state from remote controller");
		return m_RemoteController->getUpdate(current_time, interval, detail);
	}
	return 0;
}

void DynamicObject::setState(simdata::Ref<simnet::NetworkMessage> const &msg, simcore::TimeStamp now) {
	CSP_LOG(OBJECT, INFO, "set object state");
	if (m_LocalController.valid()) {
		CSP_LOG(OBJECT, INFO, "set object state (local controller)");
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
		doPhysics(dt);
	}
	postUpdate(dt);
	return 0.0;
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
	const simdata::Vector3 model_position = b_Position->value() - b_Attitude->value().rotate(b_CenterOfMassOffset->value());
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
	double height = (model_position.z() - b_GroundZ->value()) * b_GroundN->value().z();
	b_NearGround->value() = (height < m_Model->getBoundingSphereRadius());
}

simdata::Vector3 DynamicObject::getDirection() const {
	return b_Attitude->value().rotate(simdata::Vector3::YAXIS);
}

simdata::Vector3 DynamicObject::getUpDirection() const {
	return b_Attitude->value().rotate(simdata::Vector3::ZAXIS);
}

void DynamicObject::setAttitude(simdata::Quat const &attitude) {
	b_Attitude->value() = attitude;
	b_Position->value() = b_ModelPosition->value() + attitude.rotate(b_CenterOfMassOffset->value());
}

simdata::Vector3 DynamicObject::getNominalViewPointBody() const {
	return m_Model->getViewPoint();
}

void DynamicObject::setViewPointBody(simdata::Vector3 const &point) {
	if (m_SceneModel.valid() && b_Hud.valid()) {
		b_Hud->value()->setViewPoint(simdata::toOSG(point));
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
	if (!m_SceneModel.valid()) return;
	m_SceneModel->onViewMode(internal);
	// the following is mostly for testing; we may want to keep the detailed pit
	// in the external view as well.  Calling [de]activateStation() also works;
	// it is less efficient but doesn't create a notible delay.
	if (m_StationSceneModel.valid()) {
		m_SceneModel->setPitMask(internal ? activeStation()->getMask() : 0);
		m_StationSceneModel->getRoot()->setNodeMask(internal ? ~0 : 0);
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
	if (SystemsModelCache.size() >= MODELCACHESIZE) {
		SystemsModelCache.pop_front();
	}
	SystemsModelCache.push_back(SystemsModelStore(id(), m_SystemsModel));
}

SystemsModel::Ref DynamicObject::getCachedSystemsModel() {
	std::list<SystemsModelStore>::iterator iter;
	simdata::Ref<SystemsModel> model;
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
	bus->registerLocalDataChannel< simdata::Ref<ObjectModel> >("Internal.ObjectModel", m_Model);
}

Bus* DynamicObject::getBus() {
	return (m_SystemsModel.valid() ? m_SystemsModel->getBus().get(): 0);
}

void DynamicObject::createStationSceneModel() {
	assert(activeStation() && m_SceneModel.valid() && !m_StationSceneModel);
	m_StationSceneModel = activeStation()->createDetailModel();
	if (m_StationSceneModel.valid()) {
		m_SceneModel->addChild(m_StationSceneModel);
		m_SceneModel->setPitMask(activeStation()->getMask());
	}
}

void DynamicObject::activateStation(int index) {
	if (m_ActiveStation != index) {
		if (m_SceneModel.valid() && m_StationSceneModel.valid()) {
			m_StationSceneModel->bindAnimationChannels(0);
			m_SceneModel->removeChild(m_StationSceneModel);
			m_SceneModel->setPitMask(0);
			m_StationSceneModel = 0;
		}
		if (index < static_cast<int>(m_Model->numStations())) {
			m_ActiveStation = index;
		} else {
			m_ActiveStation = -1;
			CSP_LOG(OBJECT, ERROR, "Selecting invalid station " << index);
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
		activateStation(-1);
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
	b_Hud = bus->getChannel("HUD", false);
	bindAnimations(bus);
}

void DynamicObject::selectVehicleCore() {
	simdata::Path path;
	simdata::Ref<SystemsModel> systems;
	if (isLocal()) {
		if (isHuman()) {
			systems = getCachedSystemsModel();
			path = m_HumanModel;
			CSP_LOG(OBJECT, INFO, "selecting local human systems model for " << *this);
			activateStation(m_PreviousStation);
		} else {
			path = m_AgentModel;
			CSP_LOG(OBJECT, INFO, "selecting local agent systems model for " << *this);
			deactivateStation();
		}
	} else {
		path = m_RemoteModel;
		CSP_LOG(OBJECT, INFO, "selecting remote systems model for " << *this);
		deactivateStation();
	}
	if (!systems && !path.isNone()) {
		CSPSim *sim = CSPSim::theSim;
		if (sim) {
			simdata::DataManager &manager = sim->getDataManager();
			systems = manager.getObject(path);
			if (systems.valid()) {
				CSP_LOG(OBJECT, INFO, "registering channels and binding systems for " << *this << " " << this);
				registerChannels(systems->getBus().get());
				systems->bindSystems();
			}
		}
	}
	setVehicleCore(systems);
}

void DynamicObject::setVehicleCore(SystemsModel::Ref systems) {
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
	}
	m_SystemsModel = systems;
	if (m_SystemsModel.valid()) {
		bindChannels(getBus());
		m_SystemsModel->copyRegistration(this);
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

void DynamicObject::updateScene(simdata::Vector3 const &origin) {
	if (m_SceneModel.valid()) {
		m_SceneModel->setPositionAttitude(b_Position->value() - origin, b_Attitude->value(), b_CenterOfMassOffset->value());
		onRender();
	}
}

simdata::Ref<SceneModel> DynamicObject::getSceneModel() {
	return m_SceneModel;
}

simdata::Ref<ObjectModel> DynamicObject::getModel() const {
	return m_Model;
}

simdata::Ref<SystemsModel> DynamicObject::getSystemsModel() const {
	return m_SystemsModel;
}

DynamicObject::SystemsModelStore::SystemsModelStore(): id(0) {
}

DynamicObject::SystemsModelStore::SystemsModelStore(ObjectId id_, simdata::Ref<SystemsModel> model_): id(id_), model(model_) {
}

DynamicObject::SystemsModelStore::~SystemsModelStore() {
}


