/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This file is part of SimData.
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


#ifndef __SIMDATA_TIMING_H__
#define __SIMDATA_TIMING_H__

#include <SimData/Export.h>
#include <SimData/Namespace.h>


NAMESPACE_SIMDATA

/////////////////////////////////////////////////////////////
// 'fast' timing routines (1-2 msec accuracy)

/** Floating point time, in seconds.  Can be a relative time or an
 *  absolute time with respect to the system epoch (platform dependent).
 */
typedef double timing_t;

/** Start the timer.
 */
void SIMDATA_EXPORT tstart(void);

/** Stop the timer.
 */
void SIMDATA_EXPORT tend(void);

/** Get the time interval (in seconds) between last calls to tstart()
 *  and tend().
 *
 *  The precision should be about 1-2 ms on most platforms.
 */
double SIMDATA_EXPORT tval();

/** Return the current time in seconds.
 *
 *  The offset is platform dependent, so do not use this value for
 *  absolute time.
 */
timing_t get_realtime();


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

NAMESPACE_SIMDATA_END

#endif // __SIMDATA_TIMING_H__
