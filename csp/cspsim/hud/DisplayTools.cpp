// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file DisplayTools.cpp
 *
 **/


#include <csp/cspsim/hud/DisplayTools.h>
#include <csp/csplib/util/Math.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineStipple>
#include <osg/LineWidth>
#include <osg/Version>

#include <cassert>


CSP_NAMESPACE

namespace display {

const SymbolMaker::Color SymbolMaker::Pen;

// TODO could optimize the single color case by using BIND_OVERALL, although this
// is probably not worth the trouble given current uses.

SymbolMaker::SymbolMaker(): m_Vertices(new osg::Vec3Array), m_Colors(new osg::Vec4Array), m_Geometry(new osg::Geometry), m_DrawMode(false), m_Color(1, 1, 1, 1), m_NoColor(true) {
	m_Geometry->setVertexArray(m_Vertices.get());
	m_Geometry->setColorArray(m_Colors.get());
}

SymbolMaker::~SymbolMaker() { }

void SymbolMaker::setPenColor(Color const &color) {
	m_Color = color;
	m_NoColor = false;
}

void SymbolMaker::addCircle(float x, float y, float radius, int n_sides, Color const &color) {
	assert(!m_DrawMode);
	osg::Vec3 offset(x, 0, y);
	const unsigned v0 = m_Vertices->size();
	addColorValues(color, n_sides);
	for (int i = 0; i < n_sides; ++i) {
		float angle = 2.0 * osg::PI * i / n_sides;
		m_Vertices->push_back(offset + osg::Vec3(radius * cos(angle), 0.0, radius * sin(angle)));
	}
	m_Geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, v0, n_sides));
}

void SymbolMaker::reset() {
	m_Vertices = new osg::Vec3Array;
	m_Colors = new osg::Vec4Array;
	m_Geometry = new osg::Geometry;
	m_Geometry->setVertexArray(m_Vertices.get());
	m_Geometry->setColorArray(m_Colors.get());
	if (!m_NoColor) {
		m_Geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	}
	m_DrawMode = false;
}

void SymbolMaker::erase() {
	assert(!m_DrawMode);
	m_Vertices->clear();
	m_Colors->clear();
	m_Geometry->getPrimitiveSetList().clear();
	m_Geometry->dirtyDisplayList();
	m_Geometry->dirtyBound();
}

void SymbolMaker::addRectangle(float x0, float y0, float x1, float y1, bool filled, Color const &color) {
	assert(!m_DrawMode);
	const unsigned v0 = m_Vertices->size();
	addColorValues(color, 4);
	if (x0 > x1) swap(x0, x1);
	if (y0 > y1) swap(y0, y1);
	m_Vertices->push_back(osg::Vec3(x1, 0.0, y0));
	m_Vertices->push_back(osg::Vec3(x1, 0.0, y1));
	m_Vertices->push_back(osg::Vec3(x0, 0.0, y1));
	m_Vertices->push_back(osg::Vec3(x0, 0.0, y0));
	if (filled) {
		m_Geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, v0, 4));
	} else {
		m_Geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, v0, 4));
	}
}

void SymbolMaker::addLine(float x0, float y0, float x1, float y1, Color const &color) {
	unsigned v0 = m_Vertices->size();
	addColorValues(color, 2);
	m_Vertices->push_back(osg::Vec3(x0, 0.0, y0));
	m_Vertices->push_back(osg::Vec3(x1, 0.0, y1));
	m_Geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, v0, 2));
}

void SymbolMaker::drawLine(float x0, float y0, float x1, float y1, Color const &color) {
	assert(m_DrawMode);
	addColorValues(color, 2);
	m_Vertices->push_back(osg::Vec3(x0, 0.0, y0));
	m_Vertices->push_back(osg::Vec3(x1, 0.0, y1));
	m_DrawCount += 2;
}

void SymbolMaker::moveTo(float x, float y) {
	assert(m_DrawMode);
	m_DrawPointX = x;
	m_DrawPointY = y;
}

void SymbolMaker::lineTo(float x, float y, Color const &color) {
	assert(m_DrawMode);
	addColorValues(color, 2);
	m_Vertices->push_back(osg::Vec3(m_DrawPointX, 0.0, m_DrawPointY));
	m_Vertices->push_back(osg::Vec3(x, 0.0, y));
	m_DrawCount += 2;
	moveTo(x, y);
}

