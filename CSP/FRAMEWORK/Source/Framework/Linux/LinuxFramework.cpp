#include "LinuxFramework.h"

//#include "SDL.h"


// --- StandardFramework Constructor/Deconstructor

LinuxFramework::LinuxFramework()
{

  //  p_hShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);

  //  ResetEvent(p_hShutdown);

  p_hShutdown = false;

}

LinuxFramework::~LinuxFramework()
{
  Uninitialize();
}

// --- StandardFramework

short LinuxFramework::Initialize()
{

//  #ifndef _LICENSE
//    #error Aborting Compile.
//  #endif

  fprintf(stdout, "LinuxFramework::Initialize()\n");

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
  {
	  fprintf(stderr, "Could not initialize SDL\n");
  }
  else
    {
      fprintf(stdout, "Finished initializing SDL\n");
    }


  return 0;
}

void LinuxFramework::Uninitialize()
{

  //  if(p_hShutdown != 0)
  //  {
    //    CloseHandle(p_hShutdown);
  //    p_hShutdown = 0;
  //  }
  return;
}

void LinuxFramework::Nap(unsigned long Milliseconds)
{
  SDL_Delay(Milliseconds);
  return;
}

unsigned long LinuxFramework::GetTime()
{
  return SDL_GetTicks();
}

void* LinuxFramework::GetShutdownHandle()
{
  return (void*) p_hShutdown;
}

void LinuxFramework::TriggerShutdown()
{
  //  SetEvent(p_hShutdown);
  p_hShutdown = true;
  //  SDL_PushEvent(SDL_QuitEvent());
}

bool LinuxFramework::GetShutdown()
{
  return p_hShutdown;
}

