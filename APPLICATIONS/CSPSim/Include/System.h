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
 * @file System.h
 *
 * Vehicle systems base classes and infrastructure.
 *
 **/


#include <Bus.h>
#include <Updater.h>
#include <InputInterface.h>
#include <SimData/Object.h>
#include <SimData/Composite.h>


class System;
class Model;


/// A visitor interface for traversing System graphs 
class SystemVisitor: public simdata::Visitor<SystemVisitor> {
public:
	SIMDATA_VISITOR(SystemVisitor);
	virtual void apply(System &system) { apply((Node&)system); }
	virtual void apply(Model &model) { apply((System&)model); }
};

typedef simdata::Composite<SystemVisitor> SystemNode;


/** Base class for vehicle system components that can be connected
 *  to a data bus.
 *
 *  Subclasses should implement registerChannels() and bind().  The
 *  former is used to expose internal data that can be accessed by 
 *  other systems over the bus, and is called automatically by the
 *  bus when the system is first attached.  The bind() method is
 *  called once by the bus after all systems have been attached and
 *  registered.  Use this method to bind to data channels provided 
 *  by other systems over the bus.
 */
class System: public simdata::Object, public SystemNode, public Updater, public InputInterface {
friend class Model;

public:

	/// System reference (convenience) type.
	typedef simdata::Ref<System> Ref;

private:

	Model *m_Model;

	std::string m_Name;

	class SetModelVisitor: public SystemVisitor {
		Model *m_Model;
	public:
		SetModelVisitor(Model *model): m_Model(model) {}
		virtual void apply(System &s) { 
			s.setModel(m_Model); 
			traverse(s); 
		}
	};

	virtual void setModel(Model *model);


protected:

	/** Register channels for access to internal data by other 
	 *  systems on the bus.
	 *
	 *  Use bus->getName() to select which channels to register 
	 *  if the system may be attached to more than one bus. 
	 */
	virtual void registerChannels(Bus*) {}

	class BindVisitor: public SystemVisitor {
		Bus* m_Bus;
	public:
		BindVisitor(Bus *bus): m_Bus(bus) {}
		void apply(System &s) {
			s.bind(m_Bus);
			traverse(s);
		}
	};

	/** Bind to channels provided by other systems on the bus.
	 * 
	 *  Call bus->getChannel() and bus->getSharedChannel() to
	 *  obtain references to data channels available on the
	 *  bus.  These references should generally be stored as
	 *  system member variables for later use.
	 */
	virtual void bind(Bus*) {}


	// XXX use raw pointers here to prevent circular references
	// that keep the model alive.  the model pointer is only
	// for internal use by the system instance, which will be
	// destroyed when the model goes out of scope.
	Model* System::getModel() const { return m_Model; }

public:
	bool canBeAdded() const { return getNumParents() == 0; }

	/** Constructor.
	 *
	 *  @param name The identifier string of this system.
	 */
	System(std::string const &name): m_Name(name) {}

	/** Get the system identifier string (name).
	 */
	std::string const &getName() const { return m_Name; }

	virtual bool addChild(SystemNode *node) {
		if (!SystemNode::addChild(node)) return false;
		node->accept(new SetModelVisitor(getModel()));
		return true;
	}

	System *addSystem(System *system, bool required = true) {
		if (addChild(system)) return system;
		assert(!required);
		return 0;
	}

private:
	class UpdateChildrenVisitor: public SystemVisitor {
		double m_dt;
	public:
		UpdateChildrenVisitor(double dt): m_dt(dt) {}
		void apply(System &child) { child.update(m_dt); }
	};

public:
	/** Generate interval timer signals.
	 */
	virtual double onUpdate(double dt) {
		descend(new UpdateChildrenVisitor(dt));
		return 0.0;
	}

	SIMDATA_VISITABLE(SystemVisitor);
};



/** Base class for detailed vehicle models.
 *
 *  Model classes serve as the root node of System trees.  The
 *  model defines a data bus shared by all systems it contains,
 *  and serves as the external interface of the composite system.
 */
class Model: public System {

	Bus::Ref m_Bus;

	virtual void setModel(Model *model) {}

	class FindSystemByNameVisitor: public simdata::FindVisitor<System,SystemVisitor> {
		std::string m_Name;
	public:
		FindSystemByNameVisitor(std::string const &name): 
			simdata::FindVisitor<System,SystemVisitor>(), m_Name(name) {}
		virtual bool match(System &s) { return s.getName() == m_Name; }
	};

protected:
	virtual ~Model() {
	}

public:

	bool canBeAdded() const { return false; }

	Bus::Ref getBus() const { 
		assert(m_Bus.valid());
		return m_Bus; 
	}

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
		accept(new BindVisitor(m_Bus.get()));
	}

	Model(std::string const &name): System(name) {
		m_Model = this;
		m_Bus = new Bus("MODEL_BUS");
	}

	SIMDATA_VISITABLE(SystemVisitor);
};


void System::setModel(Model *model) {
	assert(m_Model == 0 || model == 0);
	m_Model = model;
	if (model != 0) {
		Bus::Ref bus = model->getBus();
		registerChannels(bus.get());
	}
}



// interface event visitors

class EventVisitor: public SystemVisitor {
	bool m_handled;
public:
	typedef simdata::Ref<EventVisitor> Ref;
	EventVisitor(): m_handled(false) {}
	bool handled() const { return m_handled; }
	void apply(System &system) {
		if (m_handled) return;
		m_handled = check(system);
		if (!m_handled) traverse(system);
	}
protected:
	virtual bool check(System &system) = 0;
};


class OnCommandVisitor: public EventVisitor {
	std::string const &m_id;
	int m_x, m_y;
public:
	OnCommandVisitor(std::string const &id, int x, int y):
		EventVisitor(), m_id(id), m_x(x), m_y(y) {}
protected:
	bool check(System &system) {
		return system.onCommand(m_id, m_x, m_y); 
	}
};


class OnAxisVisitor: public EventVisitor {
	std::string const &m_id;
	double m_value;
public:
	OnAxisVisitor(std::string const &id, double value):
		EventVisitor(), m_id(id), m_value(value) {}
protected:
	bool check(System &system) {
		return system.onAxis(m_id, m_value);
	}
};


class OnMotionVisitor: public EventVisitor {
	std::string const &m_id;
	int m_x, m_y, m_dx, m_dy;
public:
	OnMotionVisitor(std::string const &id, int x, int y, int dx, int dy):
		EventVisitor(), m_id(id), m_x(x), m_y(y), m_dx(dx), m_dy(dy) {}
protected:
	virtual bool check(System &system) {
		return system.onMotion(m_id, m_x, m_y, m_dx, m_dy); 
	}
};

