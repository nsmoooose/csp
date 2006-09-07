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
#include <osg/Texture1D>
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

	updateLighting(azimuth, elevation);

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
				Color c = m_SkyShader->SkyColor(elevation, azimuth, intensity);
				unsigned i0 = idx + i*3 - 1;
				unsigned i1 = idx - i*3 - 1;
				// TODO azimuth does not match up perfectly in the simulation; seems to be off
				// by half a degree or so at 180 degrees azimuth.  need to double check the
				// coordinate calculations here relative to the texture coordinates assigned
				// in the dome creation.
				// to debug horizon (fog) and sky texture alignment, uncomment the next two
				// lines and comment the following to.  same in updateHorizon.
				//shade[++i0] = shade[++i1] = static_cast<unsigned char>(255 * (int(toDegrees(azimuth)) % 2)); // XXX
				//shade[++i0] = shade[++i1] = static_cast<unsigned char>(toDegrees(azimuth));
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
		updateHorizon();

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
	{
		m_HorizonSlice = m_Elevations.size();
		m_Elevations.push_back(toRadians(0.0));
	}
	m_Elevations.push_back(toRadians(1.0));
	m_Elevations.push_back(toRadians(2.0));
	m_Elevations.push_back(toRadians(3.0));
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

	// use approximately the same sampling as the rim of the texture dome (TEXSIZE * PI)
	m_HorizonColors = new osg::Vec4Array(TEXSIZE*4);
	m_HorizonImage = new osg::Image;
	m_HorizonImage->allocateImage(m_HorizonColors->size(), 4, 1, GL_RGB, GL_UNSIGNED_BYTE);
	m_HorizonTexture = new osg::Texture2D;
	m_HorizonTexture->setImage(m_HorizonImage.get());
	m_HorizonTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::MIRROR);
	m_HorizonTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
}

void SkyDome::updateHorizon() {
	unsigned n = m_HorizonColors->size();
	unsigned char *shade = m_HorizonImage->data();
	double da = PI / (n - 1);
	unsigned index = 0;
	for (unsigned j = 0; j < 4; ++j) {
		double azimuth = 0.0;
		for (unsigned i = 0 ; i < n; ++i) {
			float intensity;
			Color c = m_SkyShader->SkyColor(j * toRadians(1.0), azimuth, intensity);
			if (j == 0) (*m_HorizonColors)[i] = osg::Vec4(c.getA(), c.getB(), c.getC(), 1.0);
			// to debug horizon (fog) and sky texture alignment, uncomment the next two
			// lines and comment the following to.  same in updateShading.
			//shade[index++] = static_cast<unsigned char>(255 * (int(toDegrees(azimuth)) % 2)); // XXX
			//shade[index++] = static_cast<unsigned char>(toDegrees(azimuth));
			shade[index++] = static_cast<unsigned char>(c.getA() * 255.0);
			shade[index++] = static_cast<unsigned char>(c.getB() * 255.0);
			shade[index++] = static_cast<unsigned char>(c.getC() * 255.0);
			azimuth += da;
		}
	}
	m_HorizonImage->dirty();  // force reload
}

void SkyDome::updateLighting(double azimuth, double elevation) {
	if (!m_Sunlight) return;

	double specular_scale = 1.0;
	double diffuse_scale = 1.0;
	double ambient_scale = 0.3;
	double ambient = m_AverageIntensity;
	double clamped_elevation = std::max(0.0, elevation);
	float intensity = 0.0f;

	if (ambient > 0.1) ambient = 0.1;
	ambient = 0.0; // XXX

	Color color = m_SkyShader->SkyColor(clamped_elevation, 0.0, intensity);
	double x = cos(clamped_elevation) * cos(azimuth);
	double y = cos(clamped_elevation) * sin(azimuth);
	double z = sin(clamped_elevation);

	// the sky shading at the sun's position is too blue when the sun
	// is high to use as the light color.  instead we use the approximate
	// chromaticity taken from Preetham.  this function probably isn't
	// ideal before sunrise and after sunset, where secondary scattering
	// dominates the diffuse lighting component.  for an interesting
	// discussion of daylight color perception, see
	//   http://www.soluxtli.com/edu13.htm

	// approximate atmospheric path length (relative to 1.0 at sunset)
	double atmospheric_distance = 0.03 / (z + 0.03);

	// very approximate fit to figure 7 in Preetham, with y shifted down
	// slightly at large x to make the light more orange at low elevation.
	// A straight fit to Preetham is closer to
	//      ciey =  0.556 + 2.3*(ciex-0.33) - 2.0*ciex*ciex;
	double ciex = 0.3233 + 0.08 * atmospheric_distance;
	double ciey = 0.598 + 2.3*(ciex-0.33) - 2.4*ciex*ciex;

	// intensity is completely ad-hoc; pupil dilation and the eye's
	// nonlinear light response make a linear mapping of light intensity
	// to screen intensity look unnatural.
	double cieY = 1.0 - 0.4 * atmospheric_distance * atmospheric_distance;

	double weight = z * 2.0;
	if (weight > 1.0) weight = 1.0;

	color = Color(ciex, ciey, cieY, Color::CIExyY, false).toRGB(true);

	// ad-hoc additional darkening once the sun has set... the gl light
	// representing the sun should not be as bright as the horizion.
	double scale = 1.0;
	double light_r = color.getA() * scale;
	double light_g = color.getB() * scale;
	double light_b = color.getC() * scale;

	double sunset = std::min(1.0, toDegrees(elevation) + 0.5);
	// fade out ambient from sunset to 60 minutes after sunset.  absolute
	// ambient light is undoubtedly dropping before sunset, but making the
	// drop symmetric around sunset causes the scene to appear too dark
	// shortly after sunset, perhaps due to dark adaptation.
	ambient_scale = 0.3 * std::max(0.0, std::min((10.0 + toDegrees(elevation)) / 20.0 + 0.5, 1.0));
	diffuse_scale = std::max(ambient_scale, sunset);
	specular_scale = std::max(0.0, sunset);

	// sunlight direction (if using spot)
	m_Sunlight->setDirection(osg::Vec3(-x, -y, -z));
	
	// place the sun far from the scene
	m_Sunlight->setPosition(osg::Vec4(x, y, z, 0.0f));

	// set the various light components
	m_Sunlight->setAmbient(osg::Vec4(ambient_scale, ambient_scale, ambient_scale, 1.0f));
	m_Sunlight->setDiffuse(osg::Vec4(diffuse_scale*light_r, diffuse_scale*light_g, diffuse_scale*light_b, 1.0f));
	m_Sunlight->setSpecular(osg::Vec4(specular_scale*light_r, specular_scale*light_g, specular_scale*light_b, 1.0f));
	//std::cout << "sunlight " << toDegrees(elevation) << " " << light_r << " " << light_g << " " << light_b << " " << ambient_scale << "\n";
}

osg::Vec4 SkyDome::getHorizonColor(double angle) const {
	const int n = static_cast<int>(m_HorizonColors->size());
	float dx = (angle - m_SunAzimuth) * n / PI;
	int index_0 = static_cast<int>(floor(dx));
	dx -= index_0;
	index_0 = index_0 % (2*n - 1);
	if (index_0 < 0) index_0 = -index_0;
	if (index_0 >= n) index_0 = 2*n - 1 - index_0;
	int index_1 = (index_0 == (n-1)) ? (n-2) : index_0 + 1;
	return (*m_HorizonColors)[index_0] * (1.0 - dx) + (*m_HorizonColors)[index_1] * dx;
}

CSP_NAMESPACE_END

