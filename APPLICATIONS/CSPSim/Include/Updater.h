// Combat Simulator Project - CSPSim
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
 * @file Updater.h
 *
 **/


#ifndef __UPDATER_H__
#define __UPDATER_H__


#include <SimData/Ref.h>
#include <sigc++/signal_system.h>
#include <list>


/** An interface for implementing synchronous timed updates.
 *
 *  The simulation calls update(dt) after each frame, with
 *  dt equal to the time between frames.  This method in
 *  turn calls onUpdate(), which should be extended by
 *  subclasses to implement time-dependent behavior.  The 
 *  onUpdate() method returns the minimum time to sleep 
 *  before the next call.  A return value of zero means to 
 *  call onUpdate() again as soon as the next update() call 
 *  occurs.  Longer return values will mean that several 
 *  update() calls will take place before the next onUpdate() 
 *  call.  
 *
 *  Additional support is provided for timed callbacks and 
 *  chaining update calls to child objects.
 */
class Updater: public virtual simdata::Referenced, public virtual SigC::Object {
public:
	/// Updater reference (convenience) type.
	typedef simdata::Ref<Updater> Ref;

protected:
	/// A signal type for interval timer callbacks
	typedef SigC::Signal1<void,double> UpdateSignal;

	/**
	 * Default constructor.
	 */
	Updater(): m_Sleep(0.0), m_Time(0.0) {}

private:

	/** An internal structure for storing timed update states.
	 */
	struct TimedUpdate: public simdata::Referenced {
		UpdateSignal _signal;
		double _interval;
		double _lapse;
		int _count;
		bool _dead;
		bool update(double dt) {
			_lapse += dt;
			if (!_dead && _lapse > _interval) {
				_signal.emit(_lapse);
				_lapse = 0.0;
				if (_count > 0 && --_count == 0) {
					_dead = true;
				}
			}
			return _dead;
		}
		TimedUpdate(double interval, int count): 
			_interval(interval), 
			_lapse(0.0), 
			_count(count)
		{
		}
	};

	/** An internal function adapter for updating TimedUpdates
	 */
	struct UpdateAndExpireOp {
		double dt;
		UpdateAndExpireOp(double dt_): dt(dt_) {}
		bool operator()(simdata::Ref<TimedUpdate> &tu) { return tu->update(dt); }
	};

	/** An internal predicate for updater removal.
	 */
	struct MatchesSignalOp {
		UpdateSignal const &signal;
		MatchesSignalOp(UpdateSignal const &signal_): signal(signal_) {}
		void operator()(simdata::Ref<TimedUpdate> &tu) { 
			if (&(tu->_signal) == &signal) {
				tu->_dead = true;
			}
		}
	};

	/** An internal helper for updating and culling the timed updates list.
	 */
	template<typename C, typename T>
	T UpdateAndCull(C &c, T t) {
		c.erase(std::remove_if(c.begin(), c.end(), t), c.end());
		return t;
	}

	/** A list of active timed updates.
	 */
	std::list< simdata::Ref<TimedUpdate> > m_TimedUpdates;

	/** An internal predicate for updating child objects.
	 */
	class UpdateOp {
		double m_dt;
	public:
		UpdateOp(double dt): m_dt(dt) {}
		void operator()(Updater::Ref u) { u->update(m_dt); }
	};

	/// Time remaining until next onUpdate() call.
	double m_Sleep;

	/// Time interval between calls to onUpdate().
	double m_Interval;

	/** Internal method to drive timed updates and call onUpdate().
	 */
	void __update() {
		double dt = m_Interval - m_Sleep;
		UpdateAndCull(m_TimedUpdates, UpdateAndExpireOp(dt));
		m_Interval = onUpdate(dt);
		m_Sleep = m_Interval;
	}

protected:
	/** Create a new timed update signal.
	 *
	 *  Connect callback slots to the returned update signal,
	 *  which will then be called automatically at regular
	 *  intervals during the usual onUpdate() handler.
	 *
	 *  @param interval The time interval between update signals
	 *                  in seconds.
	 *  @param count The number of calls before the signal is
	 *               destroyed (0 for perpetual).
	 *  @returns A SigC signal object (see the libsigc++ documentation
	 *           for details on connecting callback methods).
	 */
	UpdateSignal &addTimedUpdate(double interval, int count=0) {
		simdata::Ref<TimedUpdate> tu = new TimedUpdate(interval, count);
		m_TimedUpdates.push_back(tu);
		return tu->_signal;
	}

	/** Remove a timed update signal.
	 *
	 *  @param signal The UpdateSignal reference returned by 
	 *                addTimedUpdate.
	 */
	void removeTimedUpdate(UpdateSignal const &signal) {
		std::for_each(m_TimedUpdates.begin(),
			      m_TimedUpdates.end(),
			      MatchesSignalOp(signal));
	}

	/** A helper template for updating child objects.
	 *
	 *  @param children An STL forward container supporting begin() and end().
	 *  @param dt The time interval since the last update.
	 */
	template <typename C>
	void updateChildren(C &children, double dt) {
		std::for_each(children.begin(), children.end(), UpdateOp(dt));
	}

	/** An update method to be implemented in derived classes.
	 *
	 *  This method is called by update() to drive time-dependent
	 *  behavior in derived classes.  Subclasses that have child
	 *  components stored in a list should use updateChildren() to
	 *  propagate the update, or they may simply call each child's
	 *  update() method by hand.  The return value is the minimum
	 *  time interval to "sleep" until the next onUpdate() call.  
	 *  This value should be used by Objects that do not need 
	 *  frequent updates to reduce processing overhead.  Bear in 
	 *  mind that the minimum time resolution for the sleep interval
	 *  is determined by frequency of update() calls.  It is also
	 *  very important to note that the timed update callbacks (set
	 *  with addTimedUpdate) are only checked when onUpdate() is 
	 *  called, so long sleep intervals can interfere with these 
	 *  updates.
	 *
	 *  @param dt Time since the last call to onUpdate().
	 *  @returns The minimum time to sleep (in seconds) until the 
	 *           next onUpdate() call.  Returning zero gives the
	 *           maximum rate of once per update() call.
	 */
	virtual double onUpdate(double dt) = 0;

public:

	/** Update this object.
	 *
	 *  This method waits until the time to sleep from the last
	 *  onUpdate() call has expired, then drives the timed
	 *  interval callbacks and calls onUpdate().
	 *
	 *  @param dt The time interval since the last call to update().
	 */
	inline void update(double dt) {
		m_Sleep -= dt;
		if (m_Sleep < 0.0) __update();
	}

};


#endif // __UPDATER_H__

