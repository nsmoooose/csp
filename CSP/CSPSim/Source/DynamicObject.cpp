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
 * @file DynamicObject.cpp
 *
 **/


#include <DynamicObject.h>
#include <Controller.h>
#include <PhysicsModel.h>
#include <Log.h>
#include <VirtualBattlefield.h>
#include <VirtualScene.h>
#include <TerrainObject.h>
#include <CSPSim.h>
#include <KineticsChannels.h>

#include <SimData/Quat.h>

#include "Networking.h"


SIMDATA_REGISTER_INTERFACE(DynamicObject)


using bus::Kinetics;


DynamicObject::DynamicObject(): SimObject()
{
	b_GlobalPosition = DataChannel<simdata::Vector3>::newLocal(Kinetics::Position, simdata::Vector3::ZERO);
	b_Mass = DataChannel<double>::newLocal(Kinetics::Mass, 1.0);
	b_GroundZ = DataChannel<double>::newLocal(Kinetics::GroundZ, 0.0);
	b_GroundN = DataChannel<simdata::Vector3>::newLocal(Kinetics::GroundN, simdata::Vector3::ZAXIS);
	b_NearGround = DataChannel<bool>::newLocal(Kinetics::NearGround, false);
	b_Inertia = DataChannel<simdata::Matrix3>::newLocal(Kinetics::Inertia, simdata::Matrix3::IDENTITY);
	b_InertiaInv = DataChannel<simdata::Matrix3>::newLocal(Kinetics::InertiaInverse, simdata::Matrix3::IDENTITY);
	b_AngularVelocity = DataChannel<simdata::Vector3>::newLocal(Kinetics::AngularVelocity, simdata::Vector3::ZERO);
	b_LinearVelocity = DataChannel<simdata::Vector3>::newLocal(Kinetics::Velocity, simdata::Vector3::ZERO);
	b_Attitude = DataChannel<simdata::Quat>::newLocal(Kinetics::Attitude, simdata::Quat::IDENTITY);

	m_GroundHint = 0;
	m_ReferenceMass = 1.0;
	m_ReferenceInertia = simdata::Matrix3::IDENTITY;

	m_Controller = NULL;

	setLocal(true);
	setHuman(false);

	setGlobalPosition(simdata::Vector3::ZERO);
	m_PrevPosition = simdata::Vector3::ZERO;
}

DynamicObject::~DynamicObject()
{
}

void DynamicObject::postCreate() {
	b_Mass->value() = m_ReferenceMass;
	b_Inertia->value() = m_ReferenceInertia;
	b_InertiaInv->value() = m_ReferenceInertia.getInverse();
}

