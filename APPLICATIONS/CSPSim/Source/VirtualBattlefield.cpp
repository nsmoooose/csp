// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file VirtualBattlefield.cpp
 *
 **/


/*
 * TODO 
 *   (many things)
 *   externalize various parametrs: cellsize, bubble radii, etc.
 *   compute gridsize based on terrain size
 *   ...
 */

#include "VirtualBattlefield.h"
#include "VirtualScene.h"
#include "TerrainObject.h"
#include "DynamicObject.h"
#include "Theater.h"
#include "LogStream.h"
#include "Config.h"

#include <SimData/Types.h>
#include <SimData/Math.h>

#include <sstream>
#include <algorithm>


using simdata::Ref;


VirtualBattlefield::VirtualBattlefield()
{
	m_Debug = g_Config.getInt("Debug", "Battlefield", 0, true);
	m_Deleted = 0;
	m_CellSize = 10000.0; // 10 km
	m_GridSizeX = 100; // 1000 km
	m_GridSizeY = 100; // 1000 km
	m_Cells.resize(m_GridSizeX * m_GridSizeY);
	m_Clean = false;
	m_CameraIndex = -1;
}

VirtualBattlefield::~VirtualBattlefield()
{
	assert(m_Clean);
}

int VirtualBattlefield::create()
{
	return 1;
}

void VirtualBattlefield::cleanup()
{
	_cleanupActiveCells();
	m_Clean = true;
}

void VirtualBattlefield::setScene(simdata::Ref<VirtualScene> scene) 
{
	m_Scene = scene;
}


float VirtualBattlefield::getElevation( float x,float y ) const
{
	if (!m_Terrain) {
		return 0.0;
	} else {
		return m_Terrain->getElevation(x,y);
	}
}

void VirtualBattlefield::getNormal(float x, float y, float & normalX, 
                                   float & normalY, float & normalZ ) const
{
	normalX = 0; normalY = 0; normalZ = 1;
	if (m_Terrain.valid()) {
		m_Terrain->getNormal(x,y, normalX, normalY, normalZ);
	}
}



void VirtualBattlefield::onUpdate(float dt)
{
	updateAllUnits(dt);
	_updateActiveCells(dt);
}



void VirtualBattlefield::addUnit(Unit const &unit)
{
	assert(unit.valid());
	m_UnitList.push_front(UnitWrapper(unit));
}


void VirtualBattlefield::removeUnit(Unit const &unit)
{
	assert(unit.valid());
	UnitList::iterator i = m_UnitList.begin();
	UnitList::const_iterator end = m_UnitList.end();
	for (; i != end; ++i) {
		if (i->unit == unit) {
			i->flags |= FLAG_DELETE;
			m_Deleted++;
			return;
		}
	}
	assert(0);
}

void VirtualBattlefield::setHuman(Unit const &unit, bool human) {
	assert(unit.valid());
	UnitList::iterator i = m_UnitList.begin();
	UnitList::const_iterator end = m_UnitList.end();
	for (; i != end; ++i) {
		if (i->unit == unit) {
			_setHuman(*i, human);
			return;
		}
	}
	assert(0);
}

void VirtualBattlefield::removeUnitsMarkedForDelete()
{
	if (m_Deleted == 0) return;
	UnitList::iterator i = m_UnitList.begin();
	UnitList::const_iterator end = m_UnitList.end();
	for (; i != end; ++i) {
		if (i->flags & FLAG_DELETE) {
			_moveUnit(*i, -1);
			i = m_UnitList.erase(i);
			if (!--m_Deleted) return;
		}
	}
}

void VirtualBattlefield::removeAllUnits()
{
	while (!m_UnitList.empty()) {
		UnitWrapper wrapper = m_UnitList.front();
		_moveUnit(wrapper, -1);
		m_UnitList.pop_front();
	}
	m_Deleted = 0;
}

void VirtualBattlefield::updateAllUnits(float dt)
{
	UnitList::iterator i = m_UnitList.begin();
	UnitList::const_iterator end = m_UnitList.end();
	for (; i != end ; ++i) {
	  	_updateUnit(*i, dt);
	}
}

void VirtualBattlefield::updateOrigin(simdata::Vector3 const &origin) {
	if (m_Scene.valid()) {
		std::list<ActiveCell*>::iterator i = m_ActiveCells.begin();
		std::list<ActiveCell*>::iterator j = m_ActiveCells.end();
		for (; i != j ; ++i) {
			(*i)->updateScene(origin);
		}
	}
}

