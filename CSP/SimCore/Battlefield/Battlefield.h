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
 * TODO (partially scene related)
 *  - network interface and remote object hooks
 *  - visibility range should be tied to object size
 *  - alpha fading of objects at visibility limit and lod
 *  - implement a threaded scene manager
 **/

#ifndef __BATTLEFIELD_H__
#define __BATTLEFIELD_H__

#include <map>
#include <vector>

#include <SimData/Ref.h>
#include <SimData/ScopedPointer.h>
#include <SpatialIndex/QuadTree.h>

#include <SimCore/Util/SynchronousUpdate.h>
#include "SimObject.h"


class SceneManager;

namespace simdata {
	class Vector3;
}


/** Battlefield management class.
 *
 *  Battlefield manages static and dynamic objects in the game.  It maintains
 *  several spatial indices to keep track of object locations.  When attached
 *  to a SceneManager, it takes care of updating the camera position and
 *  scheduling the addition and removal of objects from the scene.  It also
 *  manages aggregation bubbles and coordinates object ownership and updates
 *  between remote peers.
 */
class Battlefield: public simdata::Referenced {
public:
	// we currently refer to all objects (DynamicObjects and FeatureGroups)
	// via the SimObject base class.  this may change in the future, hence
	// the typedef indirection.
	typedef simdata::Ref<SimObject> Object;
	/// reference for DynamicObject types
	typedef simdata::Ref<SimObject> Unit;
	/// reference for FeatureGroup types
	typedef simdata::Ref<SimObject> Static;

private:

	// pull in some shorthand types for quadtree components
	typedef SpatialIndex::IQuadTree::QuadTree QuadTree;
	typedef SpatialIndex::IQuadTree::Child QuadTreeChild;
	typedef SpatialIndex::IQuadTree::Coordinate GridCoordinate;
	typedef SpatialIndex::IQuadTree::Point GridPoint;
	typedef SpatialIndex::IQuadTree::Region GridRegion;

	// more shorthand typedefs
	typedef SimObject::ObjectId ObjectId;
	typedef unsigned OwnerId;

	class RemoteConnection {
	};

	/** Object wrapper is a base class used for inserting objects into
	 *  the quadtree indices.  The QuadTreeChild base class maintains the
	 *  grid coordinates of the object and the object id.  We bundle in
	 *  the object reference for convenience.
	 */
	class ObjectWrapper;

	/** Wrapper class for inserting static objects into a quadtree index.
	 */
	class StaticWrapper;

	/** Wrapper for inserting dynamic objects into a quadtree index.
	 *  Keeps track of the aggregation bubble count for an object, which
	 *  is the number aggregation bubbles projected by human-controlled
	 *  vehicles that overlap the object's position.  If this count is
	 *  greater than zero, the object should be deaggregated.
	 */
	class UnitWrapper;

	/** Remove a unit from the list of human-controlled units.
	 *
	 *  @param id The id of the object to remove.
	 *  @returns True if the object was found and removed.
	 */
	bool removeFromHumanUnits(ObjectId id);

	/** Remove a unit from the battlefield (internal).
	 *
	 *  This is an internal method; removeUnit() is the public method
	 *  for removing units from the battlefield.  Note that the public
	 *  method marks units for deletion at the next update.  This
	 *  method removes a unit immediately.
	 *
	 *  @param id The id of the object to remove.
	 */
	void _removeUnit(ObjectId id);

	/** Clear all units and static features from the battlefield, freeing resources.
	 */
	void _clear();

	/** Remove all units that have been marked for removal by removeUnit().
	 */
	void _removeUnits();

	/** Updates the positions of all units in the battlefield.
	 */
	void _updateUnitPositions();

	/** Setup the grid coordinates.
	 *
	 *  @param size The (linear) size of the theater (in meters).
	 *  @param hysteresis The minimum distance an object must move before aggregation,
	 *    visibility, and index coordinates are updated (in meters).
	 */
	void initializeGridCoordinates(int size, int hysteresis);

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

public:

	/** Construct a new battlefield.
	 */
	Battlefield();

