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

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <Bus.h>
#include <Log.h>
#include <SynchronousUpdate.h>
#include <InputInterface.h>

#include <SimData/Object.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/Composite.h>


class System;
class SystemsModel;
class DataRecorder;


/// A visitor interface for traversing System graphs 
class SystemVisitor: public simdata::Visitor<SystemVisitor> {
public:
	SIMDATA_VISITOR(SystemVisitor);
	virtual void apply(System &system) { apply((Node&)system); }
	virtual void apply(SystemsModel &model) { apply((System&)model); }
};

typedef simdata::Composite<SystemVisitor> SystemNode;


/** Base class for vehicle system components that can be connected
 *  to a data bus.
 *
 *  Subclasses should implement registerChannels() and importChannels().  
 *  The former is used to expose internal data that can be accessed by 
 *  other systems over the bus, and is called automatically by the
 *  bus when the system is first attached.  The importChannels() method 
 *  is called once by the bus after all systems have been attached and
 *  registered.  Use this method to bind to data channels provided 
 *  by other systems over the bus.
 */
class System: public simdata::Object, public SystemNode, public UpdateTarget, public InputInterface {
friend class SystemsModel;

public:
	/// System reference (convenience) type.
	typedef simdata::Ref<System> Ref;
	typedef std::vector<std::string> InfoList;

private:

	simdata::Link<System>::vector m_Subsystems;

	SystemsModel *m_Model;

	class SetModelVisitor: public SystemVisitor {
		SystemsModel *m_Model;
	public:
		SetModelVisitor(SystemsModel *model): m_Model(model) {}
		virtual void apply(System &s) { 
			s.setModel(m_Model); 
			traverse(s); 
		}
	};

	virtual void setModel(SystemsModel *model);

	void _registerUpdate(UpdateMaster *master) {
		UpdateTarget::registerUpdate(master);
	}

protected:

	/** Register channels for access to internal data by other 
	 *  systems on the bus.
	 */
	virtual void registerChannels(Bus*)=0;

	class BindVisitor: public SystemVisitor {
		Bus* m_Bus;
	public:
		BindVisitor(Bus *bus): m_Bus(bus) {}
		void apply(System &s) {
			s.importChannels(m_Bus);
			traverse(s);
		}
	};

	class UpdateMasterVisitor: public SystemVisitor {
		UpdateMaster* m_Master;
	public:
		UpdateMasterVisitor(UpdateMaster* master): m_Master(master) {}
		void apply(System &s) {
			s._registerUpdate(m_Master);
			traverse(s);
		}
	};

	/** Import channels provided by other systems on the bus.
	 * 
	 *  Call bus->getChannel() and bus->getSharedChannel() to
	 *  obtain references to data channels available on the
	 *  bus.  These references should generally be stored as
	 *  system member variables for later use.
	 */
	virtual void importChannels(Bus*)=0;


	// XXX use raw pointers here to prevent circular references
	// that keep the model alive.  the model pointer is only
	// for internal use by the system instance, which will be
	// destroyed when the model goes out of scope.
	SystemsModel* getModel() const { return m_Model; }

	/** Add and register all subsystems.
	 */
	void postCreate() {
		CSP_LOG(OBJECT, DEBUG, "System::postCreate() " << getClassName() << ", adding " << m_Subsystems.size() << " subsystems.");
		simdata::Link<System>::vector::iterator iter = m_Subsystems.begin();
		for (; iter != m_Subsystems.end(); ++iter) {
			CSP_LOG(OBJECT, DEBUG, "System::addChild() " << (*iter)->getClassName());
			addChild(iter->get());
		}
		m_Subsystems.clear();
	}

public:

	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(System)
		SIMDATA_XML("subsystems", System::m_Subsystems, false)
	END_SIMDATA_XML_INTERFACE

	/** Cannot be a child of multiple parents.
	 */
	bool canBeAdded() const { return getNumParents() == 0; }

	/** Constructor.
	 */
	System(): m_Model(0) {
	}

	/** Get the system identifier string (name).
	 */
	virtual std::string getName() const { return ""; }

	virtual bool addChild(SystemNode *node) {
		if (!SystemNode::addChild(node)) {
			CSP_LOG(OBJECT, ERROR, "SystemNode::addChild() failed.");
			return false;
		}
		node->accept(new SetModelVisitor(getModel()));
		return true;
	}

	System *addSystem(System *system, bool required = true) {
		if (addChild(system)) return system;
		assert(!required);
		return 0;
	}

	void registerUpdate(UpdateMaster *master) {
		accept(new UpdateMasterVisitor(master));
	}

	virtual void getInfo(InfoList &info) const {}

	virtual void bindRecorder(DataRecorder *) const {}


#if 0	//XXX XXX
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
#endif

	SIMDATA_VISITABLE(SystemVisitor);
};


#endif // __SYSTEM_H__

