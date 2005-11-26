/* Combat Simulator Project
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/**
 * @file Timing.h
 * @brief Uniform interface to platform specific "fast" timing functions.
 */


#ifndef __CSPLIB_UTIL_TIMING_H__
#define __CSPLIB_UTIL_TIMING_H__

#include <csp/lib/util/Exception.h>
#include <csp/lib/util/Export.h>
#include <csp/lib/util/Namespace.h>


CSP_NAMESPACE

/// Exception: Errors related to time measurement and syncronization.
CSP_EXCEPTION(TimerError)


/////////////////////////////////////////////////////////////
// 'fast' timing routines (1-2 msec accuracy)

/** Floating point time, in seconds.  Can be a relative time or an
 *  absolute time with respect to the system epoch (platform dependent).
 */
typedef double timing_t;

/** Return the current time in seconds.
 *
 *  The offset is platform dependent, so do not use this value for absolute time.
 */
CSP_EXPORT timing_t get_realtime();

/** Return seconds since the Unix epoch (1970-01-01T00:00:00Z).  The resolution is
 *  system dependenent, varying from a few usec on Linux to about 15 ms on WinXP.
 *  If the system time is accurate (e.g. using NTP), this provides a good but non-
 *  realtime measure of the true time.
 */
CSP_EXPORT double getSecondsSinceUnixEpoch();

/** Calibrate the high-performance counter used for high-resolution timing.  The
 *  first call establishes the offset between system time and the CPU counter.
 *  Subsequent calls adjust the counter frequency estimate.
 *
 *  On Windows, the first call to this function can block for up to about 150 ms,
 *  and subsequent calls can block for about 15 ms.  This method should be called
 *  frequently, as it schedules the actual calibration events internally.  These
 *  events initally occur every few seconds, with the interval doubling each time
 *  until a steady rate of about one event per hour is reached.  So even calling
 *  this function as often as 100 times per second (e.g. in the main simulation
 *  loop) should produce negligible overhead.
 *
 *  This function is a noop on Unix.
 */
CSP_EXPORT void calibrateRealTime();

/** Get the current time, in seconds.  Like getSecondsSinceUnixEpoch, the time
 *  value is relative to the Unix epoch.  Unlike getSecondsSinceUnixEpoch, the
 *  resolution is a few microseconds on most platforms (including Windows).
 *  On Windows, the time should agree with the system time to within a few tens
 *  of ms, provided that the system time and high-performance counters are
 *  stable.  On Linux this function simply wraps gettimeofday, giving the
 *  system time with us resolution on most architectures.
 *
 *  NB: to obtain accurate calibrated times, you should call calibrateRealTime
 *  periodically.
 */
CSP_EXPORT double getCalibratedRealTime();

/** A simple timing class with standard stopwatch functions.  The
 *  timing precision is equivalent to get_realtime().
 */
class Timer {
	timing_t _start;
	timing_t _elapsed;
	bool _running;

public:
	/// Construct a new timer.
	Timer(): _start(0), _elapsed(0), _running(false) { }

	/// Reset the elapsed time to zero and start the timer running.
	inline void start() {
		_elapsed = 0.0;
		resume();
	}

	/// Resumes counting without reseting the elapsed time.
	inline void resume() {
		if (!_running) {
			_start = get_realtime();
			_running = true;
		}
	}

	/// Stop the timer.
	inline timing_t stop() {
		if (_running) {
			_elapsed += get_realtime() - _start;
			_running = false;
		}
		return _elapsed;
	}

	/// Return true if the timer is running.
	inline bool running() const { return _running; }

	/// Get the total elasped time.
	inline timing_t elapsed() const {
		return (_running ? get_realtime() - _start + _elapsed : _elapsed);
	}

	/** Reset the elapsed time to zero (does not stop the timer).
	 */
	inline void reset() {
		if (_running) {
			_start = get_realtime();
		}
		_elapsed = 0;
	}

	/** Return the elapsed time and reset the timer to zero (does not
	 *  stop the timer).
	 */
	inline timing_t incremental() {
		timing_t elapsed = _elapsed;
		_elapsed = 0;
		if (_running) {
			timing_t start = _start;
			_start = get_realtime();
			elapsed += (_start - start);
		}
		return elapsed;
	}

};


/////////////////////////////////////////////////////////////
// end of timing routines

CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_TIMING_H__
