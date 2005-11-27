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
#include <DynamicModel.h>
#include <Controller.h>
#include <CSPSim.h>
#include <DataRecorder.h>
#include <HUD/HUD.h>
#include <KineticsChannels.h>
#include <ObjectModel.h>
#include <PhysicsModel.h>
#include <Station.h>
#include <Stores/StoresManagementSystem.h>
#include <SystemsModel.h>
#include <TerrainObject.h>

//#include <csp/csplib/battlefield/LocalBattlefield.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/data/DataManager.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/util/osg.h>
#include <csp/csplib/data/Quat.h>

#include <osg/Group>


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
	b_StoresDynamics = DataChannel<StoresDynamics>::newLocal(Kinetics::StoresDynamics, StoresDynamics());

	m_DynamicModel = new DynamicModel;
	b_DynamicModel = DataChannel<DynamicModel*>::newLocal("DynamicModel", m_DynamicModel.get());

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

void DynamicObject::setReferenceMass(double mass) {
	m_ReferenceMass = mass;
	b_Mass->value() = m_ReferenceMass;
}

void DynamicObject::setReferenceInertia(simdata::Matrix3 const &inertia) {
	m_ReferenceInertia = inertia;
	b_Inertia->value() = m_ReferenceInertia;
	b_InertiaInv->value() = m_ReferenceInertia.getInverse();
}

void DynamicObject::setReferenceCgOffset(simdata::Vector3 const &offset) {
	m_ReferenceCenterOfMassOffset = offset;
	b_CenterOfMassOffset->value() = m_ReferenceCenterOfMassOffset;
}

void DynamicObject::createSceneModel() {
	if (!m_SceneModel) {
		m_SceneModel = new SceneModel(m_Model);
		assert(m_SceneModel.valid());
		if (activeStation()) createStationSceneModel();
		if (getBus()) bindAnimations(getBus());
		if (m_DynamicModel.valid()) m_DynamicModel->signalCreateSceneModel(m_SceneModel.get());
	}
}

void DynamicObject::destroySceneModel() {
	if (m_DynamicModel.valid()) m_DynamicModel->signalDeleteSceneModel();
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

void DynamicObject::setAngularVelocity(simdata::Vector3 const & angular_velocity) {
	b_AngularVelocity->value() = angular_velocity;
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
	std::cout <<
		"updateDynamics: \n"
		"  mass_ref=" << m_ReferenceMass << ", mass_sms=" << dynamics.getMass() << "\n"
		"  cmas_ref=" << m_ReferenceCenterOfMassOffset << ", cmas_sms=" << dynamics.getCenterOfMass() << "\n"
		"  cmas_ofs=" << b_CenterOfMassOffset->value() << "\n"
		"  drag coe=" << dynamics.getDrag() << "\n"
		"  drag pos=" << dynamics.getCenterOfDrag() << "\n"
		"  I_ref   =\n" << m_ReferenceInertia << "\n"
		"  I_sms   =\n" << dynamics.getInertia() << "\n";
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
	// notify interested subsystems that the view has changed.
	if (m_DynamicModel.valid()) m_DynamicModel->signalInternalView(internal);
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
	bus->registerChannel(b_DynamicModel.get());
	bus->registerChannel(b_StoresDynamics.get());
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
	if (bus != NULL) {
		b_Hud = bus->getChannel("HUD", false);
		bindAnimations(bus);
	} else {
		CSP_LOG(OBJECT, DEBUG, "DynamicObject::bindChannels() - bus is invalid");
	}
}

// TODO need to notify new systems model of current state (e.g. allow it to connect to
// the scene model and register DynamicModel event handlers).  moveover we need to transfer
// state, such as the loadout.  ideally the loadout scene group can be shared.
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

void DynamicObject::toggleMarkers() {
	std::cout << "MARKERS +++++++++++++++++\n";
	m_Model->showDebugMarkers(!m_Model->getDebugMarkersVisible());
}