void SymbolMaker::beginDrawLines() {
	assert(!m_DrawMode);
	m_DrawMode = true;
	m_DrawStart = m_Vertices->size();
	m_DrawCount = 0;
	m_DrawPointX = 0.0;
	m_DrawPointY = 0.0;
}

void SymbolMaker::endDrawLines() {
	assert(m_DrawMode);
	if (m_DrawCount > 0) {
		m_Geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, m_DrawStart, m_DrawCount));
	}
	m_DrawMode = false;
}

void SymbolMaker::addColorValues(Color const &color, unsigned n) {
	if (m_NoColor && &color != &Pen) {
		m_NoColor = false;
		m_Colors->insert(m_Colors->end(), m_Vertices->size(), m_Color);
		m_Geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	}
	if (!m_NoColor) {
		m_Colors->insert(m_Colors->end(), n, (&color == &Pen) ? m_Color : color);
	}
}

void SymbolMaker::addToGeode(osg::Geode *geode) {
	assert(!m_DrawMode);
	geode->addDrawable(m_Geometry.get());
}


Element::Element() {
	m_Element = new osg::Geode;
}

Element::~Element() { }

osg::Node *Element::node() {
	return geode();
}

void Element::clear() {
#if OSG_VERSION_MAJOR == 1 && OSG_VERSION_MINOR > 0
	geode()->removeDrawables(0, geode()->getNumDrawables());
#else
	geode()->removeDrawables(0, geode()->getNumDrawables());
#endif
}

void Element::setLineWidth(double width) {
	osg::StateSet *stateset = node()->getOrCreateStateSet();
	if (width > 0) {
		stateset->setAttributeAndModes(new osg::LineWidth(width), osg::StateAttribute::ON);
	} else {
		stateset->removeAttribute(osg::StateAttribute::LINEWIDTH);
	}
}

void Element::setLineStipple(uint8 scale, uint16 pattern) {
	osg::StateSet *stateset = node()->getOrCreateStateSet();
	if (scale > 0) {
		osg::LineStipple *stipple = new osg::LineStipple;
		stipple->setFactor(scale);
		stipple->setPattern(pattern);
		stateset->setAttributeAndModes(stipple, osg::StateAttribute::ON);
	} else {
		stateset->removeAttribute(osg::StateAttribute::LINESTIPPLE);
	}
}

osgText::Text *Element::addText() {
	osgText::Text *text = new osgText::Text;
	geode()->addDrawable(text);
	return text;
}

MoveableElement::MoveableElement(): Element() {
	m_Transform = new osg::PositionAttitudeTransform;
	m_Transform->addChild(geode());
}

osg::Vec3 MoveableElement::position() const {
	return m_Transform->getPosition();
}

void MoveableElement::setPosition(float x, float y) {
	m_Transform->setPosition(osg::Vec3(x, 0.0, y));
}

void MoveableElement::setOrientation(float angle) {
	osg::Quat orientation;
	orientation.makeRotate(angle, 0, 1.0, 0.0);
	m_Transform->setAttitude(orientation);
}


void DirectionElement::setDirection(osg::Vec3 const &direction) {
	m_Direction = direction;
	osg::Vec3 offset = m_ViewPoint - m_Origin;
	float t = - offset.y() / direction.y();
	osg::Vec3 position = offset + direction * t;
	setPosition(position.x(), position.z());
}

void DirectionElement::updateView(osg::Vec3 const &origin, osg::Vec3 const &view_point) {
	m_Origin = origin;
	m_ViewPoint = view_point;
}

void FloatingFrame::_addElement(Element *element) {
	node()->asGroup()->addChild(element->node());
}

ElementText::ElementText(Element *element): m_Element(element) {
	assert(element);
	element->geode()->addDrawable(this);
}

void ElementText::setPosition(float x, float y) {
	osgText::Text::setPosition(osg::Vec3(x, 0.0, y));
}

LabelElement::LabelElement(): m_Text(new osgText::Text) {
	geode()->addDrawable(m_Text);
}

} // namespace display

CSP_NAMESPACE_END

