// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file Battlefield.h
 *
 **/

#ifndef __CSPSIM_BATTLEFIELD_BATTLEFIELD_H__
#define __CSPSIM_BATTLEFIELD_BATTLEFIELD_H__

#include <csp/cspsim/battlefield/SimObject.h>
#include <csp/csplib/util/TimeStamp.h>
#include <csp/csplib/util/SynchronousUpdate.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Verify.h>
#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/spatial/QuadTree.h>

#include <limits>
#include <map>
#include <vector>

namespace csp {

// TODO
//  move unitupdateproxy and associated code to localbattlefield.  this may require
//  subclassing unitwrapper in localbattlefield.
//
//  slow unit refresh for nearby objects; adjust update rates
//

class Vector3;


/** Battlefield management class.
 *
 *  Battlefield is a base class for managing static and dynamic objects in
 *  the game.  It maintains spatial indices to keep track of object locations.
 *  See LocalBattlefield and GlobalBattlefield.
 */
class CSPSIM_EXPORT Battlefield: public Referenced {
public:

	// we currently refer to all objects (DynamicObjects and FeatureGroups)
	// via the SimObject base class.  this may change in the future, hence
	// the typedef indirection.
	typedef Ref<SimObject> Object;

	/// reference for DynamicObject types
	typedef Ref<SimObject> Unit;

	/// reference for FeatureGroup types
	typedef Ref<SimObject> Static;

	// more shorthand typedefs
	typedef SimObject::ObjectId ObjectId;

	/** Construct a new battlefield.
	 */
	Battlefield();

	/** Destroy the battlefield and free resources.
	 */
	virtual ~Battlefield();

	/** Update the battlefield.
	 *
	 *  Triggers update callbacks of all units in the battlefield, tracks
	 *  the motion of units (and in turn visibility and aggregation state),
	 *  and removes units that have been marked by removeUnit().
	 *
	 *  @param dt The elapsed time since the last update.
	 */
	virtual void update(double dt);

	/** Add a static feature to the battlefield.
	 *
	 *  Currently all features in the theater are loaded into the battlefield
	 *  shortly after it is constructed.  Eventually a more sophisticated
	 *  scheme may be needed.
	 *
	 *  @param feature The feature to add.
	 */
	virtual void addStatic(Static const &feature);

	/** Very basic unit lookup method.
	 *
	 *  Finds the next unit after the specified unit that matching the search
	 *  criteria.  Repeated calls, passing the unit returned by the previous
	 *  result, will cycle through all units of the specified type.
	 *
	 *  TODO implement a more robust query mechanism for both units and features.
	 *
	 *  @param unit The current unit.
	 *  @param human If >0, only return human-controlled objects; if zero
	 *               only return agent-controlled objects.
	 *  @param local If >0, only return local units; if zero, return only
	 *               remote objects.
	 *  @param category Not currently implemented; intented to select specific
	 *         types of units (or eventually, features).
	 *  @returns The next matching unit; null otherwise.
	 */
	Unit getNextUnit(Unit const &unit, int human, int local, int category);

	/** Lookup a unit by its id.
	 */
	Unit getUnitById(ObjectId id);

protected:

	/** Battlefield NetworkMessage routing types
	 *
	 *  @li @c COMMAND         global-local battlefield messages
	 *  @li @c UNIT_UPDATE     peer-peer unit updates
	 *  @li @c FEATURE_UPDATE  global-local feature updates
	 */
	enum {
		ROUTE_COMMAND=16,
		ROUTE_UNIT_UPDATE=17,
		ROUTE_FEATURE_UPDATE=18
	};

	// pull in some shorthand types for quadtree components
	typedef spatial::QuadTree QuadTree;
	typedef spatial::Child QuadTreeChild;
	typedef spatial::Coordinate GridCoordinate;
	typedef spatial::Point GridPoint;
	typedef spatial::Region GridRegion;

