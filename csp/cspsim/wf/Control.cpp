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
 * @file Control.cpp
 *
 **/

#include <csp/cspsim/wf/Control.h>
#include <csp/csplib/util/Ref.h>

#include <osg/BlendFunc>
#include <osg/MatrixTransform>

CSP_NAMESPACE

namespace wf {

Control::Control() :
	m_ZPos(0.0), m_Parent(0), m_TransformGroup(new osg::MatrixTransform)
{
    osg::StateSet *stateSet = m_TransformGroup->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    stateSet->setAttributeAndModes(blendFunction.get());
}

Control::~Control() {
}

void Control::buildGeometry(WindowManager* manager) {
	ControlVector::iterator control = m_Controls.begin();
	for(;control != m_Controls.end();++control) {
		(*control)->buildGeometry(manager);
	}

	Control::ControlVector childControls = getControls();
	Control::ControlVector::iterator childControl = childControls.begin();
	for(;childControl != childControls.end();++childControl) {
		m_TransformGroup->addChild((*childControl)->getNode());
	}	
}

Control::ControlVector Control::getControls() {
	// We return a copy of the vector to prevent the caller from 
	// modifying the original vector. The caller shall be able to 
	// use any method on the returned controls so we cannot use a
	// const method...
	return m_Controls;
}

Control* Control::getParent() {
	return m_Parent;
}

void Control::addControl(Control* control) {
	m_Controls.push_back(control);
	control->m_Parent = this;
}

osg::Group* Control::getNode() {
	return m_TransformGroup.get();
}

float Control::getZPos() const {
	return m_ZPos;
}

void Control::setZPos(float zPos) {
	m_ZPos = zPos;
	m_TransformGroup->setMatrix(osg::Matrix::translate(m_Point.m_X, m_ZPos, m_Point.m_Y));
}

const Point& Control::getLocation() const {
	return m_Point;
}

void Control::setLocation(const Point& point) {
	m_Point = point;
	m_TransformGroup->setMatrix(osg::Matrix::translate(m_Point.m_X, m_ZPos, m_Point.m_Y));
}

const Size& Control::getSize() const {
	return m_Size;
}

void Control::setSize(const Size& size) {
	m_Size = size;
}

} // namespace wf

CSP_NAMESPACE_END
