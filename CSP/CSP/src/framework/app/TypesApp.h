#ifndef __TYPESAPP_H_
#define __TYPESAPP_H_

#include "standard/StandardFramework.h"

#include "input/StandardInput.h"
#include "audio/StandardAudio.h"
#include "net/StandardNetwork.h"
#include "graphics/StandardGraphics.h"
#include "platform/StandardPlatform.h"
#include "standard/StandardMath.h"

#include "platform/FactoryPlatform.h"
#include "graphics/FactoryGraphics.h"
#include "input/FactoryInput.h"
#include "net/FactoryNetwork.h"
#include "audio/FactoryAudio.h"
#include "standard/FactoryMath.h"



// common structs
struct _STANDARD_APP
{
  char *name;
  char *exename;
};

struct _APP_INIT_STRUCT
{
  _STANDARD_APP       app;
  _STANDARD_GRAPHICS  display;
  _STANDARD_PLATFORM  platform;
};

#endif
