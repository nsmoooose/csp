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
 * @file VirtualBattlefield.h
 *
 **/

#ifndef __VIRTUALBATTLEFIELD_H__
#define __VIRTUALBATTLEFIELD_H__

#include <list>
#include <string>

#include <SimData/Ref.h>
#include <SimData/Path.h>

#include "TerrainObject.h"
#include "VirtualScene.h"
#include "DynamicObject.h"
#include "SynchronousUpdate.h"


class FeatureGroup;
class Theater;



/**
 * class ListWalker
 *
 * A list walker divides a list into two halves, pushing the
 * dividing point either forward or backward by one step at
 * a time.  It is used by the ActiveCell instances to track
 * the progress of updating the various units and features
 * they contain.  For instance, one ListWalker divides the
 * unit list into deaggregated air units and aggregated air
 * units.  The walker advances the divider as each unit is
 * deaggregated, and moves the divider back when a unit is
 * reaggregated.  
 *
 * ListWalkers act on existing lists, they do not create a 
 * copy.  Elements can be added and removed from the list, 
 * but the ListWalker must be notified so that it can update 
 * the divider accordingly.  It is assumed that new elements 
 * added to the list have been preconfigured correctly, so 
 * that they are always added on the side of the divider that 
 * is growing.  Use the add() method to take care of this 
 * automatically.  Since multiple ListWalkers can act on the 
 * same list, element removal must be handled externally, 
 * after all the ListWalkers have been notified by calling 
 * checkRemove().
 */
template <typename T>
class ListWalker {
public:
	typedef typename std::list<T>::iterator iterator;

	/**
	 * Construct a new ListWalker.
	 *
	 * The bind() method must be called before the ListWalker
	 * can be used.  The default direction is 'forward' (true).
	 */
	ListWalker() {
		m_List = 0;
		m_Forward = true;
	}

	/**
	 * Bind to a list.  The original list is used by reference
	 * so it must not be destroyed.
	 */
	void bind(std::list<T> &list) {
		m_List = &list;
		m_Current = m_List->begin();
	}

	/**
	 * Get the next item in the list (forward or backward depending
	 * on direction).  Will repeatedly return list.begin() or list.end()
	 * once the divider reaches the end.  Use the more() method to test
	 * for this condition.
	 */
	T get() {
		assert(m_List != 0);
		if (!m_Forward && m_Current != m_List->begin()) m_Current--;
		iterator i = m_Current;
		if (m_Forward && m_Current != m_List->end()) m_Current++;
		return *i;
	}

	/**
	 * Set the direction.
	 *
	 * Forward (true) is from begin() to end().  Backward (false) is from
	 * end() to begin().
	 */
	void setDirection(bool forward) {
		m_Forward = forward;
	}

	/**
	 * Get the current direction.
	 */
	bool getDirection() const {
		return m_Forward;
	}

	/**
	 * Test if any more elements remain.  See get().
	 */
	bool more() const {
		assert(m_List != 0);
		return (( m_Forward && (m_Current != m_List->end())) ||
			(!m_Forward && (m_Current != m_List->begin())));

	}

	/**
	 * A new element has been added to the back of the list.  If the
	 * divider was previously at the end of the list, the new element 
	 * will be inserted before the divider.  Since the goal of adding
	 * the element to the back of the list was to put it on the right
	 * side of the divider, we need to adjust the divider to the left
	 * in this case.
	 */
	void checkBack() {
		assert(m_List != 0);
		if (m_Current == m_List->end() && m_Current != m_List->begin()) {
			m_Current--;
		}
	}

	/**
	 * Adjust for element removal.  You must call this before removing any
	 * elements from the list so that the internal state of the ListWalker
	 * remains consistent.
	 */
	void checkRemove(T const &x) {
		assert(m_List != 0);
		if (*m_Current == x) {
			if (m_Forward || m_Current == m_List->begin()) {
				m_Current++;
			} else {
				m_Current--;
			}
		}
	}

private:
	bool m_Forward;
	iterator m_Current;
	std::list<T> *m_List;
};



class Cell;


/**
 * class ActiveCell
 *
 * A battlefield cell that contained within a bubble may require
 * periodic updates to aggregate or deaggregate its contents.
 */
