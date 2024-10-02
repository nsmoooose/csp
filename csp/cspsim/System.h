#pragma once
// Combat Simulator Project
// Copyright 2003, 2004 The Combat Simulator Project
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
 * Detailed models of vehicle behavior are composed of multiple System
 * subclasses.  Each System subclass represents a particular aspect or
 * subcomponent of a vehicle.  Some systems subclasses are direct
 * analogs to real-world vehicle subsystems (e.g. a fire-control radar),
 * while other model general behavior, logic, or interactions with the
 * environment.  For example, the flight model of an aircraft is
 * implemented as a system.
 *
 * Systems act as nodes in a tree, so that each system can have multiple
 * subsystems.  The unique root of a systems tree is a subclass of
 * SystemsModel.  Loosely speaking, the systems model represents the
 * entire collection of subsystems forming the tree.  System models can
 * be attached to DynamicObjects to provide logic, physics, and
 * interfaces appropriate to the specific vehicle being simulated.
 *
 * Actions on systems models are typically implemented using the
 * visitor pattern, whereby an instance of a Visitor subclass is passed
 * to the systems model, and then recursively to all children until
 * each node of the tree has been visited.  At each node, the visitor
 * selectively performs its intended operation.
 *
 * In addition to visitors, which provide external access to the systems
 * model, individual subsystems can communicate directly using the
 * system bus.  The bus is a class that acts as a broker for data channels
 * defined by the subsystems.  When the system model is first constructed,
 * each subsystem exports a set of data channels to the bus.  Once this
 * process is complete, the systems may request to import channels from
 * other subsystems.  The coordination of importers and exporters is
 * handled entirely by the bus.  Other than its immediate subsystems, a
 * given system has no direct knowledge of the other systems in the
 * systems model.
 *
 * The data channels that are shared by the subsystems are identified by
 * a string identifier.  This one system may export "aileron_deflection"
 * to the bus, and another may request "aileron_deflection" from the bus.
 * Once the connection is established, each subsystem holds a reference
 * to the shared datachannel object that allows for efficient communication
 * without additional string lookups.  Each data channel is strongly typed,
 * and provides various signaling methods including both active and passive
 * push and pull.
 *
 * The systems model and subsystems conform to the InputInterface interface,
 * allowing input events to propagate to the subsystems that handle them.
 * Thus a keyboard event that maps to the "radar_lock" action would be
 * passed to the active object, down to the systems model, and through the
 * systems tree until the radar subsytem finally handled the event.
 *
 * Subsystems support synchronous, delayed updates.  Each subsystem can
 * implement an onUpdate(dt) method to perform updates.  The return value
 * indicates the minimum amount of time until the next update.  Systems
 * requiring per frame updates (e.g. a flight model) would return 0.0,
 * while less demanding systems can request less frequent updates.
 *
 **/

#include <csp/cspsim/Bus.h>  /** @todo header file csp/cspsim/Bus.h could be forward declared, but most Bus users will need it in the header anyway. */
#include <csp/cspsim/input/InputInterface.h>
#include <csp/cspsim/sound/SoundEffect.h>  // for SoundEffect::Mode

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Composite.h>
#include <csp/csplib/util/SynchronousUpdate.h>

#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Link.h>

namespace csp {


class DataRecorder;
class ResourceBundle;
class SceneModel;
class SoundSample;
class System;
class SystemsModel;


/** A visitor interface for traversing System trees.
 */
class SystemVisitor: public Visitor<SystemVisitor> {
public:
	CSP_VISITOR(SystemVisitor);
	/**
	 * System trees contain only two types of nodes: System and SystemsModel.
	 * The default visitor implementation treats both of these as generic
	 * Nodes, but specialized visitor subclasses can act independently on the
	 * different types.
	 */
	virtual void apply(System &system) { apply((Node&)system); }
	virtual void apply(SystemsModel &model) { apply((System&)model); }
};

/** SystemNodes are Composite nodes that accept SystemVisitors. */
typedef Composite<SystemVisitor> SystemNode;


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
class CSPSIM_EXPORT System: public Object, public SystemNode, public UpdateTarget, public input::InputInterface {
friend class SystemsModel;

public:
	/// System reference (convenience) type.
	typedef Ref<System> RefT;

	/// Strings used for onscreen data display.  See getInfo().
	typedef std::vector<std::string> InfoList;

	CSP_DECLARE_ABSTRACT_OBJECT(System)

	/** System nodes accept SystemVisitors */
	CSP_VISITABLE(SystemVisitor);

	System();
	virtual ~System();

	/** True if a system can be added as the child of another system.
	 *
	 *  This is used during node assembly to prevent systems from having
	 *  multiple parents, and to force SystemsModels to be the root of
	 *  the node tree.
	 */
	virtual bool canBeAdded() const { return getNumParents() == 0; }

	/** Get the system identifier string (name).
	 */
	virtual std::string getName() const { return "?"; }

	/** Add another system node as a child of this node.
	 *  @returns true if the node was added successfully.
	 */
	virtual bool addChild(SystemNode *node);

	/** Add another system node as a child of this node.  If required
	 *  is true this method will abort (assert false) on failure.
	 *
	 *  @returns a pointer to the new child on success, or null.
	 */
	System *addSystem(System *system, bool required = true) {
		if (addChild(system)) return system;
		assert(!required);
		return 0;
	}

