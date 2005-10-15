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
 * @file StoresManagementSystem.cpp
 *
 **/


#include <Stores/Projectile.h>
#include <Stores/StoresManagementSystem.h>
#include <Stores/Stores.h>
#include <Stores/StoresDatabase.h>
#include <Stores/StoresDefinition.h>
#include <Stores/StoresDynamics.h>
#include <CSPSim.h>  // XXX
#include <SystemsModel.h>  // XXX
#include <SimCore/Battlefield/LocalBattlefield.h>
#include <DynamicModel.h>
#include <DynamicObject.h>
#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>
#include <SimData/ObjectInterface.h>
#include <SimData/Timing.h>
#include <sigc++/functors/mem_fun.h>
#include <osg/Group>
#include <cassert>


SIMDATA_XML_BEGIN(StoresManagementSystem)
	SIMDATA_DEF("stores_definition", m_StoresDefinition, true)
SIMDATA_XML_END


StoresManagementSystem::StoresManagementSystem(): m_DirtyDynamics(true) { }
StoresManagementSystem::~StoresManagementSystem() { }


void StoresManagementSystem::registerChannels(Bus*) {}

void StoresManagementSystem::importChannels(Bus* bus) {
	DataChannel<DynamicModel*>::CRef channel = bus->getChannel("DynamicModel");
	if (channel.valid() && channel->value()) {
		channel->value()->addCreateSceneModelHandler(sigc::mem_fun(this, &StoresManagementSystem::onCreateSceneModel));
		channel->value()->addDeleteSceneModelHandler(sigc::mem_fun(this, &StoresManagementSystem::onDeleteSceneModel));
	}
}

void StoresManagementSystem::onCreateSceneModel(osg::Group *group) {
	std::cout << "onCreateSceneModel\n";
	assert(group);
	m_SceneGroup = group;
	for (unsigned i =  0; i < m_Hardpoints.size(); ++i) {
		assert(m_Hardpoints[i].valid());
		m_Hardpoints[i]->addModel(group);
	}
}

void StoresManagementSystem::onDeleteSceneModel() {
	m_SceneGroup = 0;
}

void StoresManagementSystem::postCreate() {
	simdata::Object::postCreate();
	if (m_StoresDefinition.valid()) {
		const unsigned n = m_StoresDefinition->getNumHardpoints();
		m_Hardpoints.reserve(n);
		for (unsigned i = 0; i < n; ++i) {
			HardpointData const *hp = m_StoresDefinition->getHardpoint(i); assert(hp);
			m_Hardpoints.push_back(hp->createHardpoint(i));
		}
	}
	addTestLoadout();
}

void StoresManagementSystem::getAllStores(StoreIndex const &index, std::vector<Store::Ref> &stores) const {
	assert(index.isHardpoint());
	Store *store = index.valid() ? m_Hardpoints.at(index.hardpoint())->getStore() : 0;
	if (store) {
		stores.push_back(store);
		if(store->asRack()) store->asRack()->getChildren(stores, true);
	}
}

StoreIndex StoresManagementSystem::getHardpointByName(std::string const &name) const {
	unsigned index = 0;
	return (m_StoresDefinition->getHardpointIndex(name, index)) ? StoreIndex::Hardpoint(index) : StoreIndex::Invalid;
}

Store *StoresManagementSystem::getStore(StoreIndex const &index) const {
	return index.valid() ? m_Hardpoints.at(index.hardpoint())->getStore(index) : 0;
}

bool StoresManagementSystem::mountStore(StoreIndex const &index, Store *store) {
	assert(store && index.valid());
	if (index.isHardpoint()) {
		return m_Hardpoints.at(index.hardpoint())->mountStore(store);
	}
	Hardpoint *hp = m_Hardpoints.at(index.hardpoint()).get();
	assert(hp);
	if (!hp->data()->isCompatible(store->key())) {
		CSP_LOG(OBJECT, INFO, "store " << store->name() << " incompatible with hardpoint " << hp->name());
		return false;
	}
	Store *parent = hp->getStore(index.parent());
	if (!parent || !parent->asRack()) {
		CSP_LOG(OBJECT, INFO, "cannot mount store " << store->name() << "; mount point not found on hardpoint " << hp->name());
		return false;
	}
	return parent->asRack()->setChild(index, store);
}


