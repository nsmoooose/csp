#include "LinuxInput.h"
//#include "SDL.h"

LinuxInput::LinuxInput()
{
  fprintf(stdout, "LinuxInput::LinuxInput()\n");
}

LinuxInput::~LinuxInput()
{
  fprintf(stdout, "LinuxInput::~LinuxInput()\n");
}

short LinuxInput::Initialize(StandardFramework *Framework, _HANDLE WindowHandle, _INSTANCE AppInstance)
{
  fprintf(stdout, "LinuxInput::Initialize()\n");
  return 0;
}

void LinuxInput::Acquire()
{

}

void LinuxInput::Unacquire()
{

}

bool LinuxInput::GetInputMessage(bool Wait)
{
  

  return true;
}

short LinuxInput::SnapshotKeyboard()
{
  return 0;
}

_KEY_UNBUFFERED*  LinuxInput::GetKeyboardInstant()
{
  fprintf(stdout, "LinuxInput::GetKeyboardInstant()\n");
  Uint8 * keystate = SDL_GetKeyState(NULL);
  return keystate;
}

short LinuxInput::GetKeyboardBufferCount()
{
  return 0;
}

void LinuxInput::GetKeyboardKeyPress(short Index, _KEY_BUFFERED *buffer)
{

}

short LinuxInput::SnapshotMouse()
{
  return 0;
}

_MOUSE* LinuxInput::GetMouseInstant()
{
  fprintf(stdout, "LinuxInput::GetMouseInput()\n");

  int x,  y;
  Uint8 mouse_state = SDL_GetMouseState(&x, &y);
  p_Mouse.x = x;
  p_Mouse.y = y;
  ( mouse_state & SDL_BUTTON(1) ) ? p_Mouse.mouse1 = 1 : p_Mouse.mouse1 = 0;
  ( mouse_state & SDL_BUTTON(2) ) ? p_Mouse.mouse2 = 1 : p_Mouse.mouse2 = 0;
  ( mouse_state & SDL_BUTTON(3) ) ? p_Mouse.mouse3 = 1 : p_Mouse.mouse3 = 0;

  fprintf(stdout, "%d,   %d   , %d , %d , %d\n", p_Mouse.x, p_Mouse.y, p_Mouse.mouse1,
	  p_Mouse.mouse2, p_Mouse.mouse3);

  return &p_Mouse;
}

short LinuxInput::GetMouseBufferCount()
{
  return 0;
}

void LinuxInput::GetMouseMove(short Index, _MOUSE *buffer)
{

}

short LinuxInput::SnapshotJoystick()
{
  return 0;
}

_JOYSTICK* LinuxInput::GetJoystickInstant()
{
  static _JOYSTICK joystick;
  return &joystick;
}

short LinuxInput::GetJoystickBufferCount()
{
  return 0;
}

void LinuxInput::GetJoystickMove(short Index, _JOYSTICK *buffer)
{

}

void LinuxInput::Uninitialize()
{

}

void LinuxInput::GetInputEvents()
{
  //  fprintf(stdout, "LinuxInput::GetInputEvents()\n");
  //   while (SDL_PollEvent(&event))
  //      {
  //      case SDL_KEYDOWN:
  //        printf("Key press detected\n");
  //        break;
  //  
  //      case SDL_KEYUP:
  //        printf("Key release detected\n");
  //        break;
  //
  //      case SDL_QUIT:
  //        printf("Quit detected\n");
  //       break;
  //  
  //      default:
  //        break;
  //  
  //      }
}
