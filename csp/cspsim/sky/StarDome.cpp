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

#include "csp/cspsim/sky/StarDome.h"
#include "csp/cspsim/sky/Stars.h"
#include "csp/csplib/util/Math.h"

#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osgDB/ReadFile>

#include <iostream>  // debugging only

namespace csp {

static const char *StarFlareImage = "sky/star-flare.png";

struct StarDome::Star {
	float nx, ny, nz;  // normal
	float cr, cg, cb, ca;  // color
	float px, py, pz;  // position
};


class StarDome::BrightStar {
public:
	BrightStar(osg::Texture2D *texture, osg::Vec3 const &position, double angle, double magnitude) {
		// mag +3 stars just beginning to flare at sky mag +6
		m_Attenuation = pow(10.0, -3.0 - magnitude);
		m_LastAlpha = 0.0;
		makeImposter(texture, position, angle);
	}

	void update(double sky_alpha_scaling) {
		// TODO the apparent brightness on the monitor is different for the single-pixel
		// alpha scale and the flare alpha scale --- these should be normalized to produce
		// a more linear apparent brightness.
		double alpha = clampTo(sky_alpha_scaling * m_Attenuation, 0.0, 1.0);
		if (fabs(alpha - m_LastAlpha) < (2.0 / 255.0)) return;
		osg::Vec4Array *colors = dynamic_cast<osg::Vec4Array*>(m_Imposter->getColorArray());
		assert(colors);
		(*colors)[0] = osg::Vec4(1, 1, 1, alpha);
		m_Imposter->dirtyDisplayList();
	}

	void updateViewAngle(double angle) {
		osg::Vec2Array *tcoords = dynamic_cast<osg::Vec2Array*>(m_Imposter->getTexCoordArray(0));
		assert(tcoords);
		double view_scaling = std::max(0.5, 40.0 / angle);
		float a = 0.5f + static_cast<float>(view_scaling);
		float b = 1.0f - a;
		(*tcoords)[0].set(b, a);
		(*tcoords)[1].set(b, b);
		(*tcoords)[2].set(a, b);
		(*tcoords)[3].set(a, a);
		m_Imposter->dirtyDisplayList();
	}

	osg::Geometry* getImposter() { return m_Imposter.get(); }

private:
	void makeImposter(osg::Texture2D *texture, osg::Vec3 const &position, double angle) {
		osg::Vec3 norm = -position;
		float r = norm.normalize();
		osg::Vec3 right = position ^ osg::Vec3(0, 0, 1);
		osg::Vec3 up = right ^ position;
		right.normalize();
		up.normalize();
		double size = 0.5 * angle * r;

		osg::Vec3Array *coords = new osg::Vec3Array(4);
		(*coords)[0] = position + (up - right) * size;
		(*coords)[1] = position + (-up - right) * size;
		(*coords)[2] = position + (right - up) * size;
		(*coords)[3] = position + (up + right) * size;

		osg::Vec2Array* tcoords = new osg::Vec2Array(4);
		(*tcoords)[0].set(0.0f, 1.0f);
		(*tcoords)[1].set(0.0f, 0.0f);
		(*tcoords)[2].set(1.0f, 0.0f);
		(*tcoords)[3].set(1.0f, 1.0f);

		osg::Vec4Array* colors = new osg::Vec4Array(1);
		(*colors)[0].set(1.0f, 1.0f, 1.0f, 0.0f);

		osg::Vec3Array* norms = new osg::Vec3Array(1);
		(*norms)[0] = norm;

		osg::Geometry *geometry = new osg::Geometry;
		geometry->setNormalArray(norms);
		geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
		geometry->setVertexArray(coords);
		geometry->setTexCoordArray(0, tcoords);
		geometry->setColorArray(colors);
		geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

		osg::StateSet *ss = geometry->getOrCreateStateSet();
		ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		ss->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
		ss->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE), osg::StateAttribute::ON);

		// adjust the z range slightly so that imposters such as the moon occlude
		// stars, which are drawn at the same depth on the skydome.
		ss->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0.1, 1.0, false), osg::StateAttribute::ON);

		m_Imposter = geometry;
	}

	osg::ref_ptr<osg::Geometry> m_Imposter;
	double m_Attenuation;
	double m_LastAlpha;
};


StarDome::StarDome(double radius): m_Radius(radius) {
	m_Stars = new Star[cNumStars];

	m_BrightStars = new std::vector<BrightStar>;
	m_BrightStars->reserve(100);

	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(StarFlareImage);
	osg::ref_ptr<osg::Texture2D> bright_texture;
	double flare_angular_size = 0.0;
	if (image.valid()) {
		bright_texture = new osg::Texture2D(image.get());
		bright_texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
		bright_texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
		bright_texture->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 0.0));
		flare_angular_size = 1.2 * toRadians(image->s() / 64.0);
	} else {
		CSPLOG(ERROR, SCENE) << "Unable to load " << StarFlareImage;
	}

	for (unsigned i = 0; i < cNumStars; ++i) {
		const float *source = cStarCatalog[i];
		const double magnitude = source[3];
		initStar(m_Stars[i], source);
		if (magnitude < 3.0 && bright_texture.valid()) {
			osg::Vec3 position(m_Stars[i].px, m_Stars[i].py, m_Stars[i].pz);
			m_BrightStars->push_back(BrightStar(bright_texture.get(), position, flare_angular_size, magnitude));
		}
	}
	m_SkyMagnitude = 4.0;
}


