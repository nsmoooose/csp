// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file StoresManagementSystem.h
 *
 **/


#ifndef __CSPSIM_STORESMANAGEMENTSYSTEM_H__
#define __CSPSIM_STORESMANAGEMENTSYSTEM_H__

#include <csp/cspsim/System.h>
#include <csp/csplib/data/Link.h>
#include <sigc++/trackable.h>
#include <set>
#include <vector>
#include <osg/ref_ptr>

namespace osg { class Group; }

CSP_NAMESPACE

class Hardpoint;
class SceneModel;
class StoresDefinition;
class StoresDynamics;
class DynamicObject;
class Store;
class StoreIndex;


// TODO start thinking about multiplayer
// TODO document this class

class StoresManagementSystem: public System, public sigc::trackable {
public:
	CSP_DECLARE_OBJECT(StoresManagementSystem)

	StoresManagementSystem();
	virtual ~StoresManagementSystem();

	StoresDefinition const *getDefinition() const { return m_StoresDefinition.get(); }

	StoreIndex getHardpointByName(std::string const &name) const;
	Store *getStore(StoreIndex const &index) const;
	void getAllStores(StoreIndex const &index, std::vector<Ref<Store> > &stores) const;
	bool mountStore(StoreIndex const &index, Store *store);
	void releaseStore(StoreIndex const &index);

	// Called by DynamicObject
	void releaseMarkedStores(DynamicObject *parent);
	bool hasStoresToRelease() const;

	/** Attempt to load stores onto a hardpoint, adding suitable racks and pylons as needed.
	 */
	bool loadStores(StoreIndex const &idx, Key const &store, unsigned count);

	void clearDirtyDynamics() { m_DirtyDynamics = false; }
	void setDirtyDynamics() { m_DirtyDynamics = true; }
	bool hasDirtyDynamics() const { return m_DirtyDynamics; }
	void getDynamics(StoresDynamics &dynamics);

	void addConfigurationHandler(sigc::slot<void> const &slot) const { m_ConfigurationSignal.connect(slot); }

	bool hasReleasedObjects() const { return !m_ReleasedObjects.empty(); }
	void getReleasedObjects(std::vector<Ref<DynamicObject> > &objects);

	double onUpdate(double dt);

protected:
	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus* bus);

	void onAttachSceneModel(SceneModel *model);
	void onDetachSceneModel(SceneModel *model);

	void signalConfiguration() { m_ConfigurationSignal.emit(); }

	virtual void postCreate();

private:
	void addTestLoadout();

	Link<StoresDefinition> m_StoresDefinition;
	osg::ref_ptr<osg::Group> m_SceneGroup;
	std::set<StoreIndex> m_MarkedForRelease;

	typedef std::vector<Ref<Hardpoint> > Hardpoints;
	Hardpoints m_Hardpoints;
	bool m_DirtyDynamics;

	mutable sigc::signal<void> m_ConfigurationSignal;

	std::vector<Ref<DynamicObject> > m_ReleasedObjects;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_STORESMANAGEMENTSYSTEM_H__

