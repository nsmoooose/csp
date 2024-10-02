#pragma once
// Combat Simulator Project
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
 * @file Profile.h
 *
 * Very simple profiling macros.  Place PROF0(id) / PROF1(id, n) pairs
 * around the code you want to time.  The 'id' parameter must be a unique
 * identifier (valid C identifier) that will be used to label the test.
 * The 'n' parameter is the number of times to average before recording
 * the result to the log file.  Logging goes to the PROFILE target, so
 * you must enable this (--log=PROFILE on the CSPSim.py command line) to
 * see the timing results.  Each log entry contains the 'id' identifier
 * and the average execution time in milliseconds.
 *
 **/

#include <csp/csplib/util/Log.h>
#include <csp/csplib/data/Date.h>

#define PROF0(id) \
	static int id##_c = 0; \
	static CSP(SimTime) id##_t = 0.0; \
	CSP(SimTime) id##_t0; \
	id##_t0 = CSP(SimDate)::getSystemTime();

#define PROF1(id, n) \
	id##_t += CSP(SimDate)::getSystemTime() - id##_t0; \
	if (++id##_c >= n) { \
		CSPLOG(DEBUG, TIMING) << "PROF " << #id << ": " << (1000.0*id##_t/id##_c) << " ms"; \
		id##_t = 0; \
		id##_c = 0; \
	}
