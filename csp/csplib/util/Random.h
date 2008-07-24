/* Combat Simulator Project
 * Copyright (C) 2002, 2003 Mark Rose <mkrose@users.sf.net>
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
 * @brief Random number generators and distributions.
 */

#ifndef __CSPLIB_UTIL_RANDOM_H__
#define __CSPLIB_UTIL_RANDOM_H__


#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Export.h>
#include <cmath>


namespace csp {

///////////////////////////////////////////////////////////////////////////////////
// Random Number Generators

/** Low-level random number generator interfaces. */
namespace rng {

/**
 *  @brief Mersenne Twister random number generator.
 *
 *  Original implementation was copyright (C) 1997 Makoto Matsumoto and
 *  Takuji Nishimura. Coded by Takuji Nishimura, considering the
 *  suggestions by Topher Cooper and Marc Rieffel in July-Aug. 1997, "A
 *  C-program for MT19937: Integer version (1998/4/6)"
 *
 *  This class was adapted from the c-implementation of Matsumoto's
 *  algorithm that is part of the GNU Scientific Library.  That
 *  implementation is copyright (C) 1998 Brian Gough.
 *
 *  Brian Gough's notes follow:
 *
 *  I reorganized the code to use the module framework of GSL.  The
 *  license on this implementation was changed from LGPL to GPL,
 *  following paragraph 3 of the LGPL, version 2.
 *
 *  Update:
 *
 *  The seeding procedure has been updated to match the 10/99 release
 *  of MT19937.
 *
 *  Update:
 *
 *  The seeding procedure has been updated again to match the 2002
 *  release of MT19937
 *
 *  The original code included the comment: "When you use this, send an
 *  email to: matumoto@math.keio.ac.jp with an appropriate reference to
 *  your work".
 *
 *  Makoto Matsumoto has a web page with more information about the
 *  generator, http://www.math.keio.ac.jp/~matumoto/emt.html.
 *
 *  The paper below has details of the algorithm.
 *
 *  From: Makoto Matsumoto and Takuji Nishimura, "Mersenne Twister: A
 *  623-dimensionally equidistributerd uniform pseudorandom number
 *  generator". ACM Transactions on Modeling and Computer Simulation,
 *  Vol. 8, No. 1 (Jan. 1998), Pages 3-30
 *
 *  You can obtain the paper directly from Makoto Matsumoto's web page.
 *
 *  The period of this generator is 2^{19937} - 1.
 */
class CSPLIB_EXPORT MT19937 {
	static const int N = 624;	/* Period parameters */
	static const int M = 397;

	// most significant w-r bits
	static const unsigned long UPPER_MASK = 0x80000000UL;	

	// least significant r bits
	static const unsigned long LOWER_MASK = 0x7fffffffUL;	

	// state
	unsigned long _mt[N];
	int _mti;

	static inline unsigned long MAGIC(unsigned long y) {
		return (((y)&0x1) ? 0x9908b0dfUL : 0);
	}

	/** Internal call to update the generator.
	 */
	void update();
	
	/** Internal generator.
	 *
	 *  This method returns a random integer in the range [0,2**32).
	 *  After every N (624) calls the generator must be updated.
	 */
	inline unsigned long generate() {
		if (_mti >= N) update(); // generate N words at one time
		unsigned long k = _mt[_mti];
		k ^= (k >> 11);
		k ^= (k << 7) & 0x9d2c5680UL;
		k ^= (k << 15) & 0xefc60000UL;
		k ^= (k >> 18);
		_mti++;
		return k;
	}

public:
	/** Structure for saving and restoring the internal state of the generator.
	 */
	struct State {
		unsigned long _mt[N];
		int _mti;
	};

	/** Get the name of this generator.
	 */
	inline std::string getName() const { return "Mersenne Twister (MT19937)"; }

	/** Construct a new generator with seed 0.
	 */
	MT19937() { setSeed(0); }

	/** Return a random integer in the range lower to upper-1 (inclusive).
	 *
	 *  @param lower lower bound (inclusive)
	 *  @param upper upper bound (exclusive)
	 *  @returns a random integer int the range lower to upper-1 (inclusive)
	 */
	template <typename T>
	inline T uniformInt(T lower, T upper) {
		return lower + static_cast<T>(unit() * (upper-lower));
	}

