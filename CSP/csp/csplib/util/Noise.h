/* Combat Simulator Project
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Noise.h
 * @brief Noise distribution classes.
 */


#ifndef __CSPLIB_UTIL_NOISE_H__
#define __CSPLIB_UTIL_NOISE_H__

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>

#include <cmath>
#include <vector>


CSP_NAMESPACE


/**
 * @brief Encapsulates a perlin noise generator in one-dimension.
 *
 * Many introductions to perlin noise and its uses are available online,
 * via Google.  Here's one such site:
 *
 *   http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
 *
 * @author Mark Rose <mkrose@users.sf.net>
 */
class CSP_EXPORT Perlin1D {
	
public:

	typedef enum { LINEAR, COSINE, CUBIC } Interpolation;

	/**
	 * Construct a new perline noise generator.
	 *
	 * @param persistence scale factor between succesive octaves
	 * @param octaves number of octaves to generate
	 * @param interpolation the method of interpolation between noise values
	 */
	Perlin1D(double persistence=0.5, int octaves=1, Interpolation interpolation=LINEAR);

	/**
	 * Set the noise parameters.
	 *
	 * @param persistence scale factor between succesive octaves
	 * @param octaves number of octaves to generate
	 */
	void setParameters(double persistence, int octaves);

	/**
	 * Set the interpolation method.
	 *
	 * @param interpolation the method of interpolation between noise values
	 */
	void setInterpolation(Interpolation interpolation);

	/**
	 * Set the offset into the noise functions.
	 *
	 * The offset can be used to reproducibly generate a given noise
	 * distribution, or to create independent finite noise distributions
	 * from the same generator.
	 */
	void setOffset(int idx);
	
	/**
	 * Randomize the offset.
	 *
	 * Subsequent noise will generally be uncorrelated to previously
	 * generated noise.  Be aware that the random offset is in the
	 * range 10^9, so for large noise sets this property will break down.
	 */
	void randomize();

	/**
	 * A simple pseudo-random number regenator.
	 *
	 * @param x the random seed.
	 * @returns a pseudo-random number in the range 0-1.
	 */
	double _simpleRandom(int x) const {
		x = (x << 13) ^ x;
		return (1.0 -
		        ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) /
		        1073741824.0);
	}

	/**
	 * Get a weighted sum of adjacent random numbers to smooth the distribution.
	 *
	 * @param x the central point for the weighted sum
	 * @returns the weighted sum
	 */
	double _getSmoothedNoise(int x) {
		return 0.5 * (_simpleRandom(x) + 0.5 * (_simpleRandom(x - 1) + _simpleRandom(x + 1)));
	}

	/**
	 * Linear interpolation.
	 *
	 * @param a the first value
	 * @param b the second value
	 * @param x the fraction from a to b (0-1)
	 * @returns the interpolated value
	 */
	double _linearInterpolate(double a, double b, double x) {
		return  a*(1.0-x) + b*x;
	}
	
	/**
	 * Cosine interpolation.
	 *
	 * @param a the first value
	 * @param b the second value
	 * @param x the fraction from a to b (0-1)
	 * @returns the interpolated value
	 */
	double _cosineInterpolate(double a, double b, double x) {
		x = (1.0 - cos(x * 3.14159265)) * 0.5;
		return  a*(1.0-x) + b*x;
	}
	
	/**
	 * Cubic interpolation.
	 *
	 * @param v0 value(i-1)
	 * @param v1 value(i)
	 * @param v2 value(i+1)
	 * @param v3 value(i+2)
	 * @param x the fraction from v1 to v2 (0-1)
	 * @returns the interpolated value
	 */
	double _cubicInterpolate(double v0, double v1, double v2, double v3, double x) {
		double P = (v3 - v2) - (v0 - v1);
		double Q = (v0 - v1) - P;
		double R = v2 - v0;
		return  (((P*x + Q) * x) + R) * x + v1;
	}

	/**
	 * Returns the noise value interpolated between the smoothed noise at
	 * successive points in the distribution.
	 *
	 * @param x the point in the distribution
	 * @returns the smoothed, interpolated noise value
	 */
	double _getInterpolatedNoise(double x);

	/**
	 * Get the noise vaule at a particular position.
	 *
	 * @param x the point in the distribution
	 * @returns the noise value
	 */
	double getValue(double x);

	/**
	 * Generate a list of noise values.
	 *
	 * This is a convenience method to pregenerate a discrete perlin noise field.
	 * Both perioidic and aperiodic fields can be created (the former have the same
	 * value at the start and at the end).  The noise field can be scaled in X and
	 * Y (e.g. time and amplitude), as well as offset so that the mean value is
	 * non-zero.
	 *
	 * @param n the number of noise values
	 * @param periodic if true, the lists ends are blended to make them continuous
	 * @param scale the scaling factor from the list index to the noise function
	 * @param amplitude the noise amplitude
	 * @param offset the noise offset
	 */
	std::vector<float> generate(int n, bool periodic, double scale=1.0, double amplitude=1.0, double offset=0.0);

private:
	double m_persistence;
	int m_octaves;
	int m_offset;
	int m_interpolation;
};


CSP_NAMESPACE_END


#endif // __CSPLIB_UTIL_NOISE_H__