VirtualBattlefield::Unit VirtualBattlefield::getNextUnit(Unit const &unit, int human, int local, int category)
{
	UnitList::iterator i = m_UnitList.begin();
	UnitList::const_iterator end = m_UnitList.end();
	for (; i != end; ++i) {
		if (i->unit == unit) break;
	}
	if (i == end) {
		return unit;
	}
	do {
		if (++i == end) i = m_UnitList.begin();
		if ((human >= 0) && (i->unit->isHuman() != (human != 0))) continue;
		if ((local >= 0) && (i->unit->isLocal() != (local != 0))) continue;
		break;
	} while (unit != i->unit);
	return i->unit;
}


void VirtualBattlefield::setTerrain(simdata::Ref<TerrainObject> terrain) {
	m_Terrain = terrain;
}


void VirtualBattlefield::saveSnapshot() {
	static int index = 0;
	static unsigned char *savebuffer = 0;
	static int n;
	if (savebuffer == 0) {
		n = m_GridSizeX*m_GridSizeY;
		savebuffer = new unsigned char[17+n];
		sprintf((char*)savebuffer, "P5 %4d %4d 255\n", m_GridSizeX, m_GridSizeY);
	}
	char fn[80];
	sprintf(fn, "battlefield%04d.pgm", index++);
	FILE *fp = fopen(fn, "wb");
	int i;
	unsigned char *b = savebuffer+17;
	for (i = 0; i < n; i++) {
		Cell &cell = m_Cells[i];
		if (!cell.isEmpty()) {
			*b++ = 255;
		} else {
			int x = 0;
			if (cell.inAirBubble()) { x += 80; }
			if (cell.inMudBubble()) { x += 80; }
			*b++ = (unsigned char) x;
		}
	}
	fwrite(savebuffer, 1, 17+n, fp);
	fclose(fp);
}

void VirtualBattlefield::toCell(int idx, int &x, int &y) {
	y = idx / m_GridSizeX;
	x = idx % m_GridSizeX;
	if (y < 0) y = 0;
	if (y >= m_GridSizeY) y = m_GridSizeY-1;
}


void VirtualBattlefield::_setHuman(UnitWrapper &wrapper, bool human) {
	Unit &unit = wrapper.unit;
	if (unit->isHuman() == human) return;
	if (wrapper.idx != -1) {
		if (human) {
			_updateBubble(-1, wrapper.idx, 100000.0, AIR_BUBBLE);
			_updateBubble(-1, wrapper.idx, 40000.0, MUD_BUBBLE);
		} else {
			_updateBubble(wrapper.idx, -1, 100000.0, AIR_BUBBLE);
			_updateBubble(wrapper.idx, -1, 40000.0, MUD_BUBBLE);
		}
	}
	unit->setHuman(human); 
}

void VirtualBattlefield::_moveUnit(UnitWrapper &wrapper, int idx) {
	Unit &unit = wrapper.unit;
	bool oldBubble, newBubble;
	bool remove = idx < 0;
	bool add = wrapper.idx < 0;
	if (m_Debug > 0) {
		std::cout << "move from " << wrapper.idx << " to " << idx << std::endl;
	}
	if (unit->isHuman()) {
		// XXX parameterize ranges
		_updateBubble(wrapper.idx, idx, 100000.0, AIR_BUBBLE);
		_updateBubble(wrapper.idx, idx, 40000.0, MUD_BUBBLE);
	} else {
		// non-human unit, check if it has entered or left a bubble.
		bool air = unit->isAir();
		if (air) {
			oldBubble = add ? false : m_Cells[wrapper.idx].inAirBubble();
			newBubble = remove ? false : m_Cells[idx].inAirBubble();
		} else {
			oldBubble = add ? false : m_Cells[wrapper.idx].inMudBubble();
			newBubble = remove ? false : m_Cells[idx].inMudBubble();
		}
		if (oldBubble != newBubble) {
			if (newBubble) {
				unit->deaggregate();
			} else {
				unit->aggregate();
			}
		}
	}
	// update visibility
	oldBubble = add ? false : m_Cells[wrapper.idx].isVisible();
	newBubble = remove ? false : m_Cells[idx].isVisible();
	if (oldBubble != newBubble) {
		unit->setVisible(newBubble);
		if (m_Scene.valid()) {
			if (newBubble) {
				m_Scene->addObject(unit);
			} else {
				m_Scene->removeObject(unit);
			}
		}
	}
	// update the cell lists
	if (!add) {
		m_Cells[wrapper.idx].removeUnit(unit);
	}
	wrapper.idx = idx;
	if (!remove) {
		m_Cells[idx].addUnit(unit);
		ActiveCell *active = m_Cells[idx].getActive();
		if (active) {
			simdata::Vector3 origin = active->getOrigin();
			osg::Group *group = active->getSceneFeatureCell().get();
			unit->setLocalFrame(origin, group);
		} else {
			unit->setLocalFrame();
		}
	} else {
		unit->setLocalFrame();
	}
	if (!add && m_Debug > 0) saveSnapshot();
}