class ActiveCell {
	friend class Cell;
	typedef simdata::Ref<DynamicObject> Unit;
	typedef simdata::Ref<FeatureGroup> Static;
public:
	/**
	 * Construct a new ActiveCell, setting the initial state to CREATE.
	 * 
	 * @param cell The active cell.
	 * @param index The index of the cell in the m_Cells vector.
	 * @param origin_x The X component of the center of the cell in 
	 *                 absolute coordinates.
	 * @param origin_y The Y component of the center of the cell in 
	 *                 absolute coordinates.
	 * @param scene Set to true if the battlefield has an active
	 *              scene (m_Scene is non-null).  If false, no scene
	 *              graph components will be constructed during
	 *              deaggragation.
	 */
	ActiveCell(Cell *cell, int index, float origin_x, float origin_y, simdata::Ref<VirtualScene> const &scene, simdata::Ref<TerrainObject> const &terrain);
	~ActiveCell();
	bool inAirBubble() const;
	bool inMudBubble() const;
	bool isVisible() const; 

	void updateScene(simdata::Vector3 const & origin);

	bool update(float dt);
	void cleanup();

	void addAirBubble();
	void removeAirBubble();
	void addMudBubble();
	void removeMudBubble();
	bool setVisible(bool visible);

	Cell *getCell() const;
	int getIndex() const;
	simdata::Vector3 const &getOrigin() const;
	osg::ref_ptr<osg::PositionAttitudeTransform> getSceneFeatureCell() const;

	inline bool needsUpdate() const { return m_NeedsUpdate; }

private:
	void _addUnit(Unit const &unit);
	void _checkRemoveUnit(Unit const &unit);

private:
	Cell *m_Cell;
	int m_Index;
	unsigned short m_AirBubbles;
	unsigned short m_MudBubbles;
	bool m_Visible;
	float m_AirBubbleTimer;
	float m_MudBubbleTimer;
	float m_VisualTimer;
	/*
	Unit::list::iterator m_CurrentAirUnit;
	Unit::list::iterator m_CurrentMudUnit;
	Unit::list::iterator m_CurrentVisUnit;
	*/
	/*
	simdata::Ref<FeatureGroup>::list::iterator m_CurrentMudFeature;
	simdata::Ref<FeatureGroup>::list::iterator m_CurrentVisFeature;
	*/
	ListWalker<Unit> m_AirUnitWalker;
	ListWalker<Unit> m_MudUnitWalker;
	ListWalker<Unit> m_UnitVisualWalker;
	ListWalker<Static> m_StaticWalker;
	ListWalker<Static> m_StaticVisualWalker;
	simdata::Ref<VirtualScene> m_Scene;
	simdata::Ref<TerrainObject> m_Terrain;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_SceneFeatureCell;
	simdata::Vector3 m_Origin;
	bool m_Destroyed;
	bool m_NeedsUpdate;
};


class Cell {
	friend class ActiveCell;
	typedef simdata::Ref<DynamicObject> Unit;
private:
	simdata::Ref<FeatureGroup>::list m_FeatureGroups;
	Unit::list m_Units;
	ActiveCell *m_Active;

	void setActive(ActiveCell* active);

public:
	Cell();

	bool isEmpty() const;
	bool isVisible() const;
	bool inAirBubble() const;
	bool inMudBubble() const;

	ActiveCell *getActive() const;

	/**
	 * Return a list of all FeatureGroups in this cell.
	 */
	simdata::Ref<FeatureGroup>::list & getFeatureGroups();
	
	/**
	 * Return a list of all units in this cell.
	 */
	Unit::list &getUnits();

	/**
	 * Add a FeatureGroup to this cell.
	 */
	void addFeatureGroup(simdata::Ref<FeatureGroup> const &group);

	/**
	 * Add a Unit to this cell.  Units moving into this cell will have already
	 * been aggregated or deaggregated by _moveUnit().  if we have an active
	 * handler, let it do the list modification to keep its iterator consistent.
	 */
	void addUnit(Unit const &unit);

	/**
	 * Remove a Unit from this cell.  Units moving out of this cell will be
	 * aggregated or deaggregated by _moveUnit().  If this cell has an active
	 * handler, let it check that none of its iterators are invalidated.
	 */
	void removeUnit(Unit const &unit);

	/**
	 * Get a list of all units in this cell.
	 */
	void getUnits(Unit::list &list);

};



/**
 * class VirtualBattlefield
 *
 */
class VirtualBattlefield: virtual public simdata::Referenced
{

	typedef simdata::Ref<DynamicObject> Unit;

	enum { 
		FLAG_DELETE = 1,
	};

