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

#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/TableControlContainer.h>
#include <csp/cspsim/windows/DisplayOptions.h>

#include <osg/Vec4>

CSP_NAMESPACE

namespace windows {

DisplayOptions::DisplayOptions(wf::Theme* theme) : wf::TabPage(theme) {
	setText("Display");
	
	Ref<wf::TableControlContainer> table = new wf::TableControlContainer(theme, 2, 2);
	table->getColumns()[0].setWidth(0.3f);
	table->getColumns()[1].setWidth(0.7f);
	table->getRows()[0].setHeight(0.8f);
	table->getRows()[1].setHeight(0.2f);
	
	Ref<wf::Label> resolutionLabel = new wf::Label(theme, "Resolution:");
	table->setControl(0, 0, resolutionLabel.get());
	
	Ref<wf::ListBox> listbox = new wf::ListBox(theme);
	listbox->setZPos(-0.5f);
	listbox->addItem(new wf::ListBoxItem(theme, "640x480"));
	listbox->addItem(new wf::ListBoxItem(theme, "800x600"));
	listbox->addItem(new wf::ListBoxItem(theme, "1024x768"));
	listbox->addItem(new wf::ListBoxItem(theme, "1280x1024"));
	table->setControl(1, 0, listbox.get());
	
	table->setControl(0, 1, new wf::Label(theme, "Fullscreen:"));
	table->setControl(1, 1, new wf::Label(theme, "TODO: Checkbox"));
		
	setControl(table.get());
}

DisplayOptions::~DisplayOptions() {
}

} // namespace windows

CSP_NAMESPACE_END
