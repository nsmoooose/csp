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
 * @file Random.cpp
 *
 * Random number generators and distributions.
 *
 */

#include <csp/csplib/util/Random.h>


CSP_NAMESPACE


namespace rng { // random number generators


void MT19937::update() {
	int kk;

	for (kk = 0; kk < N - M; kk++) {
		unsigned long y = (_mt[kk] & UPPER_MASK) | (_mt[kk + 1] & LOWER_MASK);
		_mt[kk] = _mt[kk + M] ^ (y >> 1) ^ MAGIC(y);
	}
	for (; kk < N - 1; kk++) {
		unsigned long y = (_mt[kk] & UPPER_MASK) | (_mt[kk + 1] & LOWER_MASK);
		_mt[kk] = _mt[kk + (M - N)] ^ (y >> 1) ^ MAGIC(y);
	}

	unsigned long y = (_mt[N - 1] & UPPER_MASK) | (_mt[0] & LOWER_MASK);
	_mt[N - 1] = _mt[M - 1] ^ (y >> 1) ^ MAGIC(y);

	_mti = 0;
}

void MT19937::setSeed(unsigned long int s) {
	if (s == 0) {
		s = 4357;	// the default seed is 4357
	}
	_mt[0]= s & 0xffffffffUL;
	for (int i = 1; i < N; i++) {
		// See Knuth's "Art of Computer Programming" Vol. 2, 3rd
		// Ed. p.106 for multiplier.
		_mt[i] = (1812433253UL * (_mt[i-1] ^ (_mt[i-1] >> 30)) + i);
		_mt[i] &= 0xffffffffUL;
	}
	_mti = N;
}

void MT19937::getState(State &state) const {
	for (int i = 0; i < N; i++) {
		state._mt[i] = _mt[i];
	}
	state._mti = _mti;
}

void MT19937::setState(State const &state) {
	for (int i = 0; i < N; i++) {
		_mt[i] = state._mt[i];
	}
	_mti = state._mti;
}



void Taus2::setSeed(unsigned long int s) {
	if (s == 0) {
		s = 1;	// default seed is 1
	}

	_s1 = LCG(s);
	if (_s1 < 2) _s1 += 2UL;
	_s2 = LCG(_s1);
	if (_s2 < 8) _s2 += 8UL;
	_s3 = LCG(_s2);
	if (_s3 < 16) _s3 += 16UL;

	// "warm it up"
	for (int i=0; i < 6; i++) {
		generate();
	}
}


void Taus2::getState(State &state) const {
	state._s1 = _s1;
	state._s2 = _s2;
	state._s3 = _s3;
}


void Taus2::setState(State const &state) {
	_s1 = state._s1;
	_s2 = state._s2;
	_s3 = state._s3;
}


} // namespace rng


RandomInterface::~RandomInterface() {
}



namespace rd { // random distributions


double Gauss::sample() {
	_odd = !_odd;
	// each pass generates 2 values; return 2nd
	if (_odd) {
		return _x;
	}
	// Polar (Box-Mueller) method; See Knuth v2, 3rd ed, p122
	double x, y, r2;
	do {
		x = -1.0 + 2.0 * _gen.unit();
		y = -1.0 + 2.0 * _gen.unit();
		r2 = x*x + y*y;
	} while (r2 > 1.0 || r2 == 0.0);
	double f = _sigma * ::sqrt(-2.0 * ::log(r2) / r2);
	// save one value for the next call and return the other
	_x = _mean + x * f;
	return _mean + y * f;
}

void Gauss::getState(State &state) const {
	_gen.getState(state._state);
	state._mean = _mean;
	state._sigma = _sigma;
	state._x = _x;
	state._odd = _odd;
}

void Gauss::setState(State const &state) {
	_gen.setState(state._state);
	_mean = state._mean;
	_sigma = state._sigma;
	_x = state._x;
	_odd = state._odd;
}

double BoxMueller(RandomNumberGeneratorInterface &_gen, double _mean, double _sigma) {
	// Polar (Box-Mueller) method; See Knuth v2, 3rd ed, p122
	double x, y, r2;
	do {
		x = -1.0 + 2.0 * _gen.unit();
		y = -1.0 + 2.0 * _gen.unit();
		r2 = x*x + y*y;
	} while (r2 > 1.0 || r2 == 0.0);
	double f = _sigma * ::sqrt(-2.0 * ::log(r2) / r2);
	// discard one value in the name of thread safety
	return _mean + y * f;
}

} // namespace rd


random::Taus2 g_Random;

// Force template instantiation.
template class CSPLIB_EXPORT RandomDistribution<rd::Gauss>;
template class CSPLIB_EXPORT RandomNumberGenerator<rng::MT19937>;
template class CSPLIB_EXPORT RandomNumberGenerator<rng::Taus2>;

CSP_NAMESPACE_END

