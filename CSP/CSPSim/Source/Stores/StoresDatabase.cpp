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
 * @file StoresDatabase.cpp
 *
 **/


#include <Stores/StoresDatabase.h>
#include <Stores/Stores.h>
#include <SimData/DataManager.h>

/*  currently we search for all stores at runtime, but it might be better
    to list stores explicitly.  alternatively, we could cache the store
    list to disk and compare the timestamp to the dar file to invalidate
    the cache.

class StoresDirectory: public simdata::Object {
public:
	SIMDATA_DECLARE_OBJECT(StoresDirectory)
	std::vector<simdata::Path> const &paths() const { return m_Paths; }
private:
	std::vector<simdata::Path> m_Paths;
};


SIMDATA_XML_BEGIN(StoresDirectory)
	SIMDATA_DEF("paths", m_Paths, true)
SIMDATA_XML_END

*/


StoresDatabase::StoresDatabase() {
}

StoresDatabase::~StoresDatabase() {
	for (RackMatrix::iterator iter = m_ParentMatrix.begin(); iter != m_ParentMatrix.end(); ++iter) delete iter->second;
	for (RackMatrix::iterator iter = m_ChildMatrix.begin(); iter != m_ChildMatrix.end(); ++iter) delete iter->second;
}

void StoresDatabase::load(simdata::DataManager &data_manager, std::string const &root) {
	_load(data_manager, simdata::ObjectID(root), root);
}

simdata::Ref<Store> StoresDatabase::getStore(std::string const &id) {
	return getStore(simdata::Key(id));
}

simdata::Ref<Store> StoresDatabase::getStore(simdata::Key const &key) {
	StoreData const *data = getStoreData(key);
	return data ? data->createStore() : 0;
}

StoreData const *StoresDatabase::getStoreData(std::string const &id) {
	return getStoreData(simdata::Key(id));
}

StoreData const *StoresDatabase::getStoreData(simdata::Key const &key) {
	StoresByKey::const_iterator iter = m_StoresByKey.find(key);
	return (iter == m_StoresByKey.end()) ? 0 : iter->second.get();
}

void StoresDatabase::getStoreDataSet(std::set<simdata::Key> const &keyset, std::vector<StoreData const *> &data) {
	data.reserve(data.size() + keyset.size());
	for (std::set<simdata::Key>::const_iterator iter = keyset.begin(); iter != keyset.end(); ++iter) {
		StoreData const *store_data = getStoreData(*iter);
		if (store_data) data.push_back(store_data);
	}
}

simdata::Key StoresDatabase::getBestRack(HardpointData const *hp, Store const *store) {
	if (!store) return simdata::Key();
	RackMatrix::const_iterator rack_iter = m_ParentMatrix.find(store->key());
	if (rack_iter == m_ParentMatrix.end()) return simdata::Key();
	StoreSet const *racks = rack_iter->second;
	StoreList const &preferred = hp->preferredRacks();
	for (StoreList::const_iterator iter = preferred.begin(); iter != preferred.end(); ++iter) {
		if (racks->count(*iter) > 0) return *iter;
	}
	assert(!racks->empty());
	return *(racks->begin());
}

StoreSet const *StoresDatabase::getCompatibleRacks(simdata::Key const &key) const {
	RackMatrix::const_iterator iter = m_ParentMatrix.find(key);
	return (iter == m_ParentMatrix.end()) ? 0 : iter->second;
}

StoreSet const *StoresDatabase::getCompatibleStores(simdata::Key const &key) const {
	RackMatrix::const_iterator iter = m_ChildMatrix.find(key);
	return (iter == m_ChildMatrix.end()) ? 0 : iter->second;
}

void StoresDatabase::getCompatibleRacks(StoreSet const &stores, StoreSet &output) const {
	assert(&stores != &output);
	for (StoreSet::const_iterator iter = stores.begin(); iter != stores.end(); ++iter) {
		RackMatrix::const_iterator ri = m_ParentMatrix.find(*iter);
		if (ri != m_ParentMatrix.end()) output.insert(ri->second->begin(), ri->second->end());
	}
}

bool StoresDatabase::_load(simdata::DataManager &data_manager, simdata::ObjectID root, std::string const &prefix) {
	std::vector<simdata::ObjectID> objects = data_manager.getChildren(root);
	for (unsigned i = 0; i < objects.size(); ++i) {
		// objects[i] may refer to a directory, an Object, or both
		_load(data_manager, objects[i], prefix);
		if (data_manager.hasObject(objects[i])) {
			// note that this will effectively preload all object models in the stores directory!
			simdata::LinkBase obj = data_manager.getObject(simdata::Path(objects[i]));
			assert(obj.valid());
			simdata::Ref<StoreData> data;
			if (!data.tryAssign(obj)) continue;
			std::string path = data_manager.getPathString(objects[i]).substr(prefix.size() + 1);
			assert(data.valid());
			data->setId(path);
			m_StoresByKey[data->key()] = data;
			populateRackMatrix(data->asRackData());
		}
	}
	return !objects.empty();
}

void StoresDatabase::populateRackMatrix(RackData const *rack) {
	if (rack) {
		assert(m_ChildMatrix.find(rack->key()) == m_ChildMatrix.end());
		StoreSet *childset = new StoreSet;
		m_ChildMatrix[rack->key()] = childset;
		StoreSet stores;
		rack->getCompatibleStores(stores);
		for (StoreSet::const_iterator iter = stores.begin(); iter != stores.end(); ++iter) {
			RackMatrix::iterator rackset = m_ParentMatrix.insert(RackMatrix::value_type(*iter, 0)).first;
			if (!rackset->second) rackset->second = new StoreSet;
			rackset->second->insert(rack->key());
			childset->insert(*iter);
		}
	}
}