	enum {
		VIS_BUBBLE = 0,
		MUD_BUBBLE = 1,
		AIR_BUBBLE = 2,
	};

	struct UnitWrapper {
		UnitWrapper(Unit const &u) {
			unit = u;
			flags = 0;
			idx = -1;
		}
		bool operator==(Unit const &u) { return unit == u; }
		bool operator!=(Unit const &u) { return unit != u; }
		Unit unit;
		int flags;
		int idx;
		//simdata::Ref<Connection> connection; // remote connection information
	};

	/**
	 * class Cell
	 *
	 * The battlefield is divided into a regular grid of Cells.  Each Cell
	 * contains lists of the dynamic objects (Units) and static features 
	 * (FeatureGroups) within it.  Human controlled objects in the battlefield
	 * project air and ground bubbles onto neighboring Cells.  The Cell
	 * tracks the number of each type of bubble that overlap it, and 
	 * coordinates the aggregation and deaggregation of its children.  Units
	 * are currently deaggregated in one step.  To deaggregate and aggregate
	 * FeatureGroups, Cells are added to a queue of ActiveCells maintained
	 * by the battlefield, which processes them during subsequent onUpdate
	 * callbacks to spread the aggregation / deaggregation out over a longer
	 * time and prevent stutters.
	 */

	void toCell(int idx, int &x, int &y);

	int toCell(simdata::Vector3 const &v) {
		int x = int(floor(v.x() / m_CellSize + 0.5));
		int y = int(floor(v.y() / m_CellSize + 0.5));
		return toCell(x, y);
	}

	int toCell(int x, int y) {
		x += m_GridSizeX/2;
		y += m_GridSizeY/2;
		int idx = y * m_GridSizeX + x;
		if (!(idx >= 0 && idx < int(m_Cells.size()))) {
			std::cout << "BF idx error: " << x << " " << y << " " << idx << "\n";
		}
		assert(idx >= 0 && idx < int(m_Cells.size()));
		return idx;
	}

	std::list<ActiveCell*> m_ActiveCells;
	std::vector<int> m_VisibleCells;

	void _updateActiveCells(float dt);
	void _cleanupActiveCells();

	void _moveUnit(UnitWrapper &wrapper, int idx);

	void _detachUnit(Unit &unit);

	void _removeBubble(int cell, int type);
		
	void _addBubble(int cell, int type);

	void _setHuman(UnitWrapper &wrapper, bool human);

	void _moveUnits();

	void saveSnapshot();

	std::vector<Cell> m_Cells;
	float m_CellSize;
	int m_GridSizeX;
	int m_GridSizeY;
	int m_Deleted;
	int m_Debug;
	bool m_Clean;

	UpdateMaster m_UnitUpdateMaster;

	simdata::Vector3 m_Camera;
	int m_CameraIndex;
	void _moveCamera(int index);
	void _updateBubble(int idx0, int idx1, float range, int type);

public:
	VirtualBattlefield();
	virtual ~VirtualBattlefield();

	int create();
	void cleanup();

	void onUpdate(double dt);
	void updateOrigin(simdata::Vector3 const &origin);

	void addUnit(Unit const &);
	void deleteUnit(Unit const &);
	void removeUnitsMarkedForDelete();
	void removeAllUnits();
	void setHuman(Unit const &, bool human);

	Unit getNextUnit(Unit const &unit, int human, int local, int category);

	float getGroundElevation(float x, 
	                         float y, 
	                         simdata::Vector3 &normal, 
	                         TerrainObject::IntersectionHint &hint) const;

	float getGroundElevation(float x, 
	                         float y, 
	                         TerrainObject::IntersectionHint &hint) const;

	float getGroundElevation(float x, float y, simdata::Vector3 &normal) const;
	float getGroundElevation(float x, float y) const;

	void setTheater(simdata::Ref<Theater> const &theater);
	simdata::Ref<Theater> getTheater() const;

	void setScene(simdata::Ref<VirtualScene>);
	simdata::Ref<VirtualScene> getScene() const { return m_Scene; }

	void setCamera(simdata::Vector3 const &);

protected:

	typedef std::list<UnitWrapper> UnitList;
	UnitList m_UnitList;

	simdata::Ref<TerrainObject> m_Terrain;
	simdata::Ref<VirtualScene> m_Scene;
	simdata::Ref<Theater> m_Theater;

	float m_VisualRadius;
	float m_MudBubbleRadius;
	float m_AirBubbleRadius;
};

#endif // __VIRTUALBATTLEFIELD_H__


