#include "stdafx.h"
#include "std.h"
#include "MainInterface.h"

#define __INCLUDE_WINDOWS

#include <UI/EditBox.h>
#include <UI/DropDown.h>
#include <UI/Window.h>
#include <Window.h>
#include <CallCpp.h>

#include "globals.h"

using namespace Jet;

LOGCLIENT(MainInterface);

MainInterface::MainInterface() : Task("MainInterface")
{
	ready = false;
}

bool MainInterface::Init(OS *os) 
{
	LOGWARN(("Initialising MainInterface..."));

	Plugin<InterfaceSystemFactory> interfaceSystemFactory(InterfaceSystem::uid);
	if (!interfaceSystemFactory) return false;

	LOGWARN(("Creating instance of factory..."));
	interfaceSystem = interfaceSystemFactory->CreateInstance();
	if (!interfaceSystem) return false;

	LOGWARN(("interfaceSystem->Init"));
	interfaceSystem->Init(os, renderKernel);

	console->BringToFront();
	console->Printf("Creating Database object\n");
	database = NewMem(ResourceDatabase("UI", "data/2d/"));

	console->Printf("Opening Startup Dialog\n");
	inputForm = interfaceSystem->Open("maininterface", database);

	if(!inputForm) {
		console->Printf("Couldn't Open() interfaceSystem!\n");
		return false;
	}

	console->Printf("Assigning Control Procedures...\n");
	inputForm->GetControl("Btn_Quit")->SetCallback("OnClick",CALLCPP(MainInterface,OnQuit));

	return true;
}

void MainInterface::Done()
{
	if(inputForm) {
		LOGWARN(("Debug: inputForm->DoClose();"));
		inputForm->DoClose();
	}
	if(interfaceSystem) {
		LOGWARN(("Debug: interfaceSystem->Done()..."));
		interfaceSystem->Done();
		LOGWARN(("Debug: interfaceSystem->ReleaseReference()..."));
		interfaceSystem->ReleaseReference();
	}

	LOGWARN(("Debug: DeleteMem(database)..."));
	DeleteMem(database);
	GameMode = 0;
}

Task::UpdateCode MainInterface::Update(float dt)
{
	if(ready == false) return Task::CONTINUE;
	else {
		GameMode = MENU_SHUTDOWN;
		ready = true;
	}
	return Task::CONTINUE;
}

void MainInterface::OnQuit()
{
	ready = true;
}

void MainInterface::OnToggleFullscreen()
{
	//turn on fullscreen mode with size defined in the menu.
}
