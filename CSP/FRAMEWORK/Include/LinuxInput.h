#include "Framework.h"

class LinuxInput : public StandardInput
{

  private:
    _MOUSE                    p_Mouse;

  public:
  LinuxInput();
    virtual ~LinuxInput();

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

    void              GetInputEvents();

    void              Uninitialize();
    //void*             GetWindow();
    //bool              IsAnalogJoystick();

};
