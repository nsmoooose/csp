#include <std.h>
#include <OS.h>
#include <UI/OSDConsole.h>
#include <RenderKernel.h>
#include <TaskManager.h>

using namespace Jet;

extern RenderKernel	*renderKernel;
extern OSDConsole	*console;
extern OS			*windows;

extern int GameMode;

extern enum GameResolution {
	ONE = 1024,
	TWO = 800,
	THREE = 640
};
//Game States
#define MENU_ACTIVE		1
#define MENU_SHUTDOWN	2
#define MENU_CLOSED		3
#define GAME_ACTIVE		4
#define GAME_CLOSED		5
#define APP_IDLE 100