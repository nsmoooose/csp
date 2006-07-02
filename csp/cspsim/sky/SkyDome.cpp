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

#include <csp/cspsim/sky/SkyDome.h>
#include <csp/cspsim/sky/SkyShader.h>
#include <csp/csplib/util/Math.h>

#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Light>
#include <osg/MatrixTransform>
#include <osg/TexEnv>
#include <osg/Texture2D>

#include <iostream>  // debugging only

CSP_NAMESPACE


SkyDome::SkyDome(double radius): m_SkyShader(new SkyShader), m_Radius(radius), m_NextSunAzimuth(0), m_NextSunElevation(0), m_SunAzimuth(0), m_SunElevation(0), m_UpdateRow(0), m_UpdateInProgress(false), m_AverageIntensity(0) {
	buildDome();
	m_SkyShader->setSunElevation(-0.1f); // XXX
}

SkyDome::~SkyDome() {
}

osg::Node *SkyDome::getDomeNode() {
	return m_DomeNode.get();
}

void SkyDome::setSunPosition(double azimuth, double elevation) {
	// Ignore very small updates.
	if (fabs(m_NextSunAzimuth - azimuth) < 0.0002 && fabs(m_NextSunElevation - elevation) < 0.0002) {
		return;
	}

	// If we are not already updating the texture then we begin the update
	// process using the new sun position.  Otherwise, store the sun coordinates
	// and let the current update finish; it will automatically trigger another
	// update using m_NextSunAzimuth and m_NextSunElevation when it completes.

	m_NextSunAzimuth = azimuth;
	m_NextSunElevation = elevation;

	if (!m_UpdateInProgress && (m_NextSunElevation != m_SunAzimuth || m_NextSunElevation != m_SunElevation)) {
		m_SunAzimuth = azimuth;
		m_SunElevation = elevation;
		m_SkyShader->setSunElevation(elevation);
		m_UpdateRow = 0;
		m_UpdateInProgress = true;
	}
}

