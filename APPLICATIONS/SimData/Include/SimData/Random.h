/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
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
 * @file Random.h
 *
 * Platform independent random number generation routines.
 */


#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <cmath>

#include <SimData/Export.h>
#include <SimData/ns-simdata.h>


NAMESPACE_SIMDATA


/**
 * Implementation of the Box Muller algorithm for generating a random
 * gaussian distribution.
 */
float box_muller(float, float);


/**
 * Random number class that encapsulates a seeded random number generator.
 * Random number generation is platform independent.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT Random {
	long _seed;
	float _r;
public:
	Random(long seed=-1) {
		_seed = seed;
		ran2(_seed);
		_r = ran2(_seed);
	}
	float getRand() const {
		return _r;
	}
	long getSeed() const {
		return _seed;
	}
	float newRand() {
		_r = ran2(_seed);
		return _r;
	}
	float ran2(long&);
};


/**
 * Random number class that encapsulates a seeded random number
 * generator and for both flat and gaussian distributions.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class Gauss: public Random {
	float _mean, _sigma, _g;
public:
	Gauss(float m=0.0, float s=1.0);
	float box_muller(float, float);
	float newGauss();
	float getGauss() const { return _g; }
};
	

/**
 * Global random number generator.
 */

SIMDATA_EXPORT extern Random g_Random;


NAMESPACE_END // namespace simdata


#endif //__RANDOM_H__

