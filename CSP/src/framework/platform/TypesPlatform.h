#include "standard/StandardFramework.h"
#include "graphics/StandardGraphics.h"
#include "graphics/FactoryGraphics.h"
#include "standard/FrameworkError.h"

// Available Preprocessors for Platform
// WIN32        - Windows 95/98/2000
// LINUX        - Linux, i386

// list of API's
const short _NOPLATFORM   = -1;
const short _WINDOWS32    = 0;
const short _LINUX        = 1;

// if we compiled with Win32 make it default.
#ifdef WIN32
  const short DEFAULT_PLATFORM = _WINDOWS32;
  #define PLATFORM_HAS_A_DEFAULT
#endif

#ifdef LINUX
  const short DEFAULT_PLATFORM = _LINUX;
  #define PLATFORM_HAS_A_DEFAULT
#endif

#ifndef PLATFORM_HAS_A_DEFAULT
  const short DEFAULT_PLATFORM = _NOPLATFORM;
#endif

// common structs
struct _STANDARD_PLATFORM
{
    void*   this_inst;
    void*   prev_inst;
    char*   cmdline;
    int     cmdshow;
    void*   icon;

    char*   exename;
    char*   displayname;

    _STANDARD_GRAPHICS display;

};

struct _PLATFORM_DESCRIPTION
{
  char   *Name;
  long    VersionMajor;
  long    VersionMinor;
  short   ID;
};
