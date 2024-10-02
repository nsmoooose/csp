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
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Vec4>
#include <csp/cspsim/sky/PhaseShader.h>

namespace osg { class Image; }
namespace osg { class Texture2D; }

namespace csp {

/** 2D imposter for images of celestial bodies.  Supports tinting the
 *  images to simulate atmospheric scattering at low elevation, and
 *  shading of the images to solar illumination as seen from the point
 *  of observation.  The imposter is intended to be rendered inside
 *  the sky dome; the alpha channel in the shaded image is used to
 *  control blending with the sky color.  For example, the half moon
 *  in the daytime sky is nearly transparent on the dark side, so
 *  that only the sky color is visible.
 */
class CSPSIM_EXPORT OrbitalBodyImposter: public osg::Geode {
public:
	/** Construct a new imposter.
	 *
	 *  @param core The base, unshade image.  See PhaseShade for details
	 *    and requirements.
	 *
	 *  // TODO document these arguments (and finish their implementation)
	 *  @param image_fraction
	 *  @param flare
	 *  @param flare_scale
	 */
	OrbitalBodyImposter(osg::Image *core, double image_fraction=1.0, osg::Image *flare=0, double flare_scale=1.0);

	/** Update the shading of the imposter.  Reshading is distributed over
	 *  multiple frames via update callbacks.
	 *
	 *  @param direction_to_sun The direction from the body to the sun in
	 *    imposter coordinates: +x = right, +y = from the observer to the
	 *    body, +z = up (toward the top of the image).
	 *  @param ambient The opacity [0-1] of the object in fully shaded
	 *    regions.  The imposter is rendered in front of the skydome, so
	 *    this controls blending of the skydome color in the shaded
	 *    regions, which approximates the ambient lighting (e.g., due to
	 *    earthshine).
	 */
	void updatePhase(osg::Vec3 const &direction_to_sun, double ambient);

	/** Set the base color of the imposter quad to simulate color-dependent
	 *  atmospheric scattering at low elevations, and update the flare
	 *  transparency.
	 *
	 *  @param color (rgba) base color of the imposter quad.
	 *  @param flare_alpha alpha value [0-1] of the flare image.
	 */
	void updateTint(osg::Vec4 const &color, double flare_alpha=0.0);

private:
	/** Internal method to generate a (size x size) textured quad, centered at
	 *  the origin and oriented such that +X is right, +Y is along the normal
	 *  on the untextured side of the quad, and +Z is up.
	 *
	 *  @param image The unshaded image for the imposter.
	 *  @param size The width and height of the quad (in meters).
	 *  @param return_texture If not null, returns the Texture2D object bound
	 *    to the quad.
	 *  @param additive_blend If true, the blend function is set to ONE, which
	 *    is useful for the flare quad.  Otherwise, ONE_MINUS_SRC_ALPHA is used.
	 */
	static osg::Geometry *makeImposter(osg::Image *image, double size, osg::ref_ptr<osg::Texture2D> *return_texture, bool additive_blend=false);

	osg::ref_ptr<osg::Texture2D> m_CoreTexture;
	osg::ref_ptr<osg::Geometry> m_CoreImposter;
	osg::ref_ptr<osg::Geometry> m_FlareImposter;
	Ref<PhaseShader> m_PhaseShader;
};

} // namespace csp
