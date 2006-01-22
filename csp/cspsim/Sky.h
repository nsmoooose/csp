// Combat Simulator Project
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

#ifndef __CSPSIM_SKY_H__
#define __CSPSIM_SKY_H__

#include <csp/cspsim/Colorspace.h>
#include <csp/csplib/data/Date.h>
#include <csp/csplib/util/ScopedPointer.h>

#include <osg/Group>
#include <osg/Light>
#include <osg/Geometry>
#include <osg/Billboard>
#include <osg/Matrix>

namespace osg {
	class Texture2D;
	class ColorMatrix;
}

CSP_NAMESPACE

class FalseHorizon;
class Moon;
class Sun;
class SkyShader;
class StarSystem;


class Sky: public osg::Group {
public:
	Sky();
	virtual ~Sky();
	void update(double lat, double lon, SimDate const &);
	osg::Light* getSunLight();
	osg::Light* getMoonLight();
	osg::Vec4 getHorizonColor(float angle) const;
	float getSunIntensity() const;
	float getSkyIntensity() const { return m_AverageIntensity; }
	void spinTheWorld(bool noreset=true);
	double getSpin() const { return m_SpinTheWorld*86400.0; }
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
	ScopedPointer<Sun> m_Sun;
	ScopedPointer<Moon> m_Moon;
	ScopedPointer<SkyShader> m_SkyShader;
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

CSP_NAMESPACE_END

#endif // __CSPSIM_SKY_H__