/**
 * Add a cell to a bubble.
 *
 * @param air Add to an air bubble.
 * @param mud Add to a mud bubble.
 */
void VirtualBattlefield::_addBubble(int cell, int type) {
	assert(cell >= 0 && cell < int(m_Cells.size()));
	ActiveCell *active;
	active = m_Cells[cell].getActive();
	if (!active) {
		int x, y;
		toCell(cell, x, y);
		float origin_x = (x+0.5) * m_CellSize;
		float origin_y = (y+0.5) * m_CellSize;
		active = new ActiveCell(&(m_Cells[cell]), cell, origin_x, origin_y, m_Scene, m_Terrain);
		m_ActiveCells.push_back(active);
	}
	switch (type) {
		case VIS_BUBBLE:
			if (active->setVisible(true)) {
				m_VisibleCells.push_back(cell);
			}
			break;
		case AIR_BUBBLE:
			active->addAirBubble();
			break;
		case MUD_BUBBLE:
			active->addMudBubble();
			break;
	}
}

/**
 * Remove a cell to a bubble.
 *
 * @param air remove from an air bubble.
 * @param mud remove from a mud bubble.
 */
void VirtualBattlefield::_removeBubble(int cell, int type) {
	assert(cell >= 0 && cell < int(m_Cells.size()));
	ActiveCell *active;
	active = m_Cells[cell].getActive();
	assert(active);
	switch (type) {
		case VIS_BUBBLE:
			if (active->setVisible(false)) {
				std::vector<int>::iterator i;
				i = std::find(m_VisibleCells.begin(), m_VisibleCells.end(), cell);	
				assert(i != m_VisibleCells.end());
				m_VisibleCells.erase(i);
			}
			break;
		case AIR_BUBBLE:
			active->removeAirBubble();
			break;
		case MUD_BUBBLE:
			active->removeMudBubble();
			break;
	}
}


void VirtualBattlefield::_updateActiveCells(float dt) {
	if (m_ActiveCells.empty()) return;
	std::list<ActiveCell*>::iterator iter = m_ActiveCells.begin();
	std::list<ActiveCell*>::iterator end = m_ActiveCells.end();
	while (iter != end) {
		if (((*iter)->needsUpdate())) {
			int steps = 0;
			bool active = (*iter)->update(dt, steps);
			if (!active) {
				delete *iter;
				iter = m_ActiveCells.erase(iter);
			} else {
				iter++;
			}
		} else {
			iter++;
		}
	}
}


void VirtualBattlefield::_cleanupActiveCells() {
	std::list<ActiveCell*>::iterator i = m_ActiveCells.begin();
	std::list<ActiveCell*>::iterator j = m_ActiveCells.end();
	for (; i!=j; i++) {
		(*i)->cleanup();
		delete *i;
	}
	m_ActiveCells.clear();
}


void VirtualBattlefield::_updateUnit(UnitWrapper &wrapper, double dt) {
	wrapper.time += dt;
	if (wrapper.time >= wrapper.sleep) {
		wrapper.sleep = wrapper.unit->onUpdate(wrapper.time);
		wrapper.time = 0.0;
		simdata::Vector3 const &pos = wrapper.unit->getGlobalPosition();
		int x = int(pos.x / m_CellSize);
		int y = int(pos.y / m_CellSize);
		int idx = toCell(x, y);
		if (idx != wrapper.idx) {
			_moveUnit(wrapper, idx);
		}
	}
}