	/** Connect this node and all children (recursively) to an update
	 *  master.  The update master provides periodic callbacks to the
	 *  onUpdate() method.  See UpdateMaster for details.
	 */
	void registerUpdate(UpdateMaster *master) {
		accept(new UpdateMasterVisitor(master));
	}

	/** Callback used to collect info for on-screen status display.
	 *  Information about the active object to be displayed on the
	 *  screen should be formatted as individual lines and added
	 *  to the InfoList instance.
	 */
	virtual void getInfo(InfoList &) const {}

	/** Connect this system to a DataRecorder instance.  Use this
	 *  method to bind exported data channels to the recorder.  When
	 *  the recorder is active, these channels will be saved to disk
	 *  for later analysis.  See DataRecorder for details.
	 */
	virtual void bindRecorder(DataRecorder *) const {}

protected:

	/** Register channels for access to internal data by other 
	 *  systems on the bus.
	 */
	virtual void registerChannels(Bus*)=0;

	/** A visitor to connect systems to the update master that provides
	 *  periodic update callbacks.  See UpdateMaster for details.
	 */
	class UpdateMasterVisitor: public SystemVisitor {
		UpdateMaster* m_Master;
	public:
		UpdateMasterVisitor(UpdateMaster* master): m_Master(master) {}
		virtual void apply(System &s) {
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

	/** Event callbacks.  These methods are called by the vehicle object class
	 *  in response to various events.  These methods are only called for the
	 *  active system model; inactive models do not receive notification until
	 *  they are reactivated.  The default implementation does nothing.
	 */
	virtual void onInternalView(bool);
	virtual void onAttachSceneModel(SceneModel*);
	virtual void onDetachSceneModel(SceneModel*);

	/** Get the resource bundle for this system, or null.
	 */
	ResourceBundle *getResourceBundle() { return m_ResourceBundle.get(); }

	/** Retrieve a sound sample from the resource bundle.  Returns null if
	 *  the sample is not found.
	 */
	SoundSample const *getSoundSample(std::string const &name) const;

	/** Convenience method to create a new sound effect from a sound sample
	 *  in the system resource bundle.  Returns null if the named sample is
	 *  not found.  Otherwise returns a new object that must be assigned to
	 *  a Ref pointer to avoid memory leaks.
	 */
	SoundEffect *addSoundEffect(std::string const &name, SoundEffect::Mode mode);

	/** Remove a sound effect that was added using addSoundEffect.
	 */
	void removeSoundEffect(Ref<SoundEffect> const &sound);

	/** Convenience method to both remove a sound effect and reset the reference
	 *  to null in one step.
	 */
	void removeAndDeleteSoundEffect(Ref<SoundEffect> &sound);

	/** 
	 * @warning use raw pointers here to prevent circular references
	 * that keep the model alive.  the model pointer is only
	 * for internal use by the system instance, which will be
	 * destroyed when the model goes out of scope.
	 */
	SystemsModel* getModel() const { return m_Model; }

	/** Add and register all subsystems.
	 *
	 *  Called automatically as part of the SimData deserialization
	 *  infrastructure.
	 */
	virtual void postCreate();

private:

	/** External XML variable for specifying and serializing child
	 *  systems.
	 *
	 *  After deserialization, references to theso subsystems are added
	 *  to the node tree.  See postCreate() for details.
	 */
	Link<System>::vector m_Subsystems;

	/** External XML variable specifying resources, such as sound
	 *  samples, that this system may use (optional).
	 */
	Link<ResourceBundle> m_ResourceBundle;

	/** Keep a pointer to the root of the systems tree.  We store a raw
	 *  pointer rather than a Ref<> in order to prevent cyclic references.
	 *  All Ref references are from parents to children.
	 */
	SystemsModel *m_Model;

	/** Called by the SetModelVisitor to bind to the root SystemsModel
	 *  after the full tree has been assembled.
	 */
	virtual void setModel(SystemsModel *model);

	/** Called by the UpdateMasterVisitor to bind to an UpdateMaster and
	 *  receive periodic update callbacks.  You must extend onUpdate() to
	 *  handle these callbacks.  If not extended, the first update event
	 *  will automatically disconnect this system from further updates.
	 */
	void _registerUpdate(UpdateMaster *master) {
		UpdateTarget::registerUpdate(master);
	}

	/** A visitor used to connect systems to existing channels on the bus.
	 *  Systems export channels to the system bus when first added to the
	 *  systems model, but do not import channels until after all systems
	 *  have been added.  This two-phased approach eliminates dependancy
	 *  on the order in which the systems are added to the model.
	 */
	class BindVisitor: public SystemVisitor {
		Bus* m_Bus;
	public:
		BindVisitor(Bus *bus): m_Bus(bus) {}
		virtual void apply(System &s);
	};

	/** Visitor class to inform all subsystems of the root SystemsModel
	 *  instance.  Each subsystem maintains a reference to the root of
	 *  the systems tree.  Subsystems use this callback to register
	 *  channels on the bus and perform other initialization.
	 */
	class InitVisitor: public SystemVisitor {
		SystemsModel *m_Model;
	public:
		InitVisitor(SystemsModel *model): m_Model(model) {}
		virtual void apply(System &s);
	};
};


} // namespace csp
