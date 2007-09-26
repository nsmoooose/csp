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

#include <csp/cspsim/wf/ScrollBar.h>
#include <csp/cspsim/wf/ControlGeometryBuilder.h>

#include <osg/Group>

CSP_NAMESPACE

namespace wf {

ScrollBar::ScrollBar(std::string name) : 
	Container(name), m_Value(0.0f), m_Minimum(0.0f), 
	m_Maximum(10.0f) {
}

float ScrollBar::getValue() const {
	return m_Value;
}

void ScrollBar::setValue(float value) {
	m_Value = value;
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

VerticalScrollBar::VerticalScrollBar() : 
	ScrollBar("VerticalScrollBar"), 
	m_ScrollUpButton(new ScrollUpButton),
	m_ScrollDownButton(new ScrollDownButton) {
}

HorizontalScrollBar::HorizontalScrollBar() : 
	ScrollBar("HorizontalScrollBar"),
	m_ScrollLeftButton(new ScrollLeftButton),
	m_ScrollRightButton(new ScrollRightButton) {
}

void VerticalScrollBar::layoutChildControls() {
}

ControlVector VerticalScrollBar::getChildControls() {
	ControlVector childControls;
	childControls.push_back(m_ScrollUpButton);
	childControls.push_back(m_ScrollDownButton);
	return childControls;
}

void HorizontalScrollBar::layoutChildControls() {
}

ControlVector HorizontalScrollBar::getChildControls() {
	ControlVector childControls;
	childControls.push_back(m_ScrollLeftButton);
	childControls.push_back(m_ScrollRightButton);
	return childControls;
}

} // namespace wf

CSP_NAMESPACE_END
