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
 * @file SimObject.h
 *
 **/

#ifndef __SIMOBJECT_H__
#define __SIMOBJECT_H__


#include "ObjectModel.h"
#include "SynchronousUpdate.h"

#include <SimData/Object.h>
#include <SimData/Vector3.h>
#include <SimData/Quat.h>
#include <SimData/Link.h>
#include <SimData/InterfaceRegistry.h>


class VirtualScene;
class VirtualBattlefield;
class ActiveCell;


/**
 * class SimObject - Base class for all objects in the simulation.
 *
 */
class SimObject: public simdata::Object, public UpdateTarget
{
	friend class VirtualScene;
	friend class VirtualBattlefield;
	friend class ActiveCell;

private:
	enum { 
	       // bits 0-7 reserved for SimObject
	       F_FREEZE     = 0x00000001,
	       F_NEARFIELD  = 0x00000002,
	       F_INSCENE    = 0x00000004,
	       F_AGGREGATE  = 0x00000008,
	       F_VISIBLE    = 0x00000010,
	     };

	void setSceneFlag(bool flag) { setFlags(F_INSCENE, flag); }
	void setNearFlag(bool flag) { setFlags(F_NEARFIELD, flag); }
	void setAggregateFlag(bool flag) { setFlags(F_AGGREGATE, flag); }
	void setVisibleFlag(bool flag) { setFlags(F_VISIBLE, flag); }

	void _aggregate() {
		assert(!getAggregateFlag());
		setAggregateFlag(true);
		aggregate();
	}

	void _deaggregate() {
		assert(getAggregateFlag());
		setAggregateFlag(false);
		deaggregate();
	}

protected:

	void setFlags(int flag, bool on) {
		if (on) m_Flags |= flag; else m_Flags &= ~flag;
	}

	int getFlags(int flags) const {
		return m_Flags & flags;
	}

	/** Called when an object that was deaggregated leaves a human
	 *  player's bubble.  
	 *
	 *  Extend this method to implement aggregation logic.  
	 */
	virtual void aggregate();

	/** Called when an object that was aggregated enters a human
	 *  player's bubble.  
	 *
	 *  Extend this method to implement deaggregation logic.  
	 */
	virtual void deaggregate();

	/** Set the object's unique identifier number.
	 *
	 *  Can only be called once.
	 */
	void setID(unsigned int id) {
		assert(m_ID == 0);
		m_ID = id; 
	}

	/** Update callback. 
	 *
	 *  @param dt time since elapsed last call to onUpdate()
	 *  @returns sleep time between updates (return 0.0 for
	 *           fastest possible updating).
	 */
	virtual double onUpdate(double dt) { return 10.0; }

public:

	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(SimObject)
	END_SIMDATA_XML_INTERFACE

	SimObject();
	virtual ~SimObject();

	virtual void dump() {}
	virtual void initialize() {}

	/** Get the unique object id of this instance.
	 */
	unsigned int getID() const { return m_ID; }

	/** Called before an object is added to the scene graph.
	 *
	 *  This method is called even if no scene graph exists (e.g. on an 
	 *  AI client).  
	 */
	virtual void enterScene();

	/** Called before an object is removed from the scene graph.
	 *
	 *  This method is called even if no scene graph exists (e.g. on an 
	 *  AI client).  
	 */
	virtual void leaveScene();

	void setFreezeFlag(bool flag) { setFlags(F_FREEZE, flag); }
	bool getFreezeFlag() const { return getFlags(F_FREEZE) != 0; }
	bool getSceneFlag() const { return getFlags(F_INSCENE) != 0; }
	bool getNearFlag() const { return getFlags(F_NEARFIELD) != 0; }
	bool getAggregateFlag() const { return getFlags(F_AGGREGATE) != 0; }
	bool getVisibleFlag() const { return getFlags(F_VISIBLE) != 0; }

	// position accessor methods
	virtual simdata::Vector3 getGlobalPosition() const = 0;
	
protected:

	unsigned int m_ID;
	unsigned int m_Flags;
};


#endif // __SIMOBJECT_H__