	/** Destroy the battlefield and free resources.
	 */
	~Battlefield();

	/** Attach a scene manager to the battlefield.  Unless a scene manager is
	 *  set, camera and object visibility updates will not be computed.
	 */
	void setSceneManager(simdata::Ref<SceneManager> manager);

	/** Temporary hack for assigning object ids to units and static features.
	 *  Eventually the Index Server will be responsible for assigning unique
	 *  ids.
	 */
	ObjectId hackObjectId;

	/** Add a (local) unit to the battlefield.
	 *
	 *  The initial state of the unit must be agent-controlled.
	 *
	 *  @param unit The unit to add.
	 *  @param human True if the unit is controlled by a human player.
	 */
	void addUnit(Unit const &unit, bool human);

	/** Remove a unit from the battlefield.
	 *
	 *  This method marks the unit for removal, which will occur at the
	 *  next call to update().
	 *
	 *  @param id The id of the unit to remove.
	 */
	void removeUnit(ObjectId id);

	/** Set a (local) unit to be human-controlled or agent-controlled.
	 *
	 *  All human/agent transitions are managed by the battlefield,
	 *  since these changes affect the aggregation state of other
	 *  units.
	 *
	 *  @param id The id of the unit to change.
	 *  @param human True if the unit is controlled by a human player.
	 */
	void setHuman(ObjectId id, bool human);

	/** Update the battlefield.
	 *
	 *  Triggers update callbacks of all units in the battlefield, tracks
	 *  the motion of units (and in turn visibility and aggregation state),
	 *  and removes units that have been marked by removeUnit().
	 *
	 *  @param dt The elapsed time since the last update.
	 */
	void update(double dt);

	/** Add a static feature to the battlefield.
	 *
	 *  All features in the theater are typically loaded into the battlefield
	 *  shortly after it is constructed.
	 *
	 *  @param feature The feature to add.
	 */
	void addStatic(Static const &feature);

	/** Very basic unit lookup method.
	 *
	 *  Finds the next unit after the specified unit that matching the search
	 *  criteria.  Repeated calls, passing the unit returned by the previous
	 *  result, will cycle through all units of the specified type.
	 *
	 *  TODO implement a more robust query mechanism for both units and features.
	 *
	 *  @param unit The current unit.
	 *  @param human If true, only return human-controlled objects; otherwise
	 *               only return agent-controlled objects.
	 *  @param local If true, only return local units; otherwise only return
	 *               remote objects.
	 *  @param category Not currently implemented; intented to select specific
	 *         types of units (or eventually, features).
	 *  @returns The next matching unit; null otherwise.
	 */
	Unit getNextUnit(Unit const &unit, bool human, bool local, int category);

	/** Update the camera position.
	 *
	 *  Adjusts the camera position and object visibility.  Does nothing unless
	 *  a SceneManager has been specified with setSceneManager().  Should be
	 *  called everytime the camera moves (typically every frame).
	 *
	 *  @param eye_point the position of the camera in global coordinates.
	 *  @param look_pos the aim point of the camera in global coordinates.
	 *  @param up_vec a vector fixing the camera orientation around the viewing direction.
	 */
	void setCamera(simdata::Vector3 const &eye_point, const simdata::Vector3& look_pos, const simdata::Vector3& up_vec);

private:

	/** Add and remove objects from the scene as the camera moves.
	 *
	 *  Called by setCamera() to update the scene when the camera grid position has
	 *  changed.  Although setCamera() may be called once for every frame, the grid
	 *  position of the camera should change much more slowly (with hysteresis), so
	 *  this method should not incur much overhead.
	 *
	 *  @param old_camera_position the grid coordinates of the previous camera position.
	 *  @param new_camera_position the grid coordinates of the new camera position.
	 */
	void updateVisibility(GridPoint old_camera_position, GridPoint new_camera_position);

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
	inline GridPoint globalToGrid(simdata::Vector3 const &pos) {
		return GridPoint(globalToGrid(pos.x()), globalToGrid(pos.y()));
	}

