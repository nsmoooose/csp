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
 * @file Window.cpp
 *
 **/

#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/MessageBox.h>
#include <csp/cspsim/wf/MultiControlContainer.h>
#include <csp/cspsim/wf/Theme.h>

#include <osg/Group>

CSP_NAMESPACE

namespace wf {

/*
	TODO:
	* Should be able to resize window according to the length of the text message to display.
	* Nice with a yes/no/cancel buttons?
	* Bitmap with an icon according to warning / information / error etc.
 */

MessageBox::MessageBox(Theme* theme, std::string caption, std::string message) : Window(theme, caption), m_Label(new Label(theme, message)) {
	setSize(Size(140.0f, 45.0f));
	
	Ref<MultiControlContainer> container = new MultiControlContainer(theme);
	setControl(container.get());
	
	Ref<Button> button = new Button(theme, "OK");
	button->setSize(Size(20.0f, 10.0f));
	button->setLocation(Point(0.0f, -10.0f));
	button->addButtonClickedHandler(sigc::mem_fun(*this, &MessageBox::onOk));
	container->addControl(button.get());

	m_Label->setAlignment(osgText::Text::CENTER_CENTER);
	m_Label->setLocation(Point(0.0f, 5.0f));
	m_Label->setSize(Size(140.0f, 20.0f));
	container->addControl(m_Label.get());
}

MessageBox::~MessageBox() {
}

void MessageBox::buildGeometry() {
	Window::buildGeometry();
}

std::string MessageBox::getMessage() const {
	return m_Label->getText();
}

void MessageBox::setMessage(std::string message) {
	m_Label->setText(message);
}

void MessageBox::onOk() {
	close();
}

Ref<MessageBox> MessageBox::Show(Theme* theme, std::string caption, std::string message) {
	Ref<MessageBox> messageBox = new MessageBox(theme, caption, message);
	return messageBox;
}

} // namespace wf

CSP_NAMESPACE_END
