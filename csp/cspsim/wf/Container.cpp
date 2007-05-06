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

#include <csp/cspsim/wf/Container.h>
#include <csp/cspsim/wf/StyleBuilder.h>

CSP_NAMESPACE

namespace wf {

Container::Container() {
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
	Style style = StyleBuilder::buildStyle(this);
		
	Rectangle clientArea;
	clientArea.x0 = style.borderWidth ? *style.borderWidth : 0;
	if(style.borderLeftWidth) {
		clientArea.x0 = *style.borderLeftWidth;
	}
	
	clientArea.y0 = style.borderWidth ? *style.borderWidth : 0;
	if(style.borderTopWidth) {
		clientArea.y0 = *style.borderTopWidth;
	}
	
	clientArea.x1 = getSize().width;
	if(style.borderWidth && !style.borderRightWidth) {
		clientArea.x1 -= *style.borderWidth;
	}
	if(style.borderRightWidth) {
		clientArea.x1 -= *style.borderRightWidth;
	}
	
	clientArea.y1 = getSize().height;
	if(style.borderWidth && !style.borderBottomWidth) {
		clientArea.y1 -= *style.borderWidth;
	}
	if(style.borderBottomWidth) {
		clientArea.y1 -= *style.borderBottomWidth;
	}
	
	return clientArea;	
}

} // namespace wf

CSP_NAMESPACE_END
