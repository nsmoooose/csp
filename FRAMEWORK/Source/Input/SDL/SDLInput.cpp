//**************
//* SDLInput.cpp
//*
//* Input system based on the SDL api.
//*
//* $Revision: 1.1 $
//

#include "SDLInput.h"
//#include "SDL.h"

SDLInput::SDLInput()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::SDLInput()\n");
#endif
}

SDLInput::~SDLInput()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::~SDLInput()\n");
#endif
}

short SDLInput::Initialize(StandardFramework *Framework, _HANDLE WindowHandle, _INSTANCE AppInstance)
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::Initialize()\n");
#endif

  keyboardEventQueue = new SDL_Event[10];

  fprintf(stdout, "LinuxFramework - number of of joysticks: %d\n", 
	  SDL_NumJoysticks());
  for(int i=0; i<SDL_NumJoysticks() ; i++)
    {
      fprintf(stdout, " %d  %s\n", i, SDL_JoystickName(i));
    }
  SDL_JoystickEventState(SDL_DISABLE);
  m_joystick = SDL_JoystickOpen(0);
  if (m_joystick)
    {
      fprintf(stdout, "Opened Joystick zero\n");
    }
  else
    {
      fprintf(stdout, "Did not open Joystick zero\n");
    }

  

  return 0;
}

void SDLInput::Acquire()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::Acquire()\n");
#endif
}

void SDLInput::Unacquire()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::Unacquire()\n");
#endif
}

bool SDLInput::GetInputMessage(bool Wait)
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::GetInputMessage()\n");
#endif  

  return true;
}


short SDLInput::SnapshotKeyboard()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::SnapshotKeyboard()\n");
#endif
   
  keyboardEventCount = 0;
  int numReturned = SDL_PeepEvents(keyboardEventQueue, 10, SDL_GETEVENT, SDL_KEYDOWNMASK | SDL_KEYUPMASK);

  if (numReturned > 0)
    {
      fprintf(stdout, "SDLInput::SnapshotKeyboard() - %d keyboard events found  \n", numReturned);
      SDL_KeyboardEvent * keyEvent = (SDL_KeyboardEvent*)keyboardEventQueue;
      for (int i = 0; i<numReturned; i++)
	{
	  fprintf(stdout, "%d - %d - %d - %d\n", keyEvent[i].type, keyEvent[i].which, keyEvent[i].state, keyEvent[i].keysym);
	}
    }

  keyboardEventCount = numReturned;

  if (numReturned > 0)
    return _A_OK;
  else 
    return _INPUT_LOST;

}

_KEY_UNBUFFERED*  SDLInput::GetKeyboardInstant()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::GetKeyboardInstant()\n");
#endif

  SDL_PumpEvents();
  Uint8 * keystate = SDL_GetKeyState(NULL);
  return keystate;
}

short SDLInput::GetKeyboardBufferCount()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::GetKeyboardBufferCount()\n");
#endif
  return keyboardEventCount;
}

void SDLInput::GetKeyboardKeyPress(short Index, _KEY_BUFFERED *buffer)
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::GetKeyboardKeyPress()\n");
#endif

  memset(buffer, 0, sizeof(_KEY_BUFFERED));

  SDL_KeyboardEvent * keyEvent = (SDL_KeyboardEvent*)keyboardEventQueue;
  SDL_keysym keysym = keyEvent[Index].keysym;
  buffer->key = keysym.scancode;

  if(keyEvent[Index].state != 0)
  {
    buffer->down = true;
  }
  else
  {
    buffer->down = false;
  }

  return;

}

short SDLInput::SnapshotMouse()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::SnapshotMouse()\n");
#endif
  return 0;
}

_MOUSE* SDLInput::GetMouseInstant()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::GetMouseInstant()\n");
#endif

  int x,  y;
  SDL_PumpEvents();
  Uint8 mouse_state = SDL_GetMouseState(&x, &y);
  p_Mouse.x = x;
  p_Mouse.y = y;
  ( mouse_state & SDL_BUTTON(1) ) ? p_Mouse.mouse1 = 1 : p_Mouse.mouse1 = 0;
  ( mouse_state & SDL_BUTTON(2) ) ? p_Mouse.mouse2 = 1 : p_Mouse.mouse2 = 0;
  ( mouse_state & SDL_BUTTON(3) ) ? p_Mouse.mouse3 = 1 : p_Mouse.mouse3 = 0;

#ifdef TRACE_INPUT
  fprintf(stdout, "Current Mouse Position: %d,   %d   , %d , %d , %d\n", p_Mouse.x, p_Mouse.y, p_Mouse.mouse1,
	  p_Mouse.mouse2, p_Mouse.mouse3);
#endif

  return &p_Mouse;
}

short SDLInput::GetMouseBufferCount()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::GetMouseBufferCount()\n");
#endif

  return 0;
}

void SDLInput::GetMouseMove(short Index, _MOUSE *buffer)
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::GetMouseMove()\n");
#endif

}

short SDLInput::SnapshotJoystick()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::SnapshotJoystick()\n");
#endif

  return 0;
}

_JOYSTICK* SDLInput::GetJoystickInstant()
{

  static _JOYSTICK joystick;


#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::GetJoystickInstant()\n");
#endif

  SDL_JoystickUpdate();

  joystick.x = SDL_JoystickGetAxis(m_joystick, 0)/327;
  joystick.y = SDL_JoystickGetAxis(m_joystick, 1)/327;
  joystick.throttle = SDL_JoystickGetAxis(m_joystick, 2);
  joystick.button1 = SDL_JoystickGetButton(m_joystick, 0);
  joystick.button2 = SDL_JoystickGetButton(m_joystick, 1);
  joystick.button3 = SDL_JoystickGetButton(m_joystick, 2);
  joystick.button4 = SDL_JoystickGetButton(m_joystick, 3);
  joystick.hat = SDL_JoystickGetHat(m_joystick, 0);

#ifdef TRACE_INPUT
   fprintf(stdout, "SDLInput::GetJoystickInstant() - X %d, Y %d\n", 
    x_move, y_move);
#endif

  return &joystick;
}

short SDLInput::GetJoystickBufferCount()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::GetJoystickBufferCount()\n");
#endif

  return 0;
}

void SDLInput::GetJoystickMove(short Index, _JOYSTICK *buffer)
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::GetJoystickMove()\n");
#endif

  

}

void SDLInput::Uninitialize()
{
#ifdef TRACE_INPUT
  fprintf(stdout, "SDLInput::Uninitialize()\n");
#endif
}


