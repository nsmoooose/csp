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


/////////////////////////////////////////////////////////////
// end of timing routines

NAMESPACE_SIMDATA_END

#endif // __SIMDATA_TIMING_H__
