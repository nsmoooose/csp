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
 * @file SystemsModel.h
 *
 * Vehicle systems base classes and infrastructure.
 *
 **/

#ifndef __CSPSIM_SYSTEMSMODEL_H__
#define __CSPSIM_SYSTEMSMODEL_H__

#include <csp/cspsim/System.h>
#include <csp/cspsim/PhysicsModel.h>
#include <csp/cspsim/Controller.h>

#include <csp/lib/util/Namespace.h>


CSP_NAMESPACE

class DataRecorder;
class StoresManagementSystem;


/** Base class for detailed vehicle models.
 *
 *  Systems model classes serve as the root node of System trees.  The
 *  model defines a data bus shared by all systems it contains,
 *  and serves as the external interface of the composite system.
 */
class SystemsModel: public System {

	Ref<Bus> m_Bus;

	bool m_Bound;

	virtual void setModel(SystemsModel *) {}

	class FindSystemByNameVisitor: public FindVisitor<System,SystemVisitor> {
		std::string m_Name;
	public:
		FindSystemByNameVisitor(std::string const &name):
			FindVisitor<System,SystemVisitor>(), m_Name(name) {}
		virtual bool match(System &s) { return s.getName() == m_Name; }
	};

	class EventVisitor: public SystemVisitor {
		bool m_handled;
		MapEvent m_event;
	public:
		EventVisitor(MapEvent const &event): m_handled(false), m_event(event) {}
		bool handled() const { return m_handled; }
		void apply(System &system) {
			if (m_handled) return;
			m_handled = system.onMapEvent(m_event);
			if (!m_handled) traverse(system);
		}
		void apply(SystemsModel &model) { traverse(model); }
	};

	class InfoVisitor: public SystemVisitor {
		InfoList &m_info;
	public:
		InfoVisitor(InfoList &info): m_info(info) {}
		void apply(System &system) { system.getInfo(m_info); traverse(system); }
		void apply(SystemsModel &model) { traverse(model); }
	};

	// A few subsystems bind tightly to the parent DynamicObject instance, so it
	// is convenient to declare the separately.  If defined these systems will be
	// added to the System tree when the SystemsModel is first created, but are
	// also directly available via accessor methods.
	Link<PhysicsModel> m_PhysicsModel;
	Link<LocalController> m_LocalController;
	Link<RemoteController> m_RemoteController;
	Link<StoresManagementSystem> m_StoresManagementSystem;

protected:
	virtual ~SystemsModel() { }

public:

	CSP_DECLARE_OBJECT(SystemsModel)

	/** Initialize from an existing model.
	 *
	 *  This method is called when a new model is bound to a vehicle,
	 *  allowing relevant state data from the old model to be preserved.
	 *
	 *  @param other The previous model (may be NULL).
	 */
	virtual void init(Ref<SystemsModel> other);

	bool canBeAdded() const { return false; }

	/*  why?  remove if not needed.  2005-06-05
	template<typename T>
	void publish(std::string const &name, T const &value) {
		assert(!m_Bound);
		m_Bus->registerLocalDataChannel(name, value);
	}
	*/

	Ref<Bus> getBus() const {
		assert(m_Bus.valid());
		return m_Bus;
	}

	virtual void postCreate();

	Ref<PhysicsModel> getPhysicsModel() const;
	Ref<LocalController> getLocalController() const;
	Ref<RemoteController> getRemoteController() const;
	Ref<StoresManagementSystem> getStoresManagementSystem() const;

	void setDataRecorder(DataRecorder*);

	virtual void getInfo(InfoList &info);

	Ref<System> getSystem(std::string const &name, bool required = true) {
		Ref< FindVisitor<System,SystemVisitor> > visitor;
		visitor = accept(new FindSystemByNameVisitor(name));
		Ref<System> found = visitor->getNode();
		if (!found) {
			assert(!required);
			return 0;
		}
		return found;
	}

	void bindSystems() {
		assert(!m_Bound);
		accept(new SetModelVisitor(getModel()));
		Ref<BindVisitor> binder = new BindVisitor(m_Bus.get());
		accept(binder);
		m_Bound = true;
	}

	SystemsModel(): m_Bound(false) {
		m_Model = this;
		m_Bus = new Bus("MODEL_BUS");
	}

	virtual bool onMapEvent(MapEvent const &event) {
		Ref<EventVisitor> visitor = accept(new EventVisitor(event));
		return visitor->handled();
	}

	virtual void registerChannels(Bus *) {}
	virtual void importChannels(Bus *) {}

	CSP_VISITABLE(SystemVisitor);
};


CSP_NAMESPACE_END

#endif // __CSPSIM_SYSTEMSMODEL_H__

