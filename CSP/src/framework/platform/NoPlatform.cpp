#include "NoPlatform.h"

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
NoPlatformClass::NoPlatformClass()
{

}

NoPlatformClass::~NoPlatformClass()
{

}


// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  NoPlatformClass::SetSetup(void *setup)
{
  globals = (_STANDARD_PLATFORM *)setup;
}

short NoPlatformClass::GetDependencyCount()
{
  return 0;
}

void  NoPlatformClass::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short NoPlatformClass::SetDependencyData(short Index, void* Data)
{
  switch(Index)
  {
    case 0:
      return _NO_DEPENDENCY_TO_SET;
      break;
    default:
      return _NO_DEPENDENCY_TO_SET;
      break;
  }
  return 0;
}

void  NoPlatformClass::GetSetup(void*setup)
{
  setup = globals;
}

short NoPlatformClass::CheckStatus()
{
  if(globals == 0)
  {
    return _SETUP_NOT_SET;
  }

  return 0;
}

// ----------------------------------------
// GetPlatformVersion
// Gets the version number of the platform (windows)
// -----------------------------------------
char* NoPlatformClass::GetPlatformVersion()
{
  return 0;
}

// ------------------------------------------
// CreateAppWindow
// Creates the initial window
// -------------------------------------------
short NoPlatformClass::CreateAppWindow()
{
  return 0;
};

// ------------------------------------------
// DestroyAppWindow
// Destroy the initial window
// -------------------------------------------
void NoPlatformClass::DeleteAppWindow()
{
  return;
}

// ------------------------------------------
// GetWindowHandle
// Destroy the initial window
// -------------------------------------------
void* NoPlatformClass::GetWindowHandle()
{
  return 0;
}

// ------------------------------------------
// GetShutdown
// Returns the handle for the shutdown event.
// -------------------------------------------
inline void* NoPlatformClass::GetShutdown()
{
  return FALSE;
}

// ------------------------------------------
// TriggerShutdown
// The APP needs to tell the subsystems when
// its shutting down. This is the event to do it.
// -------------------------------------------
void NoPlatformClass::TriggerShutdown()
{
  return;
}

// ------------------------------------------
// HandleSystemMessages
// Windows sends messages and the queue grows
// so we must handle the system messages to remove them.
// Since the WindowsProc does nothing, the cue is flushed
// with minimal overhead.
// -------------------------------------------
void NoPlatformClass::HandleSystemMessages()
{
  return;
}

// ------------------------------------------
// GetTime
// This returns the time for FPS calculations
// ------------------------------------------
inline unsigned long NoPlatformClass::GetTime()
{
  return 0;
}

// ------------------------------------------
// GetRegainedFocus
// This checks to see if the application got focus
// again.
// -------------------------------------------
inline short NoPlatformClass::GetRegainedFocus()
{
  return TRUE;
}

// ------------------------------------------
// GetQuit
// This checks to see if the application got a system quit message.
// -------------------------------------------
inline short NoPlatformClass::GetQuit()
{
  return FALSE;
}

// ------------------------------------------
// Nap
// Wait n milliseconds
// -------------------------------------------
inline void NoPlatformClass::Nap(short milliseconds)
{

}

// -------
// Create the 3D Device
// -------
short NoPlatformClass::CreateRenderingDevice(StandardGraphics **display)
{
  short                 dependencycount;
  short                 count;
  _DEPENDENCY           dependency;

  FactoryGraphics *GraphicsFactory = 0;

  // -----
  // GRAPHICAL API
  // -----
  GraphicsFactory = new FactoryGraphics;
  
  Error.sError = GraphicsFactory->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  // we already know which API they want from startup dialog.
  GraphicsFactory->Create(globals->display.ID, display);

  _DEL(GraphicsFactory);

  // ask object for dependency list
  dependencycount = (*display)->GetDependencyCount();
  if(dependencycount > 0)
  {
    // check which depedency we need.
    for(count = 0; count < dependencycount; count++)
    {
      (*display)->GetDependencyDescription(count, &dependency);
      switch(dependency.ID)
      {
        case _PLATFORM_DEPENDENCY:
          (*display)->SetDependencyData(dependency.ID, this);
          break;
      }
    }
  }

  (*display)->SetSetup(&globals->display);

  Error.sError = (*display)->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  return _A_OK;

}

void NoPlatformClass::DeleteRenderingDevice(StandardGraphics *display)
{
  _DEL(display);
}

void NoPlatformClass::Flip()
{

}