// TODO do callers need immediate feedback that the store was or is about to be
// successfully released?  this would require a bit more work up front, but is
// not too difficult (Hardpoint would need a canRelease(index) method).
void StoresManagementSystem::releaseStore(StoreIndex const &index) {
	assert(index.valid());
	m_MarkedForRelease.insert(index);
}


bool StoresManagementSystem::hasStoresToRelease() const {
	return !m_MarkedForRelease.empty();
}

void StoresManagementSystem::releaseMarkedStores(DynamicObject *parent) {
	assert(parent);
	bool released = false;
	for (std::set<StoreIndex>::const_iterator iter = m_MarkedForRelease.begin(); iter != m_MarkedForRelease.end(); ++iter) {
		StoreIndex const &index = *iter;
		CSP_LOG(OBJECT, INFO, "Removing store @ " << index);

		Hardpoint *hp = m_Hardpoints.at(index.hardpoint()).get();
		assert(hp);

		Store::Ref store;
		simdata::Vector3 store_position;
		simdata::Quat store_attitude;
		simdata::Vector3 ejection_velocity;
		simdata::Vector3 ejection_angular_velocity;

		if (!hp->removeStore(index, store, store_position, store_attitude, ejection_velocity, ejection_angular_velocity)) {
			continue;
		}

		assert(store.valid());
		released = true;

		CSP_LOG(OBJECT, INFO, "Creating dynamic object for released store");

		// open questions / problems:
		//  - for racks, need to include child models and dynamics
		//    (for now assume that we have a single store, as opposed to a rack of stores)
		simdata::Ref<Projectile> object = store->data()->createObject();
		if (object.valid()) {
			store_attitude = parent->getAttitude() * store_attitude;
			ejection_velocity = store_attitude.rotate(ejection_velocity);
			ejection_angular_velocity = store_attitude.rotate(ejection_angular_velocity);

			object->prepareRelease(parent, store);

			object->setAttitude(store_attitude);
			object->setGlobalPosition(parent->getGlobalPosition() + parent->getAttitude().rotate(store_position));
			object->setVelocity(parent->getVelocity() + (parent->getAngularVelocity() ^ parent->getAttitude().rotate(store_position)) + ejection_velocity);
			object->setAngularVelocity(parent->getAngularVelocity() + ejection_angular_velocity);

			CSPSim::theSim->getBattlefield()->__test__addLocalHumanUnit(object);  // XXX TOTAL HACK FOR NOW!!!
		}
	}

	m_MarkedForRelease.clear();

	if (released) {
		setDirtyDynamics();
		signalConfiguration();
	}

}


#if 0
bool StoresManagementSystem::DEPRECATED_releaseStore(StoreIndex const &index) {
	assert(index.valid());
	Hardpoint *hp = m_Hardpoints.at(index.hardpoint()).get();
	assert(hp);

	Store::Ref store;
	simdata::Vector3 position;
	simdata::Quat attitude;
	CSP_LOG(OBJECT, INFO, "Removing store @ " << index);
	if (!hp->removeStore(index, store, position, attitude)) return false;
	assert(store.valid());

	CSP_LOG(OBJECT, INFO, "Removing store from vehicle scene graph");
	osg::Group *group = store->getParentGroup();
	if (group) {
		assert(group->getNumParents() == 1);
		group->getParent(0)->asGroup()->removeChild(group);
	}

	CSP_LOG(OBJECT, INFO, "Creating dynamic object for released store");
	// TODO constructing and return a wrapper object
	// open questions / problems:
	//  - for racks, need to include child models and dynamics
	//    (for now assume that we have a single store, as opposed to a rack of stores)
	//  - StoreData subclasses should take care of initializing the internal state of the object
	//  - need global position, although that could be set by the parent object
	//  - the object model is specified in two locations (store and object), so it could be out of sync.
	simdata::Ref<DynamicObject> object = store->data()->createObject();
	if (object.valid()) {
		object->setGlobalPosition(position);
		object->setAttitude(attitude);
		CSP_LOG(OBJECT, INFO, "Created dynamic object, queueing for parent.");
		m_ReleasedObjects.push_back(object);
	}

	setDirtyDynamics();
	signalConfiguration();
	return true;
}
#endif


