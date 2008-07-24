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

#ifndef __CSP_CSPSIM_SKY_PHASESHADER_H__
#define __CSP_CSPSIM_SKY_PHASESHADER_H__

#include <osg/ref_ptr>
#include <osg/Vec3>

#include <csp/csplib/util/Ref.h>

namespace osg { class Image; }
namespace osg { class NodeCallback; }
namespace osg { class Texture2D; }

namespace csp {

/** PhaseShader shades the 2D image of a spherical body to approximate
 *  directional illumination.  This is used to render the appropriate
 *  phase of celestial bodies such as the Earth's moon.  Note that the
 *  shadowed side of the body is made partially transparent rather than
 *  darkened.  It is expected that the body will be rendered on a
 *  billboard in front of the sky dome.
 */
class PhaseShader: public Referenced {
public:
	/** Construct a new PhaseShader, optionally setting the source
	 *  image.  The source image may also be set after construction
	 *  using setImage.
	 */
	PhaseShader(osg::Image *image=0) {
		if (image) setImage(image);
	}

	/** Set the source image for the shader, which should be a square
	 *  RGBA image of a round, unshaded body.  Regions outside of the
	 *  body should be transparent.  The source image is not modified
	 *  by the shader; see getShadedImage.
	 */
	void setImage(osg::Image *image);

	/** Returns a shaded version of the source image set by setImage,
	 *  or null if a source image has not been set.
	 */
	osg::Image *getShadedImage() { return m_Shaded.get(); }

	/** Updates the shading for the specified light direction and ambient
	 *  light level.  Shadowed areas are made partially transparent
	 *  depending on the ambient light level, with a soft transition at
	 *  the terminus.
	 *
	 *  Shading is fairly expensive (more so for large images), and so
	 *  should be done infrequently.  For example, updating the shading
	 *  once per day before the body rises above the horizon is typically
	 *  sufficient.
	 *
	 *  For a 256x256 image this function takes about 80 ms on a reasonably
	 *  fast computer (circa 2004).  As such, the update generally cannot
	 *  be performed in a single frame without causing noticeable stutter.
	 *  See makeCallback for a simple way to spread the update over multiple
	 *  frames.
	 *
	 *  @param dir_to_sun The direction from the body to the light source,
	 *    in local image coordinates: +x to the right, +z to the top,
	 *    and +y opposite the image normal (i.e., along the vector from
	 *    the viewer to the body).
	 *  @param ambient The opacity of the object in fully shaded regions, in
	 *    the range [0,1].
	 */
	void reshade(osg::Vec3 const &dir_to_sun, double ambient);

	/** Create a new NodeCallback instance to incrementally reshade the
	 *  image over several frames and update the specified texture on
	 *  completion.  The callback automatically removes itself after the
	 *  texture is updated.
	 *
	 *  @param texture The texture the shaded image is assigned to upon
	 *    completion.
	 *  @param dir_to_sun The direction from the body to the light source,
	 *    in local image coordinates: +x to the right, +z to the top,
	 *    and +y opposite the image normal (i.e., along the vector from
	 *    the viewer to the body).
	 *  @param ambient The opacity of the object in fully shaded regions, in
	 *    the range [0,1].
	 */
	osg::NodeCallback *makeCallback(osg::Texture2D *texture, osg::Vec3 const &dir_to_sun, double ambient);

protected:
	~PhaseShader();

private:
	class Callback;
	
	/** Internal method to update several lines of the shaded image.  See
	 *  makeCallback for details.
	 *
	 *  @param pixels The approximate number of pixels to update per call.
	 *  @param index The line to begin shading at.  This should be set to zero
	 *    initially and then passed to subsequent calls so that shading picks
	 *    up where the last call ended.
	 *  @return True when the shading is done.
	 */
	bool incrementalReshade(osg::Vec3 const &dir_to_sun, double ambient, int lines, unsigned &index);

	osg::ref_ptr<osg::Image> m_Unshaded;
	osg::ref_ptr<osg::Image> m_Shaded;
};

} // namespace csp

#endif // __CSP_CSPSIM_SKY_PHASESHADER_H__

