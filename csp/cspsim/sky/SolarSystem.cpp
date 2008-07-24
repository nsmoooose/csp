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

#include <csp/cspsim/sky/SolarSystem.h>
#include <csp/cspsim/sky/Magnitude.h>

namespace csp {

// for typical fields of view (e.g. 60 degrees) on a standard monitor
// the moon looks unrealistically (albeit accurately) small.  this
// factor can be used to oversize the moon billboard when aesthetics
// are more important that realism.  set to 1.0 for accurate sizing.
const double cOversize = 5.0;

const double Moon::cEccentricity = 0.054900;
const double Moon::cInclination = 0.0898042;  // radians
const double Moon::cMass = 7.36e+22;  // kg
const double Moon::cRadius = cOversize * 1.7374e+6;  // m
const double Moon::cSemimajorAxis = 3.83898e+8;  // m

Moon::Moon(OrbitalBody *earth)
	: OrbitalBody(
			"Moon",
			cSemimajorAxis,
			cEccentricity,
			true,  // perturbations
			false,  // heliocentric
			cMass,
			cRadius,
			earth,
			earth,
			new PolynomialReflector(0.23, 1.49, 0.0, 0.0, 0.043)) { }


void Moon::updateOrbitalState(OrbitalState &state, double time) {
	state.time = time;
	state.longitude_of_ascending_node = toRadians(125.1228) - toRadians(0.0529538083) * time;
	state.argument_of_perihelion = toRadians(318.0634) + toRadians(0.1643573223) * time;
	state.mean_anomaly = toRadians(115.3654) + toRadians(13.0649929509) * time;
	state.inclination = cInclination;
	computePosition(state);
}

void Moon::addPerturbations(OrbitalState const &state, double &lng, double &lat) const {
	if (!getCenter()) return;
	double Mm = state.mean_anomaly;
	double Ms = getCenter()->getMeanAnomaly(state.time);
	double Ls = getCenter()->getMeanLongitude(state.time);
	double Lm = state.mean_longitude;
	double D = Lm - Ls;
	double F = Lm - state.longitude_of_ascending_node;
	double lng_correction = -1.274 * sin(Mm - 2*D) + 0.658 * sin(2*D) - 0.186 * sin(Ms) - 0.059 * sin(2*Mm - 2*D) - 0.057 * sin(Mm - 2*D + Ms) + 0.053 * sin(Mm + 2*D) + 0.046 * sin(2*D - Ms) + 0.041 * sin(Mm - Ms) - 0.035 * sin(D) - 0.031 * sin(Mm + Ms) - 0.015 * sin(2*F - 2*D) + 0.011 * sin(Mm - 4*D);
	double lat_correction = -0.173 * sin(F - 2*D) - 0.055 * sin(Mm - F - 2*D) - 0.046 * sin(Mm + F - 2*D) + 0.033 * sin(F + 2*D) - 0.017 * sin(2*Mm + F);
	lng += toRadians(lng_correction);
	lat += toRadians(lat_correction);
}

// 1.496e+11 /*AU*/

Planet::Planet(
			std::string const &label,
			double semimajor_axis,
			double eccentricity,
			double mass,
			double radius,
			OrbitalBody const *observer,
			MagnitudeFunction *magnitude_function,
			double N0, double dN,
			double i0, double di,
			double w0, double dw,
			double M0, double dM)
		: OrbitalBody(label, semimajor_axis, eccentricity, false /*perturbations*/, true, mass, radius, 0, observer, magnitude_function) {
	m_N0 = toRadians(N0); m_dN = toRadians(dN);
	m_i0 = toRadians(i0); m_di = toRadians(di);
	m_w0 = toRadians(w0); m_dw = toRadians(dw);
	m_M0 = toRadians(M0); m_dM = toRadians(dM);
}

void Planet::updateOrbitalState(OrbitalState &state, double time) {
	state.time = time;
	state.longitude_of_ascending_node = m_N0 + m_dN * time;
	state.inclination = m_i0 + m_di * time;
	state.argument_of_perihelion = m_w0 + m_dw * time;
	state.mean_anomaly = m_M0 + m_dM * time;
	computePosition(state);
}


const double SolarSystem::cEarthEccentricity = 0.016709;
const double SolarSystem::cEarthMass = 5.9742e+24; // kg
const double SolarSystem::cEarthRadius = 6.3781e+6; // m
const double SolarSystem::cEarthSemimajorAxis = 1.49598e+11; // m

const double SolarSystem::cMarsEccentricity = 0.09339;
const double SolarSystem::cMarsMass = 6.4191e+23; // kg
const double SolarSystem::cMarsRadius = 3.3970e+6; // m
const double SolarSystem::cMarsSemimajorAxis = 2.27943e+11;  // m

const double SolarSystem::cVenusEccentricity = 0.006773;
const double SolarSystem::cVenusMass = 4.86900e+24; // kg
const double SolarSystem::cVenusRadius = 6.0520e+6; // m
const double SolarSystem::cVenusSemimajorAxis = 1.082096e+11;  // m

SolarSystem::SolarSystem() {
	// for the earth relative to the sun, subtract 180 from the value of w given by Schlyter for the sun relative to the earth.
	m_Earth = new Planet(
		"Earth",
		cEarthSemimajorAxis,
		cEarthEccentricity,
		cEarthMass,
		cEarthRadius,
		NULL,  // observer
		NULL,  // magnitude_function
		0.0, 0.0,                // longitude_of_ascending_node
		0.0, 0.0,                // inclination
		102.9404, 4.70935E-5,    // argument_of_perihelion
		356.0470, 0.9856002585); // mean_anomaly

	m_Venus = new Planet(
		"Venus",
		cVenusSemimajorAxis,
		cVenusEccentricity,
		cVenusMass,
		cVenusRadius,
		m_Earth.get(),
		new PolynomialReflector(-4.34, 0.775, 0.0, 0.0790),
		76.6799, 2.46590E-5,     // longitude_of_ascending_node
		3.3946, 2.75E-8,         // inclination
		54.8910, 1.38374E-5,     // argument_of_perihelion
		48.0052, 1.6021302244);  // mean_anomaly

	m_Mars = new Planet(
		"Mars",
		cMarsSemimajorAxis,
		cMarsEccentricity,
		cMarsMass,
		cMarsRadius,
		m_Earth.get(),
		new PolynomialReflector(-1.51, 0.92),
		49.5574, 2.11081E-5,     // longitude_of_ascending_node
		1.8497, -1.78E-8,        // inclination
		286.5016, 2.92961E-5,    // argument_of_perihelion
		18.6021, 0.5240207766);  // mean_anomaly

	m_Moon = new Moon(m_Earth.get());
}

void SolarSystem::update(double time) {
	m_Earth->update(time);
	m_Moon->update(time);
	m_Venus->update(time);
	m_Mars->update(time);
}

OrbitalBody *SolarSystem::body(unsigned i) {
	switch (i) {
		case 0: return m_Earth.get();
		case 1: return m_Moon.get();
		case 2: return m_Venus.get();
		case 3: return m_Mars.get();
		default: break;
	}
	return 0;
}


} // namespace csp

