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

#ifndef __CSP_CSPSIM_SKY_ORIBITALBODY_H__
#define __CSP_CSPSIM_SKY_ORIBITALBODY_H__

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Vector3.h>

#include <string>

namespace csp {

class MagnitudeFunction;


/** Base class for planets and moons.  Subclasses need to implement one or
 *  two virtual methods to define the base orbit and perturbations to that
 *  orbit.  The implementation computes accurate values of various orbital
 *  parameters and body properties at regular (angular) intervals along
 *  the orbit and interpolates the values at intermediate times.  Time is
 *  assumed to progress in small increments relative to the interpolation
 *  interval, which is typically 2 or more hours.
 *
 *  @see OrbitalBodyModel
 *  @see Sky
 */
class OrbitalBody: public Referenced {
public:
	static const double cG;  // gravitational constant (Nm^2/kg^2)
	static const double cMs;  // mass of the sun (kg)

	/** Construct a new orbital body.
	  *
	  * @param label Arbitrary text label for this body.
	  * @param semimajor_axis Semi-major axis of the orbit (in meters).
	  * @param eccentricity Eccentricity of the orbit.  Values above 0.6
	  *   are not currently supported.
	  * @param perturbations Set true if addPerturbations should be called
	  *   when updating the position of this body.
	  * @param heliocentry Set to true if this body orbits the sun, in
	  *   which case center must be null.
	  * @param mass Mass of the body in kg.
	  * @param radius Radius of the body in m.
	  * @param center OrbitalBody that this body orbits, may be null.
	  * @param observer OrbitalBody that views this body, if any.  This value
	  *   is used to compute the apparent magnitude of this body as seen by
	  *   the observer efficiently (using interpolation) via getMagnitude.
	  *   Note that the apparent magnitude at arbitrary observation points
	  *   may be computed using computeMagnitude, which is more expensive.
	  * @param magnitude_function A functor used to compute the apparent
	  *   magnitude of this body given the distances to the sun and observer
	  *   and the phase angle.  May be null, in which case getMagnitude and
	  *   computeMagnitude will return zero.
	  */
	OrbitalBody(
		std::string const &label,
		double semimajor_axis,
		double eccentricity,
		bool perturbations,
		bool heliocentric,
		double mass,
		double radius=0,
		OrbitalBody const *center=0,
		OrbitalBody const *observer=0,
		MagnitudeFunction const *magnitude_function=0);

	virtual ~OrbitalBody();

	// static properties
	std::string const &label() const { return m_Label; }
	double getSemiMajorAxis() const { return m_SemiMajorAxis; }
	double getEccentricity() const { return m_Eccentricity; }
	double getPerihelionDistance() const { return m_PerihelionDistance; }
	double getAphelionDistance() const { return m_AphelionDistance; }
	double getOrbitalPeriodInDays() const { return m_OrbitalPeriod; }
	double getOrbitalPeriodInSeconds() const { return m_OrbitalPeriod * 86400.0; }
	double getMass() const { return m_Mass; }
	double getRadius() const { return m_Radius; }
	bool isHeliocentric() const { return m_Heliocentric; }
	OrbitalBody const *getCenter() const { return m_Center.get(); }

	// dynamic (interpolated) properties
	Vector3 getAxis(double time) const {
		return interpolate(time, &OrbitalState::axis);
	}

	double getLongitudeOfPerihelion(double time) const {
		return interpolate(time, &OrbitalState::longitude_of_perihelion);
	}

	double getMeanAnomaly(double time) const {
		return interpolate(time, &OrbitalState::mean_anomaly);
	}

	double getMeanLongitude(double time) const {
		return interpolate(time, &OrbitalState::mean_longitude);
	}

	double getEclipticLatitude(double time) const {
		return interpolate(time, &OrbitalState::ecliptic_latitude);
	}
	
	double getEclipticLongitude(double time) const {
		return interpolate(time, &OrbitalState::ecliptic_longitude);
	}
		
	Vector3 getRelativePosition(double time) const {
		return interpolate(time, &OrbitalState::position);
	}

	Vector3 getAbsolutePosition(double time) const {
		Vector3 position = getRelativePosition(time);
		if (m_Center.valid()) position += m_Center->getAbsolutePosition(time);
		return position;
	}

	Vector3 getHeliocentricPosition(double time) const {
		return getAbsolutePosition(time);
	}

	double getMagnitude(double time) const {
		return interpolate(time, &OrbitalState::magnitude);
	}

	/** Compute the apparent magnitude of this body at the position of the
	 *  observer and at the specified time.  This computation is less efficient
	 *  than getMagnitude, and should only be used if the magnitude is needed
	 *  from multiple observers or from observers that are not associated with
	 *  an OrbitalBody.
	 */
	double computeMagnitude(double time, Vector3 const &observer) const;

