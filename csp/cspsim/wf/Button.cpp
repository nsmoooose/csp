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

#include <csp/cspsim/Animation.h>
#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/ControlGeometryBuilder.h>
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/WindowManager.h>

#include <osg/Group>

CSP_NAMESPACE

namespace wf {

Button::Button() : SingleControlContainer() {
	Ref<Label> label = new Label();
	label->setAlignment(osgText::Text::CENTER_CENTER);
	setControl(label.get());
}

Button::Button(const std::string text) 
	: SingleControlContainer(), m_text(text) {

	Ref<Label> label = new Label();
	label->setAlignment(osgText::Text::CENTER_CENTER);
	setControl(label.get());
}

Button::~Button() {
}

std::string Button::getName() const {
	return "Button";
}

void Button::buildGeometry() {
	// If we have a label as a child control then we set the default text on it.
	Label* label = dynamic_cast<Label*>(getControl());
	if(label != NULL) {
		label->setText(m_text);
	}

	// Make sure that all our child controls onInit() is called.
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

CSP_NAMESPACE_END
