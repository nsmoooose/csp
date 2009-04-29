// Combat Simulator Project
// Copyright (C) 2003, 2005 The Combat Simulator Project
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


#include <csp/cspsim/SystemsModel.h>
#include <csp/cspsim/DataRecorder.h>
#include <csp/cspsim/sound/SoundModel.h>
#include <csp/cspsim/stores/StoresManagementSystem.h>

#include <csp/csplib/data/ObjectInterface.h>


namespace csp {

CSP_XML_BEGIN(SystemsModel)
	CSP_DEF("physics_model", m_PhysicsModel, false)
	CSP_DEF("local_controller", m_LocalController, false)
	CSP_DEF("remote_controller", m_RemoteController, false)
	CSP_DEF("stores_management_system", m_StoresManagementSystem, false)
CSP_XML_END

class SystemsModel::FindSystemByNameVisitor: public FindVisitor<System,SystemVisitor> {
	std::string m_Name;
public:
	FindSystemByNameVisitor(std::string const &name):
		FindVisitor<System,SystemVisitor>(), m_Name(name) {}
	virtual bool match(System &s) { return s.getName() == m_Name; }
};

class SystemsModel::EventVisitor: public SystemVisitor {
	bool m_handled;
	input::MapEvent m_event;
public:
	EventVisitor(input::MapEvent const &event): m_handled(false), m_event(event) {}
	bool handled() const { return m_handled; }
	void apply(System &system) {
		m_handled = m_handled || system.onMapEvent(m_event);
		if (!m_handled) traverse(system);
	}
	void apply(SystemsModel &model) { traverse(model); }
};

class SystemsModel::InfoVisitor: public SystemVisitor {
	InfoList &m_info;
public:
	InfoVisitor(InfoList &info): m_info(info) {}
	void apply(System &system) { system.getInfo(m_info); traverse(system); }
	void apply(SystemsModel &model) { traverse(model); }
};


class SystemsModel::BindRecorderVisitor: public SystemVisitor {
	DataRecorder *m_Recorder;
public:
	BindRecorderVisitor(DataRecorder *recorder): m_Recorder(recorder) {}
	void apply(System &system) {
		system.bindRecorder(m_Recorder);
		traverse(system);
	}
	void apply(SystemsModel &model) { traverse(model); }
};

void SystemsModel::importChannelsProxy(System &system) {
	system.importChannels(getBus());
}

void SystemsModel::registerChannelsProxy(System &system) {
	system.registerChannels(getBus());
}

SystemsModel::SystemsModel(): m_Bound(false) {
	m_Model = this;
	m_Bus = new Bus("MODEL_BUS");
	m_SoundModel = new SoundModel;
}

SystemsModel::~SystemsModel() {
}

void SystemsModel::setDataRecorder(DataRecorder *recorder) {
	if (!recorder) return;
	accept(new BindRecorderVisitor(recorder));
}

Ref<PhysicsModel> SystemsModel::getPhysicsModel() const {
	return m_PhysicsModel;
}

Ref<RemoteController> SystemsModel::getRemoteController() const {
	return m_RemoteController;
}

Ref<LocalController> SystemsModel::getLocalController() const {
	return m_LocalController;
}

Ref<StoresManagementSystem> SystemsModel::getStoresManagementSystem() const {
	return m_StoresManagementSystem;
}

void SystemsModel::postCreate() {
	System::postCreate();
	if (m_PhysicsModel.valid()) {
		CSPLOG(DEBUG, OBJECT) << "Adding PhysicsModel (" << m_PhysicsModel->getClassName() << ")";
		addChild(m_PhysicsModel.get());
	}
	assert(!(m_RemoteController.valid() && m_LocalController.valid()));
	if (m_RemoteController.valid()) {
		CSPLOG(DEBUG, OBJECT) << "Adding RemoteController (" << m_RemoteController->getClassName() << ")";
		addChild(m_RemoteController.get());
	}
	if (m_LocalController.valid()) {
		CSPLOG(DEBUG, OBJECT) << "Adding LocalController (" << m_LocalController->getClassName() << ")";
		addChild(m_LocalController.get());
	}
	if (m_StoresManagementSystem.valid()) {
		CSPLOG(DEBUG, OBJECT) << "Adding StoresManagementSystem (" << m_StoresManagementSystem->getClassName() << ")";
		addChild(m_StoresManagementSystem.get());
	}
}

void SystemsModel::getInfo(InfoList &info) {
	accept(new InfoVisitor(info));
}

void SystemsModel::init(Ref<SystemsModel>) {
}

bool SystemsModel::onMapEvent(input::MapEvent const &event) {
	Ref<EventVisitor> visitor = accept(new EventVisitor(event));
	return visitor->handled();
}

void SystemsModel::registerChannels(Bus *) {
}

void SystemsModel::importChannels(Bus *) {
}

void SystemsModel::setModel(SystemsModel *) {
}

Ref<System> SystemsModel::getSystem(std::string const &name, bool required) {
	Ref< FindVisitor<System,SystemVisitor> > visitor;
	visitor = accept(new FindSystemByNameVisitor(name));
	Ref<System> found = visitor->getNode();
	if (!found) {
		assert(!required);
		return 0;
	}
	return found;
}

void SystemsModel::bindSystems() {
	assert(!m_Bound);
	accept(new InitVisitor(getModel()));
	Ref<BindVisitor> binder = new BindVisitor(getBus());
	accept(binder);
	m_Bound = true;
}

// Visitor for triggering internal event callbacks.  These callbacks should
// not be confused with user event callbacks, which are handled by onMapEvent
// and EventVisitor.  This visitor works with callbacks that take a signal
// argument and return null.
template <typename VALUE>
class InternalEventVisitor: public SystemVisitor {
	typedef void (System::*METHOD)(VALUE);
	METHOD m_Method;
	VALUE m_Value;
public:
	InternalEventVisitor(METHOD method, VALUE value): m_Method(method), m_Value(value) { }
	void apply(System &system) {
		(system.*m_Method)(m_Value);
		traverse(system);
	}
};

void SystemsModel::setInternalView(bool internal_view) {
	accept(new InternalEventVisitor<bool>(&System::onInternalView, internal_view));
	m_SoundModel->setInternalView(internal_view);
}

void SystemsModel::attachSceneModel(SceneModel* model) {
	m_SoundModel->bind(model);
	accept(new InternalEventVisitor<SceneModel*>(&System::onAttachSceneModel, model));
}

void SystemsModel::detachSceneModel(SceneModel* model) {
	accept(new InternalEventVisitor<SceneModel*>(&System::onDetachSceneModel, model));
	m_SoundModel->unbind(model);
}

} // namespace csp

