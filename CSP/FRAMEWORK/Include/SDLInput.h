#include "Framework.h"

class SDLInput : public StandardInput
{

  private:
    _MOUSE                    p_Mouse;

    SDL_Event  * keyboardEventQueue;
    int keyboardEventCount;

    SDL_Event  * mouseEventQueue;
    int mouseEventCount;

    SDL_Joystick * m_joystick;

  public:
  SDLInput();
    virtual ~SDLInput();

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
    //void*             GetWindow();
    //bool              IsAnalogJoystick();

    unsigned char     MapKey(char key) 
    { return key; }


    bool              isKeyDown(_KEY_UNBUFFERED *key_array, unsigned char key)
          { return key_array[MapKey(key)];  }



};
