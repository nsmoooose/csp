#ifdef DirectInput

#include "DirectInput.h"

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
DirectInputClass::DirectInputClass()
{
  dim = 0;
  dik = 0;
  dij = 0;
  di = 0;

  MouseHandle = 0;
  KeyboardHandle = 0;
  JoystickHandle = 0;

  KeyboardBuffer = 0;
  MouseBuffer = 0;
  JoystickBuffer = 0;

  globals = 0;
  platform = 0;
}

DirectInputClass::~DirectInputClass()
{


}


// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  DirectInputClass::SetSetup(void *setup)
{
  globals = (_INPUT_INIT_STRUCT *)setup;
}

short DirectInputClass::GetDependencyCount()
{
  return 1;
}

void  DirectInputClass::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
  switch(Index)
  {
    case 0:
      Dependency->ID              = _PLATFORM_DEPENDENCY;
      Dependency->Description     = "It requires an instance and shutdown/windows handle.\0";
      Dependency->Required        = TRUE;
      break;
  }
}

short DirectInputClass::SetDependencyData(short ID, void* Data)
{
  switch(ID)
  {
    case _PLATFORM_DEPENDENCY:
      platform = (StandardPlatform *)Data;
      break;
    default:
      return _NO_DEPENDENCY_TO_SET;
      break;
  }
  return 0;
}

void  DirectInputClass::GetSetup(void*setup)
{
  setup = globals;
}

short DirectInputClass::CheckStatus()
{
  if(globals == 0)
  {
    return _SETUP_NOT_SET;
  }
  if(platform == 0)
  {
    return _DEPENDENCY_NOT_SET;
  }

  return 0;
}


//-----------------------------------------------------------------------------
// Name: EnumAxesCallback()
//-----------------------------------------------------------------------------
BOOL CALLBACK DirectInputClass::SetAxisParams( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                VOID* pContext )
{
  DirectInputClass *ourclass = (DirectInputClass *)pContext;

  DIPROPRANGE diprg; 
  diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
  diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
  diprg.diph.dwHow        = DIPH_BYID; 
  diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
  diprg.lMin              = -100; 
  diprg.lMax              = +100; 
    
	// Set the range for the axis
	ourclass->dij->SetProperty( DIPROP_RANGE, &diprg.diph );

  return DIENUM_CONTINUE;

}


// ------------------------------------------------
// Callback for seeing how many joysticks we have.
// ------------------------------------------------
BOOL CALLBACK DirectInputClass::FindJoysticks(const DIDEVICEINSTANCE*     
                                       pdidInstance, VOID* pContext)
{
    HRESULT hr;

    DirectInputClass *ourclass = (DirectInputClass *)pContext;

    // Obtain an interface to the enumerated joystick.
    hr = ourclass->di->CreateDevice(pdidInstance->guidInstance,  
                                &ourclass->dij, NULL);
    if(hr != 0)
    {
        return DIENUM_CONTINUE;
    }

    return DIENUM_STOP;
}

