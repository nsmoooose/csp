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

#include <csp/cspsim/sky/OrbitalBodyModel.h>
#include <csp/cspsim/sky/OrbitalBody.h>
#include <csp/cspsim/sky/OrbitalBodyImposter.h>
//#include <csp/cspsim/sky/SkyObserver.h>
#include <csp/cspsim/sky/Common.h>
#include <csp/csplib/util/osg.h>
#include <csp/csplib/util/Timing.h>

#include <osg/LightSource>

#include <iostream>  // XXX

CSP_NAMESPACE

OrbitalBodyModel::OrbitalBodyModel(OrbitalBodyImposter *imposter, double minimum_visible_angle, OrbitalBody *body, OrbitalBody *earth, double accuracy):
	m_Transform(new osg::MatrixTransform),
	m_Imposter(imposter),
	m_Body(body),
	m_Earth(earth),
	m_Accuracy(0),
	m_MinimumVisibleAngle(minimum_visible_angle),
	m_LastTime(0),
	m_LastElevation(0),
	m_LastSkyMagnitude(0),
	m_Azimuth(-PI_2),
	m_Elevation(PI_2),
	m_Latitude(0.0),
	m_Longitude(0.0),
	m_TopocentricCorrection(false),
	m_PhaseAngle(0.0),
	m_AtmosphericAttenuation(0.0),
	m_EarthDistance(0.0),
	m_SkyDomeRadius(1.0),
	m_Tint(1, 1, 1, 1)
{
	assert(imposter);
	m_Transform->addChild(imposter);
	setVisualAccuracy(accuracy);
}

OrbitalBodyModel::~OrbitalBodyModel() {
	std::cout << "~OrbitalBodyModel() " << m_Body->label() << "\n";
}

void OrbitalBodyModel::updateVisibility(double magnitude, double sky_magnitude) {
	if (m_Elevation < toRadians(-10.0)) return; // well below the horizon, don't bother
	if (fabs(m_Elevation - m_LastElevation) > toRadians(0.015) || fabs(sky_magnitude - m_LastSkyMagnitude) > 0.02) {
		m_LastElevation = m_Elevation;
		m_LastSkyMagnitude = sky_magnitude;

		// compute the approximate optical mass; a more accuate formula is:
		//     1.0 / (sin(E) + 0.15 / (3.885 + E)^1.253)
		// but the difference is probably not appreciable relative to subsequent approximations.
		double optical_mass = 1.0 / sin(std::max(m_Elevation + 0.028, 0.028));

		double magnitude_attenuation = 0.328 * optical_mass;  // +10 at ~1.9 degrees, +3 at ~6.2 degrees, +1 at 19 degrees.
		double atten = (10.0 / 255.0) * pow(10.0, sky_magnitude - magnitude_attenuation);
		double shine = pow(10.0, -magnitude);
		double flare_shine = std::min(shine * 0.001, 0.001);
		std::cout << "mag=" << magnitude << " sky=" << sky_magnitude << " atten=" << atten << " alpha=" << (atten*shine) << "\n";

		double r = 1.0;
		double g = exp(-0.005 * optical_mass);  // ~0.83 min
		double b = exp(-0.02 * optical_mass);  // ~0.50 min
		double alpha = std::min(1.0, shine * atten);
		double flare_alpha = std::min(1.0, flare_shine * atten);

		m_AtmosphericAttenuation = alpha;
		updateTint(osg::Vec4(r, g, b, alpha), flare_alpha);
	}
}

void OrbitalBodyModel::addTopocentricCorrection(double latitude, double longitude) {
	m_TopocentricCorrection = true;
	m_Latitude = latitude - toRadians(0.1924) * sin(2.0*latitude);
	m_Longitude = longitude;
	// note that this correction is on the same order as aircraft altitude
	// variations, which are neglected.
	m_Radius = 0.99833 + 0.00167 * cos(2*latitude);
}

void OrbitalBodyModel::updateTint(osg::Vec4 const &color, double flare_alpha) {
	// don't update if totally invisible
	if (color.w() + m_Tint.w() < (2.0/256.0)) return;
	// otherwise only update if there is a significant change
	if ((color - m_Tint).length2() > square(2.0/256.0) || fabs(flare_alpha - m_FlareAlpha) > (1.0/256.0)) {
		m_Tint = color;
		m_FlareAlpha = flare_alpha;
		//std::cout << m_Body->label() << " color = " << color << ", flare = " << flare_alpha << "\n";
		m_Imposter->updateTint(color, flare_alpha);
	}
}

void OrbitalBodyModel::atmosphericShading(double elevation, double &r, double &g, double &b) {
	elevation = std::max(0.0, toDegrees(elevation));
	r = 1.0;
	g = std::min(1.0, 0.80 + elevation * 0.010);
	b = std::min(1.0, 0.35 + elevation * 0.033);
}

