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
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/Theme.h>
#include <csp/cspsim/wf/WindowManager.h>

#include <osg/Group>

CSP_NAMESPACE

namespace wf {

class Button::ButtonClickedCallback : public AnimationCallback {
public:
	ButtonClickedCallback(ButtonClickedSignal& signal, Button* button) : m_ButtonClicked(signal), m_Button(button) {}
	virtual ~ButtonClickedCallback() {}
	virtual bool pick(int /*flags*/) {
		m_ButtonClicked();
		return true;
	}
	
private:
	ButtonClickedSignal& m_ButtonClicked;
	Button* m_Button;
};

Button::Button(Theme* theme) : SingleControlContainer(theme) {
	Ref<Label> label = new Label(theme);
	label->setAlignment(osgText::Text::CENTER_CENTER);
	setControl(label.get());
}

Button::Button(Theme* theme, const std::string text) 
	: SingleControlContainer(theme), m_text(text) {

	Ref<Label> label = new Label(theme);
	label->setAlignment(osgText::Text::CENTER_CENTER);
	setControl(label.get());
}

Button::~Button() {
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
	osg::ref_ptr<osg::Group> button = getTheme()->buildButton(this);
	osg::ref_ptr<ButtonClickedCallback> callback = new ButtonClickedCallback(m_ButtonClicked, this);
	if(button->getUpdateCallback() == NULL) {
		button->setUpdateCallback(callback.get());
	}
	getNode()->addChild(button.get());		
	
	// Bind the update callback to the child control also.
	Control* childControl = getControl();
	if(childControl != NULL) {	
		childControl->getNode()->setUpdateCallback(callback.get());
	}
}

const std::string Button::getText() const {
	return m_text;
}

void Button::setText(const std::string& text) {
	m_text = text;
	buildGeometry();
}

void Button::addButtonClickedHandler(const sigc::slot<void> &handler) {
	m_ButtonClicked.connect(handler);
}

} // namespace wf

CSP_NAMESPACE_END
