// Combat Simulator Project - CSPSim
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
 * @file Battlefield.cpp
 *
 */

#include <SimCore/Util/Log.h>
#include <SimCore/Battlefield/Battlefield.h>
#include <SimCore/Battlefield/SceneManager.h>


class Battlefield::ObjectWrapper: public Battlefield::QuadTreeChild {
public:
	ObjectWrapper(Object const &obj, GridCoordinate x, GridCoordinate y):
		QuadTreeChild(obj->id(), x, y),
		m_object(obj)
	{
	}
	inline Object const &object() const { return m_object; }
	inline Object &object() { return m_object; }
	inline bool operator==(Object const &o) { return object() == o; }
	inline bool operator!=(Object const &o) { return object() != o; }
private:
	Object m_object;
};


class Battlefield::StaticWrapper: public Battlefield::ObjectWrapper {
public:
	StaticWrapper(Static const &s, GridCoordinate x, GridCoordinate y): ObjectWrapper(s, x, y) { }
	inline Static const &feature() const { return object(); }
	inline Static &feature() { return object(); }
};


class Battlefield::UnitWrapper: public Battlefield::ObjectWrapper {
public:
	UnitWrapper(Unit const &u):
		ObjectWrapper(u, 0, 0),
		m_aggregation_count(0),
		m_remote(new RemoteConnection())
	{
		// note: coordinate initialization will be handled when the unit is first added
		// to the battlefield.
	}
	inline Unit const &unit() { return object(); }

	/** Test if the aggregation bubble count is zero.
	 *
	 *  @returns true if the object should be in the aggregated state.
	 */
	inline bool isAggregated() const { return m_aggregation_count == 0; }

	/** Set the aggregation bubble count.
	 *  @param count the new aggregation bubble count.
	 *  @returns true if the aggregation state has changed.
	 */
	bool setAggregationBubbleCount(unsigned count) {
		CSP_LOG(BATTLEFIELD, DEBUG, "setting aggregation count to " << count << " for " << *unit());
		bool state_change = ((count == 0 && m_aggregation_count > 0) ||
                             (count > 0 && m_aggregation_count == 0));
		m_aggregation_count = static_cast<simdata::uint16>(count);
		return state_change;
	}

	/** Get the aggregation bubble count.  If greater than zero, the
	 *  object should be deaggregated.
	 */
	unsigned getAggregationBubbleCount() const { return m_aggregation_count; }

	/** Increase the aggregation bubble count by one.
	 *  @returns true if the bubble count was previously zero (ie. the object
	 *           needs to be deaggregated).
	 */
	bool increaseAggregationBubbleCount() {
		CSP_LOG(BATTLEFIELD, DEBUG, "increasing aggregation count (from " << m_aggregation_count << ") for " << *unit());
		return (++m_aggregation_count == 1);
	}

	/** Decrease the aggregation bubble count by one.  This method should
	 *  not be called if the count is already zero (asserts).
	 *  @returns true if the new bubble count is zero (ie. the object
	 *           needs to be aggregated).
	 */
	bool decreaseAggregationBubbleCount() {
		CSP_LOG(BATTLEFIELD, DEBUG, "decreasing aggregation count (from " << m_aggregation_count << ") fort " << *unit());
		assert(m_aggregation_count > 0);
		return (m_aggregation_count > 0 && --m_aggregation_count == 0);
	}

private:
	simdata::uint16 m_aggregation_count;
	simdata::ScopedPointer<RemoteConnection> m_remote;
};


bool Battlefield::removeFromHumanUnits(ObjectId id) {
	for (unsigned i = 0; i < m_HumanUnits.size(); ++i) {
		if (static_cast<ObjectId>(m_HumanUnits[i]->unit()->id()) == id) {
			m_HumanUnits.erase(m_HumanUnits.begin() + i);
			return true;
		}
	}
	return false;
}

void Battlefield::_removeUnit(ObjectId id) {
	CSP_LOG(BATTLEFIELD, DEBUG, "removing unit " << id);

	// TODO send messages?

	// first find the unit
	UnitMap::iterator iter = m_UnitMap.find(id);
	assert(iter != m_UnitMap.end());
	if (iter == m_UnitMap.end()) return;
	UnitWrapper *wrapper = iter->second;
	assert(!wrapper->unit()->isStatic());

	// the object is first moved to a position outside the active
	// battlefield grid (0,0), which takes care of visibility and
	// aggregation updates.
	moveUnit(wrapper, GridPoint(0,0));

	// finally, remove the object from all indices, disconnect it from the
	// update master, and delete the wrapper.
	m_UnitMap.erase(iter);
	if (wrapper->unit()->isHuman()) {
		if (!removeFromHumanUnits(id)) {
			CSP_LOG(BATTLEFIELD, ERROR, "removing unregistered human-controlled unit");
			assert(false);
		}
	}
	m_DynamicIndex->remove(*wrapper);
	m_MotionIndex->remove(*wrapper);
	wrapper->unit()->disconnectFromUpdateMaster();
	delete wrapper;
}