	/** Object wrapper is a base class used for inserting objects into
	 *  the quadtree indices.
	 *
	 *  The QuadTreeChild base class maintains the grid coordinates of the object
	 *  and the object id.  We bundle in the object reference and path for
	 *  convenience.
	 *
	 *  ObjectWrappers can be created with a null object reference, for example
	 *  when a LocalBattlefield is first informed of a unit by the GlobalBattlefield.
	 */
	class ObjectWrapper: public QuadTreeChild {
	public:
		/** Construct an ObjectWrapper for an existing object.
		 */
		ObjectWrapper(Object const &object, GridCoordinate x, GridCoordinate y): QuadTreeChild(object->id(), x, y), m_Object(object), m_Path(object->getObjectPath()) { }

		/** Construct an ObjectWrapper for a non-instantiated object.
		 */
		ObjectWrapper(ObjectId id, Path const &path, GridCoordinate x, GridCoordinate y): QuadTreeChild(id, x, y), m_Path(path) { }

		virtual ~ObjectWrapper() {}
		inline Object const &object() const { return m_Object; }
		inline Object &object() { return m_Object; }

		inline bool operator==(Object const &o) { return id() == o->id(); }
		inline bool operator!=(Object const &o) { return id() != o->id(); }

		/** Get the (csplib object) path for this object.  The path uniquely identifies the
		 *  object type, and can used to construct an instance of the object.
		 */
		inline const Path path() const { return m_Path; }

		/** Returns true if a local instance of the wrapped object exists.
		 */
		inline bool isInstantiated() const { return m_Object.valid(); }

	protected:
		/** Bind an object instance to the wrapper.
		 */
		void setObject(Object const &object);

	private:
		Object m_Object;
		Path m_Path;
	};


	/** Wrapper class for inserting static objects into a quadtree index.
	 */
	class StaticWrapper: public ObjectWrapper {
	public:
		StaticWrapper(Static const &s, GridCoordinate x, GridCoordinate y): ObjectWrapper(s, x, y) { }
		inline Static const &feature() const { return object(); }
		inline Static &feature() { return object(); }
		virtual void setStatic(Static const &object);
	};


	/** Wrapper for inserting dynamic objects into a quadtree index.
	 *
	 *  TODO
	 *  Keep track of the aggregation bubble count for an object, which
	 *  is the number aggregation bubbles projected by human-controlled vehicles
	 *  that overlap the object's position.  If this count is greater than zero,
	 *  the object should be deaggregated.  This functionality was temporarily
	 *  removed for the initial implementation of the distributed battlefield.
	 */
	class UnitWrapper: public ObjectWrapper {
		PeerId m_Owner;
		//ScopedPointer<UnitUpdateProxy> m_UpdateProxy;
	public:
		UnitWrapper(Unit const &u, PeerId owner);
		UnitWrapper(ObjectId id, Path const &path, PeerId owner);
		inline Unit const &unit() const { return object(); }
		inline Unit &unit() { return object(); }
		inline PeerId owner() const { return m_Owner; }
	};


	/** Move a unit, updating the spatial indices and managing aggregation
	 *  and visibility changes.
	 *
	 *  @param wrapper The unit to move.
	 *  @param old_position The old grid position.
	 *  @param new_position The new grid position.
	 */
	virtual void moveUnit(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position);

	/** Helper method for adding two grid coordinates, clamped to the grid
	 *  boundary without overflowing.
	 */
	static inline GridCoordinate gridAdd(GridCoordinate a, GridCoordinate b) {
		if (a > std::numeric_limits<GridCoordinate>::max() - b) {
			return std::numeric_limits<GridCoordinate>::max();
		}
		return a + b;
	}

	/** Helper method for subtracting two grid coordinates, clamped to the
	 *  grid boundary without overflowing.
	 */
	static inline GridCoordinate gridSub(GridCoordinate a, GridCoordinate b) {
		if (a <= b) return 0;
		return a - b;
	}

