#include "Std.h"
#include "Task.h"
#include "stdafx.h"
#include "MainManager.h"
#include <OS.h>
#define __INCLUDE_WINDOWS
#include <OpSysWindows.h>
#include <TaskManager.h>

#include "MainInterface.h"
#include "globals.h"
using namespace Jet;

LOGCLIENT(MainManager);

MainManager::MainManager() : Task("MainManager", 20)
{
	renderKernel = NULL;
	database = NULL;
	clearLayer = NULL;
	spriteManager = NULL;
	splashScreen = NULL;
	LOGWARN(("Created Task object"));
}

Task::UpdateCode MainManager::Update(float dt)
{
	static float modificator, x, y, time = 0.0f;
//	static double  = 0.0f;

	if(modificator < 0.75f) 
		modificator += 0.001f;

	//wavy motion of the logo
	x = (float) sin(time) /4 + 0.33f;
	y = (float)(0.5f - (cos(time*4)-0.25f)) /40;
	time += 0.01f;

	splashScreen->SetPosition(Vector2((float)x, (float)-y + .93f));
	splashScreen->SetOpacity(modificator);

	if(GameMode == MENU_SHUTDOWN) {
		mainInterface->Done();
		DeleteMem(mainInterface);
		GameMode = MENU_CLOSED;
		//return Task::EXIT;
	}
	return Task::CONTINUE;
}

bool MainManager::Init(OS *os)
{
	GameMode = 0;
	LOGWARN(("Initialising MainManager..."));
	renderKernel	= NewMem(RenderKernel);
	spriteManager	= NewMem(SpriteManager("SpriteManager", 10));
	clearLayer		= NewMem(ClearLayer("ClearLayer", 10));
	database		= NewMem(ResourceDatabase("splash", "data/2d"));
	splashScreen	= NewMem(SpriteInstance);

	if (!renderKernel->Init(os))
	{
		LOGERR(("SpritesTask::Init() Unable to initialize render kernel"));
		return false;
	}

	FontInstance	font;
	font.Init(database, "Verdana, 10,0xFFFFFF, 1.font");

	console			= NewMem(OSDConsole("Ground Combat", 1));
	
	console->Init(font);               // initialize console's font
	console->Init(Size(80, 25));       // initialize console's dimensions
	console->SetPos(Point(20, 20));    // set screen position
	console->SetOpacity(1.0f);         // set overall opacity
	console->Printf("Ground Combat Demo v0.0.1 initialised.\n");

	console->Printf("Initialising Main Interface Dialog...\n");
	
	clearLayer->SetClearColor(Color((float)0.45, (float)0.45, (float)0.55));

	LOGWARN(("splashScreen->Open()...."));

	bool success = splashScreen->Open(renderKernel, "splash.sprite", database);

	if(success) {
		LOGWARN(("spriteManager->Add()...."));
		spriteManager->Add(splashScreen);
	}
	else {
		LOGERR(("Could not Open() splashScreen!"));
		return false;
	}

	splashScreen->SetPosition(Vector2(1.0 - 0.25, 1.0 - 0.0625));
	splashScreen->SetOpacity(0.0f);

	mainInterface = NewMem(MainInterface);
	return true;
}

void MainManager::Done()
{
	DeleteMemZ(splashScreen);
	if(GameMode != MENU_CLOSED) DeleteMemZ(mainInterface);
	DeleteMemZ(console);
	DeleteMemZ(database);
	DeleteMemZ(clearLayer);
	DeleteMemZ(spriteManager);
	if (renderKernel)
	{
		renderKernel->DestroySceneGraph();
	}
	DeleteMemZ(renderKernel);
}