void VirtualBattlefield::setTheater(simdata::Ref<Theater> const &theater) {
	m_Theater = theater;
	if (theater.valid()) {
		simdata::Ref<FeatureGroup>::vector groups = theater->getAllFeatureGroups();	
		simdata::Ref<FeatureGroup>::vector::iterator i = groups.begin();
		simdata::Ref<FeatureGroup>::vector::iterator j = groups.end();
		for (; i != j; i++) {
			simdata::Vector3 pos = (*i)->getPosition();
			int x = int(pos.x / m_CellSize);
			int y = int(pos.y / m_CellSize);
			int idx = toCell(x, y);
			if (m_Debug > 0) {
				std::cout << "adding feature to cell " << x << ":" << y << std::endl;
			}
			m_Cells[idx].addFeatureGroup(*i);
		}
	}
} 


void VirtualBattlefield::setCamera(simdata::Vector3 const &pos) {
	m_Camera = pos;
	int x = int(pos.x / m_CellSize);
	int y = int(pos.y / m_CellSize);
	int idx = toCell(x, y);
	if (idx != m_CameraIndex) {
		_moveCamera(idx);
	}
}


void VirtualBattlefield::_moveCamera(int index) {
	// XXX parameterize range
	_updateBubble(m_CameraIndex, index, 40000.0, VIS_BUBBLE);
	m_CameraIndex = index;
}


void VirtualBattlefield::_updateBubble(int idx0, int idx1, float range, int type) {
	// adding new bubble?
	bool adding_bubble = (idx0 < 0);
	// removing old bubble?
	bool removing_bubble = (idx1 < 0);
	range = range / m_CellSize;
	int int_range = int(range + 0.5);
	int x0=0, y0=0, x1=0, y1=0;
	// special handling for addition and removal
	if (!removing_bubble) {
		toCell(idx1, x1, y1);
	}
	if (!adding_bubble) {
		toCell(idx0, x0, y0);
	} else {
		x0 = x1;
		y0 = y1;
	}
	if (removing_bubble) {
		x1 = x0;
		y1 = y0;
	}
	// get the bounding box
	int xlo, xhi, ylo, yhi;
	if (x0 < x1) {
		xlo = x0-int_range;
		xhi = x1+int_range;
	} else {
		xlo = x1-int_range;
		xhi = x0+int_range;
	}
	if (y0 < y1) {
		ylo = y0-int_range;
		yhi = y1+int_range;
	} else {
		ylo = y1-int_range;
		yhi = y0+int_range;
	}
	// limit bbox to gridsize
	if (xlo < 0) xlo = 0;
	if (xhi >= m_GridSizeX) xhi = m_GridSizeX-1;
	if (ylo < 0) ylo = 0;
	if (yhi >= m_GridSizeY) yhi = m_GridSizeY-1;
	int i, j;
	// work with squared distance to avoid sqrt()
	int sq_range = int(range*range + 0.5);
	for (j = ylo; j <= yhi; j++) {
		int dy0 = j - y0;
		int dy1 = j - y1;
		dy0 *= dy0;
		dy1 *= dy1;
		int r0 = sq_range - dy0;
		int r1 = sq_range - dy1;
		for (i = xlo; i <= xhi; i++) {
			int dx0 = i - x0;
			int dx1 = i - x1;
			dx0 *= dx0;
			dx1 *= dx1;
			bool in0 = (dx0 <= r0) && !adding_bubble;
			bool in1 = (dx1 <= r1) && !removing_bubble;
			if (in0 != in1) {
				int cell = toCell(i, j);
				if (in0) {
					_removeBubble(cell, type);
				} else {
					_addBubble(cell, type);
				}
			}
		}
	}
}




/////////////////////////////////////////////////////////////////////////////////////////////
// VirtualBattlefield::ActiveCell
/////////////////////////////////////////////////////////////////////////////////////////////


VirtualBattlefield::ActiveCell::ActiveCell(Cell *cell, int index, float origin_x, float origin_y, simdata::Ref<VirtualScene> const &scene, simdata::Ref<TerrainObject> const &terrain) {
	//std::cout << "NEW ACTIVE CELL " << index << std::endl;
	assert(cell);
	cell->setActive(this);
	m_Cell = cell;
	m_Index = index;
	m_AirBubbleTimer = 0.0;
	m_MudBubbleTimer = 0.0;
	m_VisBubbleTimer = 0.0;
	m_Origin = simdata::Vector3(origin_x, origin_y, 0.0);
	m_CurrentAirUnit = cell->getUnits().begin();
	m_CurrentMudUnit = cell->getUnits().begin();
	m_CurrentVisUnit = cell->getUnits().begin();
	m_CurrentMudFeature = cell->getFeatureGroups().begin();
	m_CurrentVisFeature = cell->getFeatureGroups().begin();
	m_SceneFeatureCell = NULL;
	m_AirBubbles = 0;
	m_MudBubbles = 0;
	m_Visible = false;
	m_Scene = scene;
	m_Terrain = terrain;
	m_Destroyed = false;
	m_NeedsUpdate = true;
}

