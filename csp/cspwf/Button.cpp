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
 * @file Button.cpp
 *
 **/

#include <csp/cspwf/Button.h>
#include <csp/cspwf/ControlGeometryBuilder.h>
#include <csp/cspwf/Label.h>
#include <csp/cspwf/WindowManager.h>

#include <osg/Group>

namespace csp {
namespace wf {

Button::Button() : SingleControlContainer("Button") {
}

Button::~Button() {
}

void Button::buildGeometry() {
	SingleControlContainer::buildGeometry();

	// Build our own button control and add it to the group.
	ControlGeometryBuilder geometryBuilder;
	osg::ref_ptr<osg::Group> button = geometryBuilder.buildButton(this);

	// When the button is invisible there will not be any geometry created.
	if(button.valid()) {
		getNode()->addChild(button.get());		
	}
}

const std::string Button::getText() const {
	return m_text;
}

void Button::setText(const std::string& text) {
	m_text = text;
	buildGeometry();
}

} // namespace wf
} // namespace csp
