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
 * @file QuitResume.cpp
 *
 **/

#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/TableControlContainer.h>
#include <csp/cspsim/wf/WindowManager.h>
#include <csp/cspsim/windows/Options.h>
#include <csp/cspsim/windows/QuitResume.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <osg/Vec4>

CSP_NAMESPACE

namespace windows {

QuitResume::QuitResume(wf::Theme* theme) : wf::Window(theme) {
	setSize(wf::Size(100.0, 25.0));
	setZPos(0.0);
	setCaption("Combat Simulator Project");
	
	Ref<wf::TableControlContainer> table = new wf::TableControlContainer(getTheme(), 3, 1);
	setControl(table.get());
	
	Ref<wf::Button> resumeButton = new wf::Button(getTheme(), "Resume");
	resumeButton->addButtonClickedHandler(sigc::mem_fun(*this, &QuitResume::onResume));
	table->setControl(0, 0, resumeButton.get());

	Ref<wf::Button> optionsButton = new wf::Button(getTheme(), "Options");
	optionsButton->addButtonClickedHandler(sigc::mem_fun(*this, &QuitResume::onOptions));
	table->setControl(1, 0, optionsButton.get());

	Ref<wf::Button> quitButton = new wf::Button(getTheme(), "Quit");
	quitButton->addButtonClickedHandler(sigc::mem_fun(*this, &QuitResume::onQuit));
	table->setControl(2, 0, quitButton.get());
}

void QuitResume::onResume() {
	close();
}

void QuitResume::onQuit() {
	CSPSim::theSim->quit();
}

void QuitResume::onOptions() {
	wf::WindowManager* manager = getWindowManager();
	if(manager != NULL) {
		if(!manager->windowIsOpen<Options>()) {
			Ref<Window> options = new Options(getTheme());
			manager->show(options.get());
		}
	}
	
	close();
}

QuitResume::~QuitResume() {
}

} // namespace windows

CSP_NAMESPACE_END