void OrbitalBodyModel::update(double time, double sky_magnitude, osg::Matrix const &equatorial_to_local) {
	if (time - m_LastTime < m_MinimumInterval && m_LastTime != 0.0) return;

	double t0 = get_realtime();

	// for the Earth's moon, this portion of the update runs roughly once
	// every 90 seconds.

	Vector3 earth_pos = m_Earth->getAbsolutePosition(time);
	Vector3 pos = toEquatorial(m_Body->getAbsolutePosition(time) - earth_pos);
	Vector3 axis = toEquatorial(m_Body->getAxis(time)).normalized();

	if (m_TopocentricCorrection) {
		pos = topocentricCorrection(pos, MeanSiderealTime(time, m_Longitude));
	}

	double angular_radius = m_Body->getRadius() / pos.normalize();
	assert(angular_radius < 1.0);
	angular_radius = std::max(m_MinimumVisibleAngle, angular_radius);
	double scale = angular_radius * m_SkyDomeRadius;

	double magnitude = m_Body->getMagnitude(time);
	updateVisibility(magnitude, sky_magnitude);

	osg::Vec3d direction = toOSG(pos);

	m_ApparentDirection = direction;
	osg::Vec3d local = equatorial_to_local.preMult(direction);
	m_Elevation = asin(local.z());
	m_Azimuth = atan2(local.y(), local.x());

	// construct local imposter axes in equatorial coordinates.
	osg::Vec3d right = (direction ^ toOSG(axis));
	right.normalize();
	osg::Vec3d up = right ^ direction;

	// rotation matrix from imposter to equatorial coordinates
	osg::Matrixd m(
		right.x(), right.y(), right.z(), 0.0,
		direction.x(), direction.y(), direction.z(), 0.0,
		up.x(), up.y(), up.z(), 0.0,
		0.0, 0.0, 0.0, 1.0);

	// don't bother with phase updates, which are relatively expensive,
	// when the body is well below the horizon.
	if (m_Elevation > toRadians(-10.0)) {
		// postmult to convert equatorial sun direction to imposter coordinates,
		// then use that vector to start reshading the texture.
		osg::Vec3 local_dir_to_sun = m.postMult(toOSG(-earth_pos));
		local_dir_to_sun.normalize();
		m_Imposter->updatePhase(local_dir_to_sun, 0.04);
		m_PhaseAngle = acos(0.5 * (1.0 + local_dir_to_sun * osg::Vec3(0.0, 1.0, 0.0)));
		m_EarthDistance = earth_pos.length();
	}

	// the orientation is correct, now scale and position the imposter.
	m.preMult(osg::Matrixd::scale(scale, scale, scale));
	m.setTrans(direction * m_SkyDomeRadius);
	m_Transform->setMatrix(m);

	m_LastTime = time;
	t0 = get_realtime() - t0;
	std::cout << m_Body->label() << " model update took " << int(t0 * 1000000.0) << " us\n";

}

Vector3 OrbitalBodyModel::topocentricCorrection(Vector3 const &e, double lst) {
	double mpar = asin(1.0 / e.length());
	double RA = atan2(e.y(), e.x());
	double DE = atan2(e.z(), sqrt(e.x()*e.x() + e.y()*e.y()));
	double HA = lst - RA;
	double g = atan2(tan(m_Latitude), cos(HA));
	double topRA = RA - mpar * m_Radius * cos(m_Latitude) * sin(HA) / cos(DE);
	double topDE = DE - mpar * m_Radius * (g == 0 ? sin(-DE) * cos(HA) : sin(m_Latitude) * sin(g - DE) / sin(g));
	return Vector3(cos(topDE) * cos(topRA), cos(topDE) * sin(topRA), sin(topDE));
}

void OrbitalBodyModel::setVisualAccuracy(double angle) {
	m_Accuracy = angle;
	// compute the minimum time interval to update the apparent position
	// of the body in equatorial coordinates to achieve the desired accuracy.
	// the period of motion is taken as a blend of the orbital periods of the
	// body and the earth, since both the earth (observer) and body are in
	// motion.
	double period0 = m_Body->getOrbitalPeriodInDays();
	double period1 = 365.25;
	double reduced_period = 1.0 / ((1.0 / period0) + (1.0 / period1));
	m_MinimumInterval = m_Accuracy * reduced_period / 360.0;
	std::cout << angle << " " << m_MinimumInterval << " " << period0 << " *** \n";
}

void OrbitalBodyModel::bindLightSource(osg::LightSource *light_source) {
	if (light_source == m_LightSource.get()) return;
	if (m_LightSource.valid()) {
		m_Transform->removeChild(m_LightSource.get());
	}
	if (light_source) {
		if (light_source->getLight()) {
			light_source->getLight()->setDirection(osg::Vec3(0.0, -1.0, 0.0));
			light_source->getLight()->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0));
		}
		m_Transform->addChild(light_source);
	}
	m_LightSource = light_source;
}

CSP_NAMESPACE_END

