/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
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

%module Random
%{
#include "SimData/Random.h"
%}

//%include "SimData/Random.h"

%include "SimData/Namespace.h"

NAMESPACE_SIMDATA


class RandomInterface {
protected:
public:
	typedef simdata::Ref<simdata::RandomStateWrapper> State;
	typedef unsigned long int SeedType;
	virtual ~RandomInterface();
	virtual void setSeed(SeedType seed)=0;
	virtual State getState() const=0;
	virtual void setState(State const &state)=0;
	virtual std::string getName() const=0;
};


/**
 * Abstract interface for random number generators.
 */
class RandomNumberGeneratorInterface: public RandomInterface {
public:
	virtual double unit()=0;
	virtual double uniform(double lower, double upper)=0;
	virtual long uniformInt(long lower, long upper)=0;
	virtual long uniformInt(long upper)=0;
	virtual unsigned long uniformUInt(unsigned long lower, unsigned long upper)=0;
	virtual unsigned long uniformUInt(unsigned long upper)=0;
};

template <class RNG>
class RandomNumberGenerator: public RandomNumberGeneratorInterface {
	RNG _gen;
public:
	/**
	 * Reseed the random number generator.
	 *
	 * @param seed the new seed.
	 */
	virtual void setSeed(SeedType seed);

	/**
	 * Save the current state of the generator.  The state can be
	 * restored at any time with setState().
	 */
	virtual State getState() const;

	/**
	 * Restore the generator to a specific state saved with getState().
	 * The subsequent random numbers will be identical to the sequence
	 * following the corresponding getState() call.
	 */
	virtual void setState(State const &);

	/**
	 * Return a random floating point value in the range [0, 1)
	 */
	virtual double unit();

	/**
	 * Return a random floating point value in the range [lower, upper)
	 */
	virtual double uniform(double, double);

	/**
	 * Return a random integer in the range lower to upper-1 (inclusive)
	 * @param lower lower bound (inclusive)
	 * @param upper upper bound (exclusive)
	 * @returns a random integer int the range lower to upper-1 (inclusive)
	 */
	virtual long uniformInt(long lower, long upper);

	/**
	 * Equivalent to uniformInt(0, upper)
	 */
	virtual long uniformInt(long upper);

	/**
	 * Return a random integer in the range lower to upper-1 (inclusive)
	 * @param lower lower bound (inclusive)
	 * @param upper upper bound (exclusive)
	 * @returns a random integer int the range lower to upper-1 (inclusive)
	 */
	virtual unsigned long uniformUInt(unsigned long lower, unsigned long upper);

	/**
	 * Equivalent to uniformUInt(0, upper)
	 */
	virtual unsigned long uniformUInt(unsigned long upper);

	/**
	 * Get the name of this generator
	 */
	virtual std::string getName() const;

	/**
	 * Direct access to the underlying generator, providing
	 * faster access and generator-specific methods.
	 */
	inline RNG *operator->() { return &_gen; }
};

%template(MersenneTwister)  RandomNumberGenerator<simdata::rng::MT19937>;
%template(Taus2) RandomNumberGenerator<simdata::rng::Taus2>;

NAMESPACE_SIMDATA_END

