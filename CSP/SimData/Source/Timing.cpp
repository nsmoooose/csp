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
 * @file Timing.cpp
 * @brief Uniform interface to platform specific "fast" timing functions.
 */

#include <SimData/Timing.h>
#include <SimData/Log.h>
#include <SimData/Uniform.h>

// for platform-specific fast timing routines
#ifdef _WIN32
	#include <Windows.h>
#else
	#include <sys/time.h>
	#include <unistd.h>
#endif


NAMESPACE_SIMDATA


#ifdef _WIN32
	static LARGE_INTEGER _tstart, _tend;
	static LARGE_INTEGER freq;

	void tstart(void) {
		static int first = 1;
		if (first) {
			QueryPerformanceFrequency(&freq);
			first = 0;
		}
		QueryPerformanceCounter(&_tstart);
	}

	void tend(void) {
		QueryPerformanceCounter(&_tend);
	}

	double tval() {
		return ((double)_tend.QuadPart -
			(double)_tstart.QuadPart)/((double)freq.QuadPart);
	}

	timing_t get_realtime() {
		static double scale;
		static bool first = true;
		LARGE_INTEGER x;
		double now;
		if (first) {
			QueryPerformanceFrequency(&x);
			first = false;
			scale = 1.0 / (double)x.QuadPart;
		}
		QueryPerformanceCounter(&x);
		now = (double)x.QuadPart;
		return (timing_t) (now * scale);
	}

	// Return seconds since the Unix epoch (1970-01-01T00:00:00Z), with an accuracy of
	// about 16 msec (limited by the 64 Hz timer tick rate under WinXP).
	double getSecondsSinceUnixEpoch() {
		FILETIME filetime;
		GetSystemTimeAsFileTime(&filetime);
		uint64 date_time = (static_cast<uint64>(filetime.dwHighDateTime) << 32) | static_cast<uint64>(filetime.dwLowDateTime);
		// convert from hectonanoseconds since 1601-01-01T00:00:00Z to milliseconds since 1970-01-01T00:00:00Z
		return 1e-7 * (date_time - SIMDATA_ULL(116444736000000000));
	}

	static uint64 first_counter = 0;
	static uint64 first_time = 0;
	static double counter_scale = 0.0;
	static double counter_offset = 0.0;
	static int calibrations = 0;
	static uint64 next_calibration = 0;
	static double calibration_interval = 10.0;

	inline uint64 cvt_filetime(FILETIME const &x) {
		return (static_cast<uint64>(x.dwHighDateTime) << 32) | static_cast<uint64>(x.dwLowDateTime);
	}

	inline uint64 getPerformanceFrequency() {
		LARGE_INTEGER frequency;
		if (!QueryPerformanceFrequency(&frequency)) {
			throw TimerError("QueryPerformanceFrequency failed");
		}
		return static_cast<uint64>(frequency.QuadPart);
	}

	inline uint64 getPerformanceCounter() {
		LARGE_INTEGER counter;
		if (!QueryPerformanceCounter(&counter)) {
			throw TimerError("QueryPerformanceCounter failed");
		}
		return static_cast<uint64>(counter.QuadPart);
	}

	void calibrateRealTime() {
		int max_tries = 1;
		uint64 counter = 0;
		if (calibrations == 0) {
			counter_scale = 1.0 / static_cast<double>(getPerformanceFrequency());
			max_tries = 10;
		} else {
			if (getPerformanceCounter() < next_calibration) return;
		}
		FILETIME start_time, update_time;
		GetSystemTimeAsFileTime(&start_time);
		// blocks for up to max_tries * 16 ms on WinXP (assuming 64Hz timer tick)
		int tries = 0;
		for (; tries < max_tries; ) {
			uint64 lock_counter = getPerformanceCounter();
			GetSystemTimeAsFileTime(&update_time);
			if (update_time.dwLowDateTime != start_time.dwLowDateTime) {
				counter = getPerformanceCounter();
				// make sure we weren't interrupted between getting the system time and the counter
				if ((counter - lock_counter) * counter_scale <= 0.0001) break;
				// make sure we weren't interrupted for more than one timer tick.  it's still
				// possible that we could be interrupted for less than 20 ms, causing jitter.
				// not much to do about that, except to hope that the return of control to
				// this thread corresponded to a timer tick event so that we are at the start
				// of the tick window.
				if ((cvt_filetime(update_time) - cvt_filetime(start_time)) < SIMDATA_ULL(200000) /* 20 ms */) break;
				// we were probably interrupted (>100us between GSTAFT and QPC), so try again
				++tries;
			}
		}
		if (tries >= max_tries) {
			if (calibrations == 0) {
				throw TimerError("Unable to calibrate high resolution timer using system time");
			}
			SIMDATA_LOG(LOG_TIME, LOG_ERROR, "High resolution timer calibration failed");
		} else {
			if (calibrations++ == 0) {
				first_counter = counter;
				first_time = cvt_filetime(update_time);
				counter_offset = 1e-7 * static_cast<double>(first_time - SIMDATA_ULL(116444736000000000));
			} else {
				uint64 elapsed_count = counter - first_counter;
				uint64 elapsed_time = cvt_filetime(update_time);
				double new_scale = static_cast<double>(elapsed_time) * 1e-7 / elapsed_count;
				// bump the offset to keep the current calibrated time continuous
				counter_offset += elapsed_count * (counter_scale - new_scale);
				counter_scale = new_scale;
				if (calibration_interval < 3600.0) calibration_interval *= 2.0;
			}
		}
		// schedule the next calibration
		next_calibration = counter + static_cast<uint64>(calibration_interval / counter_scale);
	}

	double getCalibratedRealTime() {
		if (calibrations == 0) calibrateRealTime();
		return counter_offset + (getPerformanceCounter() - first_counter) * counter_scale;
	}

#else

	static struct timeval _tstart, _tend;
	static struct timezone tz;

	void tstart(void) {
		gettimeofday(&_tstart, &tz);
	}

	void tend(void) {
		gettimeofday(&_tend,&tz);
	}

	double tval() {
		double t1, t2;
		t1 =  (double)_tstart.tv_sec + (double)_tstart.tv_usec/(1000*1000);
		t2 =  (double)_tend.tv_sec + (double)_tend.tv_usec/(1000*1000);
		return t2-t1;
	}

	timing_t get_realtime() {
		struct timezone tz_;
		struct timeval now;
		gettimeofday(&now, &tz_);
		return (timing_t) ((double)now.tv_sec + (double)now.tv_usec*1.0e-6);
	}

	// Return seconds since the Unix epoch (1970-01-01T00:00:00Z), with an accuracy of
	// a few usec (uses gettimeofday).
	double getSecondsSinceUnixEpoch() { return get_realtime(); }

	void calibrateRealTime() {}
	double getCalibratedRealTime() { return get_realtime(); }

#endif


NAMESPACE_SIMDATA_END

