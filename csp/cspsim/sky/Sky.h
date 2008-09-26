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

#ifndef __CSP_CSPSIM_SKY_SKY_H__
#define __CSP_CSPSIM_SKY_SKY_H__

#include <csp/csplib/util/Ref.h>

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/Vec3>

#include <vector>

namespace osg { class Node; }
namespace osg { class Group; }
namespace osg { class Light; }
namespace osg { class LightSource; }
namespace osg { class MatrixTransform; }
namespace osg { class StateSet; }

namespace csp {

class OrbitalBodyModel;
class SkyDome;
class SolarSystem;
class StarDome;

/** Scene graph manager for celestial objects, stars, and sky dome as seen from
 *  an observation point on the surface of the Earth.  All objects are
 *  positioned under a transformation that converts equatorial coordinates to
 *  local observation coordinates.
 *
 *  Earth's rotation, with a 5 deg fov and 1000 pixel resolution requires an
 *  update once per second.  Since this is very cheap (just a single matrix
 *  repositions all objects) it is done every frame.
 *
 *  Other updates are done less frequently, at most every 10 seconds.  These
 *  include updating the position of orbital bodies, reshading the sky dome,
 *  and adjusting the sun and moon light.  For the first, the moon requires
 *  the most frequent updates.  It moves at about 360 degrees/month / (30 *
 *  24 * 3600 s/month) = 8.3 millidegrees/min = 2 pixels/min at 5 deg fov and
 *  1200 pixel resolution.  The planets are within an order of magnitude of
 *  this, since even the slowly moving outer planets must be repositioned in
 *  response to the Earth's motion.  For sky shading and lighting, the most
 *  frequent updates need to occur during sunrise and sunset.  Ten seconds
 *  seems to be adequate to provide smooth transitions.  Note that sky shading
 *  is fairly expensive, so each update is spread out over many subsequent
 *  frames.
 */
class CSPSIM_EXPORT Sky: public osg::Referenced {
public:
	Sky(double radius=1.0);
	~Sky();

	/** Set the position of the observer.
	 *
	 *  @param latitude Latitude of the observer, in radians.
	 *  @param longitud Longitude of the observer, in radians.
	 */
	void setPosition(double latitude, double longitude);

	/** Add an OrbitalBodyModel to the view.
	 */
	void addModel(OrbitalBodyModel *model);

	/** Direct access to the sky dome instance.
	 */
	SkyDome *getSkyDome() { return m_SkyDome.get(); }

	/** Direct access to the star dome instance.
	 */
	StarDome *getStarDome() { return m_StarDome.get(); }

	/** Direct access to the solar system instance, which manages the
	 *  planets.
	 */
	SolarSystem *getSolarSystem() { return m_SolarSystem.get(); }

	/** Update the view for the specified time, including the equatorial to
	 *  local coordinate transformation due to rotation of the Earth, the
	 *  sky dome, the star dome, and the planets.
	 */
	void update(double time, bool forceFullUpdate = false);

	/** Get the root scene graph node of the view.
	 */
	osg::Group *group();

	/** Convert from equatorial to local coordinates, at the time specified by
	 *  the last call to update().  The local coordinate frame is oriented such
	 *  that +X is east, +Y is north, and +Z is up.
	 */
	osg::Vec3 equatorialToLocal(osg::Vec3 const &vec) const;

	void addSunlight(int lightnum);
	void addMoonlight(int lightnum);
	osg::LightSource *getSunlight();
	osg::LightSource *getMoonlight();
	void addGlobalLights(osg::Group *group, osg::StateSet *ss);

private:
	/** Adjust the light representing the moon.  The light intensity is a
	 *  function of several parameters including the phase of the moon, its
	 *  distance from the Earth, atmospheric scattering near the horizon,
	 *  and the relative brightness of the sky due to sunlight.
	 */
	void updateMoonlight(double sky_magnitude);

	/** Compute the sun position in observer coordinates and the approximate
	 *  brightness of the sky.
	 */
	void getSunPositionAndSkyMagnitude(double time, double &elevation, double &azimuth, double &magnitude) const;

	osg::ref_ptr<osg::MatrixTransform> m_Transform;
	osg::ref_ptr<osg::Group> m_Group;
	osg::ref_ptr<osg::Group> m_ImposterGroup;
	osg::ref_ptr<osg::LightSource> m_Sunlight;
	osg::ref_ptr<osg::LightSource> m_Moonlight;
	osg::ref_ptr<StarDome> m_StarDome;

	Ref<SkyDome> m_SkyDome;
	Ref<SolarSystem> m_SolarSystem;

	Ref<OrbitalBodyModel> m_Moon;
	std::vector<Ref<OrbitalBodyModel> > m_Models;

	double m_Radius;
	double m_Latitude;
	double m_Longitude;
	double m_LastFullUpdate;
	int m_LastMoonIntensity;
};

} // namespace csp

#endif // __CSP_CSPSIM_SKY_SKY_H__

