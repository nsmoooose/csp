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
 * Generate random number with a normal distribution.
 *
 * Classic, fast method using polar coordinates.  See
 * http://mathworld.wolfram.com/Box-MullerTransformation.html
 *
 * @param mean Mean value of the distribution.
 * @param sigma Standard deviation of the distribution.
 */
float box_muller(float mean, float sigma);


/**
 * Random number class that encapsulates a seeded random number generator.
 * Random number generation is platform independent.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT Random {
	long _state;
	float _r;
public:
	/**
	 * Create a new random number generator.
	 *
	 * @param seed seed value for the ran2 generator.
	 */
	Random(long seed=-1) {
		_state = seed;
		ran2(_state);
		_r = ran2(_state);
	}

	/**
	 * Get the current random number.
	 *
	 * Multiple calls will return the same value.
	 */
	float getRand() const {
		return _r;
	}

	/**
	 * Get the current state variable of the ran2 generator.
	 */
	long getState() const {
		return _state;
	}

	/**
	 * Generate a new (pseude) random number and return it.
	 *
	 * @return pseudo-random number in the range 0-1
	 */
	float newRand() {
		_r = ran2(_state);
		return _r;
	}

	/**
	 * Random number generator.
	 * 
	 * Return a random floating point value between 0.0 and 1.0 exclusive.  
	 * If idum is negative, a new series starts (and idum is made positive 
	 * so that subsequent calls using an unchanged idum will continue in 
	 * the same sequence).
	 *
	 * This random number generator is from William H. Press, et al.,
	 * _Numerical Recipes in C_, Second Ed. with corrections (1994), 
	 * p. 282.  This excellent book is available through the
	 * WWW at http://nr.harvard.edu/nr/bookc.html.
	 * The specific section concerning ran2, Section 7.1, is in
	 * http://cfatab.harvard.edu/nr/bookc/c7-1.ps
	 *
	 * @param idum state variable.
	 */
	float ran2(long &idum);
};


/**
 * Random number class that encapsulates a seeded random number
 * generator and for both flat and gaussian distributions.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT Gauss: public Random {
	float _mean, _sigma, _g;
public:
	/**
 	 * Create a new gaussian random number with the specified 
	 * distribution.
	 *
	 * @param mean the center of the distribution
	 * @param sigma the standard deviation of the distribution
	 */
	Gauss(float mean=0.0, float sigma=1.0);

	/**
	 * Generate a random number with a normal distribution.
	 *
	 * Classic, fast method using polar coordinates.  See
	 * http://mathworld.wolfram.com/Box-MullerTransformation.html
	 *
	 * @param mean Mean value of the distribution.
	 * @param sigma Standard deviation of the distribution.
	 */
	float box_muller(float mean, float sigma);

	/**
	 * Generate a new (pseudo) random number with a gaussian distribution 
	 */
	float newGauss();

	/**
	 * Return the last gaussion random number.
	 *
	 * Repeated calls will return the same value.
	 */
	float getGauss() const { return _g; }
};
	

/**
 * Global random number generator.
 */

SIMDATA_EXPORT extern Random g_Random;


NAMESPACE_END // namespace simdata


#endif //__RANDOM_H__

