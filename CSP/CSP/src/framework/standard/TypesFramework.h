#include <memory.h>
#include <stdio.h>
#ifdef WIN32
#include <direct.h>
#endif
#include <math.h>
#include <string.h>
#include <time.h>

#include "License.h"
#include "FrameworkError.h"

// const
const short   _PLATFORM_DEPENDENCY      = 1;
const short   _GRAPHICS_DEPENDENCY      = 2;
const short   _INPUT_DEPENDENCY         = 3;
const short   _NETWORKING_DEPENDENCY    = 4;
const short   _AUDIO_DEPENDENCY         = 5;

// defines
#define TRUE                1
#define FALSE               0

// this is a hack to fix a compile bug.
#ifdef WIN32
typedef int                           BOOL;
#else
typedef unsigned char                 BOOL;
#endif

// structs
struct _DEPENDENCY
{
  short ID;
  char *Description;
  short Required;
};

// Macros
#undef _DELARRAY
#define _DELARRAY(x) if (x != 0) {delete[]x; x = 0;}

#undef _DEL
#define _DEL(x) if (x != 0) {delete x; x = 0;}

#undef _REL
#define _REL(x) if (x != NULL) {x->Release(); x = NULL;}
