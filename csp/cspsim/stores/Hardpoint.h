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
 * @file Hardpoint.h
 * @brief classes representing vehicle hardpoints
 **/


#ifndef __CSPSIM_STORES_HARDPOINT_H__
#define __CSPSIM_STORES_HARDPOINT_H__

#include <csp/cspsim/stores/Stores.h>
#include <csp/cspsim/stores/StoresDynamics.h>

/*
#include <csp/csplib/data/Key.h>
#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Real.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/osg.h>
*/

#include <vector>

namespace csp {


class Hardpoint;
class Rack;
class RackMount;
/** Data for a vehicle hardpoint.
 */
class HardpointData: public Object {
public:
	CSP_DECLARE_STATIC_OBJECT(HardpointData)

	HardpointData(): m_External(true), m_Attitude(Quat::IDENTITY) { }

	// XXX csplib should not zero m_Attitude when the xml tag is not set.  temporary (hacked) fix for testing purposes.
	virtual void postCreate() { if (m_Attitude == Quat::ZERO) m_Attitude = Quat::IDENTITY; }

	virtual Hardpoint *createHardpoint(unsigned index) const;
	std::string const &name() const { return m_Name; }
	Vector3 const &offset() const { return m_Offset; }
	Vector3 const &mountOffset() const { return m_MountOffset; }
	Quat const &attitude() const { return m_Attitude; }
	bool isExternal() const { return m_External; }
	bool hasFixedStore() const { return m_FixedStore.valid(); }
	Store *createFixedStore() const { return !m_FixedStore ? 0 : m_FixedStore->createStore(); }

	bool isCompatible(Key const &key) const { return !m_FixedStore && (!m_Compatibility || m_Compatibility->isCompatible(key)); }
	
	void getCompatibleStores(StoreSet &stores) const {
		stores.clear();
		m_Compatibility->mergeStoreSet(stores);
	}

	bool isMountCompatible(Key const &key) const { return !m_FixedStore && (!m_MountCompatibility || m_MountCompatibility->isCompatible(key)); }
	
	void getMountCompatibleStores(StoreSet &stores) const {
		stores.clear();
		m_MountCompatibility->mergeStoreSet(stores);
	}

	std::vector<Key> const &preferredRacks() const { return m_RackPreference; }

private:
	std::string m_Name;
	bool m_External;
	Link<StoreCompatibilityTable> m_Compatibility;
	Link<StoreCompatibilityTable> m_MountCompatibility;
	std::vector<Key> m_RackPreference;
	Vector3 m_Offset;
	Vector3 m_MountOffset;
	Quat m_Attitude;
	Link<StoreData> m_FixedStore;
};


class Hardpoint: public Referenced {
friend class StoresManagementSystem;
public:
	typedef Ref<Hardpoint> RefT;

	Hardpoint(HardpointData const *data, unsigned index);

	HardpointData const *data() const { return m_Data.get(); }
	bool isFixed() const { return m_Fixed; }
	unsigned getIndex() const { return m_Index; }
	std::string const &name() const { return m_Data->name(); }
	void getCompatibleStores(StoreSet &stores) const { m_Data->getCompatibleStores(stores); }

	bool mountStore(Store *store);

	Store *getStore() { return m_Store.get(); }
	Store *getStore(StoreIndex const &index) { return !m_Store ? 0 : m_Store->findStore(index); }

	bool removeStore(StoreIndex const &idx, Store::RefT &store, Vector3 &offset, Quat &attitude, Vector3 &velocity, Vector3 &angular_velocity);

	void addModel(osg::Group *group) const;

	void sumDynamics(StoresDynamics &dynamics) const {
		if (!m_Store) return;
		const bool no_drag = !m_Data->isExternal();
		m_Store->sumDynamics(dynamics, m_Data->offset(), m_Data->attitude(), no_drag);
	}

private:
	Ref<const HardpointData> m_Data;
	Ref<Store> m_Store;

	bool m_Fixed;
	unsigned m_Index;
};

} // namespace csp

#endif // __CSPSIM_STORES_HARDPOINT_H__

