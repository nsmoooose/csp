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



Battlefield::Unit Battlefield::getNextUnit(Unit const &unit, int human, int local, int /*category*/) {
	UnitMap::iterator iter = m_UnitMap.find(unit->id());
	if (iter != m_UnitMap.end()) {
		for (UnitMap::iterator scan = iter; 1; )  {
			if (++scan == m_UnitMap.end()) scan = m_UnitMap.begin();
			if (scan == iter) break;
			Unit u = scan->second->unit();
			if ((human < 0) || ((u->isHuman() == (human > 0))) && ((local < 0) || (u->isLocal() == (local > 0))) /* && category... */) return u;
		}
	}
	return 0;
}

void Battlefield::getLocalUnits(std::vector<Unit> &units) const {
	units.clear();
	// m_LocalUnits.size() is slow, so avoid std::copy (which requires that the
	// destination be preallocated).  push_back should be fast as long as the
	// caller reuses the vector.
	for (std::list<Unit>::const_iterator iter = m_LocalUnits.begin(); iter != m_LocalUnits.end(); ++iter) {
		units.push_back(*iter);
	}
}

int Battlefield::getLocalUnitsSignature() const {
	return m_LocalUnitsSignature;
}

bool Battlefield::removeFromHumanUnits(UnitWrapper *wrapper) {
	for (unsigned i = 0; i < m_HumanUnits.size(); ++i) {
		if (m_HumanUnits[i] == wrapper) {
			m_HumanUnits.erase(m_HumanUnits.begin() + i);
			return true;
		}
	}
	return false;
}

void Battlefield::_removeUnit(UnitWrapper *wrapper) {
	const ObjectId id = wrapper->id();
	CSP_LOG(BATTLEFIELD, DEBUG, "removing unit " << id);

	// TODO send messages?

	// first find the unit
	assert(!(wrapper->unit().valid() && wrapper->unit()->isStatic()));

	// the object is first moved to a position outside the active
	// battlefield grid (0,0), which takes care of visibility and
	// aggregation updates.
	const GridPoint old_position = wrapper->point();
	moveUnit(wrapper, old_position, GridPoint(0,0));

	// finally, remove the object from all indices, disconnect it from the
	// update master, and delete the wrapper.
	UnitMap::iterator iter = m_UnitMap.find(id);
	assert(iter != m_UnitMap.end());
	m_UnitMap.erase(iter);
	m_DynamicIndex->remove(*wrapper);
	m_MotionIndex->remove(*wrapper);
	if (wrapper->unit().valid()) {
		wrapper->unit()->disconnectFromUpdateMaster();
		if (wrapper->unit()->isHuman()) {  // XXX placeholders can be human, no?
			if (!removeFromHumanUnits(wrapper)) {
				CSP_LOG(BATTLEFIELD, ERROR, "removing unregistered human-controlled unit");
				assert(false);
			}
		}
		if (wrapper->unit()->isLocal()) {
			m_LocalUnits.remove(wrapper->unit());
			m_LocalUnitsSignature++;
		}
	}
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
	assert(m_LocalUnits.empty());
	assert(m_DynamicIndex->childCount() == 0);
	assert(m_MotionIndex->childCount() == 0);

	// eliminate outstanding references to StaticWrappers
	m_StaticIndex->clear();

	// delete StaticWrappers
	for (StaticMap::iterator siter = m_StaticMap.begin(); siter != m_StaticMap.end(); ++siter) {
		StaticWrapper *wrapper = static_cast<StaticWrapper*>(siter->second);
		_removeStatic(wrapper->feature());
		delete wrapper;
	}
	m_StaticMap.clear();
	assert(m_StaticIndex->childCount() == 0);
}

void Battlefield::_removeStatic(Static &) {
}