void Battlefield::_clear() {
	CSP_LOG(BATTLEFIELD, DEBUG, "clearing battlefield");

	m_UnitsToRemove.clear();
	for (UnitMap::iterator iter = m_UnitMap.begin(); iter != m_UnitMap.end(); ++iter) {
		m_UnitsToRemove.push_back(iter->first);
	}
	_removeUnits();

	assert(m_UnitMap.empty());
	assert(m_HumanUnits.empty());
	assert(m_DynamicIndex->childCount() == 0);
	assert(m_MotionIndex->childCount() == 0);

	// eliminate outstanding references to StaticWrappers
	m_StaticIndex->clear();

	// delete StaticWrappers
	for (StaticMap::iterator siter = m_StaticMap.begin(); siter != m_StaticMap.end(); ++siter) {
		StaticWrapper *wrapper = static_cast<StaticWrapper*>(siter->second);
		m_StaticIndex->remove(*wrapper);
		if (m_SceneManager.valid() && wrapper->feature()->isVisible()) {
			m_SceneManager->scheduleHide(wrapper->feature());
		}
		delete wrapper;
	}
	m_StaticMap.clear();
	assert(m_StaticIndex->childCount() == 0);
}

void Battlefield::_removeUnits() {
	for (unsigned i = 0; i < m_UnitsToRemove.size(); ++i) {
		_removeUnit(m_UnitsToRemove[i]);
	}
	m_UnitsToRemove.clear();
}


void Battlefield::_updateUnitPositions() {
	UnitMap::iterator iter = m_UnitMap.begin();
	for ( ; iter != m_UnitMap.end(); ++iter) {
		updatePosition(iter->second);
	}
}

void Battlefield::initializeGridCoordinates(int size, int hysteresis) {
	double margin = 0.2;  // 20% extra at edges
	m_GridToGlobalScale = ((1.0 + 2.0 * margin) * size) / std::numeric_limits<GridCoordinate>::max();
	m_GlobalToGridScale = 1.0 / m_GridToGlobalScale;
	m_GridOffset = 0.5 * std::numeric_limits<GridCoordinate>::max();
	m_Hysteresis = static_cast<GridCoordinate>(hysteresis * m_GlobalToGridScale);
}

// TODO make the quadtree parameters configurable, or at least tailored to the
// theater.
Battlefield::Battlefield():
	hackObjectId(0),
	m_MotionIndex(new QuadTree(10, 4)),
	m_DynamicIndex(new QuadTree(16, 20)),
	m_StaticIndex(new QuadTree(16, 20)),
	m_CameraGridPosition(0,0)
{
	// TODO will want to tune the grid size to roughly match the terrain
	// size.  otherwise we end up with useless branches at the top of the
	// quadtrees, although these incur very little overhead.  Battlefield
	// should not have to know about either Theater or TerrainObject.
	// for now, just set the grid size to 4000 km with 2 km hysteris
	initializeGridCoordinates(4000000, 2000);
}

Battlefield::~Battlefield() {
	_clear();
}

void Battlefield::setSceneManager(simdata::Ref<SceneManager> manager) {
	// TODO add the ability to change/remove the manager (need to
	// remove all objects from the old manager and (potentially)
	// add them to the new manager)?
	assert(!m_SceneManager);
	m_SceneManager = manager;
}

void Battlefield::addUnit(Unit const &unit, bool human) {
	assert(!unit->isStatic());
	assert(unit->id() == 0);
	assert(!unit->isHuman());
	unit->setId(++hackObjectId);
	{ // hack
		if (unit->isRemote()) unit->setLocal();
	}
	UnitWrapper *wrapper = new UnitWrapper(unit);
	m_UnitMap[unit->id()] = wrapper;
	if (human) {
		unit->setHuman();
		m_HumanUnits.push_back(wrapper);
	}
	updatePosition(wrapper);
	unit->registerUpdate(&m_UnitUpdateMaster);
}

void Battlefield::removeUnit(ObjectId id) {
	m_UnitsToRemove.push_back(id);
}

