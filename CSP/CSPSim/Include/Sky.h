// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
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


/**
 * @file Sky.h
 *
 **/

#ifndef __SKY_H__
#define __SKY_H__

#include <osg/Group>
#include <osg/Light>
#include <osg/Geometry>
#include <osg/Billboard>
#include <osg/Matrix>
#include <SimData/Date.h>
#include "Colorspace.h"

class StarSystem;
class FalseHorizon;

namespace osg {
	class Texture2D;
	class ColorMatrix;
}

class SkyShader {
protected:

	typedef float coeff[5];
	struct sky_c { coeff x, y, Y; };
	struct perez { float x, y, Y; };

	// control parameters
	Color m_FullMoonColor;
	float m_HaloSharpness;
	float m_NightBase;
	float m_SunsetSharpness;
	float m_OverLuminescence;
	float m_Turbidity;
	float m_SunElevation;

	// internal parameters
	sky_c m_Coefficients;
	Color m_Zenith;
	float m_MaxY;
	float m_F;
	float m_SunTheta;
	float m_SunVector[3];
	float m_AzimuthCorrection;
	perez m_PerezFactor;
	bool m_Dirty;

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

	void _computeBase();

public:
	SkyShader();
	void setTurbidity(float T);
	void setSunElevation(float h);
	Color SkyColor(float elevation, float azimuth, float dark, float &intensity);
};


// OSG's billboard implementation is incomplete as of 0.9.3, and does
// not properly implement axial rotations with an arbitrary 'up' vector.
// This class provides that functionality.
class AstroBillboard: public osg::Billboard {
public:
	virtual bool computeMatrix(osg::Matrix& modelview, const osg::Vec3& eye_local, const osg::Vec3& pos_local) const;
	void setAxes(const osg::Vec3& axis, const osg::Vec3 &up, const osg::Vec3 &normal);
protected:
	osg::Vec3 _onormal;
	osg::Matrix _orient;
};


class AstronomicalBody
{
protected:
	// the earth's tilt varies periodically in time, by about 3 degrees
	// over a 41,000 year cycle.  but we'll ignore that for now ;-)
	// that fact here.
	static const double _earth_tilt;
	static const double _cos_earth_tilt;
	static const double _sin_earth_tilt;
	static const double _earth_radius;
	static const double _epoch;
	
	mutable double _alpha, _delta;
	double  _beta, _lambda, _pi;
	double _radius;
	mutable double _angular_radius, _distance;
	mutable bool _stale;

public:
	AstronomicalBody();
	virtual ~AstronomicalBody() {}

	/**
	 * Compute atmospheric refraction
	 * 
	 * Really very minor effect in the grand scheme of things.
	 */
	double refract(double h) const;

	void updateEquatorial() const;

	void toObserver(double lat, double lmst, double &h, double &A) const;

	void getEliptic(double &beta, double &lambda) const;

	void getEquatorial(double &delta, double &alpha) const;

	virtual void updatePosition(double julian_date) = 0;
	
	double getEpoch() const { return _epoch; }
	
	double getRadius() const { return _radius; }
	double getAngularRadius() const { return _angular_radius; }

	double getLambda() const { return _lambda; }
	double getBeta() const { return _beta; }
	double getPi() const { return _pi; }

	osg::Light *getLight() { return m_Light.get(); }
	void initLight(int n);

protected:

	osg::ref_ptr<osg::Light> m_Light;
	int m_LightNum;
};


class Sun: public AstronomicalBody {
public:
	Sun();
	virtual void updatePosition(double);
	void updateScene(double h, double A, Color const &color, float intensity, float background);
	void _updateLighting(float x, float y, float z, float h, Color const &color, float intensity, float background);
	Color const &getColor() const { return m_Color; }
	float getIntensity() const { return m_Intensity; }
protected:
	Color m_Color;
	float m_Intensity;
};

class Moon: public AstronomicalBody {
public:
	Moon();
	virtual void updatePosition(double);
	osg::Node* getNode();
	virtual void updateScene(double lat, double lmst, Sun const &sun, float intensity);
	float getPhase() { return m_Phase; }
	void setLight(int n);
	float getApparentBrightness() { return m_ApparentBrightness; }
private:
	void _initImposter();
	void _maskPhase(float phase, float beta);
	void _updateShading(float h, float intensity);
	void _updateIllumination(Sun const &sun);
	void _updateLighting(double x, double y, double z, double h, float intensity);
	bool m_DirtyImage;
	osg::ref_ptr<osg::MatrixTransform> m_Transform;
	osg::ref_ptr<AstroBillboard> m_Billboard;
	osg::ref_ptr<osg::Image> m_Image;
	osg::ref_ptr<osg::Image> m_Phased;
	osg::ref_ptr<osg::Texture2D> m_Texture;
	osg::ref_ptr<osg::Geometry> m_Moon;
	osg::ref_ptr<osg::ColorMatrix> m_CM;
	double m_Latitude;
	double m_Phase;
	float m_MoonShine;
	float m_EarthShine;
	float m_SunShine;
	float m_ApparentBrightness;
	float m_RenderDistance;
};


class Sky: public osg::Group {
public:
	Sky();
	virtual ~Sky();
	void update(double lat, double lon, simdata::SimDate const &);
	osg::Light* getSunLight();
	osg::Light* getMoonLight();
	osg::Vec4 getHorizonColor(float angle);
	float getSunIntensity() { return m_Sun.getIntensity(); }
	float getSkyIntensity() { return m_AverageIntensity; }
	void spinTheWorld(bool noreset=true);
	double getSpin() { return m_SpinTheWorld*86400.0; }
	void updateHorizon(osg::Vec4 const &fog_color, float altitude, float clip);
protected:
	void _init();
	void _initLights();
	void _updateSun();
	void _updateStars();
	void _updateMoon(bool quick);
	void _updateShading(double sun_h, double sun_A);
	osg::ref_ptr<StarSystem> m_StarDome;
	osg::ref_ptr<FalseHorizon> m_Horizon;
	osg::ref_ptr<osg::Geometry> m_SkyDome;
	osg::ref_ptr<osg::Light> m_SunLight;
	osg::ref_ptr<osg::Light> m_MoonLight;
	osg::Vec2Array *m_TexCoords;
	osg::Vec4Array *m_Colors;
	osg::Vec4Array *m_HorizonColors;
	osg::ref_ptr<osg::Texture2D> m_SkyDomeTexture;
	osg::ref_ptr<osg::Image> m_SkyDomeTextureImage;
	Sun m_Sun;
	Moon m_Moon;
	SkyShader m_SkyShader;
	int m_nlev, m_nseg;
	float *m_lev;
	int m_HorizonIndex;
	Color m_ZenithColor;
	float m_ZenithIntensity;
	float m_AverageIntensity;
	double m_JD;
	double m_LastMoonFullUpdate;
	double m_LMST;
	double m_Latitude;
	double m_SpinTheWorld;
};

#endif // __SKY_H__