	/** Convert a grid coordinate to global coordinates.
	 */
	inline double gridToGlobal(GridCoordinate x) {
		return (static_cast<double>(x) - m_GridOffset) * m_GridToGlobalScale;
	}

	/** Update the position of a unit.
	 *
	 *  Calls moveUnit() if the unit has moved by more than the hysteresis
	 *  distance.  Otherwise the grid coodinates of the unit remain unchanged.
	 *  New units are created with grid coordinates set to (0,0) (which is
	 *  outside the active area of the grid), so the first update will move
	 *  them onto the grid, update their visibility and aggregation, etc.
	 */
	bool updatePosition(UnitWrapper *wrapper);

	/** Move a unit, updating the spatial indices and managing aggregation
	 *  and visibility changes.
	 *
	 *  @param wrapper The unit to move.
	 *  @param new_position The new grid position.
	 */
	void moveUnit(UnitWrapper *wrapper, GridPoint const &new_position);

	/** Update the visibility of a unit.
	 *
	 *  If a scene manager has been set, this method will schedule a unit to be
	 *  show or hidden if it moves into or out of the visibility bubble, respectively.
	 *
	 *  @param wrapper The unit that has moved.
	 *  @param old_position The old grid position of the unit.
	 *  @param new_position The new grid position of the unit.
	 *  @returns True if the visibility state changed.
	 */
	bool updateUnitVisibility(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position);

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

	/** Update the aggregation state of agent-controlled units when a human unit moves.
	 *
	 *  Do not call this method directly; instead use updateAggregation().
	 *
	 *  @param wrapper An human-controlled unit that has moved.
	 *  @param old_position The old position of the unit.
	 *  @param new_position The new position of the unit.
	 */
	void updateAggregationHuman(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position);

	/** Update the aggregation state of an agent-controlled unit that has moved.
	 *
	 *  Do not call this method directly; instead use updateAggregation().
	 *
	 *  @param wrapper An agent-controlled unit that has moved.
	 *  @param old_position The old position of the unit.
	 *  @param new_position The new position of the unit.
	 */
	void updateAggregationAgent(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position);

	/** Update the aggregation state of a unit that has moved.
	 *
	 *  @param wrapper A unit that has moved.
	 *  @param old_position The old position of the unit.
	 *  @param new_position The new position of the unit.
	 */
	void updateAggregation(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position);

	typedef std::map<ObjectId, StaticWrapper*> StaticMap;
	typedef std::vector<StaticWrapper*> StaticVector;
	typedef std::map<ObjectId, UnitWrapper*> UnitMap;
	typedef std::vector<UnitWrapper*> UnitVector;
	typedef std::vector<ObjectId> IdVector;

	/// An index of all static features by object id.
	StaticMap m_StaticMap;

	/// An index of all units by object id.
	UnitMap m_UnitMap;
	
	/// A list of all human-controlled units in the battlefield.
	UnitVector m_HumanUnits;

	/// A list of units to remove at the next update.
	IdVector m_UnitsToRemove;

	/// conversion factor from grid to global coordinates
	double m_GridToGlobalScale;
	/// conversion factor from global to grid coordinates
	double m_GlobalToGridScale;
	/// offset (in grid units) of the center of the theater; typically 1/2 * MAX_GRID_COORDINATE
	double m_GridOffset;

	/// Update master that manages synchronous updates of units in the battlefield.
	UpdateMaster m_UnitUpdateMaster;

	/// Spatial index for dynamic objects that have moved recently.
	simdata::ScopedPointer<QuadTree> m_MotionIndex;
	/// Spatial index for all dynamic objects in the battlefield.
	simdata::ScopedPointer<QuadTree> m_DynamicIndex;
	/// Spatial index for all static features in the battlefield.
	simdata::ScopedPointer<QuadTree> m_StaticIndex;

	/// Scene manager; used to add and remove units and static features from the scene (if present).
	simdata::Ref<SceneManager> m_SceneManager;

	/// The current position of the camera, in grid coordinates.
	GridPoint m_CameraGridPosition;
};

#endif // __BATTLEFIELD_H__


