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
 * @file SingleControlContainer.cpp
 *
 **/

#include <csp/cspwf/SingleControlContainer.h>
#include <osg/Group>

namespace csp {

namespace wf {

SingleControlContainer::SingleControlContainer() : Container("SingleControlContainer") {
}

SingleControlContainer::SingleControlContainer(std::string name) : Container(name) {
}

SingleControlContainer::~SingleControlContainer() {
}

ControlVector SingleControlContainer::getChildControls() {
	ControlVector controls;
	if(m_Control.valid()) {
		controls.push_back(m_Control);
	}
	return controls;
}

void SingleControlContainer::performLayout() {
	Control::performLayout();
	if(m_Control.valid()) {
		m_Control->performLayout();
		getNode()->addChild(m_Control->getNode());
	}
}

void SingleControlContainer::setControl(Control* childControl) {
	m_Control = childControl;
	childControl->setParent(this);
}

Control* SingleControlContainer::getControl() {
	return m_Control.get();
}

void SingleControlContainer::layoutChildControls() {
	if(m_Control.valid()) {
		Ref<Style> style = m_Control->getStyle();
		Size size = getSize();
		style->setLeft(Style::UnitValue(Style::Pixels, 0));
		style->setTop(Style::UnitValue(Style::Pixels, 0));
		style->setHeight(Style::UnitValue(Style::Pixels, size.height));
		style->setWidth(Style::UnitValue(Style::Pixels, size.width)); 
		Container* container = dynamic_cast<Container*>(m_Control.get());
		if(container != NULL) {
			container->layoutChildControls();
		}
	}
}

} // namespace wf

} // namespace csp
