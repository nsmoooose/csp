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
 * @file FeatureQuad.cpp
 *
 **/


#include <csp/cspsim/theater/FeatureQuad.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/Shader.h>

#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>
#include <osg/TexEnv>
#include <osgDB/ReadFile>
#include <osg/CullFace>
#include <osg/Material>

CSP_NAMESPACE

CSP_XML_BEGIN(FeatureQuad)
	CSP_DEF("texture", m_Texture, true)
	CSP_DEF("width", m_Width, true)
	CSP_DEF("height", m_Height, true)
	CSP_DEF("x_offset", m_OffsetX, false)
	CSP_DEF("y_offset", m_OffsetY, false)
	CSP_DEF("lighting", m_Lighting, false)
CSP_XML_END


osg::Geometry *FeatureQuad::makeGeometry() const {
	float vv[][3] = {
		{ m_Width*(m_OffsetX-0.5), 0.0, m_Height*(m_OffsetY-0.5) },
		{ m_Width*(m_OffsetX+0.5), 0.0, m_Height*(m_OffsetY-0.5) },
		{ m_Width*(m_OffsetX+0.5), 0.0, m_Height*(m_OffsetY+0.5) },
		{ m_Width*(m_OffsetX-0.5), 0.0, m_Height*(m_OffsetY+0.5) },
	};

	osg::Vec3Array& v = *(new osg::Vec3Array(4));
	osg::Vec3Array& n = *(new osg::Vec3Array(1));
	osg::Vec2Array& t = *(new osg::Vec2Array(4));
	osg::Vec4Array& l = *(new osg::Vec4Array(1));

	l[0].set(1.0, 1.0, 1.0, 1.0);

	for(int i = 0; i < 4; i++ ) {
		v[i][0] = vv[i][0];
		v[i][1] = vv[i][1];
		v[i][2] = vv[i][2];
	}

	t[0].set(0.0, 0.0);
	t[1].set(1.0, 0.0);
	t[2].set(1.0, 1.0);
	t[3].set(0.0, 1.0);
	
	n[0].set(0.0, -1.0, 0.0);

	osg::Geometry *geom = new osg::Geometry;
	geom->setVertexArray(&v);
	geom->setTexCoordArray(0, &t);
	geom->setColorArray(&l);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	geom->setNormalArray(&n);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
	return geom;
}

osg::StateSet* FeatureQuad::makeStateSet() const {
	osg::Texture2D* tex = new osg::Texture2D;
	std::string image_path = getDataPath("ImagePath");
	std::string path = ospath::join(image_path, m_Texture.getSource());
	osg::Image* image = osgDB::readImageFile(path);
	tex->setImage(image);
	osg::StateSet *state = new osg::StateSet;
	state->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
	state->setTextureAttribute(0, new osg::TexEnv);
	state->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
	if (m_Lighting) {
		state->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	} else {
		state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	}
	osg::Material *material = new osg::Material;
	material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0,1.0,1.0,1.0));
	material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0,1.0,1.0,1.0));
	state->setAttributeAndModes(material, osg::StateAttribute::ON);
	state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	Shader::instance()->applyShader("object", state);
	return state;
}

FeatureQuad::FeatureQuad() {
	m_OffsetX = 0.0;
	m_OffsetY = 0.5;
	m_Geometry = NULL;
	m_StateSet = NULL;
	m_Lighting = true;
}

FeatureQuad::~FeatureQuad() {
}

osg::Geometry * FeatureQuad::getGeometry() const {
	if (!m_Geometry) {
		m_Geometry = makeGeometry();
	}
	return m_Geometry.get();
}

osg::StateSet * FeatureQuad::getStateSet() const {
	if (!m_StateSet) {
		m_StateSet = makeStateSet();
	}
	return m_StateSet.get();
}

CSP_NAMESPACE_END

