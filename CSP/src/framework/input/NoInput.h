// NoInputClass specific for StandardInput

#include "input/StandardInput.h"
#include "platform/StandardPlatform.h"

class NoInputClass: public StandardInput
{

private:


public:
  NoInputClass();
  ~NoInputClass();

  // Standard Framework - required for every framework object
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();

  // Standard Input
  void CreateInput(short BufferSize);
  void Acquire();
  void Unacquire();
  short GetInputMessage(short Wait);
  short GetKeyboardBufferCount();
  void GetKeyboardKeyPress(short Index, _KEY_STRUCT *buffer);
  short GetMouseBufferCount();
  void GetMouseMove(short Index, _MOUSE_STRUCT *buffer);
  short GetJoystickBufferCount();
  void GetJoystickMove(short Index, _JOYSTICK_STRUCT *buffer);
  _JOYSTICK_STRUCT GetJoystickAll();
  void DeleteInput();

};