	/** Equivalent to uniformInt(0, upper)
	 */
	template <typename T>
	inline T uniformInt(T upper) {
		return uniformInt(static_cast<T>(0), upper);
	}

	/** Return a random floating point value in the range [0, 1)
	 */
	inline double unit() {
  		return generate() / 4294967296.0 ;
	};

	/** Return a random floating point value in the range [lower, upper)
	 */
	inline double uniform(double lower, double upper) {
		return lower + unit() * (upper-lower);
	}

	/** Reseed the random number generator.
	 *
	 *  @param s the new seed.
	 */
	void setSeed(unsigned long s);

	/** Save the current state of the generator.
	 *
	 *  The state can be restored at any time with setState().
	 */
	void getState(State &state) const;

	/** Restore the generator to a specific state saved with getState().
	 *
	 *  The subsequent random numbers will be identical to the sequence
	 *  following the corresponding getState() call.
	 */
	void setState(State const &state);
};




/**
 * @brief Maximally Equidistributed Combined Tausworthe generator.
 *
 * This class was adapted from the c-implementation of the taus2
 * random number generator included in the GNU Scientific Library
 * (rng/taus.c).  That implementation is copyright (C) 1996, 1997, 1998,
 * 1999, 2000 James Theiler, Brian Gough.
 *
 * Comments from the GSL implementation follow:
 *
 * This is a maximally equidistributed combined Tausworthe generator.
 * The sequence is,
 *
 * x_n = (s1_n ^ s2_n ^ s3_n)
 *
 * s1_{n+1} = (((s1_n & 4294967294) <<12) ^ (((s1_n <<13) ^ s1_n) >>19))
 * s2_{n+1} = (((s2_n & 4294967288) << 4) ^ (((s2_n << 2) ^ s2_n) >>25))
 * s3_{n+1} = (((s3_n & 4294967280) <<17) ^ (((s3_n << 3) ^ s3_n) >>11))
 *
 * computed modulo 2^32. In the three formulas above '^' means
 * exclusive-or (C-notation), not exponentiation. Note that the
 * algorithm relies on the properties of 32-bit unsigned integers (it
 * is formally defined on bit-vectors of length 32). I have added a
 * bitmask to make it work on 64 bit machines.
 *
 * We initialize the generator with s1_1 .. s3_1 = s_n MOD m, where
 * s_n = (69069 * s_{n-1}) mod 2^32, and s_0 = s is the user-supplied
 * seed.
 *
 * The theoretical value of x_{10007} is 2733957125. The subscript
 * 10007 means (1) seed the generator with s=1 (2) do six warm-up
 * iterations, (3) then do 10000 actual iterations.
 *
 * The period of this generator is about 2^88.
 *
 * From: P. L'Ecuyer, "Maximally Equidistributed Combined Tausworthe
 * Generators", Mathematics of Computation, 65, 213 (1996), 203--213.
 *
 * This is available on the net from L'Ecuyer's home page,
 *
 * http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme.ps
 * ftp://ftp.iro.umontreal.ca/pub/simulation/lecuyer/papers/tausme.ps
 *
 * Update: April 2002
 *
 * There is an erratum in the paper "Tables of Maximally
 * Equidistributed Combined LFSR Generators", Mathematics of
 * Computation, 68, 225 (1999), 261--269:
 * http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme2.ps
 *
 *	... the k_j most significant bits of z_j must be non-
 *	zero, for each j. (Note: this restriction also applies to the
 *	computer code given in [4], but was mistakenly not mentioned in
 *	that paper.)
 *
 * This affects the seeding procedure by imposing the requirement
 * s1 > 1, s2 > 7, s3 > 15.
 *
 * The generator taus2 has been added to satisfy this requirement.
 * The original taus generator is unchanged.
 *
 * Update: November 2002
 *
 * There was a bug in the correction to the seeding procedure for s2.
 * It affected the following seeds 254679140 1264751179 1519430319
 * 2274823218 2529502358 3284895257 3539574397 (s2 < 8).
 *
 */
class CSPLIB_EXPORT Taus2 {
	// state
	unsigned long _s1, _s2, _s3;