void Battlefield::_removeUnits() {
	for (unsigned i = 0; i < m_UnitsToRemove.size(); ++i) {
		const ObjectId id = m_UnitsToRemove[i];
		UnitMap::iterator iter = m_UnitMap.find(id);
		assert(iter != m_UnitMap.end());
		if (iter != m_UnitMap.end()) {
			_removeUnit(iter->second);
		}
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
	m_LocalUnitsSignature(0),
	m_DynamicIndex(new QuadTree(16, 20)),
	m_StaticIndex(new QuadTree(16, 20)),
	m_MotionIndex(new QuadTree(10, 4))
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

void Battlefield::addUnit(UnitWrapper *wrapper) {
	assert(wrapper && (wrapper->id() != 0));
	if (wrapper->unit().valid()) {
		assert(!wrapper->unit()->isStatic());
		assert(!wrapper->unit()->isHuman());
		if (wrapper->unit()->isLocal()) {
			m_LocalUnits.push_back(wrapper->unit());
			m_LocalUnitsSignature++;
		}
	}
	m_UnitMap[wrapper->id()] = wrapper;
	updatePosition(wrapper);
}

void Battlefield::removeUnit(ObjectId id) {
	m_UnitsToRemove.push_back(id);
}

void Battlefield::update(double) {
	_updateUnitPositions();
	_removeUnits();
}

void Battlefield::addStatic(Static const &feature) {
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

bool Battlefield::updatePosition(UnitWrapper *wrapper, const GridPoint new_position) {
	if (hasMoved(wrapper->point(), new_position)) {
		if (wrapper->unit().valid() && !wrapper->unit()->isContact()) {
			CSP_LOG(BATTLEFIELD, DEBUG, "update position " << *(wrapper->unit()) << ": " << wrapper->unit()->getGlobalPosition());
		}
		const GridPoint old_position = wrapper->point();
		moveUnit(wrapper, old_position, new_position);
		return true;
	}
	return false;
}

bool Battlefield::updatePosition(UnitWrapper *wrapper) {
	Unit unit = wrapper->unit();
	if (!unit || unit->isContact()) return false;
	simdata::Vector3 pos = unit->getGlobalPosition();
	CSP_LOG(BATTLEFIELD, TRACE, "update position " << *unit << ": " << pos);
	return updatePosition(wrapper, globalToGrid(pos));
}

void Battlefield::moveUnit(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position) {
	CSP_LOG(BATTLEFIELD, DEBUG, "moving object " << wrapper->id() << " from " << old_position << " to " << new_position);
	if (!isNullPoint(old_position)) {
		m_MotionIndex->remove(*wrapper);
		m_DynamicIndex->remove(*wrapper);
	}
	wrapper->mutablePoint() = new_position;
	if (!isNullPoint(new_position)) {
		m_MotionIndex->insert(*wrapper);
		m_DynamicIndex->insert(*wrapper);
	}
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

void Battlefield::setHumanUnit(UnitWrapper *wrapper, bool human) {
	assert(wrapper->unit().valid());
	// TODO update the index server and/or clients (will be done by subclasses
	// overriding this method)
	if (human) {
		m_HumanUnits.push_back(wrapper);
		wrapper->unit()->setHuman();
		// update the aggregation by pretending a human unit was added from the battlefield.
		CSP_LOG(BATTLEFIELD, DEBUG, "setting " << *(wrapper->unit()) << " to human");
		// XXX XXX updateAggregationHuman(wrapper, GridPoint(0,0), wrapper->point());
	} else {
		removeFromHumanUnits(wrapper);
		// update the aggregation by pretending a human unit was removed from the battlefield.
		CSP_LOG(BATTLEFIELD, DEBUG, "setting " << *(wrapper->unit()) << " to agent");
		// XXX XXX updateAggregationHuman(wrapper, wrapper->point(), GridPoint(0,0));
		wrapper->unit()->setAgent();
	}
}

void Battlefield::initializeLocalUnit(Unit unit, bool local) {
	assert(unit.valid() && unit->id() == 0);
	if (unit->isLocal() == local) return;
	if (local) {
		unit->setLocal();
	} else {
		unit->setRemote();
	}
}

Battlefield::UnitWrapper *Battlefield::findUnitWrapper(ObjectId id) {
	UnitMap::iterator iter = m_UnitMap.find(id);
	if (iter == m_UnitMap.end()) return 0;
	return iter->second;
}

void Battlefield::setHumanUnit(ObjectId id, bool human) {
	UnitWrapper *wrapper = findUnitWrapper(id);
	if (wrapper && wrapper->unit().valid() && wrapper->unit()->isHuman() != human) {
		setHumanUnit(wrapper, human);
	}
}


void Battlefield::ObjectWrapper::setObject(Object const &object) {
	if (object.valid()) {
		SIMDATA_VERIFY(!m_Object);
		SIMDATA_VERIFY_EQ(object->id(), id());
		SIMDATA_VERIFY_EQ(object->getObjectPath(), m_Path);
	}
	m_Object = object;
}

void Battlefield::StaticWrapper::setStatic(Static const &object) {
	if (object.valid()) {
		SIMDATA_VERIFY(!(object->isStatic()));
	}
	setObject(object);
}

Battlefield::UnitWrapper::UnitWrapper(Unit const &u, PeerId owner): ObjectWrapper(u, 0, 0), m_Owner(owner) {
	SIMDATA_VERIFY_EQ(owner == 0, u->isLocal());
}

Battlefield::UnitWrapper::UnitWrapper(ObjectId id, simdata::Path const &path, PeerId owner): ObjectWrapper(id, path, 0, 0), m_Owner(owner) {
	SIMDATA_VERIFY_NE(owner, 0);
}