	/** Helper method for computing the square of the distance in global
	 *  coordinates (meters^2) between to grid points.
	 */
	inline double globalDistance2(GridCoordinate x0, GridCoordinate y0, GridCoordinate x1, GridCoordinate y1) {
		double dx = (static_cast<double>(x0) - static_cast<double>(x1)) * m_GridToGlobalScale;
		double dy = (static_cast<double>(y0) - static_cast<double>(y1)) * m_GridToGlobalScale;
		return dx*dx + dy*dy;
	}

	/** Helper method for computing the square of the distance in global
	 *  coordinates (meters^2) between to grid points.
	 */
	inline double globalDistance2(GridPoint p1, GridPoint p2) {
		return globalDistance2(p1.x(), p1.y(), p2.x(), p2.y());
	}

	/** Convert a global position coordinate to the grid coordinates.
	 */
	inline GridCoordinate globalToGrid(double x) {
		x = x * m_GlobalToGridScale + m_GridOffset;
		if (x < 0.0) return 0;
		if (x >= std::numeric_limits<GridCoordinate>::max()) {
			return std::numeric_limits<GridCoordinate>::max();
		}
		return static_cast<GridCoordinate>(x);
	}

	/** Convert a global position vector to (2D) grid coordinates.
	 */
	inline GridPoint globalToGrid(Vector3 const &pos) {
		return GridPoint(globalToGrid(pos.x()), globalToGrid(pos.y()));
	}

	/** Convert a grid coordinate to global coordinates.
	 */
	inline double gridToGlobal(GridCoordinate x) {
		return (static_cast<double>(x) - m_GridOffset) * m_GridToGlobalScale;
	}

	/** Construct a GridRegion enlosing a circle.
	 *
	 *  @param x The x coordinate of the center of the circle.
	 *  @param y The y coordinate of the center of the circle.
	 *  @param radius The radius of the circle.
	 */
	GridRegion makeGridRegionEnclosingCircle(GridCoordinate x, GridCoordinate y, double radius);

	/** Construct a GridRegion enlosing a circle.
	 *
	 *  @param point The center of the circle.
	 *  @param radius The radius of the circle.
	 */
	inline GridRegion makeGridRegionEnclosingCircle(GridPoint const &point, double radius) {
		return makeGridRegionEnclosingCircle(point.x(), point.y(), radius);
	}

	/** Test if a point corresponds to the "null" point, which is
	 *  as a special point to indicate "no coordinates".  For example,
	 *  when adding a unit to the battlefield, it is first placed at
	 *  the null point, then moved to its actual location.
	 */
	static inline bool isNullPoint(GridPoint const &point) {
		return point.x() == 0 && point.y() == 0;
	}

	/** Implements hysteresis for objects moving on the grid.
	 *
	 *  Will return true if the new coordinates are sufficiently far
	 *  from the old coordinates.  The minimum distance is controlled
	 *  by m_Hysteresis.
	 */
	inline bool hasMoved(GridCoordinate old_x, GridCoordinate old_y, GridCoordinate new_x, GridCoordinate new_y) {
		return ((new_x > (old_x + m_Hysteresis)) ||
		        (new_x < (old_x - m_Hysteresis)) ||
		        (new_y > (old_y + m_Hysteresis)) ||
		        (new_y < (old_y - m_Hysteresis)));
	}

	/** Implements hysteresis for objects moving on the grid.
	 *
	 *  Will return true if the new coordinates are sufficiently far
	 *  from the old coordinates.  The minimum distance is controlled
	 *  by m_Hysteresis.
	 */
	inline bool hasMoved(GridPoint old_pos, GridPoint new_pos) {
		return ((new_pos.x() > (old_pos.x() + m_Hysteresis)) ||
		        (new_pos.x() < (old_pos.x() - m_Hysteresis)) ||
		        (new_pos.y() > (old_pos.y() + m_Hysteresis)) ||
		        (new_pos.y() < (old_pos.y() - m_Hysteresis)));
	}