void SkyDome::initSunlight(int light_num) {
	assert(!m_Sunlight);
	// Create a new light.  The color values are set to zero so that
	// the light has no effect until updateLighting is called.
	m_Sunlight = new osg::Light;
	m_Sunlight->setLightNum(light_num);
	m_Sunlight->setAmbient(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	m_Sunlight->setDiffuse(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	m_Sunlight->setSpecular(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
}

void SkyDome::updateShading(bool force) {
	// Only continue if there is work to be done (i.e., the sun position has
	// changed as a result of calling setSunPosition).
	if (!m_UpdateInProgress && !force) return;

	float dark = 0.9f; //0.9f;
	unsigned char *shade = m_Image->data();

	// Normally update one texture row per call, but if force is true generate
	// the entire texture at once.
	const int end = (force ? TEXSIZE : m_UpdateRow + 1);
	for (; m_UpdateRow < end; ++m_UpdateRow) {
		unsigned idx = (m_UpdateRow * TEXSIZE + TEXSIZE / 2) * 3;
		double y = (m_UpdateRow - TEXSIZE/2) / (TEXSIZE/2 - 2.0);
		for (int i = 0; i < TEXSIZE/2; ++i) {
			double x = i / (TEXSIZE/2 - 2.0);
			double elevation = (1.0 - sqrt(x*x + y*y)) * 0.5f * PI;
			if (elevation >= -0.15) {
				if (elevation < 0.0) elevation = 0.0;
				double azimuth = atan2(x, y);
				float intensity = 0.0f;
				Color c = m_SkyShader->SkyColor(elevation, azimuth, dark, intensity);
				unsigned i0 = idx + i*3 - 1;
				unsigned i1 = idx - i*3 - 1;
				shade[++i0] = shade[++i1] = static_cast<unsigned char>(c.getA() * 255.0);
				shade[++i0] = shade[++i1] = static_cast<unsigned char>(c.getB() * 255.0);
				shade[++i0] = shade[++i1] = static_cast<unsigned char>(c.getC() * 255.0);
				assert(i0 >= 2 && i0 < TEXSIZE*TEXSIZE*3);
				assert(i1 >= 2 && i1 < TEXSIZE*TEXSIZE*3);
				m_AverageIntensitySum += intensity;
				++m_AverageIntensityCount;
			}
		}
	}

	if (m_UpdateRow == TEXSIZE) {  // texture generation complete
		m_UpdateRow = 0;
		m_UpdateInProgress = false;
		m_Image->dirty();  // force reload

		// orient the sky dome to place the sun is at the desired azimuth.  the texture
		// map is aligned so that the sun rises from -Y to +Z at 0 azimuth, so we need
		// to subtract 90 degrees to line up with atan(y/x).
		if (m_DomeNode.valid()) {
			m_DomeNode->setMatrix(osg::Matrix::rotate(m_SunAzimuth - PI_2, 0, 0, 1));
		}
		updateLighting();

		// update the average skydome intensity.
		m_AverageIntensity = m_AverageIntensitySum / m_AverageIntensityCount;
		m_AverageIntensitySum = 0.0;
		m_AverageIntensityCount = 0;

		// if the sun position has changed since we started the last update, start a
		// new update using the most recent coordinates.
		if (m_NextSunElevation != m_SunElevation || m_NextSunAzimuth != m_SunAzimuth) {
			m_SunAzimuth = m_NextSunAzimuth;
			m_SunElevation = m_NextSunElevation;
			m_SkyShader->setSunElevation(m_SunElevation);
			m_UpdateInProgress = true;
		}
	}
}

void SkyDome::buildDome() {
	assert(!m_Dome.get());

	m_Elevations.push_back(toRadians(-10.0));
	m_Elevations.push_back(toRadians(-10.0));
	m_Elevations.push_back(toRadians(-5.0));
	m_Elevations.push_back(toRadians(0.0));
	m_Elevations.push_back(toRadians(1.0));
	for (float elev = 5.0f; elev <= 90.0; elev += 5.0) m_Elevations.push_back(toRadians(elev));

	m_Slices = m_Elevations.size();
	m_Segments = 37;
	const unsigned n = m_Slices * m_Segments;

	osg::Vec2Array *tcoord = new osg::Vec2Array(n);
	osg::Vec3Array *vertex = new osg::Vec3Array(n);

	for (unsigned i = 0, k = 0; i < m_Slices; ++i) {
		float cos_elev = cosf(m_Elevations[i]);
		float sin_elev = sinf(m_Elevations[i]);
		float tex_rad = static_cast<float>((TEXSIZE/2 - 2.0) / (TEXSIZE/2) * std::min(0.5, 0.5 - m_Elevations[i] / PI));
		const float rz = m_Radius;
		const float rh = (i > 0 ? 1.0 : 0.3) * rz;
		for (unsigned j = 0; j < m_Segments; ++j, ++k) {
			float theta = (j * 2 * PI) / m_Segments;
			float cos_theta = cosf(theta);
			float sin_theta = sinf(theta);
			float x = rh * cos_elev * cos_theta;
			float y = rh * cos_elev * sin_theta;
			float z = rz * sin_elev;
			float u = 0.5f + tex_rad * cos_theta - 0.5f / TEXSIZE;
			float v = 0.5f + tex_rad * sin_theta - 0.5f / TEXSIZE;
			(*vertex)[k].set(x, y, z);
			(*tcoord)[k].set(u, v);
		}
	}

	osg::DrawElementsUShort *tris = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLE_STRIP);
	tris->reserve(m_Slices * (m_Segments + 1) * 2);

	for (unsigned i = 0; i < m_Slices - 1; ++i) {
		for (unsigned j = 0; j <= m_Segments; ++j) {
			tris->push_back((i+0) * m_Segments + (j % m_Segments));
			tris->push_back((i+1) * m_Segments + (j % m_Segments));
		}
	}

	osg::Vec4Array *colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.0f, 1.0f, 1.0f, 1.0f);

	osg::Vec3Array *normals = new osg::Vec3Array(1);
	(*normals)[0].set(0.0f, 0.0f, 1.0f);

	m_Dome = new osg::Geometry;
	m_Dome->addPrimitiveSet(tris);
	m_Dome->setVertexArray(vertex);
	m_Dome->setTexCoordArray(0, tcoord);
	m_Dome->setColorArray(colors);
	m_Dome->setColorBinding(osg::Geometry::BIND_OVERALL);
	m_Dome->setNormalArray(normals);
	m_Dome->setNormalBinding(osg::Geometry::BIND_OVERALL);
	m_TexCoords = tcoord;

	m_Image = new osg::Image;
	m_Image->allocateImage(TEXSIZE, TEXSIZE, 1, GL_RGB, GL_UNSIGNED_BYTE);
	memset(m_Image->data(), 255, TEXSIZE*TEXSIZE*3);
	m_Image->setInternalTextureFormat(GL_RGB);

	osg::Texture2D *tex = new osg::Texture2D;
	tex->setDataVariance(osg::Object::DYNAMIC);
	tex->setImage(m_Image.get());
	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP);
	tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	tex->setUseHardwareMipMapGeneration(false);
	tex->setUnRefImageDataAfterApply(false);
	tex->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);

	osg::StateSet *dome_state = m_Dome->getOrCreateStateSet();
	dome_state->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
	dome_state->setTextureAttributeAndModes(0, new osg::TexEnv);
	dome_state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	dome_state->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
	dome_state->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	dome_state->setMode(GL_FOG, osg::StateAttribute::OFF);
	dome_state->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0.0f, 1.0f, false), osg::StateAttribute::ON);

	osg::Geode *geode = new osg::Geode;
	geode->addDrawable(m_Dome.get());
	m_DomeNode = new osg::MatrixTransform;
	m_DomeNode->addChild(geode);
}

