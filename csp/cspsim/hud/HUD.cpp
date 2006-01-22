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
	m_Material = new osg::Material;
	m_Material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.85, 0.34, 0.68, 1.0));
	//stateset->setAttributeAndModes(m_Material.get(), osg::StateAttribute::ON);
	//stateset->setRenderBinDetails(11, "RenderBin");
	model()->addChild(m_ForwardFrame.transform());

	osg::StateSet *stateset = model()->getOrCreateStateSet();
	// the HUD is drawn in the XZ plane, while the HUD glass in the cockpit model is
	// tilted at 45 degrees.  Therefore half of the HUD display is behind the glass
	// and depth tests must be disabled for it to render properly.
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
}

HUD::~HUD() { }

// TODO remove me
osg::PositionAttitudeTransform *HUD::hud() {
	if (!m_Placement) {
		m_Placement = new osg::PositionAttitudeTransform;
		m_Placement->addChild(model());
		osg::StateSet *stateset = m_Placement->getOrCreateStateSet();
		stateset->setAttributeAndModes(m_Material.get(), osg::StateAttribute::ON);
		stateset->setRenderBinDetails(11, "RenderBin");
	}
	return m_Placement.get();
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

display::Frame *HUD::forwardFrame() {
	return &m_ForwardFrame;
}

osg::Vec3 HUD::getForwardFrameOrigin() const {
	return m_ForwardFrame.transform()->getPosition();
}

void HUD::ForwardFrame::_addElement(display::Element *element) {
	assert(element);
	m_Transform->addChild(element->node());
}

void HUD::addFrameElement(display::Element *element) {
	m_ForwardFrame.transform()->addChild(element->node());
}

void HUD::addFloatingElement(display::Element *element) {
	model()->addChild(element->node());
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

CSP_NAMESPACE_END