VirtualBattlefield::ActiveCell::~ActiveCell() {
	// should either be completely inactive or should have 
	// called cleanup() first if exitting.
	assert(m_AirBubbles == 0);
	assert(m_MudBubbles == 0);
	assert(!m_Visible);
	m_Cell->setActive(0);
	if (m_SceneFeatureCell.valid()) {
		m_Scene->removeFeatureCell(m_SceneFeatureCell.get());
	}
}

bool VirtualBattlefield::ActiveCell::inAirBubble() const { return m_AirBubbles > 0; }
bool VirtualBattlefield::ActiveCell::inMudBubble() const { return m_MudBubbles > 0; }
bool VirtualBattlefield::ActiveCell::isVisible() const { return m_Visible; }

void VirtualBattlefield::ActiveCell::updateScene(simdata::Vector3 const & origin) {
	if (m_SceneFeatureCell.valid()) {
		simdata::Vector3 pos = m_Origin - origin;
		m_SceneFeatureCell->setPosition(osg::Vec3(pos.x, pos.y, pos.z));
		Unit::list::iterator i = m_Cell->getUnits().begin();
		Unit::list::iterator j = m_Cell->getUnits().end();
		for (; i != j; i++) {
			(*i)->updateScene(origin);
		}
	}
}

void VirtualBattlefield::ActiveCell::addAirBubble() {
	assert(!m_Destroyed);
	if (m_AirBubbles == 0) m_NeedsUpdate = true;
	m_AirBubbles++;
}

void VirtualBattlefield::ActiveCell::removeAirBubble() {
	assert(!m_Destroyed);
	assert(m_AirBubbles > 0);
	if (m_AirBubbles == 1) m_NeedsUpdate = true;
	m_AirBubbles--;
}

void VirtualBattlefield::ActiveCell::addMudBubble() {
	assert(!m_Destroyed);
	if (m_MudBubbles == 0) m_NeedsUpdate = true;
	m_MudBubbles++;
}

void VirtualBattlefield::ActiveCell::removeMudBubble() {
	assert(!m_Destroyed);
	assert(m_MudBubbles > 0);
	if (m_MudBubbles == 1) m_NeedsUpdate = true;
	m_MudBubbles--;
}

bool VirtualBattlefield::ActiveCell::setVisible(bool visible) {
	assert(!m_Destroyed);
	if (visible == m_Visible) return false;
	//std::cout << "ACTIVECELL::setVisible " << m_Index << " = " << visible << std::endl;
	m_NeedsUpdate = true;
	if (visible && m_Scene.valid()) {
		if (!m_SceneFeatureCell) {
			m_SceneFeatureCell = new osg::PositionAttitudeTransform;
			m_SceneFeatureCell->setPosition(osg::Vec3(m_Origin.x, m_Origin.y, 0.0));
		}
		m_Scene->addFeatureCell(m_SceneFeatureCell.get());
	}
	m_Visible = visible;
	return true;
}

VirtualBattlefield::Cell *VirtualBattlefield::ActiveCell::getCell() const { return m_Cell; }

int VirtualBattlefield::ActiveCell::getIndex() const { return m_Index; }

simdata::Vector3 const &VirtualBattlefield::ActiveCell::getOrigin() const { return m_Origin; }

osg::ref_ptr<osg::PositionAttitudeTransform> VirtualBattlefield::ActiveCell::getSceneFeatureCell() const { return m_SceneFeatureCell; }

// this is some rather tricky stuff, so be careful.  basically
// we are storing iterators to a unit list in the associated
// cell, and walking through this list over several updates.
// during this process, units may be added or removed from 
// the list, which means we have to be very careful that our
// iterators stay consistent.  
//
// the iterators work from begin() to end() to deaggregate the 
// units and/or add them to the scene graph.  they go from end() 
// to begin() to aggregate the units and/or remove them from the
// scene graph.  thus each iterator divides the list into two
// distinct sets.
//
// when a unit is added to the list, the _moveUnit() method has
// already set it correctly for the current state of the cell,
// so we add it to whichever end of the list the appropriate
// iterator is moving away from.  (e.g. end() if we are 
// aggregating and begin() if we are deaggregating).
//
// when a unit is removed from the list, _moveUnit() will take
// care of setting its state appropriately.  the only trick is
// that if any of our iterators point to the unit they must
// be adjusted before it is removed.  if we are deaggregating
// we adjust toward end(). if we are aggregating then we adjust 
// toward begin().   if we are already at begin() we adjust
// toward end() since that will be the first element once the
// unit is removed.

