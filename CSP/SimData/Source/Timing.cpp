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
		static int first = 1;
		LARGE_INTEGER x;
		double now;
		if (first) {
			QueryPerformanceFrequency(&x);
			first = 0;
			scale = 1.0 / (double)x.QuadPart;
		}
		QueryPerformanceCounter(&x);
		now = (double)x.QuadPart;
		return (timing_t) (now * scale);
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

#endif


NAMESPACE_SIMDATA_END

