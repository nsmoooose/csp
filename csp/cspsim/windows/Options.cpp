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
 * @file Options.cpp
 *
 **/

#include <sstream>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/CheckBox.h>
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/MessageBox.h>
#include <csp/cspsim/wf/WindowManager.h>
#include <csp/cspsim/windows/Options.h>

CSP_NAMESPACE

namespace windows {

Options::Options() {
	Ref<wf::Serialization> serializer = new wf::Serialization(getUIPath());
	serializer->load(this, "default", "options.xml");
	
	Ref<wf::Button> okButton = getById<wf::Button>("ok");
	if(okButton.valid()) {
		okButton->addButtonClickedHandler(sigc::mem_fun(*this, &Options::ok_Click));
	}

	Ref<wf::Button> cancelButton = getById<wf::Button>("cancel");
	if(cancelButton.valid()) {
		cancelButton->addButtonClickedHandler(sigc::mem_fun(*this, &Options::cancel_Click));
	}

	// Get the current screen settings.
	const ScreenSettings screenSettings = getScreenSettings();

	// Find the listbox control if there is any.
	Ref<wf::ListBox> resolution = getById<wf::ListBox>("resolution");	
	if(resolution.valid()) {
		// Format a string in the same format as we display resolutions.
		std::stringstream textStream;
		textStream << screenSettings.width << 'x' << screenSettings.height;
		std::string itemText = textStream.str();
	
		// Iterate all our resolution items and set the selected item
		// property on our current resolution.
		wf::ListBoxItemVector items = resolution->getItems();
		wf::ListBoxItemVector::iterator item = items.begin();
		for(;item != items.end();++item) {
			if((*item)->getText() == itemText) {
				resolution->setSelectedItem(item->get());
				break;
			}
		}
	}

	// Check / uncheck the checkbox with the current value.
	Ref<wf::CheckBox> fullScreen = getById<wf::CheckBox>("fullscreen");
	if(fullScreen.valid()) {
		fullScreen->setChecked(screenSettings.fullScreen);
	}
}

void Options::ok_Click() {
	// Read current display settings.
	ScreenSettings screenSettings = getScreenSettings();
	
	// If we find our list box with resolutions then we retreives
	// the resolution and stores it.
	Ref<wf::ListBox> resolution = getById<wf::ListBox>("resolution");
	if(resolution.valid()) {
		Ref<wf::ListBoxItem> selectedItem = resolution->getSelectedItem();
		if(selectedItem.valid()) {
			// Perhaps not the most elegant solution. But it works...
			// We take the string that is in the listbox and splits it where the 'x' is.
			std::string selectedText = selectedItem->getText();
			std::string::size_type position = selectedText.find('x');
			if(position != std::string::npos) {
				screenSettings.width = atoi(selectedText.substr(0, position).c_str());
				screenSettings.height = atoi(selectedText.substr(position+1).c_str());				
			}
		}
	}

	// If there is a check box we read the current value and store it.
	Ref<wf::CheckBox> fullScreen = getById<wf::CheckBox>("fullscreen");
	if(fullScreen.valid()) {
		screenSettings.fullScreen = fullScreen->getChecked();
	}

	// Store the new settings.
	setScreenSettings(screenSettings);
	
	// Display a message box that states that you will need to restart the 
	// simulator.
	Ref<wf::MessageBox> messageBox = wf::MessageBox::Show("Information", 
		"Changes will not take effect\nuntil you restart the simulator!");
	getWindowManager()->show(messageBox.get());

	// Closes this window.
	close();
}

void Options::cancel_Click() {
	close();
}

Options::~Options() {
}

} // namespace windows

CSP_NAMESPACE_END
