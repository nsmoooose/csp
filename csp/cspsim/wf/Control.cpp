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
#include <csp/cspsim/wf/ControlCallback.h>
#include <csp/cspsim/wf/ControlGeometryBuilder.h>
#include <csp/cspsim/wf/Window.h>
#include <csp/csplib/util/Ref.h>

#include <osg/BlendFunc>
#include <osg/MatrixTransform>

CSP_NAMESPACE

namespace wf {

Control::Control() :
	m_ZPos(1.0), m_TransformGroup(new osg::MatrixTransform)
{
    osg::StateSet *stateSet = m_TransformGroup->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    stateSet->setAttributeAndModes(blendFunction.get());
    
    // Attach a control callback to the control. This makes it possible 
	// for the window manager to find out what control exists on a specific
	// coordinate.
    osg::ref_ptr<ControlCallback> callback = new ControlCallback(this);
	m_TransformGroup->setUpdateCallback(callback.get());
}

Control::~Control() {
}

const std::string& Control::getId() const {
	return m_Id;
}

void Control::setId(const std::string& id) {
	m_Id = id;
}

std::string Control::getName() const {
	return "Control";
}

void Control::buildGeometry() {
	m_TransformGroup->removeChild(0, m_TransformGroup->getNumChildren());
	updateMatrix();	
}

Container* Control::getParent() {
	return m_Parent.get();
}

const Container* Control::getParent() const {
	return m_Parent.get();
}

void Control::setParent(Container* parent) {
	m_Parent = parent;
}

WindowManager* Control::getWindowManager() {
	if(!m_Parent.valid())
		return NULL;
		
	return m_Parent->getWindowManager();
}

optional<std::string> Control::getCssClass() const {
	return m_CssClass;
}

void Control::setCssClass(const optional<std::string>& cssClass) {
	m_CssClass = cssClass;
}

osg::Group* Control::getNode() {
	return m_TransformGroup.get();
}

float Control::getZPos() const {
	return m_ZPos;
}

void Control::setZPos(float zPos) {
	m_ZPos = zPos;
	updateMatrix();
}

const Point& Control::getLocation() const {
	return m_Point;
}

void Control::setLocation(const Point& point) {
	m_Point = point;
	updateMatrix();
}

const Size& Control::getSize() const {
	return m_Size;
}

void Control::setSize(const Size& size) {
	m_Size = size;
	updateMatrix();
}

const Style& Control::getStyle() const { 
	return m_Style; 
}

Style& Control::getStyle() { 
	return m_Style; 
}

void Control::addState(const std::string& state) {
	m_States.insert(state);
}

void Control::removeState(const std::string& state) {
	m_States.erase(state);
}

std::string Control::getState() const {
	StateSet::const_iterator state = m_States.begin();
	std::string statesToReturn;
	for(;state != m_States.end();++state) {
		statesToReturn += ":" + *state;
	}
	return statesToReturn;
}

void Control::updateMatrix() {
	double parentX = 0, parentY = 0;
	if(m_Parent.valid()) {
		parentX = m_Parent->m_Size.width / 2;
		parentY = m_Parent->m_Size.height / 2;
	}
	
	// The control has been loaded. Lets reflect our properties with
	// our osg object.
	m_TransformGroup->setMatrix(osg::Matrix::translate(m_Point.x - parentX + (m_Size.width / 2), m_Point.y - parentY + (m_Size.height / 2), m_ZPos));	
}

void Control::onClick(ClickEventArgs& event) {
	Click(event);
	if(event.handled == false) {
		Control* parent = getParent();
		if(parent != NULL) {
			parent->onClick(event);
		}
	}
}

void Control::onHover(HoverEventArgs& event) {
	Hover(event);
	if(event.handled == false) {
		Control* parent = getParent();
		if(parent != NULL) {
			parent->onHover(event);
		}
	}
}

} // namespace wf

CSP_NAMESPACE_END