	static const unsigned long MASK = 0xffffffffUL;

	inline static unsigned long LCG(unsigned long n) {
		return ((69069 * n) & 0xffffffffUL);
	}

	inline static unsigned long TAUSWORTHE(unsigned long s, unsigned long a, unsigned long b, unsigned long c, unsigned long d) {
		return (((s &c) <<d) &MASK) ^ ((((s <<a) &MASK)^s) >>b);
	}

	/** Internal generator, returning a random integer in the
	 *  range [0,2**32).
	 */
	inline unsigned long generate() {
		_s1 = TAUSWORTHE(_s1, 13, 19, 4294967294UL, 12);
		_s2 = TAUSWORTHE(_s2, 2, 25, 4294967288UL, 4);
		_s3 = TAUSWORTHE(_s3, 3, 11, 4294967280UL, 17);
		return (_s1 ^ _s2 ^ _s3);
	}
public:
	/** Structure for saving and restoring the internal state of the generator.
	 */
	struct State {
		unsigned long _s1, _s2, _s3;
	};

	/** Get the name of this generator.
	 */
	inline std::string getName() const { return "Tausworthe (Taus2)"; }

	/** Construct a new generator with seed 0.
	 */
	Taus2() { setSeed(0); }

	/** Return a random integer in the range lower to upper-1 (inclusive).
	 *
	 *  @param lower lower bound (inclusive)
	 *  @param upper upper bound (exclusive)
	 *  @returns a random integer int the range lower to upper-1 (inclusive)
	 */
	template <typename T>
	inline T uniformInt(T lower, T upper) {
		return lower + static_cast<T>(unit() * (upper-lower));
	}

	/** Equivalent to uniformInt(0, upper).
	 */
	template <typename T>
	inline T uniformInt(T upper) {
		return uniformInt(static_cast<T>(0), upper);
	}

	/** Return a random floating point value in the range [0, 1).
	 */
	inline double unit() {
  		return generate() / 4294967296.0 ;
	};

	/** Return a random floating point value in the range [lower, upper).
	 */
	inline double uniform(double lower, double upper) {
		return lower + unit() * (upper-lower);
	}

	/** Reseed the random number generator.
	 *
	 *  @param s the new seed.
	 */
	void setSeed(unsigned long s);

	/** Save the current state of the generator.
	 *
	 *  The state can be restored at any time with setState().
	 */
	void getState(State &state) const;

	/** Restore the generator to a specific state saved with getState().
	 *
	 *  The subsequent random numbers will be identical to the sequence
	 *  following the corresponding getState() call.
	 */
	void setState(State const &state);
};


} // namespace rng



///////////////////////////////////////////////////////////////////////////////////
// Random Number Generator Wrappers


/** Base class for storing the state of a random number generator.
 */
class RandomStateWrapper: public Referenced {
protected:
	RandomStateWrapper() {}
	virtual ~RandomStateWrapper() {}
};


/** Abstract interface for random number generators and random distributions.
 */
class CSPLIB_EXPORT RandomInterface {
public:
	typedef Ref<RandomStateWrapper> State;
	typedef unsigned long SeedType;

	virtual ~RandomInterface();

	/** Set the generator seed.
	 */
	virtual void setSeed(SeedType seed)=0;
	
	/** Get the generator state.
	 */
	virtual State getState() const=0;

	/** Set the generator state.
	 */
	virtual void setState(State const &state)=0;

	/** Get the name of the generator.
	 */
	virtual std::string getName() const=0;
};


/** Abstract interface for random number generators.
 */
class CSPLIB_EXPORT RandomNumberGeneratorInterface: public RandomInterface {
public:
	/** Get a random number in the range [0,1).
	 */
	virtual double unit()=0;

	/** Get a random number in the range [lower,upper).
	 */
	virtual double uniform(double lower, double upper)=0;
	
	/** Get a random integer in the range [lower,upper).
	 */
	virtual long uniformInt(long lower, long upper)=0;

	/** Equivalent to uniformInt(0, upper)
	 */
	virtual long uniformInt(long upper)=0;

	/** Get a random (unsigned) integer in the range [lower,upper).
	 */
	virtual unsigned long uniformUInt(unsigned long lower, unsigned long upper)=0;

