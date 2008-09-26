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

#ifndef __CSP_CSPSIM_SKY_SKYSHADER_H__
#define __CSP_CSPSIM_SKY_SKYSHADER_H__

#include <csp/cspsim/Export.h>
#include <csp/cspsim/sky/Colorspace.h>

namespace csp {

/** Implements a variation of the sky shading model described in
 *  "A Practical Analytic Model for Daylight" by A. J. Preetham,
 *  P. Shirley, and B. Smits.
 */
class CSPSIM_EXPORT SkyShader {
public:
	SkyShader();

	/**
	 * Set turbidity of the atmosphere, which approximates the scattering
	 * of sunlight due to haze.  The first call to SkyColor after calling
	 * this method is relatively expensive.
	 *
	 * @param T The new turbidity value.  Turbidity of 1.0 corresponds to
	 * no scattering due to haze, while 10.0 and above gives dense haze.
	 * Values in the range 2.0 to 3.0 are typical for clear skies.
	 */
	void setTurbidity(float T);

	/**
	 * Set the sun elevation used for shading calculations.  The first call
	 * to SkyColor after calling this method is relatively expensive.
	 *
	 * @param h The elevation of the sun above the horizon in radians.
	 */
	void setSunElevation(float h);

	/**
	 * Get the sky color at the specified position using the current sun
	 * elevation.
	 *
	 * @param elevation the elevation above the horizon in radians.
	 * @param azimuth the azimuthal angle relative to the sun in radians.
	 * @param intensity returns the unnormalized cie colorspace luminance (Y).
	 * @return the RGB color at the specified point.
	 */
	Color SkyColor(float elevation, float azimuth, float &intensity);

protected:

	typedef float coeff[5];
	struct sky_c { coeff x, y, Y; };
	struct perez { float x, y, Y; };

	// control parameters
	float m_HaloSharpness;
	float m_NightBase;
	float m_SunsetSharpness;
	float m_SunsetElevation;
	float m_OverLuminescence;
	float m_Turbidity;
	float m_SunElevation;

	// internal parameters
	sky_c m_Coefficients;
	Color m_Zenith;
	float m_PupilFactor;
	float m_MaxY;
	float m_F;
	float m_SunTheta;
	float m_SunVector[3];
	float m_AzimuthCorrection;
	perez m_PerezFactor;
	bool m_Dirty;

	// extensions
	float m_DarkAdjustment;
	Color m_DarkSkyColor;

	/**
	 * Get Skylight Distribution Coefficients (c.f. Preetham et al.) for
	 * a given atmospheric turbitity.
	 *
	 * @param T Turbidity.
	 */
	void getSkyCoefficients(float T, sky_c &skylight);

	/**
	 * Perez Sky Model function.
	 *
	 * @param theta An angle (radians).
	 * @param gamma An angle (radians).
	 * @param p Skylight coefficients.
	 */
	float F(float theta, float gamma, coeff &p);

	// Testing minor optimization over calling FastPerez; probably not
	// worth it.
	float FasterF(float cos_theta, float gamma, float cos_gamma, coeff &p);

	/**
	 * Normalized Perez Sky Model function for use with precomputed sun position.
	 *
	 * @param theta View vector declination.
	 * @param gamma Angle between view vector and sun vector.
	 * @praam factor Perez precomputed scaling factor for this color component.
	 * @param z Zenith color component.
	 * @param p Skylight coefficients.
	 */
	float FastPerez(float theta, float gamma, float factor, float z, coeff &p);
	
	/**
	 * Normalized Perez Sky Model function.
	 *
	 * @param theta View vector declination.
	 * @param gamma Angle between view vector and sun vector.
	 * @praam theta_s Sun vector declination.
	 * @param z Zenith color component.
	 * @param p Skylight coefficients.
	 */
	float Perez(float theta, float gamma, float theta_s, float z, coeff &p);

	/**
	 * Get zenith color in CIE xyY colorspace.
	 *
	 * @param T Atmospheric turbidity.
	 * @param theta_s Sun vector declination.
	 */
	Color getZenith(float T, float theta_s);

	/**
	 * Update internal values that depend only on sun position and turbidity.
	 * This method is called lazily by SkyColor when the sun position or
	 * atmospheric turbidity change.
	 */
	void _computeBase();
};


} // namespace csp

#endif // __CSP_CSPSIM_SKY_SKYSHADER_H__