	/// Spatial index for all dynamic objects in the battlefield.
	inline QuadTree const *dynamicIndex() const { return m_DynamicIndex.get(); }

	/// Spatial index for all static features in the battlefield.
	inline QuadTree const *staticIndex() const { return m_StaticIndex.get(); }

	/// Spatial index for all tracking motion of dynamic objects.
	inline QuadTree const *motionIndex() const { return m_MotionIndex.get(); }

	typedef std::map<ObjectId, StaticWrapper*> StaticMap;
	typedef std::vector<StaticWrapper*> StaticVector;
	typedef std::map<ObjectId, UnitWrapper*> UnitMap;
	typedef std::vector<UnitWrapper*> UnitVector;
	typedef std::vector<ObjectId> IdVector;

	/// Map of all units by object id
	inline UnitMap const &unitMap() const { return m_UnitMap; }

	/// Number of units in the (local) battlefield.
	int unitCount() const { return m_UnitMap.size(); }

	/** Battlefield type-specific implementation of setHuman.
	 *
	 *  This implementation modifies the SimObject human/agent flag.
	 *  Subclass extensions need to update aggregation counts and/or
	 *  communicate changes to remote hosts.
	 *
	 *  @param wrapper The unit wrapper to change.
	 *  @param human True if the unit is controlled by a human player.
	 */
	virtual void setHumanUnit(UnitWrapper *wrapper, bool human);

	/** Initialize a new unit to be either local or remote.  Can only be done
	 *  before the unit is assigned an id number.  After that point, the unit
	 *  must be deleted and recreated to change locality (since other data
	 *  structures depend on the immutability of locality).  Local units are
	 *  added to the local units list.
	 */
	void initializeLocalUnit(Unit unit, bool local);

	/** Assign a (unique) id to a new object.
	 *
	 *  @param object The new object (current id must be zero).
	 *  @param id The new id (caller's responsibility to ensure uniqueness).
	 */
	void _assignObjectId(Object const &object, ObjectId id) {
		assert(object.valid() && object->id() == 0);
		object->setId(id);
	}

	UnitWrapper *findUnitWrapper(ObjectId id);


//// These methods are carry overs from the original battlefield implementation
//// and need to be refactored.  They provide direct access to the battlefield
//// indices in ways that may circumvent standard procedures in the new bf
//// subclasses.
protected:

	/** Add a (local) unit to the battlefield.
	 *
	 *  The initial state of the unit must be agent-controlled.
	 *
	 *  @param unit The unit to add.
	 *  @return The wrapper for the new unit.
	 */
	virtual void addUnit(UnitWrapper *wrapper);

	/** Remove a unit from the battlefield.
	 *
	 *  This method marks the unit for removal, which will occur at the
	 *  next call to update().
	 *
	 *  @param id The id of the unit to remove.
	 */
	virtual void removeUnit(ObjectId id);

	/** Set a unit to be human-controlled or agent-controlled.
	 *
	 *  All human/agent transitions are managed by the battlefield,
	 *  since these changes affect the aggregation state of other
	 *  units.
	 *
	 *  @param id The id of the unit to change.
	 *  @param human True if the unit is controlled by a human player.
	 */
	void setHumanUnit(ObjectId id, bool human);

	/** Update the position of a unit.
	 *
	 *  Calls moveUnit() if the unit has moved by more than the hysteresis
	 *  distance.  Otherwise the grid coodinates of the unit remain unchanged.
	 *  New units are created with grid coordinates set to (0,0) (which is
	 *  outside the active area of the grid), so the first update will move
	 *  them onto the grid, update their visibility and aggregation, etc.
	 *
	 *  @return true if the unit motion exceeds the hysteresis distance.
	 */
	bool updatePosition(UnitWrapper *wrapper, const GridPoint new_position);

	/** Updates the position of a unit based on its current global position.
	 *  TODO This method is specific to LocalBattlefield and should be moved
	 *  there.
	 *
	 *  @return true if the unit motion exceeds the hysteresis distance.
	 */
	bool updatePosition(UnitWrapper *wrapper);

