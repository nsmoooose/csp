#include "Framework.h"

class StandardInput
{

  private:

  public:
 
    virtual ~StandardInput() {};

    // functions
    virtual short             Initialize(StandardFramework *Framework, _HANDLE WindowHandle, _INSTANCE AppInstance) = 0;

    virtual void              Acquire() = 0;
    virtual void              Unacquire() = 0;
    virtual bool              GetInputMessage(bool Wait) = 0;

    virtual short             SnapshotKeyboard() = 0;
    virtual _KEY_UNBUFFERED*  GetKeyboardInstant() = 0;
    virtual short             GetKeyboardBufferCount() = 0;
	virtual void              GetKeyboardKeyPress(short Index, _KEY_BUFFERED *buffer) = 0;
	virtual unsigned char     MapKey(char key) = 0;

    virtual short             SnapshotMouse() = 0;
    virtual _MOUSE*           GetMouseInstant() = 0;
    virtual short             GetMouseBufferCount() = 0;
    virtual void              GetMouseMove(short Index, _MOUSE *buffer) = 0;

    virtual short             SnapshotJoystick() = 0;
    virtual _JOYSTICK*        GetJoystickInstant() = 0;
    virtual short             GetJoystickBufferCount() = 0;
    virtual void              GetJoystickMove(short Index, _JOYSTICK *buffer) = 0;

    virtual void              Uninitialize() = 0;

    virtual bool              isKeyDown(_KEY_UNBUFFERED *key_array, unsigned char key) = 0;

};
