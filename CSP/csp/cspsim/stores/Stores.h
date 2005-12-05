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
 * @file Stores.h
 * @brief Classes representing internal and external vehicle stores.
 *
 **/

// TODO(OS) add more documentation and split into multiple headers/sources.

#ifndef __CSPSIM_STORES_STORES_H__
#define __CSPSIM_STORES_STORES_H__


#include <csp/cspsim/stores/StoresDynamics.h>

#include <csp/csplib/data/Key.h>
#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Real.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/osg.h>

#include <osg/PositionAttitudeTransform>

#include <algorithm>
#include <set>
#include <vector>

CSP_NAMESPACE

class StoreData;
class StoresManagementSystem;
class DynamicObject;
class ObjectModel;

class FuelTankData;
class RackData;

class Ammunition;
class Bomb;
class FuelTank;
class Missile;
class Rack;

class Hardpoint;


#ifndef __CSP_STORESET__
#define __CSP_STORESET__
typedef std::set<Key> StoreSet;
#endif // __CSP_STORESET__

inline StoreSet operator&(StoreSet const &a, StoreSet const &b) {
	std::vector<Key> output(std::min(a.size(), b.size()));
	return StoreSet(output.begin(), std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), output.begin()));
}

inline StoreSet operator|(StoreSet const &a, StoreSet const &b) {
	std::vector<Key> output(a.size() + b.size());
	return StoreSet(output.begin(), std::set_union(a.begin(), a.end(), b.begin(), b.end(), output.begin()));
}


/** A class used to identify and index stores attached to a vehicle.  The index consists
 *  of up to three hierarchical subindices.  The top level subindex identifies the vehicle
 *  hardpoint to which the store is attached.  The lower two subindices, if present, are
 *  the pylon and rack mounts, respectively.
 */
class StoreIndex {
public:
	static const StoreIndex Invalid;

	/** Construct a StoreIndex corresponding to the given hardpoint.
	 */
	static StoreIndex Hardpoint(unsigned index) { return (index <= 0xff) ? StoreIndex(index << 16, 0) : Invalid; }

	/** Test if this index is valid.  Returs false if this index is StoreIndex::Invalid.
	 */
	bool valid() const { return m_Key != ~0U; }

	/** Test if this index refers to a hardpoint or a store.
	 */
	bool isHardpoint() const { return valid() && level() == 0; }

	/** Get the hardpoint subindex.
	 */
	unsigned hardpoint() const {
		assert(valid());
		return (m_Key >> 16) & 0xff;
	}

	/** Get the StoreIndex of the specified child of this index.  For example, if
	 *  this index corresponded to a rack on hardpoint 4, child(3) would correspond
	 *  to the store attached to the third moint point of this rack.
	 */
	StoreIndex child(unsigned index) const {
		unsigned c = index * (0x100 >> (8 * level()));
		return (valid() && index <= 0xff) ? StoreIndex(m_Key | c, level() + 1) : Invalid;
	}

	/** Get the StoreIndex of the parent of this index, or StoreIndex::Invalid if no
	 *  parent exists.
	 */
	StoreIndex parent() const {
		const unsigned lev = level();
		return (valid() && (lev > 0)) ? StoreIndex(m_Key, lev - 1) : Invalid;
	}

	/** Test if this StoreIndex is a parent (no necessarily immediate) of the specified
	 *  index.
	 */
	bool isParent(StoreIndex const &index) const {
		if (!valid() || !index.valid()) return false;
		unsigned m = ~mask() & 0xffffff;
		return (index.m_Key & m) == (m_Key & m) && (index.level() > level());
	}

	/** Test if this StoreIndex is the immediate parent of the specified index.
	 */
	bool isImmediateParent(StoreIndex const &index) const {
		if (!valid() || !index.valid()) return false;
		unsigned m = ~mask() & 0xffffff;
		return (index.m_Key & m) == (m_Key & m) && (index.level() == level() + 1);
	}

	/** Given a SpatialIndex corresponding to a child store, return the mount
	 *  index relative to this SpatialIndex.  It is an error to call this method
	 *  with an index that is not a child (i.e., isParent(index) must be true).
	 */
	unsigned relativeChild(StoreIndex const &index) const {
		assert(isParent(index));
		return (index.m_Key & mask()) >> (8 * (1 - level()));
	}

	/** Get the lowest-level subindex.  For example, if this corresponds to a
	 *  rack-mounted munition, index() returns the mount index.
	 */
	unsigned index() const {
		assert(valid());
		return (m_Key >> (8 * (2 - level()))) & 0xff;
	}

