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
 *
 *   externalize various parametrs: cellsize, bubble radii, etc.
 *
 *   compute gridsize based on terrain size
 *   
 *   XXX leaveScene() and enterScene() must manage the inScene 
 *   flag of SimObject, since this is not set by VirtualScene for 
 *   features.
 *
 *   add string-based parsing of log classes to CSPSim::init so 
 *   that we can filter out all but the battlefield messages.
 *
 *   test various bubble radii, including vis > mud and mud > air
 *  
 *   add sub-timing of feature agg/degg to see where the bottle-
 *   neck really is.
 */

#include "VirtualBattlefield.h"
#include "VirtualScene.h"
#include "TerrainObject.h"
#include "DynamicObject.h"
#include "Theater.h"
#include "Theater/FeatureGroup.h"
#include "Theater/FeatureSceneGroup.h"
#include "Config.h"
#include "Log.h"

#include <SimData/Types.h>
#include <SimData/Math.h>
#include <SimData/Date.h>

#include <sstream>
#include <algorithm>


using simdata::Ref;


VirtualBattlefield::VirtualBattlefield()
{
	m_Debug = g_Config.getInt("Debug", "Battlefield", 0, true);
	m_Deleted = 0;
	m_CellSize = 10000.0; // 10 km
	m_GridSizeX = 101; // 1010 km
	m_GridSizeY = 101; // 1010 km
	m_Cells.resize(m_GridSizeX * m_GridSizeY);
	m_Clean = false;
	m_CameraIndex = -1;
	// XXX temporary: radii should be retrieved from the active 
	//     object.  perhaps these should just be scaling factors
	//     (the equivalent of bubble sliders in falcon)
	m_VisualRadius = g_Config.getInt("Testing", "VisualRadius",  40000, true);
	m_MudBubbleRadius = g_Config.getInt("Testing", "MudBubbleRadius",  60000, true);
	m_AirBubbleRadius = g_Config.getInt("Testing", "AirBubbleRadius",  80000, true);
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

float VirtualBattlefield::getGroundElevation(float x, 
                                             float y, 
                                             TerrainObject::IntersectionHint &hint) const
{
	if (!m_Terrain) {
		return 0.0;
	} else {
		return m_Terrain->getGroundElevation(x, y, hint);
	}
}

float VirtualBattlefield::getGroundElevation(float x, 
                                             float y, 
                                             simdata::Vector3 &normal, 
                                             TerrainObject::IntersectionHint &hint) const
{
	if (!m_Terrain) {
		normal = simdata::Vector3::ZAXIS;
		return 0.0;
	} else {
		return m_Terrain->getGroundElevation(x, y, normal, hint);
	}
}

float VirtualBattlefield::getGroundElevation(float x, float y) const
{
	if (!m_Terrain) {
		return 0.0;
	} else {
		static TerrainObject::IntersectionHint hint = 0;
		return m_Terrain->getGroundElevation(x, y, hint);
	}
}

float VirtualBattlefield::getGroundElevation(float x, float y, simdata::Vector3 &normal) const
{
	if (!m_Terrain) {
		return 0.0;
	} else {
		static TerrainObject::IntersectionHint hint = 0;
		return m_Terrain->getGroundElevation(x, y, normal, hint);
	}
}

void VirtualBattlefield::onUpdate(double dt)
{
	m_UnitUpdateMaster.update(dt);
	_moveUnits();
	_updateActiveCells(dt);
}


void VirtualBattlefield::addUnit(Unit const &unit)
{
	assert(unit.valid());
	unit->registerUpdate(&m_UnitUpdateMaster);
	m_UnitList.push_front(UnitWrapper(unit));
}


void VirtualBattlefield::deleteUnit(Unit const &unit)
{
	assert(unit.valid());
	UnitList::iterator i = std::find(m_UnitList.begin(), m_UnitList.end(), unit);
	if (i != m_UnitList.end()) {
		i->unit->registerUpdate(0);
		i->flags |= FLAG_DELETE;
		m_Deleted++;
		return;
	}
	assert(0);
}

void VirtualBattlefield::setHuman(Unit const &unit, bool human) {
	assert(unit.valid());
	UnitList::iterator i = std::find(m_UnitList.begin(), m_UnitList.end(), unit);
	if (i != m_UnitList.end()) {
		_setHuman(*i, human);
		return;
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

// XXX this routine does not need to run every cycle, and could
// in priciple distribute its loop over multiple cycles (although 
// this is complicated by the fact the that m_UnitList may change)
void VirtualBattlefield::_moveUnits()
{
	UnitList::iterator i = m_UnitList.begin();
	UnitList::const_iterator end = m_UnitList.end();
	for (; i != end ; ++i) {
		simdata::Vector3 const &pos = i->unit->getGlobalPosition();
		int idx = toCell(pos);
		if (idx != i->idx) {
			_moveUnit(*i, idx);
		}
	}
}

void VirtualBattlefield::updateOrigin(simdata::Vector3 const &origin) {
	if (m_Scene.valid()) {
		std::list<ActiveCell*>::iterator i = m_ActiveCells.begin();
		std::list<ActiveCell*>::const_iterator end = m_ActiveCells.end();
		for (; i != end ; ++i) {
			(*i)->updateScene(origin);
		}
	}
}

VirtualBattlefield::Unit VirtualBattlefield::getNextUnit(Unit const &unit, int human, int local, int category)
{
	UnitList::iterator end = m_UnitList.end();
	UnitList::iterator i = std::find(m_UnitList.begin(), end, unit);
	if (i == m_UnitList.end()) {
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
	y -= m_GridSizeY/2;
	x -= m_GridSizeX/2;
}

void VirtualBattlefield::_setHuman(UnitWrapper &wrapper, bool human) {
	Unit &unit = wrapper.unit;
	if (unit->isHuman() == human) return;
	if (wrapper.idx != -1) {
		if (human) {
			_updateBubble(-1, wrapper.idx, m_AirBubbleRadius, AIR_BUBBLE);
			_updateBubble(-1, wrapper.idx, m_MudBubbleRadius, MUD_BUBBLE);
		} else {
			_updateBubble(wrapper.idx, -1, m_AirBubbleRadius, AIR_BUBBLE);
			_updateBubble(wrapper.idx, -1, m_MudBubbleRadius, MUD_BUBBLE);
		}
	}
	unit->setHuman(human); 
}

void VirtualBattlefield::_moveUnit(UnitWrapper &wrapper, int idx) {
	Unit &unit = wrapper.unit;
	bool remove = idx < 0;
	bool add = wrapper.idx < 0;
	CSP_LOG(BATTLEFIELD, DEBUG, "moveUnit " << wrapper.idx << " to " << idx);
	if (unit->isHuman()) {
		_updateBubble(wrapper.idx, idx, m_AirBubbleRadius, AIR_BUBBLE);
		_updateBubble(wrapper.idx, idx, m_MudBubbleRadius, MUD_BUBBLE);
	}
	if (!add) {
		m_Cells[wrapper.idx].removeUnit(unit);
	}
	wrapper.idx = idx;
	if (!remove) {
		// the active cell will handle state updates for the unit
		m_Cells[idx].addUnit(unit);
	} else {
		// aggregate and remove from the scene
		_detachUnit(unit);
	}
	if (!add && m_Debug > 0) saveSnapshot();
}


void VirtualBattlefield::_detachUnit(Unit &unit) {
	if (unit->getVisibleFlag()) {	
		CSP_LOG(BATTLEFIELD, TRACE, "detachUnit");
		unit->leaveScene();
		if (m_Scene.valid()) {
			m_Scene->removeObject(unit);
		}
	}
	if (!unit->isHuman() && !unit->getAggregateFlag()) {
		unit->_aggregate();
	}
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
		float origin_x = x * m_CellSize;
		float origin_y = y * m_CellSize;
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
	simdata::SimTime start = simdata::SimDate::getSystemTime();
	while (iter != end) {
		if (((*iter)->needsUpdate())) {
			if ((*iter)->update(dt)) {
				iter++;
			} else {
				delete *iter;
				iter = m_ActiveCells.erase(iter);
			}
			// don't spend more than 10 ms at a time
			if (simdata::SimDate::getSystemTime() - start > 0.010) {
				// take credit for the stutter
				std::cout << "ACTIVE CELL UPDATE PAUSED " << 1000.0*(simdata::SimDate::getSystemTime() - start) << "ms\n";
				return;
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



void VirtualBattlefield::setTheater(simdata::Ref<Theater> const &theater) {
	m_Theater = theater;
	if (theater.valid()) {
		simdata::Ref<FeatureGroup>::list groups = theater->getAllFeatureGroups();	
		simdata::Ref<FeatureGroup>::list::iterator i = groups.begin();
		simdata::Ref<FeatureGroup>::list::iterator j = groups.end();
		for (; i != j; i++) {
			simdata::Vector3 pos = (*i)->getGlobalPosition();
			std::cout << pos << "\n";
			int idx = toCell(pos);
			CSP_LOG(BATTLEFIELD, DEBUG, "Adding feature to cell " << idx);
			m_Cells[idx].addFeatureGroup(*i);
		}
		m_Terrain = theater->getTerrain();
	} else {
		m_Terrain = NULL;
	}
} 

simdata::Ref<Theater> VirtualBattlefield::getTheater() const {
	return m_Theater;
}

void VirtualBattlefield::setCamera(simdata::Vector3 const &pos) {
	m_Camera = pos;
	int idx = toCell(pos);
	if (idx != m_CameraIndex) {
		CSP_LOG(BATTLEFIELD, DEBUG, "Moving camera to cell " << idx);
		_moveCamera(idx);
	}
}


void VirtualBattlefield::_moveCamera(int index) {
	_updateBubble(m_CameraIndex, index, m_VisualRadius, VIS_BUBBLE);
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
	if (xlo < -m_GridSizeX/2) xlo = -m_GridSizeX/2;
	if (xhi >= m_GridSizeX/2) xhi =  m_GridSizeX/2 - 1;
	if (ylo < -m_GridSizeY/2) ylo = -m_GridSizeY/2;
	if (yhi >= m_GridSizeY/2) yhi =  m_GridSizeY/2 - 1;
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
// ActiveCell
/////////////////////////////////////////////////////////////////////////////////////////////


ActiveCell::ActiveCell(Cell *cell, int index, float origin_x, float origin_y, simdata::Ref<VirtualScene> const &scene, simdata::Ref<TerrainObject> const &terrain) {
	CSP_LOG(BATTLEFIELD, DEBUG, "New ActiveCell " << index);
	assert(cell);
	cell->setActive(this);
	m_Cell = cell;
	m_Index = index;
	m_AirBubbleTimer = 0.0;
	m_MudBubbleTimer = 0.0;
	m_VisualTimer = 0.0;
	m_Origin = simdata::Vector3(origin_x, origin_y, 0.0);

	m_AirUnitWalker.bind(cell->getUnits());
	m_AirUnitWalker.setDirection(false);

	m_MudUnitWalker.bind(cell->getUnits());
	m_MudUnitWalker.setDirection(false);

	m_UnitVisualWalker.bind(cell->getUnits());
	m_UnitVisualWalker.setDirection(false);

	m_StaticWalker.bind(cell->getFeatureGroups());
	m_StaticWalker.setDirection(false);

	m_StaticVisualWalker.bind(cell->getFeatureGroups());
	m_StaticVisualWalker.setDirection(false);

	m_SceneFeatureCell = NULL;
	m_AirBubbles = 0;
	m_MudBubbles = 0;
	m_Visible = false;
	m_Scene = scene;
	m_Terrain = terrain;
	m_Destroyed = false;
	m_NeedsUpdate = true;
}

ActiveCell::~ActiveCell() {
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

bool ActiveCell::inAirBubble() const { return m_AirBubbles > 0; }
bool ActiveCell::inMudBubble() const { return m_MudBubbles > 0; }
bool ActiveCell::isVisible() const { return m_Visible; }

void ActiveCell::updateScene(simdata::Vector3 const & origin) {
	if (m_SceneFeatureCell.valid()) {
		simdata::Vector3 pos = m_Origin - origin;
		m_SceneFeatureCell->setPosition(osg::Vec3(pos.x(), pos.y(), pos.z()));
		Unit::list::iterator i = m_Cell->getUnits().begin();
		Unit::list::iterator j = m_Cell->getUnits().end();
		for (; i != j; i++) {
			(*i)->updateScene(origin);
		}
	}
}

void ActiveCell::addAirBubble() {
	assert(!m_Destroyed);
	if (m_AirBubbles == 0) {
		m_NeedsUpdate = true;
		m_AirUnitWalker.setDirection(true);
	}
	m_AirBubbles++;
}

void ActiveCell::removeAirBubble() {
	assert(!m_Destroyed);
	assert(m_AirBubbles > 0);
	if (m_AirBubbles == 1) {
		m_NeedsUpdate = true;
		m_AirUnitWalker.setDirection(false);
	}
	m_AirBubbles--;
}

void ActiveCell::addMudBubble() {
	assert(!m_Destroyed);
	if (m_MudBubbles == 0) {
		m_NeedsUpdate = true;
		m_MudUnitWalker.setDirection(true);
		m_StaticWalker.setDirection(true);
	}
	m_MudBubbles++;
}

void ActiveCell::removeMudBubble() {
	assert(!m_Destroyed);
	assert(m_MudBubbles > 0);
	if (m_MudBubbles == 1) {
		m_NeedsUpdate = true;
		m_MudUnitWalker.setDirection(false);
		m_StaticWalker.setDirection(false);
	}
	m_MudBubbles--;
}

bool ActiveCell::setVisible(bool visible) {
	assert(!m_Destroyed);
	if (visible == m_Visible) return false;
	CSP_LOG(BATTLEFIELD, DEBUG, "ActiveCell::setVisible " << m_Index << " = " << visible);
	m_NeedsUpdate = true;
	if (visible && m_Scene.valid() && !m_SceneFeatureCell) {
		m_SceneFeatureCell = new osg::PositionAttitudeTransform;
		m_SceneFeatureCell->setPosition(osg::Vec3(m_Origin.x(), m_Origin.y(), 0.0));
		m_Scene->addFeatureCell(m_SceneFeatureCell.get());
	}
	m_UnitVisualWalker.setDirection(visible);
	m_StaticVisualWalker.setDirection(visible);
	m_Visible = visible;
	return true;
}

Cell *ActiveCell::getCell() const { return m_Cell; }

int ActiveCell::getIndex() const { return m_Index; }

simdata::Vector3 const &ActiveCell::getOrigin() const { return m_Origin; }

osg::ref_ptr<osg::PositionAttitudeTransform> ActiveCell::getSceneFeatureCell() const { return m_SceneFeatureCell; }


// add the unit to the cell in the correct place
// relative to the aggregation and visibility
// ListWalkers.  if necessary, modify the units
// state right now.
// XXX this code needs more work... optimize the placement
// so that it works without a force transition in most cases.
void ActiveCell::_addUnit(Unit const &unit) {
	bool visible = unit->getVisibleFlag();
	bool aggregated = unit->getAggregateFlag();
	//bool bubble_direction = unit->isAir() ? m_AirUnitWalker.getDirection() : 
	//                                        m_MudUnitWalker.getDirection(); 
	bool visual_direction = m_UnitVisualWalker.getDirection();
	CSP_LOG(BATTLEFIELD, DEBUG, "ActiveCell addUnit (vis=" << visible << ", agg=" << aggregated << ")");
	//if (visual_direction ^ visible != bubble_direction ^ !aggregated) {
	if (visible == aggregated) {
		// cannot place the unit at either end and still be consistent 
		// with both walkers, so we have to modify the unit state
		if (visible) {
			unit->leaveScene();
			if (m_Scene.valid()) {
				m_Scene->removeObject(unit);
			}
		} else {
			unit->enterScene();
			if (m_Scene.valid()) {
				m_Scene->addObject(unit);
			}
		} 
		visible = !visible;
	}
	//if (visual_direction ^ visible) {
	if (!visible) {
		m_Cell->getUnits().push_back(unit);
		m_UnitVisualWalker.checkBack();
		m_AirUnitWalker.checkBack();
		m_MudUnitWalker.checkBack();
		CSP_LOG(BATTLEFIELD, DEBUG, "ActiveCell addUnit push_back " << visual_direction);
	} else {
		m_Cell->getUnits().push_front(unit);
		CSP_LOG(BATTLEFIELD, DEBUG, "ActiveCell addUnit push_front " << visual_direction);
	}
	m_NeedsUpdate = true;
}



// removing a unit is fine as long as none of the iterators
// point to it.  if they do we need to adjust them.
void ActiveCell::_checkRemoveUnit(Unit const &unit) {
	assert(!m_Destroyed);
	m_AirUnitWalker.checkRemove(unit);
	m_MudUnitWalker.checkRemove(unit);
	m_UnitVisualWalker.checkRemove(unit);
}

bool ActiveCell::update(float dt) {
	assert(!m_Destroyed);
	if (!m_NeedsUpdate) return true;
	bool active = false;
	int complete = 0;
	// update the air bubble
	if (m_AirBubbles > 0) {
		active = true;
		m_AirBubbleTimer = 10.0;
		while (m_AirUnitWalker.more()) {
			Unit current = m_AirUnitWalker.get();
			if (current->isAir()) {
				current->_deaggregate();
				break;
			}
		}
		complete++;
	} else {
		m_AirBubbleTimer -= dt;
		if (m_AirBubbleTimer > 0.0) {
			active = true;
		} else {
			while (m_AirUnitWalker.more()) {
				active = true;
				Unit current = m_AirUnitWalker.get();
				if (current->isAir()) {
					current->_aggregate();
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
		while (m_MudUnitWalker.more()) {
			Unit current = m_MudUnitWalker.get();
			if (!current->isAir()) {
				current->_deaggregate();
				break;
			}
		}
		if (m_StaticWalker.more()) {
			m_StaticWalker.get()->_deaggregate();
		} else {
			complete++;
		}
	} else {
		m_MudBubbleTimer -= dt;
		if (m_MudBubbleTimer > 0.0) {
			active = true;
		} else {
			while (m_MudUnitWalker.more()) {
				active = true;
				Unit current = m_MudUnitWalker.get();
				if (!current->isAir()) {
					current->_aggregate();
					break;
				}
			}
			if (m_StaticWalker.more()) {
				active = true;
				m_StaticWalker.get()->_aggregate();
			} else {
				complete++;
			}

		}
	}
	// update the visibility bubble
	if (m_Visible) {
		active = true;
		m_VisualTimer = 10.0;
		// add cell to scene
		if (m_UnitVisualWalker.more()) {
			Unit current = m_UnitVisualWalker.get();
			current->enterScene();
			if (m_Scene.valid()) {
				m_Scene->addObject(current);
			}
		} else complete++;
		if (m_StaticVisualWalker.more()) {
			Static current = m_StaticVisualWalker.get();
			current->enterScene();
			osg::Node* node = current->makeSceneGroup(m_Origin, m_Terrain.get());
			m_SceneFeatureCell->addChild(node);
		} else complete++;
	} else {
		// no longer visible, start visibility timer
		m_VisualTimer -= dt;
		if (m_VisualTimer > 0.0) {
			active = true;
		} else {
			// timer expired, start to remove cell from scene
			if (m_UnitVisualWalker.more()) {
				active = true;
				Unit current = m_UnitVisualWalker.get();
				CSP_LOG(BATTLEFIELD, DEBUG, "ActiveCell::update remove unit from scene.");
				current->leaveScene();
				if (m_Scene.valid()) {
					m_Scene->removeObject(current);
				}
			} else {
				complete+=2;
				// all dynamic objects are gone; now take out the cell 
				// transform with all the visible features in one step
				while (m_StaticVisualWalker.more()) {
					CSP_LOG(BATTLEFIELD, DEBUG, "ActiveCell::update remove feature from scene.");
					m_StaticVisualWalker.get()->leaveScene();
				}
				if (m_SceneFeatureCell.valid()) {
					CSP_LOG(BATTLEFIELD, DEBUG, "ActiveCell::update remove feature cell: " << m_Index << " " << int(m_SceneFeatureCell.get()));
					m_Scene->removeFeatureCell(m_SceneFeatureCell.get());
					m_SceneFeatureCell = NULL;
				}
			}
		}
	}
	if (complete == 4) m_NeedsUpdate = false;
	return active;
}

void ActiveCell::cleanup() {
	// close all bubbles
	CSP_LOG(BATTLEFIELD, DEBUG, "ActiveCell::cleanup");
	m_AirUnitWalker.setDirection(false);
	while (m_AirUnitWalker.more()) {
		Unit current = m_AirUnitWalker.get();
		if (current->isAir()) {
			current->_aggregate();
		}
	}
	m_MudUnitWalker.setDirection(false);
	while (m_MudUnitWalker.more()) {
		Unit current = m_MudUnitWalker.get();
		if (!current->isAir()) {
			current->_aggregate();
		}
	}
	m_UnitVisualWalker.setDirection(false);
	while (m_UnitVisualWalker.more()) {
		Unit current = m_UnitVisualWalker.get();
		current->leaveScene();
		if (m_Scene.valid()) {
			m_Scene->removeObject(current);
		}
	}
	m_StaticWalker.setDirection(false);
	while (m_StaticWalker.more()) {
		m_StaticWalker.get()->_aggregate();
	}
	m_StaticVisualWalker.setDirection(false);
	while (m_StaticVisualWalker.more()) {
		m_StaticVisualWalker.get(); //->XXX();
	}
	// remove the cell transform from the scene graph
	if (m_SceneFeatureCell.valid()) {
		m_Scene->removeFeatureCell(m_SceneFeatureCell.get());
		m_SceneFeatureCell = NULL;
	}
	m_AirBubbles = 0;
	m_MudBubbles = 0;
	m_Visible = false;
	m_Destroyed = true;
}



/////////////////////////////////////////////////////////////////////////////////////////////
// Cell
/////////////////////////////////////////////////////////////////////////////////////////////



Cell::Cell() { m_Active = 0; }

bool Cell::isEmpty() const { return m_Units.empty(); }

bool Cell::isVisible() const { return m_Active != 0 && m_Active->isVisible(); }

bool Cell::inAirBubble() const { return m_Active != 0 && m_Active->inAirBubble(); }

bool Cell::inMudBubble() const { return m_Active != 0 && m_Active->inMudBubble(); }

void Cell::setActive(ActiveCell* active) { m_Active = active; }

ActiveCell *Cell::getActive() const { return m_Active; }

simdata::Ref<FeatureGroup>::list & Cell::getFeatureGroups() { return m_FeatureGroups; }

Cell::Unit::list &Cell::getUnits() { return m_Units; }

void Cell::addFeatureGroup(simdata::Ref<FeatureGroup> const &group) {
	m_FeatureGroups.push_back(group);
}

void Cell::addUnit(Unit const &unit) {
	CSP_LOG(BATTLEFIELD, DEBUG, "CELL add unit " << int(unit.get()) << " : " << unit->isAir() << "|" << int(m_Active));
	if (m_Active) { 
		m_Active->_addUnit(unit);
	} else {
		m_Units.push_back(unit);
	}
}

void Cell::removeUnit(Unit const &unit) {
	if (m_Active) { 
		m_Active->_checkRemoveUnit(unit);
	} 
	Unit::list::iterator iter = std::find(m_Units.begin(), m_Units.end(), unit);
	assert(iter != m_Units.end());
	m_Units.erase(iter);
}

void Cell::getUnits(Unit::list &list) {
	list.insert(list.end(), m_Units.begin(), m_Units.end());
}

