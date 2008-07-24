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

#include <csp/cspsim/sky/OrbitalBody.h>
#include <csp/cspsim/sky/Magnitude.h>
#include <csp/csplib/util/Log.h>

#include <cmath>

namespace csp {

const double OrbitalBody::cG = 6.67e-11; // Nm^2/kg^2
const double OrbitalBody::cMs = 1.99e+30;  // kg

OrbitalBody::OrbitalBody(
		std::string const &label,
		double semimajor_axis,
		double eccentricity,
		bool perturbations,
		bool heliocentric,
		double mass,
		double radius,
		OrbitalBody const *center,
		OrbitalBody const *observer,
		MagnitudeFunction const *magnitude_function) {
	assert(center == 0 || !heliocentric);
	m_Label = label;
	m_SemiMajorAxis = semimajor_axis;
	m_Eccentricity = eccentricity;
	m_PerihelionDistance = m_SemiMajorAxis * (1.0 - m_Eccentricity);
	m_AphelionDistance = m_SemiMajorAxis * (1.0 + m_Eccentricity);
	m_Mass = mass;
	double center_mass = (center ? center->getMass() : cMs);
	m_OrbitalPeriod = sqrt((4.0 * PI * PI) / (cG * (mass + center_mass))) * pow(m_SemiMajorAxis, 1.5) / 86400.0;  // in days
	m_RootOneMinusE2 = sqrt(1.0 - eccentricity * eccentricity);
	m_Perturbations = perturbations;
	m_Heliocentric = heliocentric;
	m_Radius = radius;
	m_Center = center;
	m_Observer = observer;
	m_MagnitudeFunction = magnitude_function;
	m_InverseInterpolationInterval = 1.0;
	m_UpdateInProgress = true;

	// 1 degree interpolation by default (~2 hours for Earth's moon)
	setInterpolationAngle(1.0);
}

OrbitalBody::~OrbitalBody() {
	CSPLOG(DEBUG, PHYSICS) << "~OrbitalBody " << label();
}

void OrbitalBody::computePosition(OrbitalBody::OrbitalState &state) {
	const double sinM = sin(state.mean_anomaly);
	const double cosM = cos(state.mean_anomaly);

	double E = state.mean_anomaly + m_Eccentricity * sinM * (1.0 + m_Eccentricity * cosM);
	if (m_Eccentricity > 0.05) {
		if (m_Eccentricity < 0.6) {
			for (double error = 1.0; error > 1e-4; ) {
				double En = E - (E - m_Eccentricity * sin(E) - state.mean_anomaly) / (1.0 - m_Eccentricity * cos(E));
				error = En - E;
				E = En;
			}
		} else {
			// TODO log
		}
	}

	const double xv = m_SemiMajorAxis * (cos(E) - m_Eccentricity);
	const double yv = m_SemiMajorAxis * m_RootOneMinusE2 * sin(E);
	const double cosN = cos(state.longitude_of_ascending_node);
	const double sinN = sin(state.longitude_of_ascending_node);
	const double cosi = cos(state.inclination);
	const double sini = sin(state.inclination);

	state.eccentric_anomaly = E;
	state.true_anomaly = atan2(yv, xv);
	state.distance = sqrt(xv*xv + yv*yv);
	state.axis.set(sinN * sini, -cosN * sini, cosi);
	state.longitude_of_perihelion = state.longitude_of_ascending_node + state.argument_of_perihelion;
	state.mean_longitude = state.mean_anomaly + state.longitude_of_perihelion;

	const double wv = state.true_anomaly + state.argument_of_perihelion;
	const double coswv = cos(wv);
	const double sinwv = sin(wv);

	double zh = sinwv * sini;
	double yh = sinN * coswv + cosN * sinwv * cosi;
	double xh = cosN * coswv - sinN * sinwv * cosi;

	state.ecliptic_latitude = atan2(zh, sqrt(xh*xh + yh*yh));
	state.ecliptic_longitude = atan2(yh, xh);

	if (m_Perturbations) {
		addPerturbations(state, state.ecliptic_longitude, state.ecliptic_latitude);
		const double cosLat = cos(state.ecliptic_latitude);
		zh = sin(state.ecliptic_latitude);
		yh = sin(state.ecliptic_longitude) * cosLat;
		xh = cos(state.ecliptic_longitude) * cosLat;
	}

	state.position = state.distance * Vector3(xh, yh, zh);
	state.magnitude = 0.0;

	if (m_Observer.valid() && m_MagnitudeFunction.valid()) {
		Vector3 position = state.position;
		Vector3 relative = m_Observer->getHeliocentricPosition(state.time) - position;
		double distance_to_sun = position.normalize();
		double distance_to_observer = relative.normalize();
		double phase_angle = acos(relative * position);
		state.magnitude = m_MagnitudeFunction->compute(distance_to_sun, distance_to_observer, phase_angle);
	}
}

void OrbitalBody::addPerturbations(OrbitalState const&, double&, double&) const {
}

void OrbitalBody::update(double time) {
	m_UpdateInProgress = true;
	if (m_DirtyInterval) {
		m_InverseInterpolationInterval = 1.0 / m_InterpolationInterval;
		updateOrbitalState(m_OrbitalState[0], time - 0.5 * m_InterpolationInterval);
		updateOrbitalState(m_OrbitalState[1], time + 0.5 * m_InterpolationInterval);
		updateOrbitalState(m_OrbitalState[2], time + 1.5 * m_InterpolationInterval);
		m_DirtyInterval = false;
	} else if (time > m_OrbitalState[1].time + 0.5 * m_InterpolationInterval) {
		if (time - m_OrbitalState[2].time <= 0.5 * m_InterpolationInterval) {
			m_OrbitalState[0] = m_OrbitalState[1];  // fastpath
			m_OrbitalState[1] = m_OrbitalState[2];
		} else {
			updateOrbitalState(m_OrbitalState[0], time - 0.5 * m_InterpolationInterval);
			updateOrbitalState(m_OrbitalState[1], time + 0.5 * m_InterpolationInterval);
		}
		updateOrbitalState(m_OrbitalState[2], m_OrbitalState[1].time + m_InterpolationInterval);
	} else if (time < m_OrbitalState[0].time + 0.1 * m_InterpolationInterval) {
		updateOrbitalState(m_OrbitalState[0], time - m_InterpolationInterval);
		updateOrbitalState(m_OrbitalState[1], time);
		updateOrbitalState(m_OrbitalState[2], time + m_InterpolationInterval);
	}
	m_UpdateInProgress = false;
}

void OrbitalBody::setInterpolationAngle(double angle) {
	m_InterpolationInterval = (angle / 360.0) * m_OrbitalPeriod;
	m_DirtyInterval = true;
}

double OrbitalBody::computeMagnitude(double time, Vector3 const &observer) const {
	if (!m_MagnitudeFunction) return 0.0;
	Vector3 position = getAbsolutePosition(time);
	Vector3 relative = observer - position;
	double distance_to_sun = position.normalize();
	double distance_to_observer = relative.normalize();
	double phase_angle = asin((relative ^ position).length());
	return m_MagnitudeFunction->compute(distance_to_sun, distance_to_observer, phase_angle);
}

void OrbitalBody::logInterpolationError(double time) const {
	CSPLOG(ERROR, PHYSICS) << label() << " interpolation outside interval: " << time << ", [" << m_OrbitalState[0].time << ", " << m_OrbitalState[1].time << ", " << m_OrbitalState[2].time << "]";
}

} // namespace csp