StarDome::~StarDome() {
	delete[] m_Stars;
	delete m_BrightStars;
}

StarDome::StarDome(const StarDome &copy, const osg::CopyOp &copyop): osg::Drawable(copy, copyop), m_Radius(copy.m_Radius) {
	assert(0);
}

void StarDome::drawImplementation(osg::RenderInfo&) const {
	const double atten = (10.0 / 255.0) * pow(10.0, m_SkyMagnitude);
	glPushAttrib(GL_ENABLE_BIT);
	glPushMatrix();
	glEnable(GL_BLEND);
	glBegin(GL_POINTS);
	for (unsigned i = 0; i < cNumStars; ++i) {
		const Star& star = m_Stars[i];
		glNormal3f(star.nx, star.ny, star.nz);
		glColor4f(star.cr, star.cg, star.cb, std::min(1.0, star.ca * atten));
		glVertex3f(star.px, star.py, star.pz);
	}
	glEnd();
	glPopMatrix();
	glPopAttrib();
}

osg::BoundingBox StarDome::computeBoundingBox() const {
	osg::BoundingBox bbox;
	bbox._min = osg::Vec3(-m_Radius, -m_Radius, -m_Radius);
	bbox._max = osg::Vec3(m_Radius, m_Radius, m_Radius);
	return bbox;
}

void StarDome::setViewAngle(double angle) {
	for (unsigned i = 0; i < m_BrightStars->size(); ++i) {
		(*m_BrightStars)[i].updateViewAngle(angle);
	}
}

void StarDome::updateLighting(double sky_magnitude) {
	// clamp the sky magnitude at 6.0 so that magnitude 6.0 stars are just
	// visible (alpha = 10/255), no matter how dark the sky is.  in practice,
	// the visible threshold comes out to be around 5.5 on my monitor.
	sky_magnitude = std::min(6.0, sky_magnitude);
	// update less frequently when the sky is brighter than any star; when
	// the sky is magnitude 2.5, the rgb brightness of sirius is 1/255.
	// with 0.01 steps the peak brightness change is about 6/255 per update.
	double delta = (sky_magnitude > -2.5 && sky_magnitude < 6.0) ? 0.01 : 0.1;
	if (fabs(sky_magnitude - m_SkyMagnitude) >= delta) {
		m_SkyMagnitude = sky_magnitude;
		dirtyDisplayList();  // force drawImplementation to update the star colors

		const double atten = (10.0 / 255.0) * pow(10.0, m_SkyMagnitude);
		for (unsigned i = 0; i < m_BrightStars->size(); ++i) {
			(*m_BrightStars)[i].update(atten);
		}
	}
}

void StarDome::initStar(Star &star, const float source[7]) {
	// normal
	star.nx = -source[0];
	star.ny = -source[1];
	star.nz = -source[2];

	// position
	star.px = m_Radius * source[0];
	star.py = m_Radius * source[1];
	star.pz = m_Radius * source[2];

	// color
	star.cr = source[4];
	star.cg = source[5];
	star.cb = source[6];

	// desaturate dim stars.  show full color below mag=0, no color above
	// mag=5.  when fully desaturated we use the rgb709 luminance.
	double lum = (0.213*star.cr + 0.715*star.cg + 0.072*star.cb);
	double desat = clampTo(1.0 - 0.2 * source[3], 0.0, 1.0);

	star.cr += (1.0 - desat) * (lum - star.cr);
	star.cg += (1.0 - desat) * (lum - star.cg);
	star.cb += (1.0 - desat) * (lum - star.cb);

	// rescale colors to luminance = 1.0, clamping as needed.  note that
	// all the color operations here are entirely ad-hoc; intended to
	// *very* roughly approximate color perception of dim objects.
	if (lum > 0) {
		star.cr = std::min(1.0, star.cr / lum);
		star.cg = std::min(1.0, star.cg / lum);
		star.cb = std::min(1.0, star.cb / lum);
	}

	// prescale magnitudes to save work when updating point colors.  note that
	// magnitude is used to control the alpha value when rendering the star.
	star.ca = pow(10.0f, - source[3]);
}

osg::Geode *StarDome::makeGeode() {
	osg::Geode *stars = new osg::Geode;
	stars->addDrawable(this);
	stars->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	stars->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stars->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
	
	// adjust the z range slightly so that imposters such as the moon occlude
	// stars, which are drawn at the same depth on the skydome.
	stars->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0.1, 1.0, false), osg::StateAttribute::ON);

	for (unsigned i = 0; i < m_BrightStars->size(); ++i) {
		stars->addDrawable((*m_BrightStars)[i].getImposter());
	}
	return stars;
}

} // namespace csp

