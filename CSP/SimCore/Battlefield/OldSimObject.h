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
 * @file OldSimObject.h
 *
 **/

#ifndef __SIMCORE_BATTLEFIELD_OLDSIMOBJECT_H__
#define __SIMCORE_BATTLEFIELD_OLDSIMOBJECT_H__


#include <SimCore/Util/SynchronousUpdate.h>
#include <SimCore/Util/Dispatch.h>
#include <SimCore/Util/Log.h>

#include <SimData/Object.h>
#include <SimData/Vector3.h>


/**
 * class SimObject - Base class for all objects in the simulation.
 *
 */
class SimObject: public simdata::Object, public UpdateTarget, public MessageDispatcher
{
	friend class Battlefield;
	friend class VirtualScene;
	friend class SceneManager;

public:

	typedef unsigned ObjectId;

	typedef enum {
		TYPE_AIR_UNIT,
		TYPE_MUD_UNIT,
		TYPE_SEA_UNIT,
		TYPE_STATIC
	} TypeId;

private:

	void setFlags(int flag, bool on) {
		if (on) m_Flags |= flag; else m_Flags &= ~flag;
	}

	inline int getFlags(int flags) const {
		return m_Flags & flags;
	}

	enum {
	       // mutable settings
	       F_NEARFIELD  = 0x00000002,
	       F_INSCENE    = 0x00000004,
	       F_AGGREGATE  = 0x00000008,
	       F_VISIBLE    = 0x00000010,
	       F_LOCAL      = 0x00000100,  // owned by this host (not remote)
	       F_HUMAN      = 0x00000200,  // controlled by a human

	       // immutable settings
	       F_STATIC     = 0x00010000,
	       F_AIR        = 0x00020000   // air object (airplane or helo)
	     };

	void setSceneFlag(bool flag) { setFlags(F_INSCENE, flag); }
	void setVisibleFlag(bool flag) { setFlags(F_VISIBLE, flag); }
	void setNearFlag(bool flag) { setFlags(F_NEARFIELD, flag); }
	void setAggregateFlag(bool flag) { setFlags(F_AGGREGATE, flag); }

	friend std::ostream &operator << (std::ostream &os, SimObject &object);
	std::string _debugId() const;

	inline void _debug(std::string const &msg) {
		CSP_LOG(BATTLEFIELD, DEBUG, _debugId() << ": " << msg);
	}


	// HUMAN/AGENT -------------------------------------------------------

	void setHuman() {
		_debug("set human");
		assert(isAgent());
		if (isHuman()) return;
		setFlags(F_HUMAN, true);
		onHuman();
	}

	void setAgent() {
		_debug("set agent");
		assert(isHuman());
		if (isAgent()) return;
		setFlags(F_HUMAN, false);
		onAgent();
	}

	virtual void onHuman() { }
	virtual void onAgent() { }


	// LOCAL/REMOTE -----------------------------------------------------

	void setLocal() {
		_debug("set local");
		assert(isRemote());
		if (isLocal()) return;
		setFlags(F_LOCAL, true);
		onLocal();
	}

	void setRemote() {
		_debug("set remote");
		assert(isLocal());
		if (isRemote()) return;
		setFlags(F_LOCAL, false);
		onRemote();
	}

	virtual void onLocal() { }
	virtual void onRemote() { }


	// AGGREGATE/DEAGGREGATE --------------------------------------------

	void aggregate() {
		_debug("aggregate");
		assert(!isAggregated());
		if (isAggregated()) return;
		setAggregateFlag(true);
		onAggregate();
	}

	void deaggregate() {
		_debug("deaggregate");
		assert(isAggregated());
		if (!isAggregated()) return;
		setAggregateFlag(false);
		onDeaggregate();
	}

	/** Called when an object that was deaggregated leaves all
	 *  deaggregation bubbles.
	 *
	 *  Extend this method to implement aggregation logic.
	 */
	virtual void onAggregate() { }

	/** Called when an object that was aggregated enters a deaggregation
	 *  bubble.
	 *
	 *  Extend this method to implement deaggregation logic.
	 */
	virtual void onDeaggregate() { }