	/** Make a copy of the local units list.
	 */
	void getLocalUnits(std::vector<Unit> &units) const;

	/** Gets the current signature of the local units list.  Whenever
	 *  the list is updated, the signature is increased.
	 */
	int getLocalUnitsSignature() const;

private:

	/** Called when a static feature is being removed from the battlefield.
	 *  Subclasses can use this hook to perform specialized cleanup, such
	 *  as removing the feature from the scene graph.
	 */
	virtual void _removeStatic(Static &feature);

	/** Remove a unit from the list of human-controlled units.
	 *
	 *  @param wrapper The unit wrapper to remove.
	 *  @returns True if the object was found and removed.
	 */
	bool removeFromHumanUnits(UnitWrapper *wrapper);

	/** Remove a unit from the battlefield (internal).
	 *
	 *  This is an internal method; removeUnit() is the public method
	 *  for removing units from the battlefield.  Note that the public
	 *  method marks units for deletion at the next update.  This
	 *  method removes a unit immediately.
	 *
	 *  @param wrapper The unit wrapper to remove.
	 */
	void _removeUnit(UnitWrapper *wrapper);

	/** Clear all units and static features from the battlefield, freeing resources.
	 */
	void _clear();

	/** Remove all units that have been marked for removal by removeUnit().
	 */
	void _removeUnits();

	/** Updates the positions of all units in the battlefield.
	 */
	virtual void _updateUnitPositions();

	/** Setup the grid coordinates.
	 *
	 *  @param size The (linear) size of the theater (in meters).
	 *  @param hysteresis The minimum distance an object must move before aggregation,
	 *    visibility, and index coordinates are updated (in meters).
	 */
	void initializeGridCoordinates(int size, int hysteresis);

	// Hysteresis is the number of grid units that an object must move
	// before its grid coordinates are updated.  Grid coordinate changes
	// affect object aggregation/deaggregation, scene management, and
	// index server updates, all of which are spatially coarse grained
	// operations and tend to incur significant overhead.  Thus the
	// hysteresis should be set to be relatively high (e.g. 2000 m would
	// be reasonable for an aircraft).
	//
	// TODO Currently all objects use a single hysteresis value, but it
	// may be worth tailoring the value to the maximum speed of each
	// vehicle type (e.g. F-16 vs AH-64) or category (air/mud).  Another
	// option is to use a small value (say 100 m) for all objects but
	// limit the rate at which updates based on grid coordinates can
	// occur.  This would have the advantage of providing finer
	// granularity for potentially fast-moving objects that happen to be
	// moving slowly or not at all.
	double m_Hysteresis;

	/// An index of all static features by object id.
	StaticMap m_StaticMap;

	/// An index of all units by object id.
	UnitMap m_UnitMap;
	
	/// A vector of all human-controlled units in the battlefield.
	UnitVector m_HumanUnits;

	/// A list of all local unit ids.
	std::list<Unit> m_LocalUnits;

	/// A counter to keep track of changes to the local units list.
	int m_LocalUnitsSignature;

	/// A list of units to remove at the next update.
	IdVector m_UnitsToRemove;

	/// conversion factor from grid to global coordinates
	double m_GridToGlobalScale;

	/// conversion factor from global to grid coordinates
	double m_GlobalToGridScale;

	/// offset (in grid units) of the center of the theater; typically 1/2 * MAX_GRID_COORDINATE
	double m_GridOffset;

	/// Spatial index for all dynamic objects in the battlefield.
	ScopedPointer<QuadTree> m_DynamicIndex;

	/// Spatial index for all static features in the battlefield.
	ScopedPointer<QuadTree> m_StaticIndex;

	/// Spatial index for all tracking motion of dynamic objects.
	ScopedPointer<QuadTree> m_MotionIndex;
};

} // namespace csp

#endif // __CSPSIM_BATTLEFIELD_BATTLEFIELD_H__