// --------------------------------------
// CreateInput
// Create the input device
// --------------------------------------
void DirectInputClass::CreateInput(short BufferSize)
{
  DIPROPDWORD dipdw;

  subBufferSize = BufferSize;

  DirectInput8Create((HINSTANCE)globals->this_inst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&di, NULL );

  //---
  di->CreateDevice( GUID_SysMouse,  &dim, NULL );

  dim->SetDataFormat( &c_dfDIMouse );

  dim->SetCooperativeLevel((HWND)platform->GetWindowHandle(),  DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

  memset(&dipdw, 0, sizeof(dipdw));

  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = subBufferSize;

  dim->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

  //---
  di->CreateDevice( GUID_SysKeyboard, &dik, NULL );

  dik->SetDataFormat( &c_dfDIKeyboard );

  dik->SetCooperativeLevel((HWND)platform->GetWindowHandle(),  DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY);

  memset(&dipdw, 0, sizeof(dipdw));

  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = subBufferSize;

  dik->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);


  //---
  di->EnumDevices(DI8DEVCLASS_GAMECTRL, FindJoysticks,
                       this, DIEDFL_ATTACHEDONLY);

  if(dij != 0)
  {
    dij->SetDataFormat(&c_dfDIJoystick2);

    dij->SetCooperativeLevel((HWND)platform->GetWindowHandle(), 
                      DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    //dij->EnumObjects( SetAxisParams, this, DIDFT_AXIS );

    DIDEVCAPS caps;

    memset(&caps, 0, sizeof(caps));

    caps.dwSize = sizeof(caps);

    dij->GetCapabilities(&caps);

    if(caps.dwFlags & DIDC_POLLEDDEVICE)
    {
      analogjoystick = TRUE;
    }

    DIPROPRANGE diprg;
  
    memset(&diprg, 0, sizeof(diprg));

    diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    diprg.diph.dwHow        = DIPH_DEVICE; 
    diprg.diph.dwObj        = 0;
    diprg.lMin              = -100; 
    diprg.lMax              = +100; 
    
	  // Set the range for the axis
	  dij->SetProperty( DIPROP_RANGE, &diprg.diph );

    memset(&dipdw, 0, sizeof(dipdw));

    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = subBufferSize;

    dij->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

    // set the deadzone on the joystick
    memset(&dipdw, 0, sizeof(dipdw));

    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = 1500;


    dij->SetProperty(DIPROP_DEADZONE , &dipdw.diph);
    dij->SetEventNotification(JoystickHandle);
  }

  //---
  MouseHandle = CreateEvent(NULL, FALSE, FALSE,NULL);
  KeyboardHandle = CreateEvent(NULL, FALSE,FALSE,NULL);
  JoystickHandle = CreateEvent(NULL, FALSE,FALSE,NULL);

  dim->SetEventNotification(MouseHandle);
  dik->SetEventNotification(KeyboardHandle);


  KeyboardBuffer = new DIDEVICEOBJECTDATA[subBufferSize];
  MouseBuffer = new DIDEVICEOBJECTDATA[subBufferSize];
  JoystickBuffer = new DIDEVICEOBJECTDATA[subBufferSize];

}

// --------------------------------------
// Acquire
// Turn on the input
// --------------------------------------
void DirectInputClass::Acquire()
{
  if(dim)dim->Acquire();
  if(dik)dik->Acquire();
  if(dij)dij->Acquire();

  return;
}

// --------------------------------------
// Unacquire
// Turn on the input
// --------------------------------------
void DirectInputClass::Unacquire()
{
  if(dim) dim->Unacquire();
  if(dik) dik->Unacquire();
  if(dij) dij->Unacquire();

  return;
}

// ---------------------------------------
// GetInputMessage
// Check for any new input.
// ---------------------------------------
short DirectInputClass::GetInputMessage(short Wait)
{

  BOOL flag;
  HANDLE handlelist[4];

  handlelist[0] = platform->GetShutdown();
  handlelist[1] = MouseHandle;
  handlelist[2] = KeyboardHandle;
  handlelist[3] = JoystickHandle;

  DWORD result;
  short number = 4;

  if(analogjoystick == TRUE)
  {
    number = 3;
  }

  //dim->Poll();
  //dik->Poll();
  //dij->Poll();

  if(Wait==TRUE)
  {
    result = WaitForMultipleObjects(number, handlelist, FALSE, INFINITE);
  }
  else
  {
    result = WaitForMultipleObjects(number, handlelist, FALSE, 0);
  }

  switch(result)
  {
    case WAIT_TIMEOUT:
    case WAIT_OBJECT_0:
      flag = FALSE;
      break;
    default:
      flag = TRUE;
  }



  return flag;
}

// ---------------------------------------------
// GetKeyboardBufferCount
// Get the number of keypresses in the buffer
// This will flush the buffer, so pull out of the Buffer
// the keys or they will be lost.
// ----------------------------------------------
short DirectInputClass::GetKeyboardBufferCount()
{

  HRESULT result;

  DWORD Items;

  Items = subBufferSize;

  result = dik->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), KeyboardBuffer, &Items, 0); 

  if(result != 0)
  {
    Items = _INPUT_LOST;
  }

  return (short)Items;
}