/*
void StoresManagementSystem::jettisonSelected() {
	// initial strategy for releasing stores:
	// - remove 3d model from stores group
	// - create dynamic object subclass of the appropriate type (FreeFall for now)
	// - initialize the object (need position, velocity, attitude, link to parent)
	// - add the object to the battlefield via CSPSim::theSim
	for (unsigned i = 0; i < m_Hardpoints.size(); ++i) {
		Hardpoint *hp = m_Hardpoints[i].get();
		if (hp && hp->isSelected() && !hp->isFixed()) {
			osg::ref_ptr<osg::Node> node;
			// fixme: need more than just the node; for jettison need at least the
			// mass, inertia, and drag.  for weapon release, need the actual weapon
			// type and configuration.  this is something that the store should know
			// how to create. 
			if (hp->jettison(node)) {
				if (node.valid()) {
					bool removed = m_SceneGroup->removeChild(node.get());
					assert(removed);
					node->setNodeMask(0);
				}
				setDirtyDynamics();
			}
		}
	}
}
*/


// FIXME the implementation leaves a bit to be desired; it works reasonably well and provides
// a simple interface to a complex selection process, but hopefully the code can be cleaned up
// and simplified a bit.  note also that this method neglects inventory constraints (although
// this probably won't be possible until a campaign system is implemented).
// the caller must call setDirtyDynamics() and signalConfiguration() afterward (multiple
// updates should be batched before calling the latter).
bool StoresManagementSystem::loadStores(StoreIndex const &idx, simdata::Key const &store, unsigned count) {
	if (!idx.isHardpoint() || count < 1) return false;
	Hardpoint *hp = m_Hardpoints.at(idx.hardpoint()).get(); assert(hp);
	HardpointData const *data = hp->data();

	// before we do a lot of work, make sure the hardpoint supports this store.
	if (!data->isCompatible(store)) return false;

	StoresDatabase &db = StoresDatabase::getInstance();

	// existing pylon?
	//   - yes: existing, compatible rack?
	//          - yes: mount on rack
	//          - no : can mount directly on pylon?
	//                 -yes: mount on pylon
	//                 -no : add prefered rack, mount on rack
	//   - no: add compatble pylon
	//   - repeat: as above

	StoreData const *store_data = db.getStoreData(store);
	if (!store_data) return false;

	bool add_pylon = false;
	Rack::Ref pylon;
	Store *existing_store = hp->getStore();
	if (existing_store) {
		pylon = existing_store->asRack();
		if (!pylon) return false;
	}

	// if no existing pylon, we are fairly unconstrained (and hence have to do the most work).
	if (!pylon) {
		// simplest case: a single store can be mounted directly on the hardpoint.
		if (data->isMountCompatible(store) && count == 1) {
			Store::Ref instance = db.getStore(store);
			return instance.valid() && mountStore(idx, instance.get());
		}

		// otherwise we'll need a pylon, so find the best.
		StoreSet mountable;
		data->getMountCompatibleStores(mountable);

		// get all possible racks and pylons.
		StoreSet const *tmp = db.getCompatibleRacks(store);
		assert(tmp);
		StoreSet racks;  // racks of racks
		db.getCompatibleRacks(*tmp, racks);
		racks.insert(tmp->begin(), tmp->end());

		// find the best rack/stack to use as a pylon
		std::vector<simdata::Key> const &pylons = data->preferredRacks();
		for (unsigned i = 0; i < pylons.size(); ++i) {
			if (racks.count(pylons[i]) > 0) {
				Store::Ref s = db.getStore(pylons[i]);
				pylon = !s ? 0 : s->asRack();
				break;
			}
		}

		// if there isn't a preferred pylon, choose the first one that is compatible.
		if (!pylon) {
			for (StoreSet::const_iterator iter = racks.begin(); iter != racks.end(); ++iter) {
				if (data->isMountCompatible(*iter)) {
					Store::Ref s = db.getStore(*iter);
					pylon = !s ? 0 : s->asRack();
					break;
				}
			}
			if (!pylon) return false;
		}

		add_pylon = true;
	}

	Rack::Ref rack = pylon;

	// now we have a mounted pylon that we know will work!  if possible, mount the
	// store(s) directly on the pylon.  otherwise we need to use existing racks or
	// add our own.
	unsigned pylon_capacity = pylon->availableSpace(store);
	if (pylon_capacity < count) {
		// first check for existing capacity.
		unsigned capacity = 0;
		for (unsigned i = 0; i < pylon->getNumChildren(); ++i) {
			Store *child = pylon->getChild(i);
			if (child && child->asRack()) {
				capacity += child->asRack()->availableSpace(store);
				if (capacity >= count) break;
			}
		}

		// if not enough space, try to add racks
		if (capacity < count) {
			// first find suitable racks
			StoreSet const *racks = db.getCompatibleRacks(store);
			StoreSet const *subracks = db.getCompatibleStores(pylon->key());
			StoreSet suitable = (*racks & *subracks);
			// add one or more racks of the same type
			for (StoreSet::const_iterator iter = suitable.begin(); iter != suitable.end(); ++iter) {
				unsigned rack_count = pylon->availableSpace(*iter);
				if (rack_count) {
					unsigned stations = db.getStoreData(*iter)->asRackData()->capacity(store);
					if (capacity + stations * rack_count >= count) {
						if (add_pylon) mountStore(idx, pylon.get());
						unsigned add = (count - capacity) / stations;
						unsigned added = pylon->mountStores(db.getStoreData(*iter), add);
						assert(added == add);
						capacity += added * stations;
						break;
					}
				}
			}
			if (capacity < count) return false;
		} else {
			if (add_pylon) mountStore(idx, pylon.get());
		}

		// now fill up the racks
		for (unsigned i = 0; i < pylon->getNumChildren(); ++i) {
			Store *rack = pylon->getChild(i);
			if (!rack || !rack->asRack()) continue;
			count -= rack->asRack()->mountStores(store_data, count);
			if (count == 0) break;
		}
		assert(count == 0);
	} else {
		// mount all stores on the first available stations.
		if (add_pylon) mountStore(idx, pylon.get());
		unsigned added = pylon->mountStores(store_data, count);
		assert(count == added);
	}

	return true;
}

