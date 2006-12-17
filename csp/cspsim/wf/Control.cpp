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
#include <csp/cspsim/wf/Theme.h>
#include <csp/csplib/util/Ref.h>

#include <osg/BlendFunc>
#include <osg/MatrixTransform>

CSP_NAMESPACE

namespace wf {

Control::Control(Theme* theme) :
	m_Theme(theme), m_ZPos(0.0), m_Parent(0), m_TransformGroup(new osg::MatrixTransform)
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

const std::string& Control::getId() const {
	return m_Id;
}

void Control::setId(const std::string& id) {
	m_Id = id;
}

void Control::buildGeometry() {
	m_TransformGroup->removeChild(0, m_TransformGroup->getNumChildren());
	
	// The control has been loaded. Lets reflect our properties with
	// our osg object.
	m_TransformGroup->setMatrix(osg::Matrix::translate(m_Point.m_X, m_ZPos, m_Point.m_Y));	
}

Theme* Control::getTheme() {
	return m_Theme.get();
}

Control* Control::getParent() {
	return m_Parent;
}

void Control::setParent(Control* parent) {
	m_Parent = parent;
}

WindowManager* Control::getWindowManager() {
	if(m_Parent == NULL)
		return NULL;
		
	return m_Parent->getWindowManager();
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
