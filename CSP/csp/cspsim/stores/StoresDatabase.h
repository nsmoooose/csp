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
 * @file StoresDatabase.h
 *
 **/


#ifndef __CSPSIM_STORESDATABASE_H__
#define __CSPSIM_STORESDATABASE_H__

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Key.h>
#include <csp/csplib/data/Path.h>
#include <SimData/Singleton.h>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace simdata { class DataManager; }

class Hardpoint;
class HardpointData;
class RackData;
class Store;
class StoreData;

#ifndef __CSP_STORESET__
#define __CSP_STORESET__
typedef std::set<simdata::Key> StoreSet;
#endif // __CSP_STORESET__


/** StoresDatabase is a singleton that loads store data from a data file and
 *  provides various methods to query that data and create stores objects.
 */
class StoresDatabase: public simdata::Singleton<StoresDatabase> {
	friend class simdata::Singleton<StoresDatabase>;

public:
	/** Load stores data from the specified DataManager.  All static StoreData
	 *  objects under the path indicated by the root parameter will be indexed.
	 */
	void load(simdata::DataManager &data_manager, std::string const &root);

	/** Convenience method to get a store by id string, equivalent to getStore(Key(id)).
	 */
	simdata::Ref<Store> getStore(std::string const &id);

	/** Get a store by key, returning a new instance.
	 */
	simdata::Ref<Store> getStore(simdata::Key const &key);

	/** Convenience method to retrieve static store data by id string (which is just
	 *  the xml path, excluding the root prefix).  Equivalent to getStoreData(Key(id)).
	 */
	StoreData const *getStoreData(std::string const &id);

	/** Get static store data by key.  Here key is simply a compact representation
	 *  of the store id string.
	 */
	StoreData const *getStoreData(simdata::Key const &key);

	/** Get all static store data corresponding to the specified set of store
	 *  identification keys.
	 */
	void getStoreDataSet(StoreSet const &keyset, std::vector<StoreData const *> &data);

	/** Get the set of all stores that are compatible with (i.e., can be mounted to)
	 *  the specified rack.
	 */
	StoreSet const *getCompatibleStores(simdata::Key const &key) const;

	/** Get the set of all racks that this store can be mounted to.
	 */
	StoreSet const *getCompatibleRacks(simdata::Key const &key) const;

	/** Get the union of all sets of racks that are compatible with the specified
	 *  stores.
	 */
	void getCompatibleRacks(StoreSet const &stores, StoreSet &output) const;

	/** Get the preferred rack or pylon on which to mount a given store or rack to a
	 *  particular hardpoint.  Returns Key(0) if no compatible mounts are found.
	 */
	simdata::Key getBestRack(HardpointData const *hp, Store const *store);

private:
	StoresDatabase();
	virtual ~StoresDatabase();

	bool _load(simdata::DataManager &data_manager, simdata::ObjectID root, std::string const &prefix);
	void populateRackMatrix(RackData const *rack);

	// An index from store key to store data.
	typedef std::map<simdata::Key, simdata::Ref<const StoreData> > StoresByKey;
	StoresByKey m_StoresByKey;

	typedef std::vector<simdata::Key> StoreList;
	typedef std::map<simdata::Key, StoreSet*> RackMatrix;
	RackMatrix m_ParentMatrix;  // store to compatible racks
	RackMatrix m_ChildMatrix;   // rack to compatible stores
};


#endif // __CSPSIM_STOREDATABASE_H__

