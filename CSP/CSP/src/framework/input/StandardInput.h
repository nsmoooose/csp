// The pure abstract class for Input.

#ifndef SITag

  #define SITag

  #include "TypesInput.h"

  class StandardInput: public StandardFramework
  {

  private:

  public:

    virtual void              CreateInput(short BufferSize) = 0;

    virtual void              Acquire() = 0;
    virtual void              Unacquire() = 0;

    virtual short             GetInputMessage(short Wait) = 0;
    virtual short             GetKeyboardBufferCount() = 0;
    virtual void              GetKeyboardKeyPress(short Index, _KEY_STRUCT *buffer) = 0;
    virtual short             GetMouseBufferCount() = 0;
    virtual void              GetMouseMove(short Index, _MOUSE_STRUCT *buffer) = 0;
    virtual short             GetJoystickBufferCount() = 0;
    virtual void              GetJoystickMove(short Index, _JOYSTICK_STRUCT *buffer) = 0;
    virtual _JOYSTICK_STRUCT  GetJoystickAll() = 0;

    virtual void              DeleteInput() = 0;

  };

#endif
