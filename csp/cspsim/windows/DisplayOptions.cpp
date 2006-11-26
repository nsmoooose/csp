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
 * @file DisplayOptions.cpp
 *
 **/

#include <csp/cspsim/Config.h>
#include <csp/cspsim/wf/CheckBox.h>
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/TableControlContainer.h>
#include <csp/cspsim/windows/DisplayOptions.h>

#include <sstream>
#include <osg/Vec4>

CSP_NAMESPACE

/*
	TODO:
		* It would be nice if we could change display settings without restarting
		  of the simulation.
		* Enumerate supported screen resolution from your graphics card.
*/

namespace windows {

struct ScreenResolution {
	ScreenResolution(int w, int h) : width(w), height(h) {}
	int width;
	int height;
};

typedef std::vector<ScreenResolution> ScreenResolutionVector;

ScreenResolutionVector getScreenResolutions() {
	ScreenResolutionVector resolutions;
	
	resolutions.push_back(ScreenResolution(640, 480));
	resolutions.push_back(ScreenResolution(800, 600));
	resolutions.push_back(ScreenResolution(1024, 768));
	resolutions.push_back(ScreenResolution(1280, 1024));
	return resolutions;
}

DisplayOptions::DisplayOptions(wf::Theme* theme) : wf::TabPage(theme) {
	setText("Display");
	
	// We use a table control for all our child controls.
	Ref<wf::TableControlContainer> table = new wf::TableControlContainer(theme, 2, 2);
	table->getColumns()[0].setWidth(0.3f);
	table->getColumns()[1].setWidth(0.7f);
	table->getRows()[0].setHeight(0.8f);
	table->getRows()[1].setHeight(0.2f);
	
	Ref<wf::Label> resolutionLabel = new wf::Label(theme, "Resolution:");
	table->setControl(0, 0, resolutionLabel.get());
	
	ScreenSettings screenSettings = getScreenSettings();
	
	m_Listbox = new wf::ListBox(theme);
	m_Listbox->setZPos(-0.5f);
	
	// Get all supported resolutions for your graphic card and add them to the 
	// list box.
	ScreenResolutionVector resolutions = getScreenResolutions();
	ScreenResolutionVector::const_iterator resolution = resolutions.begin();
	for(;resolution != resolutions.end();++resolution) {
		// Format a string to be displayed in the list box. It will have the 
		// following format: 800x600
		std::stringstream itemText;
		itemText << resolution->width << 'x' << resolution->height;
		Ref<wf::ListBoxItem> item = new wf::ListBoxItem(theme, itemText.str());
		m_Listbox->addItem(item.get());

		// Test to see if the current setting is the same. If true then we make
		// this item the default.
		if(resolution->width == screenSettings.width && resolution->height == screenSettings.height) {
			m_Listbox->setSelectedItem(item.get());
		}
	}
	table->setControl(1, 0, m_Listbox.get());

	// Add the checkbox that will set the fullscreen settings.
	m_FullScreen = new wf::CheckBox(theme, "Fullscreen");
	m_FullScreen->setChecked(screenSettings.fullScreen);
	table->setControl(1, 1, m_FullScreen.get());
	
	// Set the table control as the single control in this page.
	setControl(table.get());
}

DisplayOptions::~DisplayOptions() {
}

void DisplayOptions::applyConfiguration() {
	// Perhaps not the most elegant solution. But it works...
	// We take the string that is in the listbox and splits it where the 'x' is.
	Ref<wf::ListBoxItem> selectedItem = m_Listbox->getSelectedItem();
	if(selectedItem.valid()) {
		std::string selectedText = selectedItem->getText();
		std::string::size_type position = selectedText.find('x');
		if(position != std::string::npos) {
			// Read current settings.
			ScreenSettings screenSettings = getScreenSettings();

			screenSettings.width = atoi(selectedText.substr(0, position).c_str());
			screenSettings.height = atoi(selectedText.substr(position+1).c_str());
			screenSettings.fullScreen = m_FullScreen->getChecked();
			
			// Store the new settings.
			setScreenSettings(screenSettings);
		}
	}
}

} // namespace windows

CSP_NAMESPACE_END
