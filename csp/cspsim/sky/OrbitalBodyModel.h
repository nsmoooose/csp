#pragma once
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

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/cspsim/Export.h>
#include <osg/MatrixTransform>

namespace csp {

class OrbitalBody;
class OrbitalBodyImposter;
class Sky;


/** Class to position an imposter of an orbital body in the local
 *  coordinates of an observer on the Earth.  Motion due to rotation
 *  of the observer is handled externally using a common transform for
 *  all models.
 */
class CSPSIM_EXPORT OrbitalBodyModel: public Referenced {
public:
	OrbitalBodyModel(OrbitalBodyImposter *imposter, double minimum_visible_angle, OrbitalBody *body, OrbitalBody *earth, double accuracy=0.01);
	virtual ~OrbitalBodyModel();

	osg::Node *node() { return m_Transform.get(); }

	/** Get the associated OrbitalBody.
	 */
	OrbitalBody *body() { return m_Body.get(); }
	OrbitalBody const *body() const { return m_Body.get(); }

	void setVisualAccuracy(double angle);

	osg::Vec3d const &getApparentDirection() const { return m_ApparentDirection; }
	double getPhaseAngle() const { return m_PhaseAngle; }
	double getEarthDistance() const { return m_EarthDistance; }
	double getAtmosphericAttenuation() const { return m_AtmosphericAttenuation; }

	void update(double time, double sky_magnitude, osg::Matrix const &equatorial_to_local);

	// Convert from ecliptic to equatorial coordinates.
	static Vector3 toEquatorial(Vector3 const &g) {
		static const double cos_ecl = cos(toRadians(23.4393));
		static const double sin_ecl = sin(toRadians(23.4393));
		return Vector3(g.x(), g.y() * cos_ecl - g.z() * sin_ecl, g.y() * sin_ecl + g.z() * cos_ecl);
	}

	void addTopocentricCorrection(double latitude, double longitude);
	void setSkyDomeRadius(double radius) { m_SkyDomeRadius = radius; }

	/** Attach a light source to the body.  If the argument is null, the light
	 *  source is removed.  Only one light source can be bound at a time.
	 */
	void bindLightSource(osg::LightSource *light_source);

	/** Get the light source bound to this body, if any.
	 */
	osg::LightSource *getLightSource() { return m_LightSource.get(); }

	/** Get elevation angle, including topocentric correction, based on the last update.
	 */
	double getElevation() const { return m_Elevation; }

	/** Get azimuth angle, including topocentric correction, based on the last update.
	 */
	double getAzimuth() const { return m_Azimuth; }

private:
	void updateVisibility(double magnitude, double sky_magnitude);
	void updateTint(osg::Vec4 const &color, double flare_alpha);
	static void atmosphericShading(double elevation, double &r, double &g, double &b);
	void recomputeMotion(double d);
	Vector3 topocentricCorrection(Vector3 const &e, double lst);

	osg::ref_ptr<osg::MatrixTransform> m_Transform;
	osg::ref_ptr<osg::LightSource> m_LightSource;
	osg::ref_ptr<OrbitalBodyImposter> m_Imposter;
	Ref<OrbitalBody> m_Body;
	Ref<OrbitalBody> m_Earth;
	double m_Accuracy;
	double m_MinimumVisibleAngle;
	double m_MinimumInterval;
	double m_LastTime;
	double m_LastElevation;
	double m_LastSkyMagnitude;
	double m_Radius;
	double m_Azimuth;
	double m_Elevation;
	double m_Latitude;
	double m_Longitude;
	bool m_TopocentricCorrection;
	double m_PhaseAngle;
	double m_AtmosphericAttenuation;
	double m_EarthDistance;
	double m_FlareAlpha;
	double m_SkyDomeRadius;
	osg::Vec4 m_Tint;
	osg::Vec3d m_ApparentDirection;
};


} // namespace csp
