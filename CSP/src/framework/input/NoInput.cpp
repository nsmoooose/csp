#include "NoInput.h"

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
NoInputClass::NoInputClass()
{
}

NoInputClass::~NoInputClass()
{
}

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  NoInputClass::SetSetup(void *setup)
{
}

short NoInputClass::GetDependencyCount()
{
  return 0;
}

void  NoInputClass::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{

}

short NoInputClass::SetDependencyData(short ID, void* Data)
{
  switch(ID)
  {
    case _PLATFORM_DEPENDENCY:
      break;
    default:
      return _NO_DEPENDENCY_TO_SET;
      break;
  }
  return 0;
}

void  NoInputClass::GetSetup(void*setup)
{

}

short NoInputClass::CheckStatus()
{
  return 0;
}


// --------------------------------------
// CreateInput
// Create the input device
// --------------------------------------
void NoInputClass::CreateInput(short BufferSize)
{

}

// --------------------------------------
// Acquire
// Turn on the input
// --------------------------------------
void NoInputClass::Acquire()
{

  return;
}

// --------------------------------------
// Unacquire
// Turn on the input
// --------------------------------------
void NoInputClass::Unacquire()
{

  return;
}

// ---------------------------------------
// GetInputMessage
// Check for any new input.
// ---------------------------------------
short NoInputClass::GetInputMessage(short Wait)
{

  return FALSE;
}

// ---------------------------------------------
// GetKeyboardBufferCount
// Get the number of keypresses in the buffer
// This will flush the buffer, so pull out of the Buffer
// the keys or they will be lost.
// ----------------------------------------------
short NoInputClass::GetKeyboardBufferCount()
{

  return 0;
}

// ---------------------------------------------
// GetKeyboardBufferCount
// Get the number of keypresses in the buffer
// This will flush the buffer, so pull out of the Buffer
// the keys or they will be lost.  A negative value means the key was released.
// ----------------------------------------------
void NoInputClass::GetKeyboardKeyPress(short Index, _KEY_STRUCT *buffer)
{
  memset(buffer, 0, sizeof(_KEY_STRUCT));

  return;
}

// ---------------------------------------------
// GetKeyboardBufferCount
// Get the number of keypresses in the buffer
// This will flush the buffer, so pull out of the Buffer
// the keys or they will be lost.
// ----------------------------------------------
short NoInputClass::GetMouseBufferCount()
{

  return 0;
}

// ---------------------------------------------
// GetKeyboardBufferCount
// Get the number of keypresses in the buffer
// This will flush the buffer, so pull out of the Buffer
// the keys or they will be lost.  A negative value means the key was released.
// ----------------------------------------------
void NoInputClass::GetMouseMove(short Index, _MOUSE_STRUCT *buffer)
{
  memset(buffer, 0, sizeof(_MOUSE_STRUCT));
 
  return;
}

// ---------------------------------------------
// GetJoystickBufferCount

// ----------------------------------------------
short NoInputClass::GetJoystickBufferCount()
{

  return 0;
}

// ---------------------------------------------
// GetJoystickBufferCount
// ----------------------------------------------
void NoInputClass::GetJoystickMove(short Index, _JOYSTICK_STRUCT *buffer)
{
  memset(buffer, 0, sizeof(_JOYSTICK_STRUCT));

  return;
}


// ---------------------------------------------
// GetJoystickAll
// Returns the current joystick. Automatically does analog/digital.
// ----------------------------------------------
_JOYSTICK_STRUCT NoInputClass::GetJoystickAll()
{
  
  _JOYSTICK_STRUCT buffer;

  memset(&buffer, 0, sizeof(buffer));

  buffer.hat = -1;

  return buffer;
}

void NoInputClass::DeleteInput()
{

  return;
}
