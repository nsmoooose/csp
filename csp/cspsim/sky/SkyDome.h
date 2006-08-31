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

#ifndef __CSP_CSPSIM_SKY_SKYDOME_H__
#define __CSP_CSPSIM_SKY_SKYDOME_H__

#include <osg/Array>
#include <csp/csplib/util/Referenced.h>
#include <csp/csplib/util/ScopedPointer.h>

namespace osg { class Geometry; }
namespace osg { class Image; }
namespace osg { class Light; }
namespace osg { class MatrixTransform; }
namespace osg { class Texture2D; }

CSP_NAMESPACE

class SkyShader;

/**
 * A dome-shaped geometry for rendering the sky.  The dome extends several
 * degrees below the horizon and wraps back toward the origin to prevent
 * visible gaps at the terrain edges.  The dome is drawn with depth test
 * disabled, so it should be rendered before other elements in the scene.
 * Note that the color buffer need not be cleared before rendering the
 * dome (as long as the view is inside and the terrain covers the small
 * hole at the bottom of the dome).  The dome texture is updated using
 * SkyShader based on the current sun position.  The osg::Light representing
 * the sun is also managed by this class.
 */
class SkyDome: public Referenced {

	// The size of the sky dome texture.  Also the number of frame across
	// which updates are spread.  Assuming 50 fps a 256 pixel texture
	// requires about 5 seconds to update, which is fast enough to provide
	// smooth shading transitions at sunrise/sunset.
	//enum { TEXSIZE = 256 };
	enum { TEXSIZE = 512 };

public:
	SkyDome(double radius=1.0);

	/**
	 * Set the sun position in local coordinates.  Zero degrees azimuth
	 * is +X (conventionally East), 90 degrees azimuth is +Y (conventionally
	 * North).  Zero degrees elevation is the horizon, 90 degrees elevation
	 * is the zenith.  Both angles are specified in radians.
	 */
	void setSunPosition(double azimuth, double elevation);

	/**
	 * Construct the OpenGL light for the sun, binding to the specified light
	 * index.  This method can only be called once; see getSunlight.
	 */
	void initSunlight(int light_num);

	/**
	 * Get the light constructed by initSunlight.  The light position, color,
	 * and intensity are updated if necessary when updateShading is called.
	 */
	osg::Light *getSunlight() { return m_Sunlight.get(); }

	/**
	 * Get the sky dome node.  The sky dome should be positioned relative to
	 * the camera in (x,y) and rendered before other elements in the scene
	 * (including stars).  The dome geometry assumes X is east, Y is north,
	 * and Z is up.  Rotate the dome node accordingly for other coordinate
	 * systems.
	 */
	osg::Node *getDomeNode();

	/**
	 * A measure of the light intensity averaged over the entire sky dome.
	 * The light intensity at each point is obtained from SkyShader::SkyColor.
	 */
	double getAverageIntensity() const { return m_AverageIntensity; }

	/**
	 * Update the sky color texture and lighting.  This method should be called
	 * once per frame.  Texture and lighting updates are only performed if the
	 * sun position has changed appreciably since the last update or if force
	 * is true.  When force is false, updates are spread across TEXSIZE calls
	 * to prevent long frames.
	 */
	void updateShading(bool force=false);

	/** Get a texture representing the sky color at the horizon.
	 */
	osg::Texture2D *getHorizonTexture() { return m_HorizonTexture.get(); }

	/** Get the sky color at the horizon.  The argument is the azimuth angle
	 *  in radians, in local coordinates (+X = east, +Y = north).
	 */
	osg::Vec4 getHorizonColor(double angle) const;

private:
	~SkyDome();

	/** Build the sky dome geometry and bind the sky shading texture.
	 */
	void buildDome();

	/** Update the sun light, if initialized.  Called by setSunPosition.
	 */
	void updateLighting(double azimuth, double elevation);

	/** Update the horizon color texture, which can be used to shade the
	 *  fog.  Called by updateShading.
	 */
	void updateHorizon();

	osg::ref_ptr<osg::Geometry> m_Dome;
	osg::ref_ptr<osg::Image> m_Image;
	osg::ref_ptr<osg::Light> m_Sunlight;
	osg::ref_ptr<osg::Vec2Array> m_TexCoords;
	osg::ref_ptr<osg::MatrixTransform> m_DomeNode;

	// the sky texture generator
	ScopedPointer<SkyShader> m_SkyShader;

	// dome geometry
	double m_Radius;
	unsigned m_Segments;
	unsigned m_Slices;
	std::vector<float> m_Elevations;

	// incremental sun position updates
	double m_NextSunAzimuth;
	double m_NextSunElevation;
	double m_SunAzimuth;
	double m_SunElevation;

	// incremental texture updates
	int m_UpdateRow;
	bool m_UpdateInProgress;

	// average light intensity across the dome
	double m_AverageIntensity;
	double m_AverageIntensitySum;
	double m_AverageIntensityCount;

	// track horizon colors separately for fog shading
	osg::ref_ptr<osg::Vec4Array> m_HorizonColors;
	osg::ref_ptr<osg::Image> m_HorizonImage;
	osg::ref_ptr<osg::Texture2D> m_HorizonTexture;
	std::vector<unsigned> m_HorizonIndex;
	unsigned m_HorizonSlice;
};

CSP_NAMESPACE_END

#endif // __CSP_CSPSIM_SKY_SKYDOME_H__

