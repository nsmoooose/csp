// GroundCombat.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include <Std.h>
#include <App.h>
#include <OS.h>
#define __INCLUDE_WINDOWS
#include <opsyswindows.h>
#include <task.h>
#include "globals.h"

#include "MainManager.h"

using namespace Jet;

LOGCLIENT(GroundCombat);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	windows = NewMem(OS(hInstance, lpCmdLine));
	
	App theApp;


	if(!theApp.Init("GroundCombat", windows)) {
		LOGERR(("WinMain(): Unable to initialize application object!"));
		return -1;
	}

	LOGERR(("Creating MainManager object..."));
	MainManager *mainmanager = NewMem(MainManager);

	LOGERR(("Starting Main Loop..."));
	theApp.Main();
	
	DeleteMem(mainmanager);
	return 0;
}