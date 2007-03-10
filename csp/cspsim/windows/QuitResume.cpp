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
#include <csp/cspsim/wf/Serialization.h>
#include <csp/cspsim/wf/TableControlContainer.h>
#include <csp/cspsim/wf/WindowManager.h>
#include <csp/cspsim/windows/Options.h>
#include <csp/cspsim/windows/QuitResume.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <osg/Vec4>

CSP_NAMESPACE

namespace windows {

QuitResume::QuitResume() {
	Ref<wf::Serialization> serializer = new wf::Serialization(getUIPath());
	serializer->load(this, getUITheme(), "quit_resume.xml");

	Ref<wf::Button> resumeButton = getById<wf::Button>("resume");
	if(resumeButton.valid())
		resumeButton->addButtonClickedHandler(sigc::mem_fun(*this, &QuitResume::resume_Click));

	Ref<wf::Button> optionsButton = getById<wf::Button>("options");
	if(optionsButton.valid())
		optionsButton->addButtonClickedHandler(sigc::mem_fun(*this, &QuitResume::options_Click));

	Ref<wf::Button> quitButton = getById<wf::Button>("quit");
	if(quitButton.valid())
		quitButton->addButtonClickedHandler(sigc::mem_fun(*this, &QuitResume::quit_Click));	
}

void QuitResume::resume_Click() {
	close();
}

void QuitResume::quit_Click() {
	CSPSim::theSim->quit();
}

void QuitResume::options_Click() {
	wf::WindowManager* manager = getWindowManager();
	if(manager != NULL) {
		if(!manager->windowIsOpen<Options>()) {
			Ref<Window> options = new Options();
			manager->show(options.get());
			options->centerWindow();
		}
	}
	
	close();
}

QuitResume::~QuitResume() {
}

} // namespace windows

CSP_NAMESPACE_END
