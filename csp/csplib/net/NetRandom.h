#pragma once
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
 * @file NetRandom.h
 *
 */

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Uniform.h>

namespace csp {

/*
 *  Borrowed/adapted from linux-2.4.25/net/core/utils.c
 *  Original author: Alan Cox, distributed under the GNU General Public
 *  License, version 2.
 */

/** LCG pseudorandom number generator  used to drop packets under high load.
 *
 *  Uses the venerable 69069 generator.
 *  @ingroup net
 */
class CSPLIB_EXPORT NetRandom {
	// internal state for lcg
	static uint32_t RandSeed;

	// not currently used
	static uint32_t NetClock;

public:

	/** Get a new random number, from 0 to 4294967295.
	 */
	inline static uint32_t random() {
		RandSeed = RandSeed * 69069L + 1;
		return RandSeed ^ NetClock;
	}

	/** Add entropy to the generator.  In practice this is not generally
	 *  needed for packet culling.
	 *
	 *  @param entropy an arbitrary 32-bit value, ideally derived from
	 *    a high entropy source (e.g. /dev/random under Linux).
	 */
	inline static void randomize(uint32_t entropy) {
		RandSeed ^= entropy;
		random();
	}
};

} // namespace csp
