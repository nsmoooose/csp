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
 * @file ScrollBar.cpp
 *
 **/

#include <csp/cspwf/ScrollBar.h>
#include <csp/cspwf/ControlGeometryBuilder.h>
#include <iostream>
#include <osg/Group>

namespace csp {

namespace wf {

ScrollButton::ScrollButton(const char* name) : Control(name) {}

void ScrollButton::performLayout() {
	// Make sure that all our child controls onInit() is called.
	Control::performLayout();

	ControlGeometryBuilder geometryBuilder;
	osg::ref_ptr<osg::Group> group = geometryBuilder.buildGenericControl(this);
	getNode()->addChild(group.get());
}

ScrollLeftButton::ScrollLeftButton() : ScrollButton("ScrollLeftButton") {}

ScrollRightButton::ScrollRightButton() : ScrollButton("ScrollRightButton") {}

ScrollUpButton::ScrollUpButton() : ScrollButton("ScrollUpButton") {}

ScrollDownButton::ScrollDownButton() : ScrollButton("ScrollDownButton") {}

ScrollBar::ScrollBar(std::string name) :
	Container(name), m_Value(0.0f), m_Minimum(0.0f),
	m_Maximum(10.0f) {
}

void ScrollBar::performLayout() {
	// Make sure that all our child controls onInit() is called.
	Container::performLayout();

	osg::Group* group = getNode();

	ControlGeometryBuilder geometryBuilder;
	osg::ref_ptr<osg::Group> geometry = geometryBuilder.buildGenericControl(this);
	group->addChild(geometry.get());

	ControlVector childControls = getChildControls();
	ControlVector::iterator control = childControls.begin();
	for(;control != childControls.end();++control) {
		(*control)->performLayout();
		group->addChild((*control)->getNode());
	}
}

float ScrollBar::getValue() const {
	return m_Value;
}

void ScrollBar::setValue(float value) {
	if(value == m_Value) {
		return;
	}

	if(value > m_Maximum) {
		value = m_Maximum;
	}

	if(value < m_Minimum) {
		value = m_Minimum;
	}

	ScrollEventArgs event(m_Value, value);
	m_Value = value;
	onScroll(event);
}

float ScrollBar::getMinimum() const {
	return m_Minimum;
}

void ScrollBar::setMinimum(float minimum) {
	m_Minimum = minimum;
}

float ScrollBar::getMaximum() const {
	return m_Maximum;
}

void ScrollBar::setMaximum(float maximum) {
	m_Maximum = maximum;
}

void ScrollBar::onScroll(ScrollEventArgs& event) {
	CSPLOG(DEBUG, APP) << "Signal Scroll on csp::wf::" << getName().c_str();
	Scroll(event);
}

void ScrollBar::handleClickToMin(ClickEventArgs& event) {
	if((getValue() - 1.0f) >= getMinimum()) {
		setValue(getValue() - 1.0f);
	}
	event.handled = true;
}

void ScrollBar::handleClickToMax(ClickEventArgs& event) {
	if((getValue() + 1.0f) <= getMaximum()) {
		setValue(getValue() + 1.0f);
	}
	event.handled = true;
}

HorizontalScrollBar::HorizontalScrollBar() :
	ScrollBar("HorizontalScrollBar"),
	m_ScrollLeftButton(new ScrollLeftButton),
	m_ScrollRightButton(new ScrollRightButton) {

	m_ScrollLeftButton->setParent(this);
	m_ScrollRightButton->setParent(this);

	m_ScrollLeftButton->Click.connect(sigc::mem_fun(this, &HorizontalScrollBar::handleClickToMin));
	m_ScrollRightButton->Click.connect(sigc::mem_fun(this, &HorizontalScrollBar::handleClickToMax));
}

void HorizontalScrollBar::layoutChildControls() {
	// Don't care about child containers.
}

ControlVector HorizontalScrollBar::getChildControls() {
	ControlVector childControls;
	childControls.push_back(m_ScrollLeftButton);
	childControls.push_back(m_ScrollRightButton);
	return childControls;
}

VerticalScrollBar::VerticalScrollBar() :
	ScrollBar("VerticalScrollBar"),
	m_ScrollUpButton(new ScrollUpButton),
	m_ScrollDownButton(new ScrollDownButton) {

	m_ScrollUpButton->setParent(this);
	m_ScrollDownButton->setParent(this);

	m_ScrollUpButton->Click.connect(sigc::mem_fun(this, &VerticalScrollBar::handleClickToMin));
	m_ScrollDownButton->Click.connect(sigc::mem_fun(this, &VerticalScrollBar::handleClickToMax));
}

void VerticalScrollBar::layoutChildControls() {
}

ControlVector VerticalScrollBar::getChildControls() {
	ControlVector childControls;
	childControls.push_back(m_ScrollUpButton);
	childControls.push_back(m_ScrollDownButton);
	return childControls;
}

} // namespace wf
} // namespace csp