	/** Equivalent to uniformUInt(0, upper)
	 */
	virtual unsigned long uniformUInt(unsigned long upper)=0;
};

/** Random number generator wrapper.
 *
 *  Random number generator wrapper, implementing the random number
 *  generator interface.  These generics provide a uniform interface
 *  to the underlying generators and generator state data.  The
 *  performance penalty relative to using the raw generators is
 *  typically about 20%.
 */
template <class RNG>
class CSPLIB_EXPORT RandomNumberGenerator: public RandomNumberGeneratorInterface {

	/** Random number generator state.
	 */
	class RNGState: public RandomStateWrapper {
	public:
		typename RNG::State _state;
	};

	RNG _gen;

public:
	/** Reseed the random number generator.
	 *
	 *  @param seed the new seed.
	 */
	virtual void setSeed(SeedType seed) {
		_gen.setSeed(seed);
	}

	/** Save the current state of the generator.
	 *
	 *  The state can be restored at any time with setState().
	 */
	virtual State getState() const {
		RNGState *rng_state = new RNGState;
		_gen.getState(rng_state->_state);
		return rng_state;
	}

	/** Restore the generator to a specific state saved with getState().
	 *
	 *  The subsequent random numbers will be identical to the sequence
	 *  following the corresponding getState() call.
	 */
	virtual void setState(State const &state) {
		Ref<RNGState> rng_state = state;
		_gen.setState(rng_state->_state);
	}

	/** Return a random floating point value in the range [0,1).
	 */
	virtual double unit() {
		return _gen.unit();
	}

	/** Return a random floating point value in the range [lower,upper).
	 */
	virtual double uniform(double lower, double upper) {
		return _gen.uniform(lower, upper);
	}

	/** Return a random integer in the range lower to upper-1 (inclusive)
	 *
	 *  @param lower lower bound (inclusive)
	 *  @param upper upper bound (exclusive)
	 *  @returns a random integer int the range lower to upper-1 (inclusive)
	 */
	virtual long uniformInt(long lower, long upper) {
		return _gen.uniformInt(lower, upper);
	}

	/** Equivalent to uniformInt(0, upper)
	 */
	virtual long uniformInt(long upper) {
		return _gen.uniformInt(upper);
	}

	/** Return a random integer in the range lower to upper-1 (inclusive)
	 *
	 *  @param lower lower bound (inclusive)
	 *  @param upper upper bound (exclusive)
	 *  @returns a random integer int the range lower to upper-1 (inclusive)
	 */
	virtual unsigned long uniformUInt(unsigned long lower, unsigned long upper) {
		return _gen.uniformInt(lower, upper);
	}

	/** Equivalent to uniformUInt(0, upper)
	 */
	virtual unsigned long uniformUInt(unsigned long upper) {
		return _gen.uniformInt(upper);
	}

	/** Get the name of this generator
	 */
	virtual std::string getName() const {
		return _gen.getName();
	}

	/** Direct access to the underlying generator.
	 *
	 *  Using the generator directly provides slightly faster access and
	 *  generator-specific methods.
	 */
	inline RNG *operator->() { return &_gen; }
};

///////////////////////////////////////////////////////////////////////////////////
// Random Distributions

/** Low-level random number distribution interfaces. */
namespace rd { 

/** Gaussian random number distribution.
 */
class CSPLIB_EXPORT Gauss {
	rng::Taus2 _gen;
	double _mean, _sigma;
	double _x;
	bool _odd;
public:

	/** Random number generator state.
	 */
	struct State {
		rng::Taus2::State _state;
		double _mean, _sigma, _x;
		bool _odd;
	};

	/** Get the name of this distribution.
	 */
	std::string getName() const { return "Gaussian (Gauss)"; }

	/** Construct a new distribution.
	 *
	 *  @param mean the mean value of the distribution (default=0)
	 *  @param sigma the standard deviation of the distribution (default=0)
	 */
	Gauss(double mean=0.0, double sigma=0.0): _mean(mean), _sigma(sigma), _odd(true) {}

