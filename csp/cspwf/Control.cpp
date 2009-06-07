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
#include <sstream>
#include <csp/cspwf/Control.h>
#include <csp/cspwf/ControlCallback.h>
#include <csp/cspwf/ControlGeometryBuilder.h>
#include <csp/cspwf/SignalData.h>
#include <csp/cspwf/StyleBuilder.h>
#include <csp/cspwf/Window.h>
#include <csp/csplib/util/Ref.h>

#include <osg/BlendFunc>
#include <osg/MatrixTransform>

namespace csp {

namespace wf {

Control::Control(std::string name) :
	m_Name(name),
	m_Enabled(true), m_TransformGroup(new osg::MatrixTransform),
	m_ZPos(1.0), m_Style(new Style), m_ClickSignal(new Signal)
{
	// Attach a control callback to the control. This makes it possible
	// for the window manager to find out what control exists on a specific
	// coordinate.
	osg::ref_ptr<ControlCallback> callback = new ControlCallback(this);
	m_TransformGroup->setUpdateCallback(callback.get());

	CSPLOG(DEBUG, APP) << "Creating csp::wf::" << name.c_str();
}

Control::~Control() {
	CSPLOG(DEBUG, APP) << "Destroing csp::wf::" << getName().c_str();
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
	return m_Name;
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

const Point Control::getLocation() const {
	Ref<Style> style = StyleBuilder::buildStyle(this);
	ControlGeometryBuilder builder;
	return builder.calculateLocation(this, style.get());
}

const Size Control::getSize() const {
	Ref<Style> style = StyleBuilder::buildStyle(this);
	ControlGeometryBuilder builder;
	return builder.calculateSize(this, style.get());
}

const Ref<Style> Control::getStyle() const {
	return m_Style;
}

Ref<Style> Control::getStyle() {
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
		Size parentSize = m_Parent->getSize();
		parentX = parentSize.width / 2;
		parentY = parentSize.height / 2;
	}

	// The control has been loaded. Lets reflect our properties with
	// our osg object.
	Size size = getSize();
	Point location = getLocation();
	m_TransformGroup->setMatrix(osg::Matrix::translate(location.x - parentX + (size.width / 2), location.y - parentY + (size.height / 2), m_ZPos));
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

	CSPLOG(DEBUG, APP) << "Signal Click on csp::wf::" << getName().c_str();
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
	CSPLOG(DEBUG, APP) << "Signal MouseHover on csp::wf::" << getName().c_str();
	Hover(event);
	if(event.handled == false) {
		Control* parent = getParent();
		if(parent != NULL) {
			parent->onHover(event);
		}
	}
}

void Control::onMouseMove(MouseEventArgs& event) {
	CSPLOG(DEBUG, APP) << "Signal MouseMove on csp::wf::" << getName().c_str();
	MouseMove(event);
	if(event.handled == false) {
		Control* parent = getParent();
		if(parent != NULL) {
			parent->onMouseMove(event);
		}
	}
}

void Control::onMouseDown(MouseButtonEventArgs& event) {
	CSPLOG(DEBUG, APP) << "Signal MouseDown on csp::wf::" << getName().c_str();
	MouseDown(event);
	if(event.handled == false) {
		Control* parent = getParent();
		if(parent != NULL) {
			parent->onMouseDown(event);
		}
	}
}

void Control::onMouseUp(MouseButtonEventArgs& event) {
	CSPLOG(DEBUG, APP) << "Signal MouseUp on csp::wf::" << getName().c_str();
	MouseUp(event);
	if(event.handled == false) {
		Control* parent = getParent();
		if(parent != NULL) {
			parent->onMouseUp(event);
		}
	}
}

void Control::suspendLayout() {
	// TODO: To be implemented...
}

void Control::resumeLayout() {
	// TODO: To be implemented...
}

void Control::performLayout() {
	if(m_Parent.valid()) {
		// Handle alignment of the control in the parents container.
		// Some containers will resize child controls. These containers
		// will alignment don't work.

		const Size controlSize = getSize();
		const int parentWidth = static_cast<int>(m_Parent->getClientRect().width());
		const int parentHeight = static_cast<int>(m_Parent->getClientRect().height());

		Ref<Style> controlStyle = StyleBuilder::buildStyle(this);
		Ref<Style> style = getStyle();
		if(controlStyle->getHorizontalAlign()) {
			if(*controlStyle->getHorizontalAlign() == "left") {
				style->setLeft(Style::UnitValue(Style::Pixels, 0));
			}
			else if(*controlStyle->horizontalAlign == "center") {
				style->setLeft(Style::UnitValue(Style::Pixels, (parentWidth / 2) - (controlSize.width / 2)));
			}
			else if(*controlStyle->horizontalAlign == "right") {
				style->setLeft(Style::UnitValue(Style::Pixels, parentWidth - controlSize.width));
			}
		}
		if(controlStyle->verticalAlign) {
			if(*controlStyle->verticalAlign == "top") {
				style->setTop(Style::UnitValue(Style::Pixels, 0));
			}
			else if(*controlStyle->verticalAlign == "middle") {
				style->setTop(Style::UnitValue(Style::Pixels, (parentHeight / 2) - (controlSize.height / 2)));
			}
			else if(*controlStyle->verticalAlign == "bottom") {
				style->setTop(Style::UnitValue(Style::Pixels, parentHeight - controlSize.height));
			}
		}
	}

	m_TransformGroup->removeChild(0, m_TransformGroup->getNumChildren());
	updateMatrix();
}

bool Control::layoutSuspended() {
	// TODO: To be implemented...
	return true;
}

} // namespace wf
} // namespace csp