	/** Construct a default (Invalid) StoreIndex.
	 */
	StoreIndex(): m_Key(~0U) { }

	/** Define equality and ordering for use in containers.
	 */
	bool operator==(StoreIndex const &other) const { return m_Key == other.m_Key; }
	bool operator<(StoreIndex const &other) const { return m_Key < other.m_Key; }

	/** Pretty print an index.
	 */
	friend std::ostream &operator<<(std::ostream &os, StoreIndex const &index);

private:
	explicit StoreIndex(unsigned key, unsigned level): m_Key((key & 0xffffff & ~(0xffff>>(level*8))) | (level << 30)) { if (level >= 3) m_Key = ~0U; }
	unsigned level() const { return m_Key >> 30; }
	unsigned mask() const { return 0xffff >> (level() * 8); }
	unsigned m_Key;
};

inline std::ostream &operator<<(std::ostream &os, StoreIndex const &index) {
	return os << "INDEX[" << ((index.m_Key>>16)&0xff) << "," << ((index.m_Key>>8)&0xff) << "," << ((index.m_Key)&0xff) << "@" << index.level() << "]";
}

/** Abstract base class for all store types.
 *
 * Store subclasses have associated StoreData subclasses that act as factories
 * for store instances and provide static data via an XML interface.
 */
class Store: public Referenced {
friend class Hardpoint;
public:
	typedef Ref<Store> RefT;

	/** Cast to a rack class, returning NULL on failure.
	 */
	virtual Rack const *asRack() const { return 0; }
	virtual Rack *asRack() { return 0; }

	/** Cast to a fuel tank class, returning NULL on failure.
	 */
	virtual FuelTank const *asFuelTank() const { return 0; }
	virtual FuelTank *asFuelTank() { return 0; }

	/** Cast to an ammunition class, returning NULL on failure.
	 */
	virtual Ammunition const *asAmmunition() const { return 0; }
	virtual Ammunition *asAmmunition() { return 0; }

	/** Cast to a missile class, returning NULL on failure.
	 */
	virtual Missile const *asMissile() const { return 0; }
	virtual Missile *asMissile() { return 0; }

	/** Cast to a bomb class, returning NULL on failure.
	 */
	virtual Bomb const *asBomb() const  { return 0; }
	virtual Bomb *asBomb() { return 0; }

	/** Retrieve the static data associated with this store.  Should never
	 *  return NULL.
	 */
	virtual StoreData const *data() const = 0;

	/** Return the unique id associated with this store type (i.e., data()->key()).
	 */
	inline Key key() const;

	/** Get the canonical name for this store.
	 */
	inline std::string const &name() const;

	/** Add the dynamic characteristics of this store to the specified StoresDynamics object.
	 *  The offset and attitude parameters specify the geometry of the store in body coordinates.
	 *  If no_drag is true, drag characteristics will not be added to dynamics.
	 */
	virtual void sumDynamics(StoresDynamics &dynamics, Vector3 const &offset, Quat const &attitude, bool no_drag) const;

	StoreIndex const &index() const { return m_Index; }

	virtual Store *findStore(StoreIndex const &idx) {
		return idx == m_Index ? this : 0;
	}

	virtual void addModel(osg::Group *group) const;
	osg::Group *getParentGroup() { return m_ParentGroup.get(); }

	virtual double mass() const;
	virtual Matrix3 const &unitInertia() const;
	virtual Vector3 const &cgOffset() const;

protected:
	// For use by Rack only.
	void setChildIndex(Store *store, unsigned idx) {
		assert(store);
		store->m_Index = m_Index.child(idx);
	}

private:
	mutable osg::ref_ptr<osg::Group> m_ParentGroup;
	StoreIndex m_Index;
};



/** A class for defining compatible combinations of hardpoints, pylons, racks,
 *  and stores through an XML interface.
 */
class StoreCompatibilityTable: public Object {
public:
	CSP_DECLARE_STATIC_OBJECT(StoreCompatibilityTable)

	/** Test if a particular store type is compatible (see Store::key()).
	 */
	bool isCompatible(Key const &key) const { return m_Stores.count(key) > 0; }

	/** Get the full set of compatible store types.
	 */
	StoreSet const &getStoreSet() const { return m_Stores; }

	/** Add the specified store types to the compatiblity table.
	 */
	void mergeStoreSet(StoreSet &keyset) const { keyset.insert(m_Stores.begin(), m_Stores.end()); }

protected:
	virtual void postCreate() {
		Object::postCreate();
		m_Stores.insert(m_StoreVector.begin(), m_StoreVector.end());
		m_StoreVector.clear();
	}

private:
	std::vector<Key> m_StoreVector;
	StoreSet m_Stores;
};


