#include "standard/StandardFramework.h"

// Available Preprocessors for Input
// DirectInput        - DirectInput 8

// list of APIS
const short _NOINPUT        = -1;
const short _DIRECTINPUT    = 0;

// if DirectInput is listed, then default to it.
#ifdef DirectInput
  const short DEFAULT_INPUT = _DIRECTINPUT;

  #define INPUT_HAS_A_DEFAULT
#endif

// if nothing else is defaulted then default noinput.
#ifndef INPUT_HAS_A_DEFAULT
  const short DEFAULT_INPUT = _NOINPUT;
#endif

// common structs
struct _INPUT_DESCRIPTION
{
  char   *Name;
  short   ID;
};

struct _KEY_STRUCT
{
  short down;
  unsigned char key;
};

struct _MOUSE_STRUCT
{
  char x;
  char y;
  char z;
  short mouse1;
  short mouse2;
  short mouse3;
};

struct _JOYSTICK_STRUCT
{
  char x;
  char y;
  char throttle;
  short hat;
  short button1;
  short button2;
  short button3;
  short button4;
};

struct _INPUT_INIT_STRUCT
{
  void *this_inst;
};
