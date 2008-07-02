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
 * @file Container.cpp
 *
 **/

#include <csp/cspwf/Container.h>
#include <csp/cspwf/StyleBuilder.h>

namespace csp {

namespace wf {

Container::Container(std::string name) : Control(name) {
}

Container::~Container() {
}

Control* Container::getById(const std::string& id) {
	// Enumerate all child controls to see if we can find the control.
	ControlVector childControls = getChildControls();
	ControlVector::iterator control = childControls.begin();
	for(;control != childControls.end();++control) {
		if(!control->valid()) {
			continue;
		}
		if((*control)->getId() == id) {
			return control->get();
		}
		
		// If the child control is a container of new controls we
		// need to check those child controls also.
		Container* childContainer = dynamic_cast<Container*>(control->get());
		if(childContainer != NULL) {
			// Yes. The child control is a container. Check if the control is found
			// there.
			Control* childControl = childContainer->getById(id);
			if(childControl != NULL) {
				// Yes the child control is found. Lets return it to the 
				// caller.
				return childControl;
			}
		}
	}
	// No control is found. 
	return NULL;
}

Rectangle Container::getClientRect() const {
	Ref<Style> style = StyleBuilder::buildStyle(this);
		
	Rectangle clientArea;
	clientArea.x0 = style->getBorderWidth() ? *style->getBorderWidth() : 0;
	if(style->getBorderLeftWidth()) {
		clientArea.x0 = *style->getBorderLeftWidth();
	}
	
	clientArea.y0 = style->getBorderWidth() ? *style->getBorderWidth() : 0;
	if(style->getBorderTopWidth()) {
		clientArea.y0 = *style->getBorderTopWidth();
	}
	
	clientArea.x1 = getSize().width;
	if(style->getBorderWidth() && !style->getBorderRightWidth()) {
		clientArea.x1 -= *style->getBorderWidth();
	}
	if(style->getBorderRightWidth()) {
		clientArea.x1 -= *style->getBorderRightWidth();
	}
	
	clientArea.y1 = getSize().height;
	if(style->getBorderWidth() && !style->getBorderBottomWidth()) {
		clientArea.y1 -= *style->getBorderWidth();
	}
	if(style->getBorderBottomWidth()) {
		clientArea.y1 -= *style->getBorderBottomWidth();
	}
	
	return clientArea;	
}

} // namespace wf

} // namespace csp