/** Abstract base class for static stores data.
 *
 * Subclassed for various major stores categories.  Each subclass defines a specialized
 * xml interface and acts as a factory for the corresponding Store subclass.
 */
class StoreData: public Object {
public:
	CSP_DECLARE_ABSTRACT_OBJECT(StoreData)

	// Type currently isn't used and it remains to be seen if this is a useful
	// concept and/or implementation.
	typedef enum {
		RACK,
		FUELTANK,
		MISSILE
	} Type;

	StoreData(Type type);
	virtual ~StoreData();

	Type type() const { return m_Type; }

	/** Factory method to create an instance of the corresponding Store subclass.
	 *  Returns a new pointer that should be reference counted by the caller.
	 */
	virtual Store *createStore() const = 0;

	/** Get the nominal mass of this store, excluding expendible contents.
	 */
	double mass() const { return m_Mass; }

	/** Get the moment of inertia per unit mass.  The product of this matrix and
	 *  the total mass (empty mass plus expendible mass) gives the full inertia
	 *  tensor.
	 */
	Matrix3 const &unitInertia() const { return m_UnitInertia; }

	/** Get the drag factor which is used to scale the drag profile defined in
	 *  StoresDynamics.
	 */
	double dragFactor() const {
		return m_DragFactor;
	}

	/** Get the canonical name for this store type.
	 */
	std::string const &name() const  { return m_Name; }

	/** Get a unique identifier string for this store type.
	 */
	std::string const &id() const  { return m_Id; }

	/** Get a precomputed Key representation of id().
	 */
	Key const &key() const  { return m_Key; }

	/** Get the nominal center of mass offset of this store, relative to the mount point.
	 *  Note that the mount point is also the origin of the 3D model.  The center of mass
	 *  position is currently independent of expendible contents.
	 */
	Vector3 const &cgOffset() const { return m_CgOffset; }

	/** Attempt to convert this StoreData instance to a specialized subclass.
	 *  Returns NULL on failure.
	 */
	virtual RackData const *asRackData() const { return 0; }
	virtual FuelTankData const *asFuelTankData() const { return 0; }

	/** Set the unique id string associated with this store type.  Affects both
	 *  id() and key().
	 */
	void setId(std::string const &id) {
		m_Id = id;
		m_Key = id;
	}

	/** Realize the 3D model for this store.  Returns a shared model node, or NULL
	 *  if no model is defined.
	 */
	osg::Node *makeModel() const;

	Ref<ObjectModel> getModel() const;

	/** Add the dynamical properties of this store type to a StoresDynamics instance.  The offset
	 *  and attitude parameters give the position of the store in body coordinates, while extra_mass
	 *  specified the expendible mass (in excess of mass()).  If no_drag is true, drag forces will
	 *  not be counted.
	 */
	void sumDynamics(StoresDynamics &dynamics, Vector3 const &offset, Quat const &attitude, double extra_mass=0, bool no_drag=false) const {
		const double mass = m_Mass + extra_mass;
		const Vector3 cg = attitude.rotate(m_CgOffset) + offset;  // center of gravity in body coordinates
		const Matrix3 i = attitude.getMatrix3() * m_UnitInertia;
		dynamics.addMassAndInertia(mass, i, cg);
		if (!no_drag) {
			dynamics.addDrag(m_DragFactor, cg);  // should be front of store, but cg is probably close enough
		}
	}

	virtual Ref<DynamicObject> createObject() const;

protected:
	virtual void postCreate() {
		Object::postCreate();
		m_Key = m_Id;
	}

private:
	const Type m_Type;
	std::string m_Name;
	std::string m_Id;
	Key m_Key;
	double m_Mass;
	Matrix3 m_UnitInertia;
	Vector3 m_CgOffset;
	double m_DragFactor;
	Link<ObjectModel> m_Model;
	Path m_Object;
};