// is alread in the correct configuration, so put it
// on the right side of the iterator.
void VirtualBattlefield::ActiveCell::_addUnit(Unit const &unit) {
	// TODO assert unit configuration....
	assert(!m_Destroyed);
	if ((unit->isAir() && m_AirBubbles > 0) ||
	   (!unit->isAir() && m_MudBubbles > 0)) {
		m_Cell->getUnits().push_front(unit);
	} else {
		m_Cell->getUnits().push_back(unit);
	}
}

// removing a unit is fine as long as none of the iterators
// point to it.  if they do we need to adjust them.
void VirtualBattlefield::ActiveCell::_checkRemoveUnit(Unit const &unit) {
	assert(!m_Destroyed);
	Unit::list::iterator begin = m_Cell->getUnits().begin();
	if (*m_CurrentAirUnit == unit) {
		if (m_AirBubbles > 0 || m_CurrentAirUnit == begin) {
			m_CurrentAirUnit++;
		} else {
			m_CurrentAirUnit--;
		}
	}
	if (*m_CurrentMudUnit == unit) {
		if (m_MudBubbles > 0 || m_CurrentMudUnit == begin) {
			m_CurrentMudUnit++;
		} else {
			m_CurrentMudUnit--;
		}
	}
	if (*m_CurrentVisUnit == unit) {
		if (m_Visible || m_CurrentVisUnit == begin) {
			m_CurrentVisUnit++;
		} else {
			m_CurrentVisUnit--;
		}
	}
}

int VirtualBattlefield::ActiveCell::update(float dt, int &steps) {
	assert(!m_Destroyed);
	if (!m_NeedsUpdate) return 1;
	Unit::list::iterator unit_begin = m_Cell->getUnits().begin();
	Unit::list::iterator unit_end = m_Cell->getUnits().end();
	simdata::Ref<FeatureGroup>::vector::iterator feature_begin = m_Cell->getFeatureGroups().begin();
	simdata::Ref<FeatureGroup>::vector::iterator feature_end = m_Cell->getFeatureGroups().end();
	bool active = false;
	int complete = 0;
	steps = 0;
	// update the air bubble
	if (m_AirBubbles > 0) {
		active = true;
		m_AirBubbleTimer = 10.0;
		while (m_CurrentAirUnit != unit_end) {
			if ((*m_CurrentAirUnit)->isAir()) {
				(*m_CurrentAirUnit)->deaggregate();
				m_CurrentAirUnit++;
				steps++;
				break;
			}
			m_CurrentAirUnit++;
		}
		complete++;
	} else {
		m_AirBubbleTimer -= dt;
		if (m_AirBubbleTimer > 0.0) {
			active = true;
		} else {
			while (m_CurrentAirUnit != unit_begin) {
				active = true;
				--m_CurrentAirUnit;
				if ((*m_CurrentAirUnit)->isAir()) {
					(*m_CurrentAirUnit)->aggregate();
					steps++;
					break;
				}
			}
			complete++;
		}
	}
	// update the mud bubble
	if (m_MudBubbles > 0) {
		active = true;
		m_MudBubbleTimer = 10.0;
		while (m_CurrentMudUnit != unit_end) {
			if (!((*m_CurrentMudUnit)->isAir())) {
				(*m_CurrentMudUnit)->deaggregate();
				m_CurrentMudUnit++;
				steps++;
				break;
			}
			m_CurrentMudUnit++;
		}
		if (m_CurrentMudFeature != feature_end) {
			(*m_CurrentMudFeature)->deaggregate();
			m_CurrentMudFeature++;
			steps++;
		} else {
			complete++;
		}
	} else {
		m_MudBubbleTimer -= dt;
		if (m_MudBubbleTimer > 0.0) {
			active = true;
		} else {
			while (m_CurrentMudUnit != unit_begin) {
				active = true;
				--m_CurrentMudUnit;
				if (!(*m_CurrentMudUnit)->isAir()) {
					(*m_CurrentMudUnit)->aggregate();
					steps++;
					break;
				}
			}
			if (m_CurrentMudFeature != feature_begin) {
				active = true;
				--m_CurrentMudFeature;
				(*m_CurrentMudFeature)->aggregate();
			} else {
				complete++;
			}

		}
	}
	// update the visibility bubble
	if (m_Visible) {
		active = true;
		m_VisBubbleTimer = 10.0;
		// add cell to scene
		if (m_CurrentVisUnit != unit_end) {
			(*m_CurrentVisUnit)->setVisible(true);
			if (m_Scene.valid()) {
				m_Scene->addObject(*m_CurrentVisUnit);
			}
			m_CurrentVisUnit++;
			steps++;
		} else complete++;
		if (m_CurrentVisFeature != feature_end) {
			(*m_CurrentVisFeature)->setVisible(true);
			osg::Node* node = (*m_CurrentVisFeature)->makeSceneGroup(m_Origin, m_Terrain.get());
			m_SceneFeatureCell->addChild(node);
			m_CurrentVisFeature++;
			steps++;
		} else complete++;
	} else {
		// no longer visible, start visibility timer
		m_VisBubbleTimer -= dt;
		if (m_VisBubbleTimer > 0.0) {
			active = true;
		} else {
			// timer expired, start to remove cell from scene
			if (m_CurrentVisUnit != unit_begin) {
				active = true;
				--m_CurrentVisUnit;
				(*m_CurrentVisUnit)->setVisible(false);
				if (m_Scene.valid()) {
					m_Scene->removeObject(*m_CurrentVisUnit);
				}
				steps++;
			} else {
				complete+=2;
				// all dynamic objects are gone; now take out the cell 
				// transform with all the visible features in one step
				while (m_CurrentVisFeature != feature_begin) {
					m_CurrentVisFeature--;
					(*m_CurrentVisFeature)->setVisible(false);
				}
				if (m_SceneFeatureCell.valid()) {
					//std::cout << "REMOVING FEATURE FROM SCENE " << m_Index << "\n";
					m_Scene->removeFeatureCell(m_SceneFeatureCell.get());
					m_SceneFeatureCell = NULL;
					steps++;
				}
			}
		}
	}
	if (complete == 4) m_NeedsUpdate = false;
	return active;
}