void StoresManagementSystem::addTestLoadout() {
	StoreData const *store;
	StoresDatabase &db = StoresDatabase::getInstance();

	store = db.getStoreData("tanks.f16.centerline_300gal");
	if (store) loadStores(getHardpointByName("HP5"), store->key(), 1);

	store = db.getStoreData("tanks.f16.wing_370gal");
	if (store) loadStores(getHardpointByName("HP4"), store->key(), 1);
	if (store) loadStores(getHardpointByName("HP6"), store->key(), 1);

	store = db.getStoreData("missiles.aim9");
	if (store) loadStores(getHardpointByName("HP1"), store->key(), 1);
	if (store) loadStores(getHardpointByName("HP9"), store->key(), 1);

	setDirtyDynamics();
	signalConfiguration();
}

void StoresManagementSystem::getDynamics(StoresDynamics &dynamics) {
	dynamics.reset();
	std::cout << "DYNAMICS MASS A" << dynamics.getMass() << "\n";
	for (unsigned i =  0; i < m_Hardpoints.size(); ++i) {
		assert(m_Hardpoints[i].valid());
		m_Hardpoints[i]->sumDynamics(dynamics);
	}
	std::cout << "DYNAMICS MASS B" << dynamics.getMass() << "\n";
	dynamics.compute();
}

void StoresManagementSystem::getReleasedObjects(std::vector<simdata::Ref<DynamicObject> > &objects) {
	objects.swap(m_ReleasedObjects);
	m_ReleasedObjects.clear();
}

double StoresManagementSystem::onUpdate(double /*dt*/) {
	return -1;
}

