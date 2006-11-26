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

#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/MessageBox.h>
#include <csp/cspsim/wf/Tab.h>
#include <csp/cspsim/wf/TableControlContainer.h>
#include <csp/cspsim/wf/WindowManager.h>
#include <csp/cspsim/windows/DisplayOptions.h>
#include <csp/cspsim/windows/Options.h>
#include <csp/cspsim/windows/SoundOptions.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <osg/Vec4>

CSP_NAMESPACE

namespace windows {

Options::Options(wf::Theme* theme) : wf::Window(theme) {
	setSize(wf::Size(140.0, 120.0));
	setCaption("Options");

	Ref<wf::TableControlContainer> topTable = new wf::TableControlContainer(getTheme(), 1, 2);
	topTable->getRows()[0].setHeight(0.85f);
	topTable->getRows()[1].setHeight(0.15f);
	setControl(topTable.get());

	// Add the tab page.
	Ref<wf::Tab> tab = new wf::Tab(getTheme());
	m_DisplayOptionsPage = new DisplayOptions(getTheme());
	tab->addPage(m_DisplayOptionsPage.get());
	m_SoundOptionsPage = new SoundOptions(getTheme());
	tab->addPage(m_SoundOptionsPage.get());

	topTable->setControl(0,0, tab.get());
	
	Ref<wf::TableControlContainer> okCancelTable = new wf::TableControlContainer(getTheme(), 3, 1);
	topTable->setControl(0, 1, okCancelTable.get());
	okCancelTable->getColumns()[0].setWidth(0.6f);
	okCancelTable->getColumns()[1].setWidth(0.2f);
	okCancelTable->getColumns()[2].setWidth(0.2f);
	Ref<wf::Button> cancelButton = new wf::Button(getTheme(), "Cancel");
	cancelButton->addButtonClickedHandler(sigc::mem_fun(*this, &Options::onCancel));
	okCancelTable->setControl(1, 0, cancelButton.get());

	Ref<wf::Button> okButton = new wf::Button(getTheme(), "OK");
	okButton->addButtonClickedHandler(sigc::mem_fun(*this, &Options::onOK));
	okCancelTable->setControl(2, 0, okButton.get());
}

void Options::onOK() {
	m_DisplayOptionsPage->applyConfiguration();
	
	// Display a message box that states that you will need to restart the 
	// simulator.
	Ref<wf::MessageBox> messageBox = wf::MessageBox::Show(getTheme(), "Information", 
		"Changes will not take effect\nuntil you restart the simulator!");
	getWindowManager()->show(messageBox.get());

	close();
}

void Options::onCancel() {
	close();
}

Options::~Options() {
}

} // namespace windows

CSP_NAMESPACE_END
