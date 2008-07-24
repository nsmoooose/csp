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

#include <csp/cspsim/sky/OrbitalBodyImposter.h>
#include <csp/cspsim/sky/PhaseShader.h>
#include <csp/csplib/util/Timing.h>

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Texture2D>

#include <iostream> // XXX

namespace csp {

OrbitalBodyImposter::OrbitalBodyImposter(osg::Image *core, double image_fraction, osg::Image *flare, double flare_scale) {
	assert(core);

	if (flare) {
		m_FlareImposter = makeImposter(flare, flare_scale / image_fraction, 0, true);
		addDrawable(m_FlareImposter.get());
	}

	// TODO need to take image_fraction into account
	m_PhaseShader = new PhaseShader;
	m_PhaseShader->setImage(core);
	m_CoreImposter = makeImposter(m_PhaseShader->getShadedImage(), 1.0 / image_fraction, &m_CoreTexture);
	addDrawable(m_CoreImposter.get());
}

void OrbitalBodyImposter::updatePhase(osg::Vec3 const &direction_to_sun, double ambient) {
	if (!getUpdateCallback()) {
		setUpdateCallback(m_PhaseShader->makeCallback(m_CoreTexture.get(), direction_to_sun, ambient));
	}
}

void OrbitalBodyImposter::updateTint(osg::Vec4 const &color, double flare_alpha) {
	osg::Vec4Array *colors = dynamic_cast<osg::Vec4Array*>(m_CoreImposter->getColorArray());
	assert(colors);
	(*colors)[0] = color;
	m_CoreImposter->dirtyDisplayList();
	if (m_FlareImposter.valid()) {
		osg::Vec4Array *colors = dynamic_cast<osg::Vec4Array*>(m_FlareImposter->getColorArray());
		assert(colors);
		(*colors)[0] = osg::Vec4(color.x(), color.y(), color.z(), flare_alpha);
		m_FlareImposter->dirtyDisplayList();
	}
}

osg::Geometry *OrbitalBodyImposter::makeImposter(osg::Image *image, double size, osg::ref_ptr<osg::Texture2D> *return_texture, bool additive_blend) {
	float s = static_cast<float>(size * 0.5);
	osg::Vec3Array *coords = new osg::Vec3Array(4);
	(*coords)[0].set(-s, 0.0f, s);
	(*coords)[1].set(-s, 0.0f, -s);
	(*coords)[2].set(s, 0.0f, -s);
	(*coords)[3].set(s, 0.0f, s);

	osg::Vec2Array* tcoords = new osg::Vec2Array(4);
	(*tcoords)[0].set(0.0f, 1.0f);
	(*tcoords)[1].set(0.0f, 0.0f);
	(*tcoords)[2].set(1.0f, 0.0f);
	(*tcoords)[3].set(1.0f, 1.0f);

	osg::Vec4Array* colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.0f, 1.0f, 1.0f, 1.0f);

	osg::Vec3Array* norms = new osg::Vec3Array(1);
	(*norms)[0].set(0.0f, -1.0f, 0.0f);

	osg::Geometry *geometry = new osg::Geometry;
	geometry->setNormalArray(norms);
	geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geometry->setVertexArray(coords);
	geometry->setTexCoordArray(0, tcoords);
	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	osg::Texture2D *texture = new osg::Texture2D(image);
	osg::StateSet *ss = geometry->getOrCreateStateSet();
	ss->setAttributeAndModes(new osg::Depth(), osg::StateAttribute::ON);
	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	ss->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, additive_blend ? osg::BlendFunc::ONE : osg::BlendFunc::ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);

	if (return_texture) *return_texture = texture;
	return geometry;
}

} // namespace csp

