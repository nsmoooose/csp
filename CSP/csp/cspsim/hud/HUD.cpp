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
 * @file HUD.cpp
 *
 **/


#include <csp/cspsim/hud/HUD.h>

#include <osg/ClipNode>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Math>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osgText/Text>

#include <cassert>

CSP_NAMESPACE

HUD::HUD(): m_ViewPoint(0, -1, 0) {
	osg::PositionAttitudeTransform* placement = new osg::PositionAttitudeTransform();
	osg::StateSet *stateset = placement->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	// TODO depth test needs to be enabled in external views, or hud should be disabled
	// or replaced with an imposter texture.
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	// TODO line width should vary with fov
	//stateset->setAttributeAndModes(new osg::LineWidth(1.5), osg::StateAttribute::ON);
	m_Material = new osg::Material;
	m_Material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.85, 0.34, 0.68, 1.0));
	stateset->setAttributeAndModes(m_Material.get(), osg::StateAttribute::ON);
	stateset->setRenderBinDetails(11, "RenderBin");
	osg::ClipNode* clipnode = new osg::ClipNode();
	m_ClipNode = clipnode;
	m_Placement = placement;
	clipnode->addChild(m_ForwardFrame.transform());
	placement->addChild(clipnode);
}

HUD::~HUD() { }

void HUD::setDimensions(float x, float y) {
	float x_min = -0.5 * x;
	float y_min = -0.5 * y;
	while (m_ClipNode->removeClipPlane(0U));  // remove all clip planes
	m_ClipNode->addClipPlane(new osg::ClipPlane(0,  1.0, 0.0,  0.0, -x_min));
	m_ClipNode->addClipPlane(new osg::ClipPlane(1, -1.0, 0.0,  0.0, -x_min));
	m_ClipNode->addClipPlane(new osg::ClipPlane(2,  0.0, 0.0,  1.0, -y_min));
	m_ClipNode->addClipPlane(new osg::ClipPlane(3,  0.0, 0.0, -1.0, -y_min));
}

void HUD::setViewPoint(osg::Vec3 const &view_point) {
	m_ViewPoint = view_point;
	updateView();
}
void HUD::setOrigin(osg::Vec3 const &origin) {
	m_Origin = origin;
	updateView();
}

void HUD::setColor(osg::Vec4 const &color) {
	m_Material->setDiffuse(osg::Material::FRONT_AND_BACK, color);
}

osg::PositionAttitudeTransform *HUD::hud() { return m_Placement.get(); }

HUD::Frame *HUD::forwardFrame() {
	return &m_ForwardFrame;
}

osg::Vec3 HUD::getForwardFrameOrigin() const {
	return m_ForwardFrame.transform()->getPosition();
}


HUD::SymbolMaker::SymbolMaker(): m_Vertices(new osg::Vec3Array), m_Geometry(new osg::Geometry), m_DrawMode(false) {
	m_Geometry->setVertexArray(m_Vertices.get());
}

HUD::SymbolMaker::~SymbolMaker() { }

void HUD::SymbolMaker::addCircle(float x, float y, float radius, int n_sides) {
	assert(!m_DrawMode);
	osg::Vec3 offset(x, 0, y);
	const unsigned v0 = m_Vertices->size();
	for (int i = 0; i < n_sides; ++i) {
		float angle = 2.0 * osg::PI * i / n_sides;
		m_Vertices->push_back(offset + osg::Vec3(radius * cos(angle), 0.0, radius * sin(angle)));
	}
	m_Geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, v0, n_sides));
}

void HUD::SymbolMaker::reset() {
	m_Vertices = new osg::Vec3Array;
	m_Geometry = new osg::Geometry;
	m_Geometry->setVertexArray(m_Vertices.get());
	m_DrawMode = false;
}

void HUD::SymbolMaker::erase() {
	assert(!m_DrawMode);
	m_Vertices->clear();
	m_Geometry->getPrimitiveSetList().clear();
	m_Geometry->dirtyDisplayList();
	m_Geometry->dirtyBound();
}

void HUD::SymbolMaker::addRectangle(float x0, float y0, float x1, float y1) {
	assert(!m_DrawMode);
	const unsigned v0 = m_Vertices->size();
	m_Vertices->push_back(osg::Vec3(x0, 0.0, y0));
	m_Vertices->push_back(osg::Vec3(x0, 0.0, y1));
	m_Vertices->push_back(osg::Vec3(x1, 0.0, y1));
	m_Vertices->push_back(osg::Vec3(x1, 0.0, y0));
	m_Geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, v0, 4));
}

