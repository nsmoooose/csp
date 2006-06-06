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

#ifndef __CSPSIM_BATTLEFIELD_SIMOBJECT_H__
#define __CSPSIM_BATTLEFIELD_SIMOBJECT_H__

#include <csp/cspsim/Export.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/SynchronousUpdate.h>
#include <csp/csplib/util/TimeStamp.h>

#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Vector3.h>

#include <csp/csplib/net/DispatchTarget.h>

#include <map>

CSP_NAMESPACE

/**
 * class SimObject - Base class for all objects in the simulation.
 *
 */
class CSPSIM_EXPORT SimObject: public Object, public UpdateTarget, public DispatchTarget
{
	friend class Battlefield;
	friend class GlobalBattlefield;
	friend class LocalBattlefield;
	friend class UnitContact;
	friend class VirtualScene; // FIXME this is a CSPSim class!
	friend class SceneManager;

public:

	typedef int ObjectId;

	typedef enum {
		TYPE_AIR_UNIT,
		TYPE_MUD_UNIT,
		TYPE_SEA_UNIT,
		TYPE_STATIC
	} TypeId;

private:

	ObjectId m_Id;
	TypeId m_Type;
	unsigned int m_Flags;
	std::string m_Name;

	int m_AirBubble;
	int m_GroundBubble;

	std::vector<ObjectId> m_ContactList;
	uint32 m_ContactSignature;

	inline void setFlags(int flag, bool on) {
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
		F_AIR        = 0x00020000,  // air object (airplane or helo)
		F_CONTACT    = 0x00040000,  // global battlefield contact
		F_INIT       = 0x80000000   // initialized
	};

	void setSceneFlag(bool flag) { setFlags(F_INSCENE, flag); }
	void setVisibleFlag(bool flag) { setFlags(F_VISIBLE, flag); }
	void setNearFlag(bool flag) { setFlags(F_NEARFIELD, flag); }
	void setAggregateFlag(bool flag) { setFlags(F_AGGREGATE, flag); }

	friend std::ostream &operator << (std::ostream &os, SimObject &object);
	std::string _debugId() const;

	inline void _debug(std::string const &msg) {
		CSPLOG(DEBUG, BATTLEFIELD) << _debugId() << ": " << msg;
	}


	// HUMAN/AGENT -------------------------------------------------------

private:
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

protected:
	virtual void onHuman() { }
	virtual void onAgent() { }


	// LOCAL/REMOTE -----------------------------------------------------

private:
	void setLocal() {
		_debug("set local");
		if (!getFlags(F_INIT)) {
			setFlags(F_INIT, true);
		} else {
			assert(isRemote());
			if (isLocal()) return;
		}
		setFlags(F_LOCAL, true);
		onLocal();
	}

	void setRemote() {
		_debug("set remote");
		if (!getFlags(F_INIT)) {
			setFlags(F_INIT, true);
		} else {
			assert(isLocal());
			if (isRemote()) return;
		}
		setFlags(F_LOCAL, false);
		onRemote();
	}

protected:
	virtual void onLocal() { }
	virtual void onRemote() { }


	// AGGREGATE/DEAGGREGATE --------------------------------------------

private:
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

protected:
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

private:
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

protected:
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

private:
	/** Set the object's unique identifier number.
	 *
	 *  Can only be called once.
	 */
	void setId(ObjectId id) {
		assert(m_Id == 0);
		m_Id = id;
	}

	// remove me!
	//void setType(TypeId type) {
	//	assert(m_Type == 0);
	//	m_Type = type;
	//}

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

	virtual Path getObjectPath() const { return Path(getPath()); }

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
	inline bool isContact() const { return getFlags(F_CONTACT) != 0; }

	// position accessor methods
	virtual Vector3 getGlobalPosition() const = 0;

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

	// get the object state to be sent to remote mirrors of the object
	virtual Ref<NetworkMessage> getState(TimeStamp /*current_time*/, SimTime /*interval*/, int /*detail*/) const { return 0; }

	// retrieve a list of ids of nearby objects.  this list is periodically
	// updated by the battlefield.  the battlefield updates are synchronous,
	// so iterators are safe to use for the duration of an object update
	// method, but should not be may not remain valid across multiple updates.
	// see also contactSignature().
	std::vector<ObjectId> const &getContacts() const { return m_ContactList; }

	// return a signature of the ids in the list returned by getContacts.  this
	// signature changes whenever objects are added or removed from the contact
	// list.  contact lists with the same ids have identical signatures, although
	// the order of the ids may differ.
	int contactSignature() const { return m_ContactSignature; }

protected:

	virtual void onNewContact(Ref<SimObject> const &/*contact*/) { }
	virtual void onLostContact(Ref<SimObject> const &/*contact*/) { }

	// set the nominal air and ground aggregation bubble radii (in meters)
	void setAggregationBubbles(int air, int ground) {
		// only set once
		assert(m_AirBubble == 0 && m_GroundBubble == 0 && air > 0 && ground > 0);
		m_AirBubble = air;
		m_GroundBubble = ground;
	}

	// set the object state of this mirror from the source object state
	virtual void setState(Ref<NetworkMessage> const &, TimeStamp /*now*/) { }

	// marker for objects in the global battlefield (immutable once set).
	virtual void setContact() {
		setFlags(F_CONTACT, true);
	}

};

inline std::ostream &operator << (std::ostream &os, SimObject &object) {
	return os << object._debugId();
}

CSP_NAMESPACE_END

#endif // __CSPSIM_BATTLEFIELD_SIMOBJECT_H__

