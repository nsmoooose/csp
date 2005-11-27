// Combat Simulator Project
// Copyright (C) 2003 The Combat Simulator Project
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
 * @file SynchronousUpdate.h
 *
 **/


#ifndef __CSPLIB_UTIL_SYNCHRONOUSUPDATE_H__
#define __CSPLIB_UTIL_SYNCHRONOUSUPDATE_H__


#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Ref.h>

#include <algorithm>
#include <vector>
#include <list>
#include <functional>


CSP_NAMESPACE

class UpdateProxy;
class UpdateMaster;


class UpdateTarget {
friend class UpdateProxy;
private:
	/// A proxy for connecting to an UpdateMaster.
	Ref<UpdateProxy> m_UpdateProxy;

	/** Detach from the update proxy.
	 */
	void detachUpdateProxy();

	UpdateMaster *getMaster() const;

public:
	/** Register our update callback with an UpdateMaster.
	 *
	 *  Note that the UpdateMaster itself must be updated in order
	 *  for our onUpdate() method to be called.  The UpdateMaster
	 *  instance is usually updated as part of the main simulation
	 *  loop.
	 */
	virtual void registerUpdate(UpdateMaster *master);

	/** Disconnect from the UpdateMaster.
	 *
	 *  An UpdateTarget can also disconnect by returning a negative
	 *  value from onUpdate().
	 */
	virtual void disconnectFromUpdateMaster();

	virtual void copyRegistration(UpdateTarget const *target) {
		if (target) {
			registerUpdate(target->getMaster());
		}
	}

	/** Disconnect our update callback.
	 */
	virtual ~UpdateTarget();

	/** Update callback.
	 *
	 *  @param dt The time interval (in seconds) since the last call
	 *            to onUpdate() or from the time of registration until 
	 *            the first callback.
	 *  @returns The minimum time until the next update callback.  Return
	 *           0.0 to receive the soonest possible callback (typically
	 *           one frame later).  Negative return values will (permanantly)
	 *           disconnect the callback.
	 */
	virtual double onUpdate(double dt) { dt = dt; return -1.0; }
};



class UpdateProxy: public Referenced {
friend class UpdateMaster;
friend class UpdateTarget;

public:
	/// Convenience typedef for UpdateProxy references.
	typedef Ref<UpdateProxy> Ref;

	/** Get the (internal) time value of the next update.
	 */
	double nextUpdateTime() const { return m_NextUpdateTime; }

	/** Get the (internal) time value at the last update.
	 */
	double lastUpdateTime() const { return m_LastUpdateTime; }

	/** Disconnect from the target.
	 */
	void detachTarget() { 
		if (m_Target) {
			m_Target->detachUpdateProxy();
			m_Target = 0;
		}
	}

private:

	/// Connection to an update master.
	UpdateMaster *m_Master;

	/// Connection to an update target.
	UpdateTarget *m_Target;

	/// UpdateMaster time of last update.
	double m_LastUpdateTime;

	/// UpdateMaster time of next update.
	double m_NextUpdateTime;

	/// Update return modes.
	enum { DEAD, IMMEDIATE, DELAYED };

	/** Disconnect from the target.
	 *
	 *  NOTE: this method should only be called by ~UpdateTarget().
	 */
	void targetSelfDetach() { 
		m_Target = 0; 
	}

	/** Update the target.
	 *
	 *  @param time The (internal) time value of the master.
	 */
	int update(double time);

	/** Construct a new proxy to connect a target to a master.
	 *
	 *  @param target The update target.
	 *  @param master The update master.
	 *  @param time The current (internal) time value of the master.
	 */
	UpdateProxy(UpdateTarget *target, UpdateMaster *master, double time): 
		m_Master(master), m_Target(target), m_LastUpdateTime(time), m_NextUpdateTime(time) 
	{ 
	}

	/** Test if connected to an UpdateTarget.
	 */
	bool hasTarget() const { return m_Target != 0; }

	/** Test if connected to an UpdateMaster.
	 */
	bool hasMaster() const { return m_Master != 0; }

	/** Get the connected UpdateMaster (if any).
	 */
	UpdateMaster *getMaster() const { return m_Master; }

};


/** Update scheduling class.
 *
 *  UpdateMaster instances activate onUpdate() callback methods of attached
 *  UpdateTarget instances.  Attachments are made via UpdateProxy instances
 *  which automatically disconnect the callbacks if the target is destroyed.
 *  The UpdateMaster maintains a separate list for callbacks that require
 *  "immediate" updates (i.e. as soon as possible), and a priority queue for
 *  callbacks that only require delayed updates.  The minimum time until the
 *  next update callback is determined by the return value of onUpdate(),
 *  where <= 0 means immediate.  Of course callbacks can only occur as
 *  often as UpdateMaster::update is called, and this interval will also
 *  determine the granularity of delayed callback intervals.
 */
class UpdateMaster {

	/// Binary predicate for prioritizing delayed update callbacks.
	struct UpdatePriority: public std::binary_function<UpdateProxy::Ref,UpdateProxy::Ref,bool> {
		bool operator()(UpdateProxy::Ref &a, UpdateProxy::Ref &b) {
			return (a->nextUpdateTime() > b->nextUpdateTime());
		}
	} m_Priority;

	typedef std::vector<UpdateProxy::Ref> UpdateVector;
	typedef std::list<UpdateProxy::Ref> UpdateList;

	/// A priority queue of delayed callbacks
	UpdateVector m_DelayQueue;

	/// A list of callbacks requiring "immediate" service
	UpdateList m_ShortList;

	/// A list used to transfer callbacks from the short list to the delay queue
	UpdateVector m_Transfer;

	/// The current internal time used for prioritization
	double m_Time;

public:

	/** Default constructor.
	 */
	UpdateMaster(): m_Time(0.0) {
		m_Transfer.reserve(64);
		m_DelayQueue.reserve(64);
	}

	/** Connect a new update callback.
	 *
	 *  @param target The UpdateTarget instance to register.
	 *  @param delay The delay until the first callback (default 0.0)
	 */
	UpdateProxy::Ref registerUpdate(UpdateTarget *target, double delay = 0.0);

	/** Update immediate callbacks and any delayed callbacks that are ready.
	 */
	void update(double dt);

};

CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_SYNCHRONOUSUPDATE_H__

