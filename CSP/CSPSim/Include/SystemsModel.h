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

#ifndef __SYSTEMSMODEL_H__
#define __SYSTEMSMODEL_H__

#include <System.h>
#include <PhysicsModel.h>
#include <Controller.h>


class DataRecorder;

/** Base class for detailed vehicle models.
 *
 *  Systems model classes serve as the root node of System trees.  The
 *  model defines a data bus shared by all systems it contains,
 *  and serves as the external interface of the composite system.
 */
class SystemsModel: public System {

	Bus::Ref m_Bus;

	bool m_Bound;

	virtual void setModel(SystemsModel *) {}

	class FindSystemByNameVisitor: public simdata::FindVisitor<System,SystemVisitor> {
		std::string m_Name;
	public:
		FindSystemByNameVisitor(std::string const &name): 
			simdata::FindVisitor<System,SystemVisitor>(), m_Name(name) {}
		virtual bool match(System &s) { return s.getName() == m_Name; }
	};

	class EventVisitor: public SystemVisitor {
		bool m_handled;
		MapEvent m_event;
	public:
		typedef simdata::Ref<EventVisitor> Ref;
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

	simdata::Link<PhysicsModel> m_PhysicsModel;
	simdata::Link<Controller> m_Controller;

protected:
	virtual ~SystemsModel() { }

public:

	typedef simdata::Ref<SystemsModel> Ref;

	SIMDATA_OBJECT(SystemsModel, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(SystemsModel, System)
		SIMDATA_XML("physics_model", SystemsModel::m_PhysicsModel, false)
		//SIMDATA_XML("controller", SystemsModel::m_Controller, false)
	END_SIMDATA_XML_INTERFACE

	/** Initialize from an existing model.
	 *  
	 *  This method is called when a new model is bound to a vehicle,
	 *  allowing relevant state data from the old model to be preserved.
	 *
	 *  @param other The previous model (may be NULL).
	 */
	virtual void init(SystemsModel::Ref other) {}

	bool canBeAdded() const { return false; }

	template<typename T>
	void publish(std::string const &name, T const &value) {
		assert(!m_Bound);
		m_Bus->registerLocalDataChannel(name, value);
	}

	Bus::Ref getBus() const { 
		assert(m_Bus.valid());
		return m_Bus; 
	}

	virtual void postCreate();

	simdata::Ref<PhysicsModel> getPhysicsModel() const;

	simdata::Ref<Controller> getController() const;

	void setDataRecorder(DataRecorder*);

	virtual void getInfo(InfoList &info);

	System::Ref getSystem(std::string const &name, bool required = true) {
		simdata::Ref< simdata::FindVisitor<System,SystemVisitor> > visitor;
		visitor = accept(new FindSystemByNameVisitor(name));
		System::Ref found = visitor->getNode();
		if (!found) {
			assert(!required);
			return 0;
		}
		return found;
	}

	void bindSystems() {
		assert(!m_Bound);
		BindVisitor::Ref binder = new BindVisitor(m_Bus.get());
		accept(binder);
		m_Bound = true;
	}

	SystemsModel(): m_Bound(false) {
		m_Model = this;
		m_Bus = new Bus("MODEL_BUS");
	}

	virtual bool onMapEvent(MapEvent const &event) {
		EventVisitor::Ref visitor = accept(new EventVisitor(event));
		return visitor->handled();
	}

	virtual void registerChannels(Bus *bus) {}
	virtual void importChannels(Bus *bus) {}

	SIMDATA_VISITABLE(SystemVisitor);
};



#endif // __SYSTEMSMODEL_H__