// ---------------------------------------------
// GetKeyboardBufferCount
// Get the number of keypresses in the buffer
// This will flush the buffer, so pull out of the Buffer
// the keys or they will be lost.  A negative value means the key was released.
// ----------------------------------------------
void DirectInputClass::GetKeyboardKeyPress(short Index, _KEY_STRUCT *buffer)
{
  memset(buffer, 0, sizeof(_KEY_STRUCT));

  buffer->key = (unsigned char)KeyboardBuffer[Index].dwOfs;

  if(KeyboardBuffer[Index].dwData != 0)
  {
    buffer->down = TRUE;
  }
  else
  {
    buffer->down = FALSE;
  }

  return;
}

// ---------------------------------------------
// GetKeyboardBufferCount
// Get the number of keypresses in the buffer
// This will flush the buffer, so pull out of the Buffer
// the keys or they will be lost.
// ----------------------------------------------
short DirectInputClass::GetMouseBufferCount()
{

  HRESULT result;

  DWORD Items;

  Items = subBufferSize;

  result = dim->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), MouseBuffer, &Items, 0); 

  if(result != 0)
  {
    Items = _INPUT_LOST;
  }

  return (short)Items;
}

// ---------------------------------------------
// GetKeyboardBufferCount
// Get the number of keypresses in the buffer
// This will flush the buffer, so pull out of the Buffer
// the keys or they will be lost.  A negative value means the key was released.
// ----------------------------------------------
void DirectInputClass::GetMouseMove(short Index, _MOUSE_STRUCT *buffer)
{
  memset(buffer, 0, sizeof(_MOUSE_STRUCT));

  switch (MouseBuffer[Index].dwOfs) 
  {
      case DIMOFS_X: 
          buffer->x = (char)MouseBuffer[Index].dwData; 
          break;
      case DIMOFS_Y:
          buffer->y = (char)MouseBuffer[Index].dwData; 
          break; 
      case DIMOFS_Z:
          buffer->z = (char)MouseBuffer[Index].dwData; 
          break;
      case DIMOFS_BUTTON0:
          if (MouseBuffer[Index].dwData & 0x80)
          {
            buffer->mouse1 = TRUE; 
          }
          break;
      case DIMOFS_BUTTON1:
          if (MouseBuffer[Index].dwData & 0x80)
          {
            buffer->mouse2 = TRUE; 
          }
          break;
      case DIMOFS_BUTTON2:
          if (MouseBuffer[Index].dwData & 0x80)
          {
            buffer->mouse3 = TRUE; 
          }
          break;
  }
 
  return;
}

// ---------------------------------------------
// GetJoystickBufferCount

// ----------------------------------------------
short DirectInputClass::GetJoystickBufferCount()
{

  if(analogjoystick == TRUE)
  {
    return _NO_DIGITAL_JOYSTICK;
  }

  if(dij == NULL)
  {
    return _NO_JOYSTICK_FOUND;
  }

  DWORD Items;

  Items = subBufferSize;

  Error.lError = dij->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), JoystickBuffer, &Items, 0); 

  switch(Error.lError)
  {
    case 0:
      break;
    case DI_BUFFEROVERFLOW:
      Items = _BUFFER_OVERFLOW;
      break;
    default:
      Items = _INPUT_LOST;
      break;
  }

  return (short)Items;
}

