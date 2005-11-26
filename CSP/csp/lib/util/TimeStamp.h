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
 * @file TimeStamp.h
 * @brief Utilities functions for working with network event timestamps.
 **/


#ifndef __CSPLIB_UTIL_TIMESTAMP_H__
#define __CSPLIB_UTIL_TIMESTAMP_H__

#include <csp/lib/util/Namespace.h>
#include <csp/lib/util/Uniform.h>
#include <csp/lib/data/Date.h>


CSP_NAMESPACE

/// Network event time, which is a rolling counter with 1us granularity
typedef int32 TimeStamp;


/** Get the time difference between two timestamps.
 *  @return The time difference, in seconds, up to a maximum of
 *   about +/- 2147 seconds (rolls over beyond that).
 */
inline SimTime timeStampDelta(TimeStamp a, TimeStamp b) {
	return (a - b) * 1e-6;
}

/** Convert the simulation time to a timestamp.
 */
inline TimeStamp getTimeStamp(SimTime time) {
	return static_cast<TimeStamp>(fmod(time * 1000000.0, 4000000000.0) - 2000000000.0);
}


CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_TIMESTAMP_H__
