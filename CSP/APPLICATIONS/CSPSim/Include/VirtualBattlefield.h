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


class FeatureGroup;
class Theater;


// XXX TODO bubble management when changing active object!


/**
 * class VirtualBattlefield
 *
 */
class VirtualBattlefield: public simdata::Referenced
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
			sleep = 0.0;
			time = 0.0;
		}
		Unit unit;
		int flags;
		int idx;
		float sleep; // time between updates
		float time; // time since last update
		//simdata::Ref<Connection> connection; // remote connection information
	};

	class ActiveCell;

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
	class Cell {
		friend class ActiveCell;
	private:
		simdata::Ref<FeatureGroup>::vector m_FeatureGroups;
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
		simdata::Ref<FeatureGroup>::vector & getFeatureGroups();
		
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


	void toCell(int idx, int &x, int &y);

	int toCell(int x, int y) {
		return y * m_GridSizeX + x;
	}

	/**
	 * class ActiveCell
	 *
	 * A battlefield cell that contained within a bubble may require
	 * periodic updates to aggregate or deaggregate its contents.
	 */
	class ActiveCell {
		friend class Cell;
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

		int update(float dt, int &steps);
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
		float m_VisBubbleTimer;
		Unit::list::iterator m_CurrentAirUnit;
		Unit::list::iterator m_CurrentMudUnit;
		Unit::list::iterator m_CurrentVisUnit;
		simdata::Ref<VirtualScene> m_Scene;
		simdata::Ref<TerrainObject> m_Terrain;
		simdata::Ref<FeatureGroup>::vector::iterator m_CurrentMudFeature;
		simdata::Ref<FeatureGroup>::vector::iterator m_CurrentVisFeature;
		osg::ref_ptr<osg::PositionAttitudeTransform> m_SceneFeatureCell;
		simdata::Vector3 m_Origin;
		bool m_Destroyed;
		bool m_NeedsUpdate;
	};

	std::list<ActiveCell*> m_ActiveCells;
	std::vector<int> m_VisibleCells;

	void _updateActiveCells(float dt);
	void _cleanupActiveCells();

	///////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
	void _aggregateFeatures(int idx) {
		std::cout << "REAGGREGATING cell " << idx << std::endl;
		Cell *cell = &(m_Cells[idx]);
		std::vector<ActiveCell>::iterator i = m_ActiveCells.begin();
		std::vector<ActiveCell>::iterator j = m_ActiveCells.end();
		for (; i!=j; i++) {
			if (i->m_Cell == cell) {
				assert(i->m_State != ActiveCell::ZOMBIE);
				if (i->m_State == ActiveCell::CREATE) {
					// never made it into existance
					m_ActiveCells.erase(i);
				} else { // ACTIVATE or ACTIVE
					i->m_State = ActiveCell::ZOMBIE;
					i->m_ZombieTime = 0.0;
				}
				return;	
			}
		}
		assert(0);
	}

	void _deaggregateFeatures(int idx) {
		std::cout << "DEAGGREGATING cell " << idx;
		Cell *cell = &(m_Cells[idx]);
		std::vector<ActiveCell>::iterator i = m_ActiveCells.begin();
		std::vector<ActiveCell>::iterator j = m_ActiveCells.end();
		for (; i!=j; i++) {
			if (i->m_Cell == cell) {
				assert(i->m_State == ActiveCell::ZOMBIE);
				if (i->m_CurrentFeature == i->m_Cell->getFeatureGroups().end()) {
					// fully deaggregated
					i->m_State = ActiveCell::ACTIVE;
				} else {
					// continue where we left off
					i->m_State = ActiveCell::ACTIVATE;
				}
				std::cout << "(revived)" << std::endl;
				return;
			}
		}
		int x, y;
		toCell(idx, x, y);
		float origin_x = x * m_CellSize;
		float origin_y = y * m_CellSize;
		m_ActiveCells.push_back(ActiveCell(cell, idx, origin_x, origin_y, m_Scene.valid()));
		std::cout << "(new)" << std::endl;
	}

	void _cleanupActiveCells() {
		std::vector<ActiveCell>::iterator i = m_ActiveCells.begin();
		std::vector<ActiveCell>::iterator j = m_ActiveCells.end();
		for (; i!=j; i++) {
			switch (i->m_State) {
			case ActiveCell::ZOMBIE: 
			case ActiveCell::ACTIVE: 
			case ActiveCell::ACTIVATE:
				if (m_Scene.valid()) {
					m_Scene->removeFeatureCell(i->m_SceneFeatureCell.get());
				}
				{
					simdata::Ref<FeatureGroup>::vector::iterator iter, end;
					iter = i->m_Cell->getFeatureGroups().begin();
					end = i->m_CurrentFeature;
					for (; iter != end; iter++) (*iter)->aggregate();
				}
				break;
			case ActiveCell::CREATE:
				break;
			}	
		}
		m_ActiveCells.clear();
	}

	void _updateActiveCells(float dt) {
		if (m_ActiveCells.empty()) return;
		std::vector<ActiveCell>::iterator i = m_ActiveCells.begin();
		std::vector<ActiveCell>::iterator j = m_ActiveCells.end();
		for (; i!=j; i++) {
			switch (i->m_State) {
			case ActiveCell::ZOMBIE: 
				if (i->m_ZombieTime < 0.01) {
					std::cout << "ZOMBIE Cell " << i->m_Index << std::endl;
				}
				i->m_ZombieTime += dt;
				if (i->m_ZombieTime > 30.0) {
					std::cout << "REAGGR Cell " << i->m_Index << std::endl;
					if (m_Scene.valid()) {
						m_Scene->removeFeatureCell(i->m_SceneFeatureCell.get());
					}
					simdata::Ref<FeatureGroup>::vector::iterator iter, end;
					iter = i->m_Cell->getFeatureGroups().begin();
					end = i->m_CurrentFeature;
					for (; iter != end; iter++) (*iter)->aggregate();
					m_ActiveCells.erase(i);
					return;
				}
				break;
			case ActiveCell::CREATE:
				std::cout << "CREATE Cell " << i->m_Index << std::endl;
				if (m_Scene.valid()) {
					m_Scene->addFeatureCell(i->m_SceneFeatureCell.get());
				}
				i->m_State = ActiveCell::ACTIVATE;
				// fallthrough
			case ActiveCell::ACTIVATE:
				if (i->m_CurrentFeature != i->m_Cell->getFeatureGroups().end()) {
					std::cout << "DEAGGR FeatureGroup\n";
					(*(i->m_CurrentFeature))->deaggregate(i->m_SceneFeatureCell.get(), i->m_Origin, m_Terrain.get());
					i->m_CurrentFeature++;
					// only one feature group deaggregated per update
					return;
				}
				std::cout << "ACTIVE Cell " << i->m_Index << std::endl;
				i->m_State = ActiveCell::ACTIVE;
				// fallthrough
			case ActiveCell::ACTIVE:
				break;
			}	
		}
	}
#endif

	void _moveUnit(UnitWrapper &wrapper, int idx);

	void _updateUnit(UnitWrapper &wrapper, double dt);

	void _removeBubble(int cell, int type);
		
	void _addBubble(int cell, int type);

	void _setHuman(UnitWrapper &wrapper, bool human);

	void saveSnapshot();

	std::vector<Cell> m_Cells;
	float m_CellSize;
	int m_GridSizeX;
	int m_GridSizeY;
	int m_Deleted;
	int m_Debug;
	bool m_Clean;

	simdata::Vector3 m_Camera;
	int m_CameraIndex;
	void _moveCamera(int index);
	void _updateBubble(int idx0, int idx1, float range, int type);

public:
	VirtualBattlefield();
	virtual ~VirtualBattlefield();

	int create();
	void cleanup();

	void onUpdate(float dt);
	void updateOrigin(simdata::Vector3 const &origin);

	void addUnit(Unit const &);
	void removeUnit(Unit const &);
	void removeUnitsMarkedForDelete();
	void removeAllUnits();
	void updateAllUnits(float dt);
	void setHuman(Unit const &, bool human);

	void setTheater(simdata::Ref<Theater> const &theater);

	Unit getNextUnit(Unit const &unit, int human, int local, int category);

	float getElevation(float x,float y) const;
	void getNormal(float x, float y, float & normalX, float & normalY, float & normalZ) const;

	void setTerrain(simdata::Ref<TerrainObject>);
	simdata::Ref<TerrainObject> getTerrain() const { return m_Terrain; }

	void setScene(simdata::Ref<VirtualScene>);
	simdata::Ref<VirtualScene> getScene() const { return m_Scene; }

	void setCamera(simdata::Vector3 const &);

protected:

	typedef std::list<UnitWrapper> UnitList;
	UnitList m_UnitList;

	simdata::Ref<TerrainObject> m_Terrain;
	simdata::Ref<VirtualScene> m_Scene;
	simdata::Ref<Theater> m_Theater;
};

#endif // __VIRTUALBATTLEFIELD_H__