void VirtualBattlefield::ActiveCell::cleanup() {
	// close all bubbles
	Unit::list::iterator unit_begin = m_Cell->getUnits().begin();
	simdata::Ref<FeatureGroup>::vector::iterator feature_begin = m_Cell->getFeatureGroups().begin();
	while (m_CurrentAirUnit != unit_begin) {
		--m_CurrentAirUnit;
		if ((*m_CurrentAirUnit)->isAir()) {
			(*m_CurrentAirUnit)->aggregate();
		}
	}
	while (m_CurrentMudUnit != unit_begin) {
		--m_CurrentMudUnit;
		if (!(*m_CurrentMudUnit)->isAir()) {
			(*m_CurrentMudUnit)->aggregate();
		}
	}
	while (m_CurrentVisUnit != unit_begin) {
		--m_CurrentVisUnit;
		(*m_CurrentVisUnit)->setVisible(false);
		if (m_Scene.valid()) {
			m_Scene->removeObject(*m_CurrentVisUnit);
		}
	}
	while (m_CurrentMudFeature != feature_begin) {
		--m_CurrentMudFeature;
		//(*m_CurrentMudFeature)->aggregate();
	}
	while (m_CurrentVisFeature != feature_begin) {
		--m_CurrentVisFeature;
		//(*m_CurrentVisFeature)->???();
	}
	// remove the cell transform from the scene graph
	if (m_SceneFeatureCell.valid()) {
		m_Scene->removeFeatureCell(m_SceneFeatureCell.get());
	}
	m_AirBubbles = 0;
	m_MudBubbles = 0;
	m_Visible = false;
	m_Destroyed = true;
}



/////////////////////////////////////////////////////////////////////////////////////////////
// VirtualBattlefield::Cell
/////////////////////////////////////////////////////////////////////////////////////////////



VirtualBattlefield::Cell::Cell() { m_Active = 0; }

bool VirtualBattlefield::Cell::isEmpty() const { return m_Units.empty(); }

bool VirtualBattlefield::Cell::isVisible() const { return m_Active != 0 && m_Active->isVisible(); }

bool VirtualBattlefield::Cell::inAirBubble() const { return m_Active != 0 && m_Active->inAirBubble(); }

