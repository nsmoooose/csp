// Combat Simulator Project - CSPSim
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file LocalUpdate.h
 *
 **/

#ifndef __CSPLIB_UTIL_LOCALUPDATE_H__
#define __CSPLIB_UTIL_LOCALUPDATE_H__

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>
#include <vector>
#include <sigc++/slot.h>


namespace csp {

/** A utility class for coordinating infrequest events within a SynchronousUpdate
 *  handler.  An UpdateTarget can register to receive updates at a given interval,
 *  but often the handler needs to perform multiple tasks, each at different
 *  intervals.  This class allows the UpdateTarget handler to efficiently dispatch
 *  updates to multiple subhandlers on independent schedules.
 *
 *  Note that LocalUpdate retains pointers to the handler class.  The safest use
 *  is to make LocalUpdate a member variable, and only add handlers in the same
 *  instance.
 *
 *  Example:
 *
 *  @code
 *  class AircraftConsole: public UpdateTarget {
 *  public:
 *
 *    Aircraft() {
 *      // update warning lights every 0.5 seconds, and displays every 0.05 seconds.
 *      m_UpdateDispatch.addHandler(this, &AircraftConsole::updateWarningLights, 0.5);
 *      m_UpdateDispatch.addHandler(this, &AircraftConsole::updateDisplays, 0.05);
 *    }
 *
 *  private:
 *
 *    // normal UpdateTarget handler.
 *    virtual double update(double dt) {
 *      // optional: other update code here.
 *
 *      // return the interval to the next scheduled event so that the UpdateMaster
 *      // won't call this method unnecessarily.  note that if update() does high-
 *      // priority work as well it might be appropriate to return 0 instead.
 *      return m_UpdateDispatch(dt);
 *    }
 *
 *    // local update handlers.
 *    // note that the return value of the handler has the following meaning:
 *    //   < 0: disable this handler.  no further calls will be made to the handler
 *    //        unless addHandler is called again.
 *    //   = 0: run again after the default handler interval has expired.
 *    //   > 0: run again after this time interval has expired.
 *
 *    double updateWarningLights(double dt);
 *    double updateDisplays(double dt);
 *
 *    LocalUpdate m_UpdateDispatch;
 *  };
 *  @endcode
 */
class CSPLIB_EXPORT LocalUpdate {
public:
	LocalUpdate(): m_Time(0), m_Next(0) { }
	~LocalUpdate();

	/** Register a handler to receive updates at the specified interval, in seconds.
	 *  The interval must be greater than zero.
	 */
	template <class C> void addHandler(C *target, double (C::*method)(double), double interval) {
		_addHandler(sigc::mem_fun(target, method), interval);
	}

	/** Dispatch updates to all handlers that are ready.  Returns the time interval
	 *  to the next waiting handler.
	 */
	double operator()(double dt) {
		m_Time += dt;
		if (m_Time > m_Next && !m_Queue.empty()) dispatch();
		// note: returns negative values if the queue is empty, which if returned directly
		// by the UpdateTarget will disconnect it from the UpdateMaster.  this may or may
		// not be what is intended, but there's no obviously better thing to do here.
		return m_Next - m_Time;
	}

private:
	typedef sigc::slot<double, double> Callback;
	class PriorityHandler;

	double m_Time;  // internal time (not absolute)
	double m_Next;  // time of the next handler call
	std::vector<PriorityHandler*> m_Queue;

	void _addHandler(Callback const &cb, double interval);
	void dispatch();
};

} // namespace csp

#endif // __CSPLIB_UTIL_LOCALUPDATE_H__

