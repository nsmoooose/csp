#ifndef __WININPUT_H__
#define __WININPUT_H__

#ifdef DirectInput

#include "Framework.h"



class WinInput: public StandardInput
{

  private:

    // functions
    static BOOL CALLBACK FindJoysticks(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
    static BOOL CALLBACK SetAxisParams(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);

    // members
    LPDIRECTINPUT8            p_Di;
    LPDIRECTINPUTDEVICE8      p_DiMouse;
    LPDIRECTINPUTDEVICE8      p_DiKeyboard;
    LPDIRECTINPUTDEVICE8      p_DiJoystick;
    DWORD                     p_dSubBufferSize;

    HANDLE                    p_hKeyboardHandle;
    DWORD                     p_dKeyboardBufferCount;
    _KEY_UNBUFFERED           p_KeyboardArray[256];
    DIDEVICEOBJECTDATA       *p_KeyboardBuffer;
    _KEY_BUFFERED             p_Keyboard;

    HANDLE                    p_hMouseHandle;
    DWORD                     p_dMouseBufferCount;
    DIDEVICEOBJECTDATA       *p_MouseBuffer;
    DIMOUSESTATE              p_MouseArray;
    _MOUSE                    p_Mouse;

    HANDLE                    p_hJoystickHandle;
    DIDEVICEOBJECTDATA       *p_JoystickBuffer;
    DWORD                     p_dJoystickBufferCount; 
    bool                      p_bAnalogJoystick;
    _JOYSTICK                 p_Joystick, p_DigitalBuffer;

    StandardFramework        *p_Framework;

    char              m_KeyMap[256];
    void              InitializeKeyMap();


  public:
    WinInput();
    ~WinInput();

    // functions
    short             Initialize(StandardFramework *Framework, _HANDLE WindowHandle, _INSTANCE AppInstance);

    void              Acquire();
    void              Unacquire();
    bool              GetInputMessage(bool Wait);

    short             SnapshotKeyboard();
    _KEY_UNBUFFERED*  GetKeyboardInstant();
    short             GetKeyboardBufferCount();
    void              GetKeyboardKeyPress(short Index, _KEY_BUFFERED *buffer);


    short             SnapshotMouse();
    _MOUSE*           GetMouseInstant();
    short             GetMouseBufferCount();
    void              GetMouseMove(short Index, _MOUSE *buffer);

    short             SnapshotJoystick();
    _JOYSTICK*        GetJoystickInstant();
    short             GetJoystickBufferCount();
    void              GetJoystickMove(short Index, _JOYSTICK *buffer);

    void              Uninitialize();

    bool              isKeyDown(_KEY_UNBUFFERED *key_array, unsigned char key)
    { return key_array[MapKey(key)];  }

    unsigned char     MapKey(char key)
    { return m_KeyMap[key];  }


};

#endif

#endif