void Battlefield::setHuman(ObjectId id, bool human) {
	UnitMap::iterator iter = m_UnitMap.find(id);
	assert(iter != m_UnitMap.end());
	if (iter == m_UnitMap.end()) return;
	UnitWrapper *wrapper = iter->second;
	assert(wrapper->unit()->isLocal());
	if (wrapper->unit()->isRemote()) return;
	if (human == wrapper->unit()->isHuman()) return;
	// TODO update the index server and/or clients
	if (human) {
		m_HumanUnits.push_back(wrapper);
		wrapper->unit()->setHuman();
		// update the aggregation by pretending a human unit was added from the battlefield.
		CSP_LOG(BATTLEFIELD, DEBUG, "setting " << *(wrapper->unit()) << " to human");
		updateAggregationHuman(wrapper, GridPoint(0,0), wrapper->point());
	} else {
		removeFromHumanUnits(id);
		// update the aggregation by pretending a human unit was removed from the battlefield.
		CSP_LOG(BATTLEFIELD, DEBUG, "setting " << *(wrapper->unit()) << " to agent");
		updateAggregationHuman(wrapper, wrapper->point(), GridPoint(0,0));
		wrapper->unit()->setAgent();
	}
}

void Battlefield::update(double dt) {
	m_UnitUpdateMaster.update(dt);
	_updateUnitPositions();
	_removeUnits();
}

void Battlefield::addStatic(Static const &feature) {
	if (feature->id() == 0) {
		// TODO feature ids should be assigned by the theater in a deterministic way
		CSP_LOG(BATTLEFIELD, ERROR, "HACK: generating unique id for feature");
		feature->setId(++hackObjectId);
	}
	simdata::Vector3 pos = feature->getGlobalPosition();
	GridCoordinate x = globalToGrid(pos.x());
	GridCoordinate y = globalToGrid(pos.y());
	CSP_LOG(BATTLEFIELD, ERROR, "adding static feature " << *feature << " @ " << x << ", " << y);
	StaticWrapper *wrapper = new StaticWrapper(feature, x, y);
	m_StaticMap[feature->id()] = wrapper;
	{ // debugging
		GridCoordinate old_x = wrapper->x();
		GridCoordinate old_y = wrapper->y();
		CSP_LOG(BATTLEFIELD, ERROR, "static feature actually " << *feature << " @ " << old_x << ", " << old_y);
	}
	m_StaticIndex->insert(*wrapper);
}

Battlefield::Unit Battlefield::getNextUnit(Unit const &unit, bool human, bool local, int /*category*/) {
	UnitMap::iterator iter = m_UnitMap.find(unit->id());
	if (iter == m_UnitMap.end()) return 0;
	UnitMap::iterator scan = iter;
	for (++scan; scan != iter; )  {
		Unit u = scan->second->unit();
		if ((u->isHuman() == human) && (u->isLocal() == local) /* && category... */) {
			return u;
		}
		if (++scan == m_UnitMap.end()) {
			scan = m_UnitMap.begin();
		}
	}
	return 0;
}

void Battlefield::setCamera(simdata::Vector3 const &eye_point, const simdata::Vector3& look_pos, const simdata::Vector3& up_vec) {
	// if there is no scene manager, then we ignore camera updates (which probably
	// shouldn't be occur anyway).
	if (!m_SceneManager) return;

	// first update the camera, which will force the local terrain to be loaded
	// before features are added to the scene (the terrain elevation must be
	// available for features to be position correctly). this works with demeter
	// since it blocks during the terrain load, but cspchunklod uses a threaded
	// loader.
	// TODO ElevationCorrection needs to be able to query the terrain object
	// to deterimine if elevation data is available, and block (in the feature
	// construction thread) if it isn't.
	CSP_LOG(BATTLEFIELD, DEBUG, "setCamera(): update scene camera");
	m_SceneManager->setCamera(eye_point, look_pos, up_vec);

	// if the camera has move sufficiently far, add and remove features and dynamic
	// objects from the scene based on the current camera position.
	GridPoint new_grid_position = globalToGrid(eye_point);
	if (hasMoved(m_CameraGridPosition, new_grid_position)) {
		CSP_LOG(BATTLEFIELD, DEBUG, "setCamera(): updating visibility " << new_grid_position.x() << ", " << new_grid_position.y());
		updateVisibility(m_CameraGridPosition, new_grid_position);
		m_CameraGridPosition = new_grid_position;
	}
}

