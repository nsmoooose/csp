#include "WinFramework.h"

// --- StandardFramework Constructor/Deconstructor

/** WinFramework
  * Default Contructor for the WinFramework class.
  */
WinFramework::WinFramework()
{

  CSP_LOG(CSP_FRAMEWORK, CSP_TRACE, "WinFramework::WinFramework()");

  p_hShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);

  ResetEvent(p_hShutdown);

  p_bShutdown = false;

}

/** ~WinFramework()
  * Destructor for the WinFramework class
  */
WinFramework::~WinFramework()
{
  CSP_LOG(CSP_FRAMEWORK, CSP_TRACE, "WinFramework::~WinFramework()");

  Uninitialize();
}

// --- StandardFramework

/** Initialize()
  * Initialize the WinFramework object
  * Currently this does nothing.
  */
short WinFramework::Initialize()
{

  #ifndef _LICENSE
    #error Aborting Compile.
  #endif

  CSP_LOG(CSP_FRAMEWORK, CSP_TRACE, "WinFramework::Initialize()");


  return 0;
}

/**
 * Uninitialize()
 * Uninitialize/Cleanup the WinFramework class
 */
void WinFramework::Uninitialize()
{
  CSP_LOG(CSP_FRAMEWORK, CSP_TRACE, "WinFramework::Uninitialize()");


  if(p_hShutdown != 0)
  {
    CloseHandle(p_hShutdown);
    p_hShutdown = 0;
  }
  return;
}

/**
 * Nap()
 * sleep for a period of milliseconds
 */
void WinFramework::Nap(unsigned long Milliseconds)
{
  CSP_LOG(CSP_FRAMEWORK, CSP_BULK, "WinFramework::Nap()");

  Sleep(Milliseconds);
  return;
}

/**
 * GetTIme()
 * returns the current system time in miliseconds
 */
unsigned long WinFramework::GetTime()
{
  CSP_LOG(CSP_FRAMEWORK, CSP_BULK, "WinFramework::GetTime()");

  return timeGetTime();
}

/**
 * GetShutdown()
 * returns the shutdown handle
 */
void* WinFramework::GetShutdownHandle()
{
  CSP_LOG(CSP_FRAMEWORK, CSP_TRACE, "WinFramework::GetShutdownHandle()");

  return p_hShutdown;
}

/**
 * TriggerShutdown
 * Posts an event to start shutdown
 */
void WinFramework::TriggerShutdown()
{
  CSP_LOG(CSP_FRAMEWORK, CSP_TRACE, "WinFramework::TriggerShutdown()");

  SetEvent(p_hShutdown);
  p_bShutdown = true;
}

/** 
 * GetShutdown
 * returns the shutdown flag.
 */
bool WinFramework::GetShutdown()
{
  CSP_LOG(CSP_FRAMEWORK, CSP_TRACE, "WinFramework::GetShutdown()");

  return p_bShutdown;
}

