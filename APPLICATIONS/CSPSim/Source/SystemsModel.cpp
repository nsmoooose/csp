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


SIMDATA_REGISTER_INTERFACE(SystemsModel)

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

simdata::Ref<Controller> SystemsModel::getController() const {
	return m_Controller;
}

void SystemsModel::postCreate() {
	System::postCreate();
	if (m_PhysicsModel.valid()) {
		CSP_LOG(OBJECT, DEBUG, "Adding PhysicsModel (" << m_PhysicsModel->getClassName() << ")");
		addChild(m_PhysicsModel.get());
	}
	if (m_Controller.valid()) {
		CSP_LOG(OBJECT, DEBUG, "Adding Controller (" << m_Controller->getClassName() << ")");
		addChild(m_Controller.get());
	}
}


void SystemsModel::serialize(simdata::Archive &archive) {
	System::serialize(archive);
	archive(m_PhysicsModel);
	// XXX temporarily disabled (no controller classes yet)
	//archive(m_Controller);
}


void SystemsModel::getInfo(InfoList &info) {
	accept(new InfoVisitor(info));
}


