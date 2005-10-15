// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2003 The Combat Simulator Project
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
 * @file SystemsModel.cpp
 *
 * Vehicle systems base classes and infrastructure.
 *
 **/


#include <SystemsModel.h>
#include <DataRecorder.h>
#include <Stores/StoresManagementSystem.h>

#include <SimData/ObjectInterface.h>


SIMDATA_XML_BEGIN(SystemsModel)
	SIMDATA_DEF("physics_model", m_PhysicsModel, false)
	SIMDATA_DEF("local_controller", m_LocalController, false)
	SIMDATA_DEF("remote_controller", m_RemoteController, false)
	SIMDATA_DEF("stores_management_system", m_StoresManagementSystem, false)
SIMDATA_XML_END


class BindRecorderVisitor: public SystemVisitor {
	DataRecorder *m_Recorder;
public:
	BindRecorderVisitor(DataRecorder *recorder): m_Recorder(recorder) {}
	void apply(System &system) {
		system.bindRecorder(m_Recorder);
		traverse(system);
	}
	void apply(SystemsModel &model) { traverse(model); }
};

void SystemsModel::setDataRecorder(DataRecorder *recorder) {
	if (!recorder) return;
	accept(new BindRecorderVisitor(recorder));
}

simdata::Ref<PhysicsModel> SystemsModel::getPhysicsModel() const {
	return m_PhysicsModel;
}

simdata::Ref<RemoteController> SystemsModel::getRemoteController() const {
	return m_RemoteController;
}

simdata::Ref<LocalController> SystemsModel::getLocalController() const {
	return m_LocalController;
}

simdata::Ref<StoresManagementSystem> SystemsModel::getStoresManagementSystem() const {
	return m_StoresManagementSystem;
}

void SystemsModel::postCreate() {
	System::postCreate();
	if (m_PhysicsModel.valid()) {
		CSP_LOG(OBJECT, DEBUG, "Adding PhysicsModel (" << m_PhysicsModel->getClassName() << ")");
		addChild(m_PhysicsModel.get());
	}
	assert(!(m_RemoteController.valid() && m_LocalController.valid()));
	if (m_RemoteController.valid()) {
		CSP_LOG(OBJECT, DEBUG, "Adding RemoteController (" << m_RemoteController->getClassName() << ")");
		addChild(m_RemoteController.get());
	}
	if (m_LocalController.valid()) {
		CSP_LOG(OBJECT, DEBUG, "Adding LocalController (" << m_LocalController->getClassName() << ")");
		addChild(m_LocalController.get());
	}
	if (m_StoresManagementSystem.valid()) {
		CSP_LOG(OBJECT, DEBUG, "Adding StoresManagementSystem (" << m_StoresManagementSystem->getClassName() << ")");
		addChild(m_StoresManagementSystem.get());
	}
}

void SystemsModel::getInfo(InfoList &info) {
	accept(new InfoVisitor(info));
}

void SystemsModel::init(SystemsModel::Ref) {
}

