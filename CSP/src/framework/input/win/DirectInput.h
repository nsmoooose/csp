// DirectInput specific for StandardInput

#include "input/StandardInput.h"
#include "platform/StandardPlatform.h"

#include <dinput.h>

class DirectInputClass: public StandardInput
{

private:
    LPDIRECTINPUT8            di;
    LPDIRECTINPUTDEVICE8      dim;
    LPDIRECTINPUTDEVICE8      dik;
    LPDIRECTINPUTDEVICE8      dij;
    HANDLE                    MouseHandle;
    HANDLE                    KeyboardHandle;
    HANDLE                    JoystickHandle;
    DWORD                     subBufferSize;
    bool                      analogjoystick;
    DIDEVICEOBJECTDATA        *KeyboardBuffer;
    DIDEVICEOBJECTDATA        *MouseBuffer;
    DIDEVICEOBJECTDATA        *JoystickBuffer;

    static BOOL CALLBACK FindJoysticks(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
    static BOOL CALLBACK SetAxisParams(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);

    _INPUT_INIT_STRUCT  *globals;
    FrameworkError       Error;
    StandardPlatform    *platform;

public:
  DirectInputClass();
  ~DirectInputClass();

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