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
#include <csp/cspsim/wf/SignalData.h>
#include <csp/cspsim/wf/StyleBuilder.h>
#include <csp/cspsim/wf/Window.h>
#include <csp/csplib/util/Ref.h>

#include <osg/BlendFunc>
#include <osg/MatrixTransform>

CSP_NAMESPACE

namespace wf {

Control::Control() :
	m_Enabled(true), m_TransformGroup(new osg::MatrixTransform), m_ZPos(1.0), m_ClickSignal(new Signal)
{
	// Attach a control callback to the control. This makes it possible
	// for the window manager to find out what control exists on a specific
	// coordinate.
	osg::ref_ptr<ControlCallback> callback = new ControlCallback(this);
	m_TransformGroup->setUpdateCallback(callback.get());
}

Control::~Control() {
}

void Control::dispose() {
	if(m_ClickSignal.valid()) {
		m_ClickSignal->dispose();
		m_ClickSignal = NULL;
	}
}

std::string Control::getId() const {
	return m_Id;
}

void Control::setId(const std::string& id) {
	m_Id = id;
}

std::string Control::getName() const {
	return "Control";
}

void Control::buildGeometry() {
	if(m_Parent.valid()) {
		// Handle alignment of the control in the parents container.
		// Some containers will resize child controls. These containers
		// will alignment don't work.

		Point controlLocation = getLocation();
		const Size controlSize = getSize();
		const int parentWidth = static_cast<int>(m_Parent->getClientRect().width());
		const int parentHeight = static_cast<int>(m_Parent->getClientRect().height());
		 
		Style controlStyle = StyleBuilder::buildStyle(this);
		if(controlStyle.horizontalAlign) {
			if(*controlStyle.horizontalAlign == "left") {
				controlLocation.x = 0;
			}
			else if(*controlStyle.horizontalAlign == "center") {
				controlLocation.x = (parentWidth / 2) - (controlSize.width / 2);
			}
			else if(*controlStyle.horizontalAlign == "right") {
				controlLocation.x = parentWidth - controlSize.width;
			}
		} 
		if(controlStyle.verticalAlign) {
			if(*controlStyle.verticalAlign == "top") {
				controlLocation.y = 0;
			}
			else if(*controlStyle.verticalAlign == "middle") {
				controlLocation.y = (parentHeight / 2) - (controlSize.height / 2);
			}
			else if(*controlStyle.verticalAlign == "bottom") {
				controlLocation.y = parentHeight - controlSize.height;
			}
		}
		setLocation(controlLocation);
	}



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

bool Control::getEnabled() const {
	// If this control is disabled then just return disabled.
	if(!m_Enabled) {
		return false;
	}

	// This control seems to be enabled. But this settings is inherited
	// from the parent control. So if we have a valid parent control
	// lets return the parents enabled property.
	if(m_Parent.valid()) {
		return m_Parent->getEnabled();
	}

	// No parent. Just return the value we got.
	return m_Enabled;
}

void Control::setEnabled(bool enabled) {
	if(m_Enabled == enabled) {
		return;
	}
	m_Enabled = enabled;
}

void Control::addState(const std::string& state) {
	m_States.insert(state);
}

void Control::removeState(const std::string& state) {
	m_States.erase(state);
}

std::string Control::getState() const {
	// Make a local copy of the state. We wish to add all
	// state that is inherited from the parent control. 
	StateSet copy = m_States;
	if(!getEnabled()) {
		copy.insert("disabled");
	}

	StateSet::const_iterator state = copy.begin();
	std::string statesToReturn;
	for(;state != copy.end();++state) {
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

Signal* Control::getClickSignal() {
	return m_ClickSignal.get();
}

void Control::onClick(ClickEventArgs& event) {
	// Only fire the click event if this control is enabled.
	if(!m_Enabled) {
		// Not enabled. Lets return.
		return;
	}

	Click(event);
	if(event.handled == false) {
		Control* parent = getParent();
		if(parent != NULL) {
			parent->onClick(event);
		}
	}

	Ref<SignalData> data = new SignalData();
	m_ClickSignal->emit(data.get());
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
