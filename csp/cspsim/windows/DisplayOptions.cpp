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
#include <csp/cspsim/wf/TableControlContainer.h>
#include <csp/cspsim/windows/DisplayOptions.h>

#include <osg/Vec4>

CSP_NAMESPACE

namespace windows {

DisplayOptions::DisplayOptions(wf::Theme* theme) : wf::TabPage(theme) {
	setText("Display");
	
	Ref<wf::TableControlContainer> table = new wf::TableControlContainer(theme, 2, 5);
	table->getColumns()[0].setWidth(0.3f);
	table->getColumns()[1].setWidth(0.7f);
	
	table->setControl(0, 0, new wf::Label(theme, "Resolution:"));
	table->setControl(1, 0, new wf::Label(theme, "TODO: 1024x768"));
	
	table->setControl(0, 1, new wf::Label(theme, "Terrain:"));
	table->setControl(1, 1, new wf::Label(theme, "TODO: Low"));
	
	table->setControl(0, 2, new wf::Label(theme, "Texture:"));
	table->setControl(1, 2, new wf::Label(theme, "TODO: Very high"));
	
	table->setControl(0, 3, new wf::Label(theme, "Effects:"));
	table->setControl(1, 3, new wf::Label(theme, "TODO: Ultra realistic"));

	table->setControl(0, 4, new wf::Label(theme, "Shadows:"));
	table->setControl(1, 4, new wf::Label(theme, "TODO: Dark"));
	
	setControl(table.get());
}

DisplayOptions::~DisplayOptions() {
}

} // namespace windows

CSP_NAMESPACE_END