inline Key Store::key() const { return data()->key(); }
inline std::string const &Store::name() const { return data()->name(); }


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
	unsigned capacity(Key const &store_key) const {
		if (m_DefaultCompatibility.valid() && m_DefaultCompatibility->isCompatible(store_key)) return capacity();
		unsigned count = 0;
		for (unsigned idx = 0; idx < m_Mounts.size(); ++idx) {
			if (m_Mounts[idx]->isCompatible(store_key)) count++;
		}
		return count;
	}

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
	void getCompatibleStores(StoreSet &stores) const {
		stores.clear();
		if (m_DefaultCompatibility.valid()) m_DefaultCompatibility->mergeStoreSet(stores);
		for (unsigned idx = 0; idx < m_Mounts.size(); ++idx) m_Mounts[idx]->mergeStoreSet(stores);
	}

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

	virtual Store *findStore(StoreIndex const &idx) {
		if (idx == index()) return this;
		if (!index().isParent(idx)) return 0;
		unsigned child = index().relativeChild(idx);
		if (child >= m_Data->capacity() || !m_Children[child]) return 0;
		return m_Children[child]->findStore(idx);
	}

	RackMount const *mount(unsigned i) { return m_Data->mount(i); }

	unsigned availableSpace(Key const &store_key) const {
		const bool default_compatible = m_Data->isDefaultCompatible(store_key);
		unsigned count = 0;
		for (unsigned idx = 0; idx < getNumChildren(); ++idx) {
			if (!m_Children[idx] && (default_compatible || m_Data->mount(idx)->isCompatible(store_key))) count++;
		}
		return count;
	}

	Store *getChild(unsigned idx) const {
		assert(idx < getNumChildren());
		return m_Children[idx].get();
	}

	bool getChildren(std::vector<Store::RefT> &children, bool recurse=false) const {
		bool any = false;
		for (unsigned i = 0; i < getNumChildren(); ++i) {
			if (m_Children[i].valid()) {
				any = true;
				children.push_back(m_Children[i]);
				if (recurse && m_Children[i]->asRack()) m_Children[i]->asRack()->getChildren(children, recurse);
			}
		}
		return any;
	}

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

	unsigned mountStores(StoreData const *data, unsigned count) {
		assert(data);
		unsigned added = 0;
		for (unsigned i = 0; i < m_Data->capacity(); ++i) {
			if (isAvailable(i, data->key())) {
				_setChild(i, data->createStore());
				assert(m_Children[i].valid());
				if (++added >= count) break;
			}
		}
		return added;
	}

	void _setChild(unsigned idx, Store *store) {
		if (store) setChildIndex(store, idx);
		m_Children[idx] = store;
	}

	Ref<const RackData> m_Data;
	Store::RefT *m_Children;
};


/** Data for a fuel tank (internal or external).
 */
class FuelTankData: public StoreData {
	// Fuel density:
	//  JP-5:  827 kg/m^3 = 0.827 kg/l at STP.
	//  JP-8:  800 kg/m^3 = 0.800 kg/l at STP.
public:
	CSP_DECLARE_STATIC_OBJECT(FuelTankData)

	FuelTankData(): StoreData(FUELTANK), m_Capacity(0.0) { }

	/** Create a fuel tank instance, returning a new pointer.
	 */
	virtual Store *createStore() const;

	/** Get the capacity in liters.
	 */
	double capacity() const { return m_Capacity; }

	/** Get the fuel density in kg/liter.
	 */
	double fuelDensity() const { return 0.800 /* kg/l */; }  // assume jp-8

private:
	double m_Capacity;  // l
};


/** Data for a fuel tank store (internal or external).
 */
class FuelTank: public Store {
friend class FuelTankData;
public:
	virtual FuelTank const *asFuelTank() const { return this; }
	virtual FuelTank *asFuelTank() { return this; }

	/** Get the mass of the tank, including fuel.
	 */
	virtual double mass() const { return m_Data->mass() + quantity() * m_Data->fuelDensity(); }

	/** Get the fuel density in kg/liter.
	 */
	double fuelDensity() const { return m_Data->fuelDensity(); }

	/** Get the capacity of the tank, in liters.
	 */
	double capacity() const { return m_Data->capacity(); }

	/** Get the quantity of fuel in the tank, in liters.
	 */
	double quantity() const { return m_Quantity; }

	/** Set the quantity of fuel in the tank, in liters.  The specified value is
	 *  clamped at the rated capacity of the tank.
	 */
	void setQuantity(double quantity) {
		m_Quantity = clampTo(quantity, 0.0, capacity());
	}

	/** Get the static data for this fuel tank.
	 */
	virtual StoreData const *data() const { return m_Data.get(); }