void Battlefield::updateVisibility(GridPoint old_camera_position, GridPoint new_camera_position) {
	assert(m_SceneManager.valid());

	const double vis_bubble = m_SceneManager->getVisibleRange();
	const double vis_r2 = vis_bubble * vis_bubble;

	GridRegion old_region = makeGridRegionEnclosingCircle(old_camera_position, vis_bubble);
	GridRegion new_region = makeGridRegionEnclosingCircle(new_camera_position, vis_bubble);

	std::vector<QuadTreeChild*> show;
	std::vector<QuadTreeChild*> hide;

	const bool old_position_is_null = isNullPoint(old_camera_position);
	const bool new_position_is_null = isNullPoint(new_camera_position);

	if (old_region.overlaps(new_region)) {
		// usual case, overlapping old and new visibility regions.  do one query
		// and filter out the objects that enter and leave the visibility bubble.
		CSP_LOG(BATTLEFIELD, DEBUG, "updateVisibility(): small camera move");

		// construct a query region that includes both the old and new bubbles
		// and find all objects in that region.
		old_region.expand(new_region);
		std::vector<QuadTreeChild*> mixed;
		m_DynamicIndex->query(old_region, mixed);
		m_StaticIndex->query(old_region, mixed);

		// now filter the mixed list into the show and hide lists
		for (unsigned i = 0; i < mixed.size(); ++i) {
			bool in_old_bubble = (globalDistance2(old_camera_position, mixed[i]->point()) <= vis_r2) && !old_position_is_null;
			bool in_new_bubble = (globalDistance2(new_camera_position, mixed[i]->point()) <= vis_r2) && !new_position_is_null;
			if (in_old_bubble && !in_new_bubble) {
				hide.push_back(mixed[i]);
			} else if (in_new_bubble && !in_old_bubble) {
				show.push_back(mixed[i]);
			}
		}
	} else {
		// less common case, the camera has jumped so far that the old and new
		// visibility regions don't overlap.  this is easier, since no sorting
		// is required.
		CSP_LOG(BATTLEFIELD, DEBUG, "updateVisibility(): large camera move");
		if (!old_position_is_null) {
			m_DynamicIndex->query(old_region, hide);
			m_StaticIndex->query(old_region, hide);
		}
		if (!new_position_is_null) {
			m_DynamicIndex->query(new_region, show);
			m_StaticIndex->query(new_region, show);
		}
	}

	CSP_LOG(BATTLEFIELD, DEBUG, "updateVisibility(): hiding " << hide.size() << " objects");
	for (unsigned i = 0; i < hide.size(); ++i) {
		Object object = static_cast<ObjectWrapper*>(hide[i])->object();
		m_SceneManager->scheduleHide(object);
	}

	CSP_LOG(BATTLEFIELD, DEBUG, "updateVisibility(): showing " << show.size() << " objects");
	for (unsigned i = 0; i < show.size(); ++i) {
		Object object = static_cast<ObjectWrapper*>(show[i])->object();
		m_SceneManager->scheduleShow(object);
	}
}

bool Battlefield::updatePosition(UnitWrapper *wrapper) {
	simdata::Vector3 pos = wrapper->unit()->getGlobalPosition();
	CSP_LOG(BATTLEFIELD, DEBUG, "update position " << *(wrapper->unit()) << ": " << pos);
	const GridPoint new_position(globalToGrid(pos));
	if (hasMoved(wrapper->point(), new_position)) {
		moveUnit(wrapper, new_position);
		return true;
	}
	return false;
}

void Battlefield::moveUnit(UnitWrapper *wrapper, GridPoint const &new_position) {
	// TODO send a position update to the index server.  this also
	// needs to be coordinated with messages to add and remove
	// units from the battlefield.
	const GridPoint old_position = wrapper->point();
	CSP_LOG(BATTLEFIELD, DEBUG, "moving object " << *(wrapper->unit()) << " from " << old_position << " to " << new_position);
	m_MotionIndex->remove(*wrapper);
	m_DynamicIndex->remove(*wrapper);
	wrapper->mutablePoint() = new_position;
	m_MotionIndex->insert(*wrapper);
	m_DynamicIndex->insert(*wrapper);
	updateAggregation(wrapper, old_position, new_position);
	updateUnitVisibility(wrapper, old_position, new_position);
}

bool Battlefield::updateUnitVisibility(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position) {
	if (!m_SceneManager.valid()) return false;
	const double vis_bubble = m_SceneManager->getVisibleRange();
	const double vis_r2 = vis_bubble * vis_bubble;
	const bool in_old_bubble = (globalDistance2(m_CameraGridPosition, old_position) <= vis_r2) && !isNullPoint(old_position);
	const bool in_new_bubble = (globalDistance2(m_CameraGridPosition, new_position) <= vis_r2) && !isNullPoint(new_position);
	if (in_old_bubble == in_new_bubble) return false;
	if (in_new_bubble) {
		m_SceneManager->scheduleShow(wrapper->unit());
	} else {
		m_SceneManager->scheduleHide(wrapper->unit());
	}
	return true;
}

