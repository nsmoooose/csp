// Linux Specific for StandardPlatform.

#include "platform/StandardPlatform.h"
#include "standard/FrameworkError.h"

class LinuxPlatform: public StandardPlatform
{

private:

  _STANDARD_PLATFORM *globals;
  FrameworkError Error;

  char *VersionNumber;

  /*
  //HDC hdc;
  HANDLE shutdown;
  BOOL regainedfocus;
  BOOL quit;*/

  //static LRESULT CALLBACK WindowProc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam);
  //rewrite this proc-control for Linux

public:
  LinuxPlatform();
  ~LinuxPlatform();

  // Standard Framework - required for every framework object
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();

  // Standard platform
  short         CreateAppWindow();
  short         CreateRenderingDevice(StandardGraphics **display);

  char*         GetPlatformVersion();
  void          TriggerShutdown();
  void          HandleSystemMessages();
  short         GetRegainedFocus();
  short         GetQuit();
  unsigned long GetTime();
  void*         GetWindowHandle();
  void*         GetShutdown();
  void          Nap(short milliseconds);
  void          Flip();

  void          DeleteRenderingDevice(StandardGraphics *display);
  void          DeleteAppWindow();
};
