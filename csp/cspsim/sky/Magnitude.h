// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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

#ifndef __CSP_CSPSIM_SKY_MAGNITUDE_H__
#define __CSP_CSPSIM_SKY_MAGNITUDE_H__

#include <csp/csplib/util/Referenced.h>
#include <csp/csplib/data/Vector3.h>

CSP_NAMESPACE


/** Interface for functions that compute the magnitude of a celestial
 *  body as a function of its heliocentric and geocentric distances and
 *  phase angle.  See "How to compute planetary positions" by Paul
 *  Schlyter (http://www.stjarnhimlen.se/comp/ppcomp.html) for details
 *  on computing the phase angle.
 */
class MagnitudeFunction: public Referenced {
public:
	virtual ~MagnitudeFunction();

	/** Distances in meters, angle in radians.
	 */
	virtual double compute(double sun_body_distance, double body_observer_distance, double phase_angle) const = 0;
};


/** Compute the apparent magnitude of a celestial body using a polynomial
 *  approximation of the phase integral.  Terms up to the fourth power of
 *  the phase angle are included.
 */
class PolynomialReflector: public MagnitudeFunction {
public:
	PolynomialReflector(double absolute_magnitude, double phase1, double phase2=0.0, double phase3=0.0, double phase4=0.0):
		m_AbsoluteMagnitude(absolute_magnitude),
		m_Phase1(phase1),
		m_Phase2(phase2),
		m_Phase3(phase3),
		m_Phase4(phase4) {
	}

	virtual double compute(double sun_body_distance, double body_observer_distance, double phase_angle) const;

private:
	double m_AbsoluteMagnitude;
	double m_Phase1;
	double m_Phase2;
	double m_Phase3;
	double m_Phase4;
};


/** Function for the apparent magnitude of a diffuse reflecting sphere.
 *  See http://en.wikipedia.org/wiki/Absolute_magnitude.
 */
class DiffuseReflector: public MagnitudeFunction {
public:
	/** Initialize the function with the absolute magnitude of the body.
	 *  This is the apparent magnitude the body would have if it were
	 *  located one au from both the Sun and the observer at a phase angle
	 *  of zero degrees.  For example, the absolute magnitude of the
	 *  Earth's moon is +0.25.
	 */
	DiffuseReflector(double absolute_magnitude):
		m_AbsoluteMagnitude(absolute_magnitude) {
	}

	/** See http://en.wikipedia.org/wiki/Absolute_magnitude for details.
	 */
	virtual double compute(double sun_body_distance, double body_observer_distance, double phase_angle) const;

private:
	double m_AbsoluteMagnitude;
};


CSP_NAMESPACE_END

#endif // __CSP_CSPSIM_SKY_MAGNITUDE_H__
