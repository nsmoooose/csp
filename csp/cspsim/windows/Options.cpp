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
#include <csp/csplib/util/FileUtility.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/CheckBox.h>
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/WindowManager.h>
#include <csp/cspsim/windows/MenuScreen.h>
#include <csp/cspsim/windows/MessageBox.h>
#include <csp/cspsim/windows/Options.h>

CSP_NAMESPACE

namespace windows {

Options::Options() {
	Ref<wf::Serialization> serializer = new wf::Serialization(getUIPath());
	serializer->load(this, getUITheme(), "options.xml");
	
	Ref<wf::Button> okButton = getById<wf::Button>("ok");
	if(okButton.valid()) {
		okButton->Click.connect(sigc::mem_fun(*this, &Options::ok_Click));
	}

	Ref<wf::Button> cancelButton = getById<wf::Button>("cancel");
	if(cancelButton.valid()) {
		cancelButton->Click.connect(sigc::mem_fun(*this, &Options::cancel_Click));
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

	// Find the theme list box control.
	Ref<wf::ListBox> themeListBox = getById<wf::ListBox>("theme");
	if(themeListBox.valid()) {
		// First we start by enumerating all the themes available to the
		// user.
		std::string currentTheme = getUITheme();
		
		// Build the path to the themes directory and get all the content in that 
		// directory.
		std::string themesPath = ospath::join(getUIPath(), "themes");
		ospath::DirectoryContents directoryContents = ospath::getDirectoryContents(themesPath);
		
		// Foreach subdirectory we add an item to the listbox with the 
		// name of the theme.
		ospath::DirectoryContents::const_iterator directoryEntry = directoryContents.begin();
		for(;directoryEntry != directoryContents.end();++directoryEntry) {
			// Uggly hack to skip files with a files and the
			// . and .. directories.
			if(directoryEntry->find(".") != std::string::npos) {
				continue;
			}
			
			Ref<wf::ListBoxItem> item = new wf::ListBoxItem(*directoryEntry);
			themeListBox->addItem(item.get());
			// TODO: Remove this row when we can set height through css.	
			item->setSize(wf::Size(0, 30));
			
			// If the theme is the same as the current we select it in the
			// list box.
			if(currentTheme == *directoryEntry) {
				themeListBox->setSelectedItem(item.get());
			}
		}
	}

	// Check / uncheck the checkbox with the current value.
	Ref<wf::CheckBox> fullScreen = getById<wf::CheckBox>("fullscreen");
	if(fullScreen.valid()) {
		fullScreen->setChecked(screenSettings.fullScreen);
	}
	
	// Handle different language settings.	
	Ref<wf::ListBox> languageListBox = getById<wf::ListBox>("language");
	if(languageListBox.valid()) {
		std::string currentLanguage = getUILanguage();
		
		std::string localizationPath = ospath::join(getUIPath(), "localization");
		ospath::DirectoryContents directoryContents = ospath::getDirectoryContents(localizationPath);
		
		ospath::DirectoryContents::const_iterator directoryEntry = directoryContents.begin();
		for(;directoryEntry != directoryContents.end();++directoryEntry) {
			if(directoryEntry->find(".") != std::string::npos) {
				continue;
			}
			
			Ref<wf::ListBoxItem> item = new wf::ListBoxItem(*directoryEntry);
			languageListBox->addItem(item.get());
			
			// TODO: Remove this row when we can set height through css.	
			item->setSize(wf::Size(0, 30));
			
			// If the language is the same as the current we select it in the
			// list box.
			if(currentLanguage == *directoryEntry) {
				languageListBox->setSelectedItem(item.get());
			}
		}
	}
}

void Options::ok_Click(wf::ClickEventArgs& event) {
	event.handled = true;
	
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
	
	// Store the current language of the application.
	Ref<wf::ListBox> language = getById<wf::ListBox>("language");
	if(language.valid()) {
		Ref<wf::ListBoxItem> selectedItem = language->getSelectedItem();
		if(selectedItem.valid()) {
			std::string selectedText = selectedItem->getText();
			setUILanguage(selectedText);
		}
	}
	
	// Store the new theme if it has changed.
	Ref<wf::ListBox> themeListBox = getById<wf::ListBox>("theme");
	if(themeListBox.valid()) {
		Ref<wf::ListBoxItem> selectedItem = themeListBox->getSelectedItem();
		if(selectedItem.valid()) {
			setUITheme(selectedItem->getText());
		}
	}
	
	wf::WindowManager* windowManager = getWindowManager();

	// Well we need to close all windows to make the theme change
	// to be in effect.
	MenuScreen* menuScreen = dynamic_cast<MenuScreen*>(CSPSim::theSim->getCurrentScreen());
	if(menuScreen != NULL) {
		menuScreen->displayDesktopAndMainMenu();
	}
	
	// Display a message box that states that you will need to restart the 
	// simulator.
	Ref<MessageBox> messageBox = MessageBox::Show("Information", 
		"Changes will not take effect\nuntil you restart the simulator!");
	windowManager->show(messageBox.get());
	messageBox->centerWindow();

	// We only need to close this window if it hasn't been already.
	if(menuScreen == NULL) {
		// Closes this window.
		close();
	}
}

void Options::cancel_Click(wf::ClickEventArgs& event) {
	event.handled = true;
	close();
}

Options::~Options() {
}

} // namespace windows

CSP_NAMESPACE_END