void SkyDome::updateLighting() {
	if (!m_Sunlight) return;

	double specular_scale = 1.0;
	double diffuse_scale = 1.0;
	double ambient = m_AverageIntensity;
	double elevation = std::max(0.0, m_SunElevation);
	float intensity = 0.0f;

	if (ambient > 0.1) ambient = 0.1;
	ambient = 0.0; // XXX

	Color color = m_SkyShader->SkyColor(elevation, 0.0, 0.0, intensity);
	double x = cos(elevation) * cos(m_SunAzimuth);
	double y = cos(elevation) * sin(m_SunAzimuth);
	double z = sin(elevation);

#if 0
	// the sky shading at the sun's position is too blue when the sun
	// is high to use as the light color.  instead we use the approximate
	// chromaticity taken from Preetham.  near sunset/rise, this function
	// decays too rapidly to account for secondary scattering from
	// the atmosphere that becomes important as the direct sunlight weakens.
	// so, as an approximation we blend the sunlight color and intensity
	// smoothly into the sky color at the sun's position as it nears the
	// horizon.  this misses the sharp drop in direct light as the sun
	// sets, but captures the nice glow from the horizon that persists for
	// a short time after sunset/before sunrise.  The discontinuity in
	// direct light at sunrise/set is approximated by diffuse_scale and
	// specular_scale below.
	if (z > 0.0) {
		// approximate atmospheric path length (1.0 at sunset)
		double atmospheric_distance = 0.03 / (z + 0.03);
		// very approximate fit to figure 7 in Preetham
		double ciex = 0.3233 + 0.08 * atmospheric_distance;
		double ciey = 0.556 + 2.3*(ciex-0.33) - 2.0*ciex*ciex;
		// completely ad-hoc
		double cieY = 1.0 - 0.4 * atmospheric_distance * atmospheric_distance;
		double weight = z * 2.0;
		if (weight > 1.0) weight = 1.0;
		color.blend(Color(ciex, ciey, cieY, Color::CIExyY, false).toRGB(true), weight);
		intensity = (1.0 - weight) * intensity + weight * cieY;
	}
#endif

	// ad-hoc additional darkening once the sun has set... the gl light
	// representing the sun should not be as bright as the horizion.
	double scale = std::min(1.0, intensity * intensity * 25.0);
	double light_r = color.getA() * scale;
	double light_g = color.getB() * scale;
	double light_b = color.getC() * scale;

	// below horizon?
	if (z < 0.0) {
		// 0.5 cuts light level in half exactly at sunset
		// (give or take the sun's diameter)
		diffuse_scale = 0.5;
		// fade out specular faster as sun drops below the horizon
		specular_scale = std::max(0.0, 0.5 + 9.0*z);
		// get the "sun shine" from the horizion, not below.
		z = 0.0;
	}

	// sunlight direction (if using spot)
	m_Sunlight->setDirection(osg::Vec3(-x, -y, -z));
	
	// place the sun far from the scene
	m_Sunlight->setPosition(osg::Vec4(x, y, z, 0.0f));

	// set the various light components
	m_Sunlight->setAmbient(osg::Vec4(0.3f * light_r + ambient, 0.3f * light_g + ambient, 0.3f * light_b + ambient, 1.0f));
	m_Sunlight->setDiffuse(osg::Vec4(diffuse_scale*light_r, diffuse_scale*light_g, diffuse_scale*light_b, 1.0f));
	m_Sunlight->setSpecular(osg::Vec4(specular_scale*light_r, specular_scale*light_g, specular_scale*light_b, 1.0f));
	std::cout << "sunlight " << light_r << " " << light_g << " " << light_b << " " << intensity << "\n";
}

CSP_NAMESPACE_END

