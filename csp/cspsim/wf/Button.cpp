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

Button::Button(Theme* theme) : SingleControlContainer(theme), m_ChildControl(new Label(theme)) {
	Label* label = (Label*)m_ChildControl.get();
	label->setAlignment(osgText::Text::CENTER_CENTER);
}

Button::Button(Theme* theme, const std::string text) 
	: SingleControlContainer(theme), m_ChildControl(new Label(theme, text)), m_text(text) {
	Label* label = (Label*)m_ChildControl.get();
	label->setAlignment(osgText::Text::CENTER_CENTER);
}

Button::~Button() {
}

void Button::onLoad() {
	setText(m_text);
	SingleControlContainer::onLoad();
}

void Button::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	SingleControlContainer::buildGeometry();

	// Build our own button control and add it to the group.
	osg::ref_ptr<osg::Group> button = getTheme()->buildButton(this);
	osg::ref_ptr<ButtonClickedCallback> callback = new ButtonClickedCallback(m_ButtonClicked, this);
	button->setUpdateCallback(callback.get());
	getNode()->addChild(button.get());	
	
	if(m_ChildControl.valid()) {
		m_ChildControl->buildGeometry();
		
		osg::ref_ptr<ButtonClickedCallback> callback = new ButtonClickedCallback(m_ButtonClicked, this);
		
		osg::ref_ptr<osg::Group> childControl = m_ChildControl->getNode();	
		childControl->setUpdateCallback(callback.get());
		getNode()->addChild(childControl.get());			
	}
}

const std::string Button::getText() const {
	return m_text;
}

void Button::setText(const std::string& text) {
	m_text = text;
	Label* label = dynamic_cast<Label*>(m_ChildControl.get());
	if(label != NULL) {
		label->setText(text);
	}
}

Control* Button::getControl() {
	return m_ChildControl.get();
}

void Button::setControl(Control* control) {
	m_ChildControl = control;
}

void Button::addButtonClickedHandler(const sigc::slot<void> &handler) {
	m_ButtonClicked.connect(handler);
}

} // namespace wf

CSP_NAMESPACE_END
