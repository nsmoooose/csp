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

#ifndef __CSPSIM_SKY_STARDOME_H__
#define __CSPSIM_SKY_STARDOME_H__

#include <osg/Drawable>
#include <csp/csplib/util/Ref.h>
#include <vector>

namespace osg { class Geode; }

namespace csp {

// An OSG Drawable that renders a set of stars as points on the
// surface of a sphere.  The position, color, and magnitude of
// the stars is hard-coded in an associated header file.
class StarDome: public osg::Drawable {
	struct Star;

public:
	META_Object(csp, StarDome);

	StarDome(double radius=1.0);
	StarDome(const StarDome &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

	virtual ~StarDome();

	virtual void drawImplementation(osg::RenderInfo&) const;
	virtual osg::BoundingBox computeBound() const;

	osg::Geode *makeGeode();

	// Adjust the contrast of the stars based on the brightness of the
	// sky.  This simulates the dynamic range and dark adaptation of the
	// human eye.  The sky_magnitude roughly corresponds to the magnitude
	// of stars that should be barely visible.  The sky_magnitude is
	// clamped at 6.0, so that dimmer stars will not be visible no matter
	// how dark the sky becomes.  In practice the visibility threshold may
	// vary depending on the monitor and ambient lighting conditions.
	//
	// This method can be called every frame without affecting performance
	// since small lighting updates are ignored.
	void updateLighting(double sky_magnitude);

	// Adjust the star flare size to account for the view angle (in degrees).
	// The size of star flares is roughly independent of view angle.
	void setViewAngle(double angle);

private:
	// Copy star parameters to a Star instance.  The source fields are
	// x, y, z, red, green, blue, apparent magnitude.  The position vector
	// should be normalized to length 1.0, and the color components are in
	// the range [0, 1].
	void initStar(Star &star, const float source[7]);

	double m_Radius;
	double m_SkyMagnitude;
	Star *m_Stars;

	class BrightStar;
	std::vector<BrightStar> *m_BrightStars;
};

} // namespace csp

#endif // __CSPSIM_SKY_STARDOME_H__