	/** Set the mean value and standard deviation of the distribution.
	 *
	 *  @param mean the mean value of the distribution
	 *  @param sigma the standard deviation of the distribution
	 */
	inline void setDistribution(double mean, double sigma) {
		_mean = mean;
		_sigma = sigma;
	}

	/** Get the mean value of the distribution.
	 */
	inline double getMean() const { return _mean; }

	/** Get the standard deviation of the distribution.
	 */
	inline double getSigma() const { return _sigma; }

	/** Sample the distribution.
	 */
	double sample();

	/** Reseed the underlying random number generator.
	 *
	 *  @param seed the new seed.
	 */
	inline void setSeed(unsigned long seed) { _gen.setSeed(seed); _odd = true; }

	/** Save the current state of the distribution.
	 *
	 *  The state can be restored at any time with setState().  Both the
	 *  state of the underlying random number generator and the
	 *  distribution parameters (mean, sigma) are saved.
	 */
	void getState(State &) const;

	/** Restore the underlying generator to a specific state saved with
	 *  getState().
	 *
	 *  The subsequent samples of the distribution will be identical to
	 *  the sequence following the corresponding getState() call.
	 */
	void setState(State const &);
};


extern CSPLIB_EXPORT double BoxMueller(RandomNumberGeneratorInterface &_gen, double _mean, double _sigma);

} // namespace rd



///////////////////////////////////////////////////////////////////////////////////
// Random Distribution Wrappers


/** Abstract interface for random distributions.
 */
class CSPLIB_EXPORT RandomDistributionInterface: public RandomInterface {
public:
	/** Sample the distribution.
	 */
	virtual double sample()=0;
};



/** Random distribution wrapper.
 *
 *  Random distribution wrapper, implementing the random distribution
 *  interface.  These generics provide a uniform interface to the
 *  underlying distributions and distribution (generator) state data.
 */
template <class RD>
class RandomDistribution: public RandomDistributionInterface {
	/** Random number generator state.
	 */
	class RDState: public RandomStateWrapper {
	public:
		typename RD::State _state;
	};
	RD _dist;
public:
	/** Sample the distribution
	 */
	double sample();

	/** Reseed the random number generator.
	 *
	 *  @param seed the new seed.
	 */
	virtual void setSeed(SeedType seed);

	/** Save the current state of the generator.
	 *
	 *  The state can be restored at any time with setState().
	 */
	virtual State getState() const;

	/** Restore the generator to a specific state saved with getState().
	 *
	 *  The subsequent random numbers will be identical to the sequence
	 *  following the corresponding getState() call.
	 */
	virtual void setState(State const &);

	/** Get the name of this generator.
	 */
	virtual std::string getName() const;

	/** Direct access to the underlying distribution.
	 *
	 *  Using the distribution directly provides slightly faster access
	 *  and distribution-specific methods.
	 */
	inline RD *operator->() { return &_dist; }
};


template <class RD>
double RandomDistribution<RD>::sample() {
	return _dist.sample();
}

template <class RD>
RandomInterface::State RandomDistribution<RD>::getState() const {
	RDState *rd_state = new RDState;
	_dist.getState(rd_state->_state);
	return rd_state;
}

template <class RD>
void RandomDistribution<RD>::setState(RandomInterface::State const &state) {
	Ref<RDState> rd_state = state;
	_dist.setState(rd_state->_state);
}

template <class RD>
void RandomDistribution<RD>::setSeed(RandomInterface::SeedType s) {
	_dist.setSeed(s);
}

template <class RD>
std::string RandomDistribution<RD>::getName() const {
	return _dist.getName();
}

/** Random number generators and distributions. */
namespace random { // generators and distributions
	typedef RandomDistribution<rd::Gauss> Gauss;
	typedef RandomNumberGenerator<rng::MT19937> MersenneTwister;
	typedef RandomNumberGenerator<rng::Taus2> Taus2;
	typedef Taus2 Standard;
}


/** Global random number generator.
 *
 *  WARNING: it is obviously not safe to use this generator from more than one
 *  thread.  In multithreaded environments, construct local generators, use
 *  a thread-specific generator, or explicitly lock a shared generator.
 */
extern CSPLIB_EXPORT random::Taus2 g_Random;


} // namespace csp

#endif // __CSPLIB_UTIL_RANDOM_H__

