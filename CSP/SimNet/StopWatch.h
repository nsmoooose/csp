// Combat Simulator Project
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file StopWatch.h
 *
 */

#ifndef __SIMNET_STOPWATCH_H__
#define __SIMNET_STOPWATCH_H__

#include <SimNet/NetLog.h>
#include <SimData/Timing.h>

namespace simnet {


/** Class for managing iterations of a timed loop.
 *
 *  StopWatch is intended to be used to regulate the number of iterations
 *  of tight loops that are restricted to run for a limited amount of time.
 *  Rather than continuously polling the system clock (which can incur
 *  significant overhead), we keep a running estimate of how long each
 *  iteration requires in order to predict how many iterations can fit
 *  within the alotted time.  If the loop takes longer than expected, the
 *  estimated time per iteration is increased.  If the iterations complete
 *  early, we increase the estimated time per iteration and continue with
 *  additional iterations.  Assuming that the iteration time is reasonably
 *  consistent, the estimated loop time and iteration counts quickly
 *  converge.
 *
 *  Example usage:
 *
 *  @code
 *  void loop(double timeout) {
 *    // static stopwatch timing data (not threadsafe as is)
 *    static simnet::StopWatch::Data swdata;
 *    StopWatch watch(timeout, swdata);
 *    watch.start();
 *    do {
 *      // main loop action here (may exit early with break;)
 *    } while (!watch.checkExpired());
 *  }
 *  @endcode
 *
 */
class StopWatch {
public:

	/** Internal timing data.  Declare separate, persistent Data instance for each
	 *  loop that needs to be timed.  Beware that using static Data instances isn't
	 *  thread-safe.
	 */
	struct Data {
		double scale;
		// default to 100 us per iteration inital guess, adjust as needed
		Data(float itertime=0.0001): scale(1.0/itertime) {}
	};

	/** Construct a new stop watch.
	 *
	 *  @param timeout The maximum time to spend in the timed loop (seconds).
	 *  @param data The persistent Data instance used to store timing data for this loop.
	 */
	StopWatch(double timeout, Data &data): m_data(data), m_tally(0), m_count(0), m_counts(0), m_timeout(timeout) {
		resetCount(timeout);
		SIMNET_LOG(TIMING, DEBUG, "STOPWATCH: " << m_counts << " iterations ~ " << (timeout * 1000.0) << " ms, scale=" << m_data.scale);
		m_DEBUG_extra = 0;
	}

	/** Start the stop watch.  Call this right before starting the timed loop.
	 */
	inline void start() { m_start_time = simdata::get_realtime(); }

	/** Force a check of the elapsed time, even if we still expect to be able to
	 *  complete more iterations in the allotted time.
	 *
	 *  Call this method after any unusual events occur within the timed loop that
	 *  may take much longer than the average iteration.  The iteration timing
	 *  estimate will not be updated, and additional iterations will be scheduled
	 *  if time remains.  If time has expired, the next checkExpired() will return
	 *  true and exit the loop.
	 */
	void forceCheck() {
		resetCount(m_timeout - elapsed());
	}

	/** Test if the timout has expired.
	 *
	 *  Simply counts iterations, returning false until the expected number have
	 *  completed.  At that point it checks the elapsed time and updates the time
	 *  per iteration estimate.  If more time is left, additional iterations are
	 *  scheduled and it returns false.  Otherwise it returns true.
	 */
	inline bool checkExpired() {
		if (--m_count > 0) return false;
		double dt = elapsed();
		if (dt > m_timeout) {
			SIMNET_LOG(TIMING, DEBUG, "STOPWATCH: TOO LONG (" << dt << " > " << m_timeout << "; " << m_DEBUG_extra << " extra)");
			if (m_counts > 1) {
				m_data.scale *= 0.99;
			}
			return true;
		}
		m_data.scale *= 1.01;
		resetCount(dt);
		if (m_count < 1) {
			SIMNET_LOG(TIMING, DEBUG, "STOPWATCH: TOO SHORT (" << (dt * 1000.0) << " <= " << (m_timeout*1000.0) << "ms; " << m_DEBUG_extra << " extra)");
		}
		m_DEBUG_extra += m_count;
		return (m_count < 1);
	}

	/** Updates the iteration time estimate based on all iterations to this
	 *  point.  Call this before exiting the loop early (ie. by any means
	 *  other than checkExpired()).
	 */
	inline void calibrate() {
		resetCount(0.0);
		if (m_tally >= 10) {
			double avg_freq = double(m_tally) / elapsed();
			if (avg_freq > m_data.scale) {
				m_data.scale *= 1.01;
			} else {
				m_data.scale *= 0.99;
			}
		}
	}

	/** Return the time elapsed since the watch was started.  Gets the
	 *  current system time in order to compute the elapsed time.
	 */
	inline double elapsed() {
		return simdata::get_realtime() - m_start_time;
	}

	/** Get the system time at which the watch was started.
	 */
	inline double getStartTime() {
		return m_start_time;
	}

private:
	Data &m_data;
	int m_tally;
	int m_count;
	int m_counts;
	bool m_DEBUG_extra;
	double m_timeout;
	double m_start_time;

	// set m_count and m_counts based on the current iteration time
	// estimate and the available time; also updates tally to keep
	// track of the total number of iterations.
	inline void resetCount(double timeout) {
		m_tally += m_counts - m_count;
		m_counts = int(timeout * m_data.scale);
		m_count = m_counts;
	}
};

} // namespace simnet

#endif // __SIMNET_STOPWATCH_H__