	// SCENE MANAGEMENT -------------------------------------------------

	void enterScene() {
		_debug("enter scene");
		assert(!isInScene());
		if (isInScene()) return;
		setSceneFlag(true);
		onEnterScene();
	}

	void leaveScene() {
		_debug("leave scene");
		assert(isInScene());
		if (!isInScene()) return;
		setSceneFlag(false);
		onLeaveScene();
	}

	/** Called before an object is added to the scene graph.
	 *
	 *  This method is called even if no scene graph exists (e.g. on an
	 *  AI client).
	 */
	virtual void onEnterScene() { }

	/** Called before an object is removed from the scene graph.
	 *
	 *  This method is called even if no scene graph exists (e.g. on an
	 *  AI client).
	 */
	virtual void onLeaveScene() { }


	// ------------------------------------------------------------------

	/** Set the object's unique identifier number.
	 *
	 *  Can only be called once.
	 */
	void setId(ObjectId id) {
		assert(m_Id == 0);
		m_Id = id;
	}

	void setType(TypeId type) {
		assert(m_Type == 0);
		m_Type = type;
	}

	/** Update callback.
	 *
	 *  @param dt time since elapsed last call to onUpdate()
	 *  @returns sleep time between updates (return 0.0 for
	 *           fastest possible updating).
	 */
	virtual double onUpdate(double dt) { dt=dt; return 10.0; }

public:

	SimObject(TypeId type);
	virtual ~SimObject();

	virtual void dump() {}
	virtual void initialize() {}

	/** Get the unique object id of this instance.
	 */
	inline ObjectId id() const { return m_Id; }
	inline TypeId type() const { return m_Type; }
	std::string const &name() const { return m_Name; }

	/** The object name holds an identifier string for in-game display.
	 *  It is not intended for unique object identification.
	 *  Examples: "Thumper", "Cowboy11", "T-62".
	 */
	void setName(std::string const &name) {
		_debug("renamed to " + name);
		m_Name = name;
	}

	inline bool isInScene() const { return getFlags(F_INSCENE) != 0; }
	inline bool isNearField() const { return getFlags(F_NEARFIELD) != 0; }
	inline bool isAggregated() const { return getFlags(F_AGGREGATE) != 0; }
	inline bool isVisible() const { return getFlags(F_VISIBLE) != 0; }
	inline bool isStatic() const { return getFlags(F_STATIC) != 0; }
	inline bool isAir() const { return getFlags(F_AIR) != 0; }

	inline bool isHuman() const { return getFlags(F_HUMAN) != 0; }
	inline bool isAgent() const { return !isHuman(); }
	inline bool isLocal() const { return getFlags(F_LOCAL) != 0; }
	inline bool isRemote() const { return !isLocal(); }

	// position accessor methods
	virtual simdata::Vector3 getGlobalPosition() const = 0;

	// get the aggregation bubble radius for air units around this object (in meters)
	inline double getAirBubbleRadius() const  {
		assert(m_AirBubble > 0);   // catch if setAggregationBubbles not called
		return m_AirBubble;
	}

	// get the aggregation bubble radius for ground units (mud & sea) around this object (in meters)
	inline double getGroundBubbleRadius() const {
		assert(m_GroundBubble > 0);  // catch if setAggregationBubbles not called
		return m_GroundBubble;
	}

protected:

	// set the nominal air and ground aggregation bubble radii (in meters)
	void setAggregationBubbles(int air, int ground) {
		// only set once
		assert(m_AirBubble == 0 && m_GroundBubble == 0 && air > 0 && ground > 0);
		m_AirBubble = air;
		m_GroundBubble = ground;
	}

private:

	ObjectId m_Id;
	TypeId m_Type;
	unsigned int m_Flags;
	std::string m_Name;

	int m_AirBubble;
	int m_GroundBubble;

};

inline std::ostream &operator << (std::ostream &os, SimObject &object) {
	return os << object._debugId();
}

#endif // __SIMCORE_BATTLEFIELD_OLDSIMOBJECT_H__