	/** Add dynamic properties of the fuel tank (mass, inertia, drag) to a StoresDynamics instance.
	 */
	virtual void sumDynamics(StoresDynamics &dynamics, Vector3 const &offset, Quat const &attitude, bool no_drag) const {
		m_Data->sumDynamics(dynamics, offset, attitude, quantity() * m_Data->fuelDensity(), no_drag);
	}

private:
	explicit FuelTank(FuelTankData const *data): m_Quantity(data->capacity()), m_Data(data) { assert(data); }
	virtual ~FuelTank() { }
	double m_Quantity;  // l
	Ref<const FuelTankData> m_Data;
};


/** Data for a fuel tank (internal or external).
 */
class MissileData: public StoreData {
public:
	CSP_DECLARE_STATIC_OBJECT(MissileData)

	MissileData(): StoreData(MISSILE) { }

	/** Create a Missile instance, returning a new pointer.
	 */
	virtual Store *createStore() const;
};


/** Data for a missiles.
 */
class Missile: public Store {
friend class MissileData;
public:
	virtual Missile const *asMissile() const { return this; }
	virtual Missile *asMissile() { return this; }

	/** Get the static data for this missile.
	 */
	virtual StoreData const *data() const { return m_Data.get(); }

private:
	explicit Missile(MissileData const *data): m_Data(data) { assert(data); }
	Ref<const MissileData> m_Data;
};


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
	Quat m_Attitude;
	Link<StoreData> m_FixedStore;
};


class Hardpoint: public Referenced {
friend class StoresManagementSystem;
public:
	typedef Ref<Hardpoint> RefT;

	Hardpoint(HardpointData const *data, unsigned index): m_Data(data), m_Fixed(false), m_Index(index) {
		assert(data);
		if (data->hasFixedStore()) {
			m_Store = data->createFixedStore();
			assert(m_Store.valid());
			m_Fixed = true;
		}
	}

	HardpointData const *data() const { return m_Data.get(); }
	bool isFixed() const { return m_Fixed; }
	unsigned getIndex() const { return m_Index; }
	std::string const &name() const { return m_Data->name(); }
	void getCompatibleStores(StoreSet &stores) const { m_Data->getCompatibleStores(stores); }

	bool mountStore(Store *store) {
		CSPLOG(INFO, OBJECT) << "mounting store";
		assert(store);
		CSPLOG(INFO, OBJECT) << "mounting store " << store->name();
		assert(m_Data.valid());
		if (!m_Data->isMountCompatible(store->key())) {
			CSPLOG(INFO, OBJECT) << "could not mount incompatible store " << store->name() << " on hardpoint " << name();
			return false;
		}
		if (m_Store.valid()) {
			CSPLOG(INFO, OBJECT) << "could not mount store " << store->name() << "; hardpoint " << name() << " already has a mounted store";
			return false;
		}
		CSPLOG(INFO, OBJECT) << "mounting " << store->name() << " on hardpoint " << name();
		store->m_Index = StoreIndex::Hardpoint(m_Index);  // FIXME kludge
		m_Store = store;
		return true;
	}

	Store *getStore() { return m_Store.get(); }
	Store *getStore(StoreIndex const &index) { return !m_Store ? 0 : m_Store->findStore(index); }

	bool removeStore(StoreIndex const &idx, Store::RefT &store, Vector3 &offset, Quat &attitude, Vector3 &velocity, Vector3 &angular_velocity) {
		if (!m_Store) return false;
		StoreIndex hpindex = StoreIndex::Hardpoint(m_Index);
		offset = m_Data->offset();
		attitude = m_Data->attitude();
		if (idx == hpindex) {
			store = m_Store;
			m_Store = 0;
		} else {
			Store *parent = m_Store->findStore(idx.parent());
			if (!parent || !parent->asRack()) return false;
			store = parent->findStore(idx);
			if (!store) return false;
			parent->asRack()->setChild(idx, 0);

			unsigned i = hpindex.relativeChild(idx);
			RackMount const *mount = m_Store->asRack()->mount(i);
			mount->applyGeometry(offset, attitude);
			if (parent != m_Store.get()) {
				mount = parent->asRack()->mount(idx.index());
				mount->applyGeometry(offset, attitude);
			}
			velocity = mount->ejectVelocity();
			angular_velocity = mount->ejectAngularVelocity();
		}
		return true;
	}

	void addModel(osg::Group *group) const {
		assert(group);
		if (!m_Store) return;
		osg::PositionAttitudeTransform *hp = new osg::PositionAttitudeTransform;
		hp->setPosition(toOSG(m_Data->offset()));
		hp->setAttitude(toOSG(m_Data->attitude()));
		m_Store->addModel(hp);
		group->addChild(hp);
	}

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

CSP_NAMESPACE_END

#endif // __CSPSIM_STORES_STORES_H__

