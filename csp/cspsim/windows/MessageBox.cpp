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
 * @file MessageBox.cpp
 *
 **/

#include <csp/cspsim/Config.h>
#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/windows/MessageBox.h>

#include <osg/Group>

CSP_NAMESPACE

namespace windows {

MessageBox::MessageBox(std::string caption, std::string message) {
	Ref<wf::Serialization> serializer = new wf::Serialization(getUIPath());
	serializer->load(this, getUITheme(), "messagebox.xml");
	
	Ref<wf::Button> okButton = getById<wf::Button>("ok");
	if(okButton.valid()) {
		okButton->Click.connect(sigc::mem_fun(*this, &MessageBox::ok_Click));
	}
	
	Ref<wf::Label> captionLabel = getById<wf::Label>("caption");
	if(captionLabel.valid()) {
		captionLabel->setText(caption);
	}
	
	Ref<wf::Label> messageLabel = getById<wf::Label>("message");
	if(messageLabel.valid()) {
		messageLabel->setText(message);
	}
}

MessageBox::~MessageBox() {
}

void MessageBox::ok_Click(wf::ClickEventArgs& event) {
	event.handled = true;
	
	close();
}

Ref<MessageBox> MessageBox::Show(std::string caption, std::string message) {
	Ref<MessageBox> messageBox = new MessageBox(caption, message);
	return messageBox;
}

} // namespace wf

CSP_NAMESPACE_END
