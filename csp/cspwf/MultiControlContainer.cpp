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
 * @file MultiControlContainer.cpp
 *
 **/

#include <csp/cspwf/MultiControlContainer.h>
#include <osg/Group>

namespace csp {
namespace wf {

MultiControlContainer::MultiControlContainer() : 
	Container("MultiControlContainer") {
}

MultiControlContainer::~MultiControlContainer() {
}

ControlVector MultiControlContainer::getChildControls() {
	return m_Controls;
}

void MultiControlContainer::performLayout() {
	Control::performLayout();

	ControlVector::iterator control = m_Controls.begin();
	for(;control != m_Controls.end();++control) {
		(*control)->performLayout();
	}

	osg::Group* group = getNode();
	ControlVector childControls = getControls();
	ControlVector::iterator childControl = childControls.begin();
	for(;childControl != childControls.end();++childControl) {
		group->addChild((*childControl)->getNode());
	}
}

void MultiControlContainer::layoutChildControls() {
	ControlVector::iterator control = m_Controls.begin();
	for(;control != m_Controls.end();++control) {
		// If the child control is a container then we 
		// need to forward this call to its children.
		Container* childContainer = dynamic_cast<Container*>(control->get());
		if(childContainer != NULL) {
			childContainer->layoutChildControls();
		}
	}	
}

ControlVector MultiControlContainer::getControls() {
	return m_Controls;
}

void MultiControlContainer::addControl(Control* control) {
	m_Controls.push_back(control);
	control->setParent(this);
}

void MultiControlContainer::removeControl(Control* control) {
	ControlVector::iterator itControl = m_Controls.begin();
	for(;itControl != m_Controls.end();++itControl) {
		if(itControl->get() == control) {
			m_Controls.erase(itControl);
			control->setParent(NULL);
			return;
		}
	}
}

} // namespace wf
} // namespace csp
