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
 * @file Stores.h
 * @brief Classes representing internal and external vehicle stores.
 *
 **/

// TODO(OS) add more documentation.

#ifndef __CSPSIM_STORES_STORES_H__
#define __CSPSIM_STORES_STORES_H__


#include <csp/csplib/data/Key.h>
#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/util/osg.h>
#include <csp/csplib/util/Ref.h>

#include <osg/PositionAttitudeTransform>

#include <algorithm>
#include <set>
#include <vector>

namespace csp {

class StoreData;
class StoresDynamics;
class StoresManagementSystem;
class DynamicObject;
class Object;
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
	void sumDynamics(StoresDynamics &dynamics, Vector3 const &offset, Quat const &attitude, double extra_mass=0, bool no_drag=false) const;

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

} // namespace csp

#endif // __CSPSIM_STORES_STORES_H__

