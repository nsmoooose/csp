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

#ifndef __CSP_CSPSIM_SKY_SOLARSYSTEM_H__
#define __CSP_CSPSIM_SKY_SOLARSYSTEM_H__

#include <csp/cspsim/sky/OrbitalBody.h>

namespace csp {


class Moon: public OrbitalBody {
public:
	Moon(OrbitalBody *earth);

private:
	static const double cEccentricity;
	static const double cInclination;
	static const double cMass;
	static const double cRadius;
	static const double cSemimajorAxis;

	virtual void updateOrbitalState(OrbitalState &state, double time);
	virtual void addPerturbations(OrbitalState const &state, double &lng, double &lat) const;
};


class Planet: public OrbitalBody {
public:
	Planet(
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
			double M0, double dM);

private:
	virtual void updateOrbitalState(OrbitalState &state, double time);

	double m_N0, m_dN;
	double m_i0, m_di;
	double m_w0, m_dw;
	double m_M0, m_dM;
};


class SolarSystem: public Referenced {
public:
	SolarSystem();
	void update(double time);

	OrbitalBody *earth() { return m_Earth.get(); }
	OrbitalBody *moon() { return m_Moon.get(); }
	OrbitalBody *venus() { return m_Venus.get(); }
	OrbitalBody *mars() { return m_Mars.get(); }
	OrbitalBody *body(unsigned i);

private:
	static const double cEarthEccentricity;
	static const double cEarthMass;
	static const double cEarthRadius;
	static const double cEarthSemimajorAxis;

	static const double cMarsEccentricity;
	static const double cMarsMass;
	static const double cMarsRadius;
	static const double cMarsSemimajorAxis;

	static const double cVenusEccentricity;
	static const double cVenusMass;
	static const double cVenusRadius;
	static const double cVenusSemimajorAxis;

	Ref<OrbitalBody> m_Earth;
	Ref<OrbitalBody> m_Moon;
	Ref<OrbitalBody> m_Mars;
	Ref<OrbitalBody> m_Venus;
};


} // namespace csp

#endif // __CSP_CSPSIM_SKY_SOLARSYSTEM_H__