// ---------------------------------------------
// GetJoystickBufferCount
// ----------------------------------------------
void DirectInputClass::GetJoystickMove(short Index, _JOYSTICK_STRUCT *buffer)
{
  memset(buffer, 0, sizeof(_JOYSTICK_STRUCT));

  switch (JoystickBuffer[Index].dwOfs) 
  {
      case DIJOFS_X: 
          buffer->x = (char)JoystickBuffer[Index].dwData; 
          break;
      case DIJOFS_Y:
          buffer->y = (char)JoystickBuffer[Index].dwData; 
          break; 
      case DIJOFS_BUTTON0:
          if (JoystickBuffer[Index].dwData & 0x80)
          {
            buffer->button1 = TRUE; 
          }
          break;
      case DIJOFS_BUTTON1:
          if (JoystickBuffer[Index].dwData & 0x80)
          {
            buffer->button2 = TRUE; 
          }
          break;
      case DIJOFS_BUTTON2:
          if (JoystickBuffer[Index].dwData & 0x80)
          {
            buffer->button3 = TRUE; 
          }
          break;
      case DIJOFS_BUTTON3:
          if (JoystickBuffer[Index].dwData & 0x80)
          {
            buffer->button4 = TRUE; 
          }
          break;
      case DIJOFS_POV(0):

          if(JoystickBuffer[Index].dwData != -1)
          {
            buffer->hat = (short)JoystickBuffer[Index].dwData / 100;
          }
          else
          {
            buffer->hat = -1;
          }
          break;
      case DIJOFS_SLIDER(0): 
        buffer->throttle = (char)JoystickBuffer[Index].dwData; 
        break;
  }
  return;
}


// ---------------------------------------------
// GetJoystickAll
// Returns the current joystick. Automatically does analog/digital.
// ----------------------------------------------
_JOYSTICK_STRUCT DirectInputClass::GetJoystickAll()
{
  
  _JOYSTICK_STRUCT buffer, digitalbuffer;

  memset(&buffer, 0, sizeof(buffer));
  memset(&digitalbuffer, 0, sizeof(digitalbuffer));

  if(dij == NULL)
  {
    buffer.hat = -1;
    buffer.throttle = 100;
    return buffer;
  }

  if(analogjoystick == TRUE)
  { // poll and retrieve analog results.
    DIJOYSTATE2 js;
    memset(&js, 0, sizeof(js));

    dij->Poll();
    dij->GetDeviceState(sizeof(DIJOYSTATE2), &js);

    buffer.x = (char)js.lX;
    buffer.y = (char)js.lY;

    if (js.rgbButtons[0] & 0x80)
    {
      buffer.button1 = TRUE; 
    }
    if (js.rgbButtons[1] & 0x80)
    {
      buffer.button2 = TRUE; 
    }
    if (js.rgbButtons[2] & 0x80)
    {
      buffer.button3 = TRUE; 
    }
    if (js.rgbButtons[3] & 0x80)
    {
      buffer.button4 = TRUE; 
    }
    if(js.rgdwPOV[0] != -1)
    {
      buffer.hat = (short)js.rgdwPOV[0] / 100;
    }
    else
    {
      buffer.hat = -1;
    }
    buffer.throttle = (char)js.rglSlider[0];
  }
  else
  {  // do the digital
    short num, indx;
    num = GetJoystickBufferCount();
    for(indx = 0; indx < num; indx++)
    {
      GetJoystickMove(indx, &digitalbuffer);
      buffer.x += digitalbuffer.x;
      buffer.y += digitalbuffer.y;
      if(digitalbuffer.button1 == TRUE)
      {
        buffer.button1 = TRUE;
      }
      if(digitalbuffer.button1 == TRUE)
      {
        buffer.button2 = TRUE;
      }
      if(digitalbuffer.button2 == TRUE)
      {
        buffer.button3 = TRUE;
      }
      if(digitalbuffer.button3 == TRUE)
      {
        buffer.button3 = TRUE;
      }
      buffer.hat += digitalbuffer.hat;
    }
  }

  return buffer;
}

void DirectInputClass::DeleteInput()
{

  Unacquire();

  if(MouseHandle != 0)
  {
    CloseHandle(MouseHandle);
  }
  if(KeyboardHandle != 0)
  {
    CloseHandle(KeyboardHandle);
  }
  if(JoystickHandle != 0)
  {
    CloseHandle(JoystickHandle);
  }

  _DEL(JoystickBuffer);
  _DEL(KeyboardBuffer);
  _DEL(MouseBuffer);

  _REL(dij);
  _REL(dim);
  _REL(dik);
  _REL(di);

  return;
}

#endif