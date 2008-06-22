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
 * @file Display.cpp
 *
 **/


#include <csp/cspsim/hud/Display.h>
#include <osg/StateSet>
#include <osg/ClipNode>
#include <osg/Depth>
#include <osg/Group>
#include <osg/Program>

CSP_NAMESPACE

Display::Display() {
	// using a clipnode for the root is not ideal since it does not preserve
	// state attributes when the clip planes change.  otoh, having a single
	// node greatly simplifies the interface, and clip planes should only be
	// set once.  so we do a little extra work in setDimensions to ensure
	// that any state attributes set during construction are preserved.
	m_Root = new osg::ClipNode;

	osg::StateSet *stateset = m_Root->getOrCreateStateSet();

	// disable any default shaders that have been set on the parent object.
	// most shaders rely on material properties, which are not used by the
	// display, and the fixed-function pipeline is adequate for this purpose.
	// of course, specialized dislay shaders could be added if desired.
	stateset->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
	stateset->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 0.0, 1.0, false), osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	// TODO line width should vary with fov
	//stateset->setAttributeAndModes(new osg::LineWidth(1.5), osg::StateAttribute::ON);
}

Display::~Display() {
}

Display::Display(Display const &other): m_Root(other.m_Root) {
}

Display const &Display::operator=(Display const &other) {
	m_Root = other.m_Root;
	return *this;
}

osg::Group *Display::model() {
	return m_Root.get();
}

void Display::setDimensions(float x, float y) {
	// every clip plane change blows away all other state attributes, so
	// we have to manually save and restort the stateset.
	osg::ref_ptr<osg::StateSet> old = new osg::StateSet(*(m_Root->getOrCreateStateSet()));
	while (m_Root->removeClipPlane(0U)) ;  // remove all clip planes
	float x_min = -0.5 * x;
	float y_min = -0.5 * y;
	m_Root->addClipPlane(new osg::ClipPlane(0,  1.0, 0.0,  0.0, -x_min));
	m_Root->addClipPlane(new osg::ClipPlane(1, -1.0, 0.0,  0.0, -x_min));
	m_Root->addClipPlane(new osg::ClipPlane(2,  0.0, 0.0,  1.0, -y_min));
	m_Root->addClipPlane(new osg::ClipPlane(3,  0.0, 0.0, -1.0, -y_min));
	if (old.valid()) {
		for (unsigned i = 0; i < 4; ++i) old->removeMode((osg::StateAttribute::GLMode)(GL_CLIP_PLANE0+i));
		m_Root->getOrCreateStateSet()->merge(*old);
	}
}

CSP_NAMESPACE_END


