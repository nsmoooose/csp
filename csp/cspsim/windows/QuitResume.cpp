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
#include <csp/cspsim/windows/QuitResume.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <osg/Vec4>

CSP_NAMESPACE

namespace windows {

QuitResume::QuitResume() {
	// Completely white and opaque background.
	setBackgroundColor(osg::Vec4(0.1f, 0.3f, 0.15f, 0.75f));
	setSize(wf::Size(80.0, 30.0));
	setZPos(0.0);
	setCaption("Combat Simulator Project");
	
	Ref<wf::Button> quitButton = new wf::Button;
	quitButton->setLocation(wf::Point(-17.5, -2.0f));
	quitButton->setSize(wf::Size(25, 10));
	quitButton->setZPos(getZPos() - 0.1f);
	quitButton->setText("Quit");
	addControl(quitButton.get());

	Ref<wf::Button> resumeButton = new wf::Button;
	resumeButton->setLocation(wf::Point(17.5, -2.0f));
	resumeButton->setSize(wf::Size(25, 10));
	resumeButton->setZPos(getZPos() - 0.1f);
	resumeButton->setText("Resume");
	addControl(resumeButton.get());
	
	quitButton->addButtonClickedHandler(sigc::mem_fun(*this, &QuitResume::onQuit));
	resumeButton->addButtonClickedHandler(sigc::mem_fun(*this, &QuitResume::onResume));
}

void QuitResume::onResume() {
	close();
}

void QuitResume::onQuit() {
	CSPSim::theSim->quit();
}

QuitResume::~QuitResume() {
}

} // namespace windows

CSP_NAMESPACE_END