void DynamicObject::createSceneModel() {
	if (!m_SceneModel) {
		m_SceneModel = new SceneModel(m_Model);
		assert(m_SceneModel.valid());
		bindAnimations(getBus());
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


void DynamicObject::setGlobalPosition(simdata::Vector3 const & position)
{
	setGlobalPosition(position.x(), position.y(), position.z());
}

void DynamicObject::setGlobalPosition(double x, double y, double z)
{
	if (isGrounded())
	{
		// if the object is fixed to the ground, ignore the z component 
		// and use the local elevation (not implemented)
		// FIXME FIXME FIXME XXX XXX
		float offset = 0.0;
		z = offset; 
	}
	b_GlobalPosition->value() = simdata::Vector3(x, y, z);
}

void DynamicObject::setVelocity(simdata::Vector3 const &velocity)
{
	b_LinearVelocity->value() = velocity;
}

void DynamicObject::setVelocity(double Vx, double Vy, double Vz)
{
	setVelocity(simdata::Vector3(Vx, Vy, Vz));
}

void DynamicObject::updateGroundPosition()
{
	float offset;
	if (m_Model.valid()) {
		//offset = m_Model->groundOffset();
		offset = 0.0;
	}
	else {
		offset = 0.0;
	}
	CSPSim *sim = CSPSim::theSim;
	assert(sim);
	VirtualBattlefield const *battlefield = sim->getBattlefield();
	assert(battlefield);
	simdata::Vector3 &pos = b_GlobalPosition->value();
	pos.z() = battlefield->getGroundElevation(pos.x(), pos.y(), m_GroundHint) + offset; 
}


void DynamicObject::registerUpdate(UpdateMaster *master) {
	SimObject::registerUpdate(master);
	if (m_SystemsModel.valid()) {
		m_SystemsModel->registerUpdate(master);
	}
}


// update 
double DynamicObject::onUpdate(double dt)
{
	// Save the objects old position
	m_PrevPosition = b_GlobalPosition->value();
	// XXX don't move non-human aircraft for now (no ai yet)
	if (isHuman()) {
		doControl(dt);
		doPhysics(dt);
	}
	postUpdate(dt);
	return 0.0;
}


void DynamicObject::doControl(double dt) {
	if (m_Controller.valid()) {
		m_Controller->doControl(dt);
	}
}

void DynamicObject::doPhysics(double dt) {
	if (m_PhysicsModel.valid()) {
		m_PhysicsModel->doSimStep(dt);
	}
}
	

// update variables that depend on position
void DynamicObject::postUpdate(double dt) {
	if (m_SceneModel.valid() && isSmoke()) {
		m_SceneModel->updateSmoke(dt, b_GlobalPosition->value(), b_Attitude->value());
	}
	if (isGrounded()) {
		// FIXME GroundZ/GroundN should be set for all vehicles
		updateGroundPosition();
	} else {
		CSPSim *sim = CSPSim::theSim;
		assert(sim);
		VirtualBattlefield const *battlefield = sim->getBattlefield();
		assert(battlefield);
		b_GroundZ->value() = battlefield->getGroundElevation(
			b_GlobalPosition->value().x(), 
			b_GlobalPosition->value().y(), 
			b_GroundN->value(), 
			m_GroundHint
		);
		double height = (b_GlobalPosition->value().z() - b_GroundZ->value()) * b_GroundN->value().z();
		b_NearGround->value() = (height < m_Model->getBoundingSphereRadius());
	}
}

simdata::Vector3 DynamicObject::getDirection() const
{
	return b_Attitude->value().rotate(simdata::Vector3::YAXIS);
}

simdata::Vector3 DynamicObject::getUpDirection() const
{
	return b_Attitude->value().rotate(simdata::Vector3::ZAXIS);
}

void DynamicObject::setAttitude(simdata::Quat const &attitude)
{
	b_Attitude->value() = attitude;
}

simdata::Vector3 DynamicObject::getViewPoint() const {
	 return b_Attitude->value().rotate(m_Model->getViewPoint());
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

void DynamicObject::setDataRecorder(DataRecorder *recorder) {
	if (!recorder) return;
	if (m_SystemsModel.valid()) {
		m_SystemsModel->setDataRecorder(recorder);
	}
	recorder->addSource(b_GlobalPosition);
	recorder->addSource(b_LinearVelocity);
	recorder->addSource(b_AngularVelocity);
}

std::list<DynamicObject::SystemsModelStore> DynamicObject::SystemsModelCache;

void DynamicObject::cacheSystemsModel() {
	std::list<SystemsModelStore>::iterator cached = SystemsModelCache.begin();
	for (; cached != SystemsModelCache.end(); ++cached) {
		if (cached->id == getID()) {
			SystemsModelCache.erase(cached);
			break;
		}
	}
	if (SystemsModelCache.size() >= MODELCACHESIZE) {
		SystemsModelCache.pop_front();
	}
	SystemsModelCache.push_back(SystemsModelStore(getID(), m_SystemsModel));
}

SystemsModel::Ref DynamicObject::getCachedSystemsModel() {
	std::list<SystemsModelStore>::iterator iter;
	simdata::Ref<SystemsModel> model;
	for (iter = SystemsModelCache.begin(); iter != SystemsModelCache.end(); ++iter) {
		if (iter->id == getID()) {
			model = iter->model;
			SystemsModelCache.erase(iter);
			break;
		}
	}
	return model;
}


void DynamicObject::registerChannels(Bus::Ref bus) {
	if (!bus) return;
	bus->registerChannel(b_GlobalPosition.get());
	bus->registerChannel(b_LinearVelocity.get());
	bus->registerChannel(b_AngularVelocity.get());
	bus->registerChannel(b_Attitude.get());
	bus->registerChannel(b_Mass.get());
	bus->registerChannel(b_Inertia.get());
	bus->registerChannel(b_InertiaInv.get());
	bus->registerChannel(b_GroundN.get());
	bus->registerChannel(b_GroundZ.get());
	bus->registerChannel(b_NearGround.get());
	bus->registerLocalDataChannel< simdata::Ref<ObjectModel> >("Internal.ObjectModel", m_Model);
}


Bus::Ref DynamicObject::getBus() {
	return (m_SystemsModel.valid() ? m_SystemsModel->getBus(): 0);
}

void DynamicObject::bindAnimations(Bus::Ref bus) {
	if (m_SceneModel.valid()) {
		m_SceneModel->bindAnimationChannels(bus);
	}
}

// called whenever the bus (ie systemsmodel) changes
void DynamicObject::bindChannels(Bus::Ref bus) {
	bindAnimations(bus);
}

void DynamicObject::selectVehicleCore() {
	simdata::Path path;
	simdata::Ref<SystemsModel> systems;
	if (isLocal()) {
		if (isHuman()) {
			systems = getCachedSystemsModel();
			path = m_HumanModel;
		} else {
			path = m_AgentModel;
		}
		if (!systems && !path.isNone()) {
			CSPSim *sim = CSPSim::theSim;
			if (sim) {
				simdata::DataManager &manager = sim->getDataManager();
				systems = manager.getObject(path);
				if (systems.valid()) {
					registerChannels(systems->getBus());
					systems->bindSystems();
				}
			}
		}
	} else { // remote control
		// systems = ...;
		// etc
	}
	setVehicleCore(systems);
}

void DynamicObject::setVehicleCore(SystemsModel::Ref systems) {
	if (systems.valid()) {
		systems->init(m_SystemsModel);
		m_PhysicsModel = systems->getPhysicsModel();
		m_Controller = systems->getController();
	} else {
		m_PhysicsModel = 0;
		m_Controller = 0;
	}
	if (m_SystemsModel.valid()) {
		m_SystemsModel->registerUpdate(0);
	}
	m_SystemsModel = systems;
	bindChannels(getBus());
	if (m_SystemsModel.valid()) {
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
		m_SceneModel->setPositionAttitude(b_GlobalPosition->value() - origin, b_Attitude->value());
		onRender();
	}
}


NetworkMessage * DynamicObject::getUpdateMessage()
{
  CSP_LOG(APP, DEBUG, "DynamicObject::getUpdateMessage()");
  unsigned short messageType = 2;
  unsigned short payloadLen  = sizeof(int) + sizeof(double) + 3*sizeof(simdata::Vector3) +
	                       sizeof(simdata::Quat) /* + sizeof(simdata::Matrix3) + sizeof(double) */;

  NetworkMessage * message = CSPSim::theSim->getNetworkMessenger()->allocMessageBuffer(messageType, payloadLen);

  ObjectUpdateMessagePayload * ptrPayload = (ObjectUpdateMessagePayload*)message->getPayloadPtr();
//  ptrPayload->id = m_ID;
//  ptrPayload->timeStamp = CSPSim::theSim->getElapsedTime();
//  b_GlobalPosition->value().writeBinary((unsigned char *)&(ptrPayload->globalPosition),24);
//  b_LinearVelocity->value().writeBinary((unsigned char *)&(ptrPayload->linearVelocity),24);
//  b_AngularVelocity->value().writeBinary((unsigned char *)&(ptrPayload->angularVelocity),24);
//  b_Attitude->value().writeBinary((unsigned char *)&(ptrPayload->attitude),32);
 
  simdata::MemoryWriter writer((simdata::uint8*)ptrPayload);
  writer << (int)m_ID;
  simdata::uint32 objType = 1;
  writer << (int)objType;
  writer << CSPSim::theSim->getElapsedTime();
  b_GlobalPosition->value().serialize(writer);
  b_LinearVelocity->value().serialize(writer);
  b_AngularVelocity->value().serialize(writer);
  b_Attitude->value().serialize(writer);
  
  CSP_LOG(APP, DEBUG, "DynamicObject::getUpdateMessage() - returning message");


  return message;
}

void DynamicObject::putUpdateMessage(NetworkMessage* message)
{
  // read message

  ObjectUpdateMessagePayload * ptrPayload = (ObjectUpdateMessagePayload*)message->getPayloadPtr();
  // verify we have the correct id in the packet for this object.
  if (m_ID == ptrPayload->id)
  {
//	printf("Loading update message of object %d\n", m_ID);
  }
  else
  {
//	printf("Error loading update message, object id (%d) does not match\n", idValue);
  }

  // we can disregard this message if the timestamp is older then the most
  // recent update.
  
  //ptrPayload->timeStamp = CSPSim::theSim->getElapsedTime();
  //
  //load the other values.
  b_GlobalPosition->value().readBinary((unsigned char*)&(ptrPayload->globalPosition),24);
  b_LinearVelocity->value().readBinary((unsigned char *)&(ptrPayload->linearVelocity),24);
  b_AngularVelocity->value().readBinary((unsigned char *)&(ptrPayload->angularVelocity),24);
  b_Attitude->value().readBinary((unsigned char *)&(ptrPayload->attitude),32);
  
}




