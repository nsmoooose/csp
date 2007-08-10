// Combat Simulator Project
// Copyright (C) 2007 The Combat Simulator Project
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

#include <csp/cspsim/stores/Hardpoint.h>
#include <csp/cspsim/stores/StoresDatabase.h>
#include <csp/cspsim/stores/Stores.h>
#include <csp/cspsim/stores/Rack.h>
#include <csp/csplib/data/DataManager.h>


#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Real.h>
#include <csp/csplib/data/Vector3.h>

CSP_NAMESPACE

/*  currently we search for all stores at runtime, but it might be better
    to list stores explicitly.  alternatively, we could cache the store
    list to disk and compare the timestamp to the dar file to invalidate
    the cache.

class StoresDirectory: public Object {
public:
	CSP_DECLARE_OBJECT(StoresDirectory)
	std::vector<Path> const &paths() const { return m_Paths; }
private:
	std::vector<Path> m_Paths;
};


CSP_XML_BEGIN(StoresDirectory)
	CSP_DEF("paths", m_Paths, true)
CSP_XML_END

*/


StoresDatabase::StoresDatabase() {
}

StoresDatabase::~StoresDatabase() {
	for (RackMatrix::iterator iter = m_ParentMatrix.begin(); iter != m_ParentMatrix.end(); ++iter) delete iter->second;
	for (RackMatrix::iterator iter = m_ChildMatrix.begin(); iter != m_ChildMatrix.end(); ++iter) delete iter->second;
}

void StoresDatabase::load(DataManager &data_manager, std::string const &root) {
	_load(data_manager, ObjectID(root), root);
}

void StoresDatabase::reset() {
	m_StoresByKey.clear();
	m_ParentMatrix.clear();
	m_ChildMatrix.clear();
}

Ref<Store> StoresDatabase::getStore(std::string const &id) {
	return getStore(Key(id));
}

Ref<Store> StoresDatabase::getStore(Key const &key) {
	StoreData const *data = getStoreData(key);
	return data ? data->createStore() : 0;
}

StoreData const *StoresDatabase::getStoreData(std::string const &id) {
	return getStoreData(Key(id));
}

StoreData const *StoresDatabase::getStoreData(Key const &key) {
	StoresByKey::const_iterator iter = m_StoresByKey.find(key);
	return (iter == m_StoresByKey.end()) ? 0 : iter->second.get();
}

void StoresDatabase::getStoreDataSet(std::set<Key> const &keyset, std::vector<StoreData const *> &data) {
	data.reserve(data.size() + keyset.size());
	for (std::set<Key>::const_iterator iter = keyset.begin(); iter != keyset.end(); ++iter) {
		StoreData const *store_data = getStoreData(*iter);
		if (store_data) data.push_back(store_data);
	}
}

Key StoresDatabase::getBestRack(HardpointData const *hp, Store const *store) {
	if (!store) return Key();
	RackMatrix::const_iterator rack_iter = m_ParentMatrix.find(store->key());
	if (rack_iter == m_ParentMatrix.end()) return Key();
	StoreSet const *racks = rack_iter->second;
	StoreList const &preferred = hp->preferredRacks();
	for (StoreList::const_iterator iter = preferred.begin(); iter != preferred.end(); ++iter) {
		if (racks->count(*iter) > 0) return *iter;
	}
	assert(!racks->empty());
	return *(racks->begin());
}

StoreSet const *StoresDatabase::getCompatibleRacks(Key const &key) const {
	RackMatrix::const_iterator iter = m_ParentMatrix.find(key);
	return (iter == m_ParentMatrix.end()) ? 0 : iter->second;
}

StoreSet const *StoresDatabase::getCompatibleStores(Key const &key) const {
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

bool StoresDatabase::_load(DataManager &data_manager, ObjectID root, std::string const &prefix) {
	std::vector<ObjectID> objects = data_manager.getChildren(root);
	for (unsigned i = 0; i < objects.size(); ++i) {
		// objects[i] may refer to a directory, an Object, or both
		_load(data_manager, objects[i], prefix);
		if (data_manager.hasObject(objects[i])) {
			// note that this will effectively preload all object models in the stores directory!
			LinkBase obj = data_manager.getObject(Path(objects[i]));
			assert(obj.valid());
			Ref<StoreData> data;
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

CSP_NAMESPACE_END