	/** Update interpolation parameters for the specified time.  This method only
	 *  performs work if the current interpolation parameters are stale, so it is
	 *  safe to call it frequently (e.g., every second or even every frame is
	 *  fine), although even for the Earth's moon the interpolation end points
	 *  only change every 2 hours (assuming a 1 degree interpolation; see
	 *  setInterpolationAngle).
	 */
	void update(double time);

	/** Adjust the angle (anamoly) over which orbital properties are interpolated.
	 *  The default angle is one degree.  The update() method should be called
	 *  frequently relative to the time required to transit this angle (i.e.,
	 *  getOrbitalPeriod * angle / 360 for near circular orbits).
	 *
	 *  @param angle Angle in degrees.
	 */
	void setInterpolationAngle(double angle);

protected:
	// dynamic parameters (interpolated)
	struct OrbitalState {
		double time;
		double true_anomaly;
		double eccentric_anomaly;
		double mean_anomaly;
		double mean_longitude;
		double ecliptic_latitude;
		double ecliptic_longitude;
		double longitude_of_ascending_node;
		double longitude_of_perihelion;
		double argument_of_perihelion;
		double inclination;
		double distance;
		double magnitude;
		Vector3 position;
		Vector3 axis;
	};

	/** Update most of the orbital state parameters given values of the
	 *  mean_anomaly, the argument of the perihelion, the longitude of the
	 *  ascending_node, and the inclination.  Does not update the magnitude.
	 *  If perturbations were specified in the constructor, addPerturbations
	 *  will be called internally.
	 */
	void computePosition(OrbitalBody::OrbitalState &state);

private:
	// static parameters
	std::string m_Label;
	double m_SemiMajorAxis;
	double m_Eccentricity;
	double m_RootOneMinusE2;
	double m_PerihelionDistance;
	double m_AphelionDistance;
	double m_OrbitalPeriod;
	double m_Mass;
	double m_Radius;
	double m_InterpolationInterval;
	double m_InverseInterpolationInterval;
	bool m_DirtyInterval;
	bool m_Heliocentric;
	bool m_Perturbations;
	bool m_UpdateInProgress;
	Ref<const OrbitalBody> m_Center;
	Ref<const OrbitalBody> m_Observer;
	Ref<const MagnitudeFunction> m_MagnitudeFunction;

	OrbitalState m_OrbitalState[3];

	void logInterpolationError(double time) const;

	// TODO could use cubic interpolation if more accuracy is desired.
	template <typename T>
	T interpolate(double time, const T OrbitalState::*argument) const {
		assert(!m_UpdateInProgress);
		double f = (time - m_OrbitalState[1].time) * m_InverseInterpolationInterval;
		if (f > 0.0) {
			if (f > 1.0) logInterpolationError(time);
			return (m_OrbitalState[1].*argument) * (1.0 - f) + (m_OrbitalState[2].*argument) * f;
		} else {
			if (f < -1.0) logInterpolationError(time);
			return (m_OrbitalState[1].*argument) * (1.0 + f) - (m_OrbitalState[0].*argument) * f;
		}
	}

	/** Subclasses must implement this update the orbital state parameters
	 *  at the given time.  Typically, only the mean_anomaly, the argument
	 *  of the perihelion, the longitude of the ascending_node, and the
	 *  inclination are assigned (some of which may be constant, depending
	 *  on the level of approximation) before calling computePosition to
	 *  update the rest of the parameters.  If m_Observer is not null, this
	 *  method must also update the magnitude using computeMagnitude.
	 *
	 *  CAUTION: The dynamic property accessors should never be called from
	 *  this method, since the interpolation interval may not be well
	 *  defined until both endpoints have been updated!
	 */
	virtual void updateOrbitalState(OrbitalState &state, double time)=0;

	/** An optional method that may be implemented by subclasses to add
	 *  perturbations to the orbit.  Called by computePosition with the
	 *  unperturbed ecliptic coordinates if m_Perturbations is true.
	 *
	 *  CAUTION: The dynamic property accessors should never be called from
	 *  this method, since the interpolation interval may not be well
	 *  defined until both endpoints have been updated!
	 *
	 *  @param state Orbital state at the point of evaluation.  All orbital
	 *    parameters except position have been computed already.
	 *  @param ecliptic_longitude The unperturbed ecliptic longitude in
	 *    radians.  Returns the perturbed value.
	 *  @param ecliptic_latitude The unperturbed ecliptic latitude in
	 *    radians.  Returns the perturbed value.
	 */
	virtual void addPerturbations(OrbitalState const &state, double &ecliptic_longitude, double &ecliptic_latitude) const;
};


} // namespace csp

#endif // __CSP_CSPSIM_SKY_ORIBITALBODY_H__