void HUD::SymbolMaker::addLine(float x0, float y0, float x1, float y1) {
	unsigned v0 = m_Vertices->size();
	m_Vertices->push_back(osg::Vec3(x0, 0.0, y0));
	m_Vertices->push_back(osg::Vec3(x1, 0.0, y1));
	m_Geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, v0, 2));
}

void HUD::SymbolMaker::drawLine(float x0, float y0, float x1, float y1) {
	assert(m_DrawMode);
	m_Vertices->push_back(osg::Vec3(x0, 0.0, y0));
	m_Vertices->push_back(osg::Vec3(x1, 0.0, y1));
	m_DrawCount += 2;
}

void HUD::SymbolMaker::moveTo(float x, float y) {
	assert(m_DrawMode);
	m_DrawPointX = x;
	m_DrawPointY = y;
}

void HUD::SymbolMaker::lineTo(float x, float y) {
	assert(m_DrawMode);
	m_Vertices->push_back(osg::Vec3(m_DrawPointX, 0.0, m_DrawPointY));
	m_Vertices->push_back(osg::Vec3(x, 0.0, y));
	m_DrawCount += 2;
	moveTo(x, y);
}

void HUD::SymbolMaker::beginDrawLines() {
	assert(!m_DrawMode);
	m_DrawMode = true;
	m_DrawStart = m_Vertices->size();
	m_DrawCount = 0;
	m_DrawPointX = 0.0;
	m_DrawPointY = 0.0;
}

void HUD::SymbolMaker::endDrawLines() {
	assert(m_DrawMode);
	if (m_DrawCount > 0) {
		m_Geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, m_DrawStart, m_DrawCount));
	}
	m_DrawMode = false;
}

void HUD::SymbolMaker::addToGeode(osg::Geode *geode) {
	assert(!m_DrawMode);
	geode->addDrawable(m_Geometry.get());
}


HUD::Element::Element() {
	m_Element = new osg::Geode;
}

HUD::Element::~Element() { }

osg::Node *HUD::Element::node() {
	return geode();
}

void HUD::Element::clear() {
	geode()->removeDrawable(0, geode()->getNumDrawables());
}

osgText::Text *HUD::Element::addText() {
	osgText::Text *text = new osgText::Text;
	geode()->addDrawable(text);
	return text;
}

HUD::MoveableElement::MoveableElement(): Element() {
	m_Transform = new osg::PositionAttitudeTransform;
	m_Transform->addChild(geode());
}

osg::Vec3 HUD::MoveableElement::position() const {
	return m_Transform->getPosition();
}

void HUD::MoveableElement::setPosition(float x, float y) {
	m_Transform->setPosition(osg::Vec3(x, 0.0, y));
}

void HUD::MoveableElement::setOrientation(float angle) {
	osg::Quat orientation;
	orientation.makeRotate(angle, 0, 1.0, 0.0);
	m_Transform->setAttitude(orientation);
}


void HUD::DirectionElement::setDirection(osg::Vec3 const &direction) {
	m_Direction = direction;
	osg::Vec3 offset = m_ViewPoint - m_Origin;
	float t = - offset.y() / direction.y();
	osg::Vec3 position = offset + direction * t;
	setPosition(position.x(), position.z());
}

void HUD::DirectionElement::updateView(osg::Vec3 const &origin, osg::Vec3 const &view_point) {
	m_Origin = origin;
	m_ViewPoint = view_point;
}

void HUD::FloatingFrame::_addElement(Element *element) {
	node()->asGroup()->addChild(element->node());
}

HUD::ElementText::ElementText(Element *element): m_Element(element) {
	assert(element);
	element->geode()->addDrawable(this);
}

void HUD::ElementText::setPosition(float x, float y) {
	osgText::Text::setPosition(osg::Vec3(x, 0.0, y));
}

void HUD::ForwardFrame::_addElement(Element *element) {
	assert(element);
	m_Transform->addChild(element->node());
}

void HUD::addFrameElement(Element *element) {
	m_ForwardFrame.transform()->addChild(element->node());
}

void HUD::addFloatingElement(Element *element) {
	m_ClipNode->addChild(element->node());
	m_FloatingElements.push_back(element);
	element->updateView(m_Origin, m_ViewPoint);
}

void HUD::updateView() {
	for (unsigned i=0; i < m_FloatingElements.size(); ++i) {
		m_FloatingElements[i]->updateView(m_Origin, m_ViewPoint);
	}
	osg::Vec3 offset = m_ViewPoint - m_Origin;
	offset.y() = 0.0;
	m_ForwardFrame.transform()->setPosition(offset);
}

HUD::LabelElement::LabelElement(): m_Text(new osgText::Text) {
	geode()->addDrawable(m_Text);
}

CSP_NAMESPACE_END

