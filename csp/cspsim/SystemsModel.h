// Combat Simulator Project
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

#include <csp/csplib/util/Namespace.h>


CSP_NAMESPACE

class DataRecorder;
class SoundModel;
class StoresManagementSystem;


/** Base class for detailed vehicle models.
 *
 *  Systems model classes serve as the root node of System trees.  The
 *  model defines a data bus shared by all systems it contains,
 *  and serves as the external interface of the composite system.
 */
class SystemsModel: public System {
	class BindRecorderVisitor;
	class EventVisitor;
	class FindSystemByNameVisitor;
	class InfoVisitor;

public:
	CSP_DECLARE_OBJECT(SystemsModel)
	CSP_VISITABLE(SystemVisitor);

	SystemsModel();

	/** Initialize from an existing model.
	 *
	 *  This method is called when a new model is bound to a vehicle,
	 *  allowing relevant state data from the old model to be preserved.
	 *
	 *  @param other The previous model (may be NULL).
	 */
	virtual void init(Ref<SystemsModel> other);

	bool canBeAdded() const { return false; }

	Bus* getBus() const {
		assert(m_Bus.valid());
		return m_Bus.get();
	}

	SoundModel *getSoundModel() const {
		assert(m_SoundModel.valid());
		return m_SoundModel.get();
	}

	Ref<PhysicsModel> getPhysicsModel() const;
	Ref<LocalController> getLocalController() const;
	Ref<RemoteController> getRemoteController() const;
	Ref<StoresManagementSystem> getStoresManagementSystem() const;

	void setDataRecorder(DataRecorder*);

	virtual void getInfo(InfoList &info);

	Ref<System> getSystem(std::string const &name, bool required=true);

	void bindSystems();

	virtual bool onMapEvent(MapEvent const &event);
	virtual void registerChannels(Bus *);
	virtual void importChannels(Bus *);

	void setInternalView(bool internal_view);
	void attachSceneModel(SceneModel *model);
	void detachSceneModel(SceneModel *model);

protected:
	virtual ~SystemsModel();
	virtual void postCreate();

private:
	virtual void setModel(SystemsModel *);

	void importChannelsProxy(System &system);
	void registerChannelsProxy(System &system);

	// A few subsystems bind tightly to the parent DynamicObject instance, so it
	// is convenient to declare the separately.  If defined these systems will be
	// added to the System tree when the SystemsModel is first created, but are
	// also directly available via accessor methods.
	Link<PhysicsModel> m_PhysicsModel;
	Link<LocalController> m_LocalController;
	Link<RemoteController> m_RemoteController;

	// Other subsystems are owned by the parent object and shared by all systems
	// models.  The parent object calls setFoo(...) to assign the shared resource,
	// which other subsystems can access via the corresponding getFoo(...) methods.
	// TODO move sms to DynamicObject.
	Link<StoresManagementSystem> m_StoresManagementSystem;

	Ref<SoundModel> m_SoundModel;
	Ref<Bus> m_Bus;

	bool m_Bound;
};


CSP_NAMESPACE_END

#endif // __CSPSIM_SYSTEMSMODEL_H__