Battlefield::GridRegion Battlefield::makeGridRegionEnclosingCircle(GridCoordinate x, GridCoordinate y, double radius) {
	radius *= m_GlobalToGridScale;
	if (radius < 0.0) {
		radius = 0.0;
	} else if (radius > std::numeric_limits<GridCoordinate>::max()) {
		radius = std::numeric_limits<GridCoordinate>::max();
	}
	GridCoordinate grad = static_cast<GridCoordinate>(radius);
	return GridRegion(gridSub(x, grad), gridSub(y, grad), gridAdd(x, grad), gridAdd(y, grad));
}

void Battlefield::updateAggregationHuman(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position) {
	// moving a human unit, which may cause other non-human units to enter
	// or leave its bubble and change aggregation state.  the basic approach
	// is to find all local units within the old+new bubbles, and update their
	// aggregation counts.

	const double air_bubble = wrapper->unit()->getAirBubbleRadius();
	const double ground_bubble = wrapper->unit()->getGroundBubbleRadius();

	// construct query region that includes both the old and new bubbles and
	// find all dynamic objects in that region.
	const double max_r = std::max(air_bubble, ground_bubble);
	GridRegion region = makeGridRegionEnclosingCircle(new_position, max_r);
	region.expand(makeGridRegionEnclosingCircle(old_position, max_r));
	std::vector<QuadTreeChild*> result;
	m_DynamicIndex->query(region, result);

	const bool old_position_is_null = isNullPoint(old_position);
	const bool new_position_is_null = isNullPoint(new_position);

	for (unsigned i = 0; i < result.size(); ++i) {
		UnitWrapper *uw = static_cast<UnitWrapper*>(result[i]);

		// only the owner of an object can update its aggregation state
		if (uw->unit()->isRemote()) continue;

		// skip self
		if (uw == wrapper) continue;

		// filter objects that have entered or left the bubble
		const double old_r2 = globalDistance2(old_position, uw->point());
		const double new_r2 = globalDistance2(new_position, uw->point());
		const double bubble_r = uw->unit()->isAir() ? air_bubble : ground_bubble;
		const double bubble_r2 = bubble_r * bubble_r;
		const bool in_old_bubble = old_r2 <= bubble_r2 && !old_position_is_null;
		const bool in_new_bubble = new_r2 <= bubble_r2 && !new_position_is_null;

		if (in_old_bubble && !in_new_bubble) {
			if (uw->decreaseAggregationBubbleCount()) {
				uw->unit()->aggregate();
			}
		} else if (in_new_bubble && !in_old_bubble) {
			if (uw->increaseAggregationBubbleCount()) {
				uw->unit()->deaggregate();
			}
		}
	}
}

void Battlefield::updateAggregationAgent(UnitWrapper *wrapper, GridPoint const &/*old_position*/, GridPoint const &new_position) {
	// only the owner of an object can update its aggregation state
	if (wrapper->unit()->isRemote()) return;

	// moving an agent-controlled unit, so it may enter or leave all bubbles and
	// need to change its aggregation state.  to determine this we just loop
	// through all human units and check overlap with their bubbles.  this is
	// probably fast enough for up to a hundred or so human units.  for much
	// larger games it may be better to keep a separate spatial index for human
	// units.
	const bool air = wrapper->unit()->isAir();
	simdata::uint16 bubble_count = 0;
	if (!isNullPoint(new_position)) {
		for (unsigned i = 0; i < m_HumanUnits.size(); ++i) {
			// square of the distance between the new position and the human-controlled unit
			double separation_2 = globalDistance2(new_position, m_HumanUnits[i]->point());
			// human units project aggregation bubbles of different sizes for
			// air and ground units.
			double bubble_radius = air ? m_HumanUnits[i]->unit()->getAirBubbleRadius() : \
			                             m_HumanUnits[i]->unit()->getGroundBubbleRadius();
			bool in_bubble = bubble_radius * bubble_radius >= separation_2;
			if (in_bubble) bubble_count++;
		}
	}

	// always reset the aggregation count (safer, although it may mask
	// counting errors elsewhere).
	if (wrapper->setAggregationBubbleCount(bubble_count)) {
		// aggregation state has changed, so inform the unit.
		if (bubble_count > 0) {
			wrapper->unit()->deaggregate();
		} else {
			wrapper->unit()->aggregate();
		}
	}
}

void Battlefield::updateAggregation(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position) {
	if (wrapper->unit()->isHuman()) {
		updateAggregationHuman(wrapper, old_position, new_position);
	} else {
		updateAggregationAgent(wrapper, old_position, new_position);
	}
}


