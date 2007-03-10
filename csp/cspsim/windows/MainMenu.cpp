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
 * @file MainMenu.cpp
 *
 **/

#include <csp/csplib/data/DataManager.h>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/battlefield/LocalBattleField.h>
#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/Serialization.h>
#include <csp/cspsim/wf/WindowManager.h>
#include <csp/cspsim/windows/MainMenu.h>
#include <csp/cspsim/windows/Options.h>

CSP_NAMESPACE

namespace windows {

MainMenu::MainMenu() {
	Ref<wf::Serialization> serializer = new wf::Serialization(getUIPath());
	serializer->load(this, getUITheme(), "main_menu.xml");

	Ref<wf::Button> instantActionButton = getById<wf::Button>("instantAction");
	if(instantActionButton.valid())
		instantActionButton->addButtonClickedHandler(sigc::mem_fun(*this, &MainMenu::instantAction_Click));

	Ref<wf::Button> optionsButton = getById<wf::Button>("options");
	if(optionsButton.valid())
		optionsButton->addButtonClickedHandler(sigc::mem_fun(*this, &MainMenu::options_Click));

	Ref<wf::Button> quitButton = getById<wf::Button>("quit");
	if(quitButton.valid())
		quitButton->addButtonClickedHandler(sigc::mem_fun(*this, &MainMenu::quit_Click));	
}

void createVehicleHelper(CSPSim *self, const char *path, Vector3 position,
                          Vector3 velocity, Vector3 attitude) {
	Ref<DynamicObject> obj = self->getDataManager().getObject(path);
	if (!obj) {
		return;
	}
	
	obj->setGlobalPosition(position);
	obj->setVelocity(velocity);
	csp::Quat q_attitude;
	attitude *= 3.1416 / 180.0;
	q_attitude.makeRotate(attitude.x(), attitude.y(), -attitude.z());
	obj->setAttitude(q_attitude);
	self->getBattlefield()->__test__addLocalHumanUnit(obj);
	if (!self->getActiveObject()) self->setActiveObject(obj);
}


void MainMenu::instantAction_Click() {
	CSPSim::theSim->displayLogoScreen();
	CSPSim::theSim->loadSimulation();

	createVehicleHelper(CSPSim::theSim, "sim:vehicles.aircraft.f16dj", Vector3(-29495, -10530, 91.1), Vector3(0, 0, 0), Vector3(0.0, 0.0, 180.0));
	createVehicleHelper(CSPSim::theSim, "sim:vehicles.aircraft.f16dj", Vector3(-29510, -10530, 91.1), Vector3(0, 0, 0), Vector3(0.0, 0.0, 180.0));
}

void MainMenu::quit_Click() {
	CSPSim::theSim->quit();
}

void MainMenu::options_Click() {
	wf::WindowManager* manager = getWindowManager();
	if(manager != NULL) {
		if(!manager->windowIsOpen<Options>()) {
			Ref<Window> options = new Options();
			manager->show(options.get());
			options->centerWindow();
		}
	}
}

MainMenu::~MainMenu() {
}

} // namespace windows

CSP_NAMESPACE_END