bool VirtualBattlefield::Cell::inMudBubble() const { return m_Active != 0 && m_Active->inMudBubble(); }

void VirtualBattlefield::Cell::setActive(ActiveCell* active) { m_Active = active; }

VirtualBattlefield::ActiveCell *VirtualBattlefield::Cell::getActive() const { return m_Active; }

simdata::Ref<FeatureGroup>::vector & VirtualBattlefield::Cell::getFeatureGroups() { return m_FeatureGroups; }

VirtualBattlefield::Unit::list &VirtualBattlefield::Cell::getUnits() { return m_Units; }

void VirtualBattlefield::Cell::addFeatureGroup(simdata::Ref<FeatureGroup> const &group) {
	m_FeatureGroups.push_back(group);
}

void VirtualBattlefield::Cell::addUnit(Unit const &unit) {
	//std::cout << "CELL add unit " << int(unit.get()) << " : " << unit->isAir() << "\n";
	if (m_Active) { 
		m_Active->_addUnit(unit);
	} else {
		m_Units.push_back(unit);
	}
}

void VirtualBattlefield::Cell::removeUnit(Unit const &unit) {
	if (m_Active) { 
		m_Active->_checkRemoveUnit(unit);
	} 
	Unit::list::iterator iter = std::find(m_Units.begin(), m_Units.end(), unit);
	assert(iter != m_Units.end());
	m_Units.erase(iter);
}

void VirtualBattlefield::Cell::getUnits(Unit::list &list) {
	list.insert(list.end(), m_Units.begin(), m_Units.end());
}







/////////////////////////////////////////////////////////////////////////////////////////////
// Old code from CSPFlightSim...
/////////////////////////////////////////////////////////////////////////////////////////////


/*
// calculates a list of objects within range from a centralObject.
void VirtualBattlefield::getObjectsInRange(SimObject * fromObj, float range, float view_angle  , int army, std::list<ObjectRangeInfo*> & rangeList  )
{
	ObjectList::iterator i;

	for (i = objectList.begin() ; i != objectList.end() ; ++i) {
		SimObject *toObj = (SimObject*)*i;
		if (fromObj->getObjectID() != toObj->getObjectID() && toObj->getArmy() == army) {
			simdata::Vector3 distVect = toObj->getGlobalPosition() - fromObj->getGlobalPosition();
			float dist = distVect.Length();
			simdata::Vector3 distVectNormalized = Normalized(distVect);
			float angle = simdata::angleBetweenTwoVectors(fromObj->getDirection(), distVectNormalized);
			angle = angle*360.0/3.1415/2.0;

			//	  std::cout << "Distance between objects " << fromTankObj->object_id << " and "
			//	       << toTankObj->object_id << " is " << dist << ", angle is " << angle*360.0/2.0/3.1415 << std::endl;
			if (dist < range && fabs(angle) < view_angle/360.0*2.0*3.1415) {
				//   std::cout << "Object " << toTankObj->object_id << " is in sight from object " << fromTankObj->object_id << std::endl;
				// todo use a better memory scheme
				ObjectRangeInfo * objectRangeInfo = new ObjectRangeInfo;
				objectRangeInfo->setObjectID( toObj->getObjectID() );
				objectRangeInfo->setRange( dist );
				objectRangeInfo->setDirection( distVectNormalized);
				rangeList.push_front(objectRangeInfo);
			}
		}
	}
}


bool VirtualBattlefield::doSphereTest( SimObject * pObj1, SimObject * pObj2)
{
	// return true if the distance between the two objects is less then then 
	// or equal there bounding sphere radius squared sum.

	// Calculate the radius squared sum.
	float r2 = pObj1->getBoundingSphereRadius() * pObj1->getBoundingSphereRadius()
	           + pObj2->getBoundingSphereRadius() * pObj2->getBoundingSphereRadius();

	// Calculate the diff vector between the two
	simdata::Vector3 pDiff = pObj1->getGlobalPosition() - pObj2->getGlobalPosition();
	float p2 = Dot( pDiff, pDiff);

	if ( (p2 - r2) <= 0) {
		return true;
	}

	// Do other tests.
	return false;
}

void VirtualBattlefield::getObjectDistance(SimObject * fromObject, SimObject * toObject, float & distance, simdata::Vector3 & direction)
{
	simdata::Vector3 distVect = fromObject->getGlobalPosition() - toObject->getGlobalPosition();
	distance = distVect.Length();
	direction = Normalized(distVect);
}
*/



