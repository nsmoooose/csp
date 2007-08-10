
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
 * @file Rack.h
 *
 **/

#ifndef __CSPSIM_STORES_RACK_H__
#define __CSPSIM_STORES_RACK_H__

#include <csp/cspsim/stores/StoresDynamics.h>
#include <csp/cspsim/stores/Stores.h>

#include <osg/PositionAttitudeTransform>

#include <algorithm>
#include <set>
#include <vector>

CSP_NAMESPACE

/** A class for specifying the mount point of a single store.  RackData
 *  defines one or more RackMount points, each with a specific location
 *  and orientation relative to the rack.  RackMount also defines a set
 *  of compatible stores.
 */
class RackMount: public Object {
public:
	CSP_DECLARE_STATIC_OBJECT(RackMount)

	RackMount(): m_Attitude(Quat::IDENTITY), m_Fixed(false) { }

	// XXX csplib should not zero m_Attitude when the xml tag is not set.
	// temporary (hacked) fix for testing purposes.  UPDATE: actually,
	// csplib doesn't zero m_Altitude.  it saved the old default value
	// in the .dar file, and doesn't realize that the default has changed.
	virtual void postCreate() { if (m_Attitude == Quat::ZERO) m_Attitude = Quat::IDENTITY; }

	/** Test if a given store is compatible with this mount point.
	 */
	bool isCompatible(Key const &store_key) const { return !m_Compatibility ? false : m_Compatibility->isCompatible(store_key); }

	/** Add the specified stores to the list of compatible stores.
	 */
	void mergeStoreSet(StoreSet &keyset) const { if (m_Compatibility.valid()) m_Compatibility->mergeStoreSet(keyset); }

	/** Get the offset of this mount point relative to the parent rack.
	 */
	Vector3 const &offset() const { return m_Offset; }

	/** Get the orientation of this mount point relative to the parent rack.
	 */
	Quat const &attitude() const { return m_Attitude; }

	void applyGeometry(Vector3 &offset, Quat &attitude) const {
		offset = offset + attitude.rotate(m_Offset);
		attitude = attitude * m_Attitude;
	}

	/** Returns true if the mount does not support release/ejection.
	 */
	bool fixed() const { return m_Fixed; }

	/** Initial velocity to impart to store when released.
	 */
	Vector3 const &ejectVelocity() const { return m_EjectVelocity; }

	/** Initial angular velocity to impart to store when released.
	 */
	Vector3 const &ejectAngularVelocity() const { return m_EjectAngularVelocity; }

private:
	Vector3 m_Offset;
	Quat m_Attitude;
	Link<StoreCompatibilityTable> m_Compatibility;
	Vector3 m_EjectVelocity;
	Vector3 m_EjectAngularVelocity;
	bool m_Fixed;
};


/** StoreData specialiation for stores racks.  A rack defines a set of mount
 *  points to which stores can be attached.
 */
class RackData: public StoreData {
public:
	CSP_DECLARE_STATIC_OBJECT(RackData)

	RackData(): StoreData(RACK) { }

	/** Create a rack using this data; returns a new instance.
	 */
	virtual Store *createStore() const;

	/** Get the number of mount points for this rack type.
	 */
	unsigned capacity() const { return m_Mounts.size(); }

	/** Get the number of mount points for this rack type that are compatible with
	 *  the specified store.
	 */
	unsigned capacity(Key const &store_key) const;
	
	bool isDefaultCompatible(Key const &store_key) const {
		return m_DefaultCompatibility.valid() && m_DefaultCompatibility->isCompatible(store_key);
	}

	/** Get a mount point by index, which must be in the range 0..capacity().
	 */
	RackMount const *mount(unsigned idx) const { assert(idx < m_Mounts.size()); return m_Mounts[idx].get(); }

	/** Test if a particular store can be mounted on this rack.
	 */
	bool isCompatible(Key const &store_key) const { return capacity(store_key) > 0; }

	bool isCompatible(unsigned idx, Key const &key) const {
		assert(idx < m_Mounts.size());
		return isDefaultCompatible(key) || m_Mounts[idx]->isCompatible(key);
	}
	
	/** Get a list of all stores that can be mounted on this rack.
	 */
	void getCompatibleStores(StoreSet &stores) const;

	/** Method used to cast from StoresData to RackData.
	 */
	virtual RackData const *asRackData() const { return this; }

private:
	Link<RackMount>::vector m_Mounts;
	Link<StoreCompatibilityTable> m_DefaultCompatibility;
};

class Rack: public Store {
friend class RackData;
friend class Hardpoint;
friend class StoresManagementSystem;

public:
	typedef Ref<Rack> RefT;

	virtual ~Rack() { delete[] m_Children; }

	virtual StoreData const *data() const { return m_Data.get(); }

	virtual Rack const *asRack() const { return this; }
	virtual Rack *asRack() { return this; }

	unsigned getNumChildren() const {
		return m_Data->capacity();
	}

	virtual Store *findStore(StoreIndex const &idx);

	RackMount const *mount(unsigned i) { return m_Data->mount(i); }

	unsigned availableSpace(Key const &store_key) const;

	Store *getChild(unsigned idx) const {
		assert(idx < getNumChildren());
		return m_Children[idx].get();
	}

	bool getChildren(std::vector<Store::RefT> &children, bool recurse=false) const;

	bool isAvailable(unsigned idx, Key const &key) const {
		assert(idx < getNumChildren());
		return (!m_Children[idx] && m_Data->isCompatible(idx, key));
	}

	virtual void sumDynamics(StoresDynamics &dynamics, Vector3 const &offset, Quat const &attitude, bool no_drag) const;

	virtual void addModel(osg::Group *group) const;

private:
	Rack(RackData const *data): m_Data(data), m_Children(0) {
		assert(data);
		m_Children = new Store::RefT[data->capacity()];
	}

	bool setChild(StoreIndex const &idx, Store *store) {
		unsigned mount_idx = index().relativeChild(idx);
		return setChild(mount_idx, store);
	}

	bool setChild(unsigned idx, Store *store) {
		assert(idx < m_Data->capacity());
		if (store && !m_Data->isCompatible(idx, store->key())) return false;
		_setChild(idx, store);
		return true;
	}

	unsigned mountStores(StoreData const *data, unsigned count);

	void _setChild(unsigned idx, Store *store) {
		if (store) setChildIndex(store, idx);
		m_Children[idx] = store;
	}

	Ref<const RackData> m_Data;
	Store::RefT *m_Children;
};
CSP_NAMESPACE_END

#endif // __CSPSIM_STORES_RACK_H__

