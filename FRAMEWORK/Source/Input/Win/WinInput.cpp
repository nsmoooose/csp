#ifdef DirectInput

#include "WinInput.h"


WinInput::WinInput()
{
  p_DiMouse = 0;
  p_DiKeyboard = 0;
  p_DiJoystick = 0;
  p_Di = 0;

  p_hMouseHandle = 0;
  p_hKeyboardHandle = 0;
  p_hJoystickHandle = 0;

  p_KeyboardBuffer = 0;
  p_MouseBuffer = 0;
  p_JoystickBuffer = 0;

  InitializeKeyMap();
}

WinInput::~WinInput()
{
  Uninitialize();
}


//-----------------------------------------------------------------------------
// Name: EnumAxesCallback()
//-----------------------------------------------------------------------------
BOOL CALLBACK WinInput::SetAxisParams( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                VOID* pContext )
{
  WinInput *ourclass = (WinInput *)pContext;

  DIPROPRANGE diprg; 
  diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
  diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
  diprg.diph.dwHow        = DIPH_BYID; 
  diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
  diprg.lMin              = -100; 
  diprg.lMax              = +100; 
    
	// Set the range for the axis
	ourclass->p_DiJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );

  return DIENUM_CONTINUE;

}


// ------------------------------------------------
// Callback for seeing how many joysticks we have.
// ------------------------------------------------
BOOL CALLBACK WinInput::FindJoysticks(const DIDEVICEINSTANCE*     
                                       pdidInstance, VOID* pContext)
{
    HRESULT hr;

    WinInput *ourclass = (WinInput *)pContext;

    // Obtain an interface to the enumerated joystick.
    hr = ourclass->p_Di->CreateDevice(pdidInstance->guidInstance,  
                                &ourclass->p_DiJoystick, NULL);
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
short WinInput::Initialize(StandardFramework *Framework, _HANDLE WindowHandle, _INSTANCE AppInstance)
{

  p_Framework = Framework;

  //-------

  DIPROPDWORD dipdw;

  p_dSubBufferSize = 16;

  DirectInput8Create((HINSTANCE)AppInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&p_Di, NULL );

  //---
  p_Di->CreateDevice( GUID_SysMouse,  &p_DiMouse, NULL );

  p_DiMouse->SetDataFormat( &c_dfDIMouse );

  p_DiMouse->SetCooperativeLevel((HWND)WindowHandle, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

  memset(&dipdw, 0, sizeof(dipdw));

  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = p_dSubBufferSize;

  p_DiMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

  //---
  p_Di->CreateDevice( GUID_SysKeyboard, &p_DiKeyboard, NULL );

  p_DiKeyboard->SetDataFormat( &c_dfDIKeyboard );

  p_DiKeyboard->SetCooperativeLevel((HWND)WindowHandle,  DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY);

  memset(&dipdw, 0, sizeof(dipdw));

  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = p_dSubBufferSize;

  p_DiKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);


  //---
  p_Di->EnumDevices(DI8DEVCLASS_GAMECTRL, FindJoysticks,
                       this, DIEDFL_ATTACHEDONLY);

  if(p_DiJoystick != 0)
  {
    p_DiJoystick->SetDataFormat(&c_dfDIJoystick2);

    p_DiJoystick->SetCooperativeLevel((HWND)WindowHandle, 
                      DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    //dij->EnumObjects( SetAxisParams, this, DIDFT_AXIS );

    DIDEVCAPS caps;

    memset(&caps, 0, sizeof(caps));

    caps.dwSize = sizeof(caps);

    p_DiJoystick->GetCapabilities(&caps);

    if(caps.dwFlags & DIDC_POLLEDDEVICE)
    {
      p_bAnalogJoystick = true;
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
	  p_DiJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );

    memset(&dipdw, 0, sizeof(dipdw));

    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = p_dSubBufferSize;

    p_DiJoystick->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

    // set the deadzone on the joystick
    memset(&dipdw, 0, sizeof(dipdw));

    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = 1500;

    p_DiJoystick->SetProperty(DIPROP_DEADZONE , &dipdw.diph);
    p_DiJoystick->SetEventNotification(p_hJoystickHandle);
  }

  //---
  p_hMouseHandle = CreateEvent(NULL, FALSE, FALSE,NULL);
  p_hKeyboardHandle = CreateEvent(NULL, FALSE,FALSE,NULL);
  p_hJoystickHandle = CreateEvent(NULL, FALSE,FALSE,NULL);

  p_DiMouse->SetEventNotification(p_hMouseHandle);
  p_DiKeyboard->SetEventNotification(p_hKeyboardHandle);

  p_KeyboardBuffer = new DIDEVICEOBJECTDATA[p_dSubBufferSize];
  p_MouseBuffer = new DIDEVICEOBJECTDATA[p_dSubBufferSize];
  p_JoystickBuffer = new DIDEVICEOBJECTDATA[p_dSubBufferSize];

  return 0;

}

// --------------------------------------
// Acquire
// Turn on the input
// --------------------------------------
void WinInput::Acquire()
{
  if(p_DiMouse) p_DiMouse->Acquire();
  if(p_DiKeyboard) p_DiKeyboard->Acquire();
  if(p_DiJoystick) p_DiJoystick->Acquire();

  return;
}

// --------------------------------------
// Unacquire
// Turn on the input
// --------------------------------------
void WinInput::Unacquire()
{
  if(p_DiMouse) p_DiMouse->Unacquire();
  if(p_DiKeyboard) p_DiKeyboard->Unacquire();
  if(p_DiJoystick) p_DiJoystick->Unacquire();

  return;
}

// ---------------------------------------
// GetInputMessage
// Check for any new input.
// ---------------------------------------
bool WinInput::GetInputMessage(bool Wait)
{

  bool flag;
  HANDLE handlelist[4];

  handlelist[0] = (HANDLE)p_Framework->GetShutdownHandle();
  handlelist[1] = p_hMouseHandle;
  handlelist[2] = p_hKeyboardHandle;
  handlelist[3] = p_hJoystickHandle;

  DWORD result;
  short number = 4;

  if(p_bAnalogJoystick == true)
  {
    number = 3;
  }

  if(p_DiJoystick != 0)
  {
    p_DiJoystick->Poll();
  }
  p_DiKeyboard->Poll();
  p_DiMouse->Poll();

  if(Wait==true)
  {
    // UGH: Word of warning. Having the window that loses focus
    // with wait on will basically lockup the input system.
    // I'd avoid the wait flag.
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
      flag = false;
      break;
    default:
      flag = true;
  }



  return flag;
}

short WinInput::SnapshotKeyboard()
{

  HRESULT result;

  DWORD Items;

  Items = p_dSubBufferSize;

  result = p_DiKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), p_KeyboardBuffer, &Items, 0); 

  if(result != 0)
  {
    Items = _INPUT_LOST;
    p_dKeyboardBufferCount = 0;
  }
  else
  {
    p_dKeyboardBufferCount = Items;
    Items = _A_OK;
  }

  return (short)Items;
}

_KEY_UNBUFFERED* WinInput::GetKeyboardInstant()
{
  p_DiKeyboard->GetDeviceState(sizeof(p_KeyboardArray), p_KeyboardArray);
  return p_KeyboardArray;
}

// ---------------------------------------------
// GetKeyboardBufferCount
// Get the number of keypresses in the buffer
// This will flush the buffer, so pull out of the Buffer
// the keys or they will be lost.
// ----------------------------------------------
short WinInput::GetKeyboardBufferCount()
{
  return (short)p_dKeyboardBufferCount;
}

// ---------------------------------------------
// GetKeyboardBufferCount
// Get the number of keypresses in the buffer
// This will flush the buffer, so pull out of the Buffer
// the keys or they will be lost.  A negative value means the key was released.
// ----------------------------------------------
void WinInput::GetKeyboardKeyPress(short Index, _KEY_BUFFERED *buffer)
{
  memset(buffer, 0, sizeof(_KEY_BUFFERED));

  buffer->key = (unsigned char)p_KeyboardBuffer[Index].dwOfs;

  if(p_KeyboardBuffer[Index].dwData != 0)
  {
    buffer->down = TRUE;
  }
  else
  {
    buffer->down = FALSE;
  }

  return;
}

short WinInput::SnapshotMouse()
{

  HRESULT result;

  DWORD Items;

  Items = p_dSubBufferSize;

  result = p_DiMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), p_MouseBuffer, &Items, 0); 

  if(result != 0)
  {
    Items = _INPUT_LOST;
    p_dMouseBufferCount = 0;
  }
  else
  {
    p_dMouseBufferCount = Items;
    Items = _A_OK;
  }

  return (short)Items;
}

_MOUSE* WinInput::GetMouseInstant()
{
  p_DiMouse->GetDeviceState(sizeof(p_MouseArray), &p_MouseArray);

  memset(&p_Mouse, 0, sizeof(p_Mouse));

  p_Mouse.x = (char)p_MouseArray.lX;
  p_Mouse.y = (char)p_MouseArray.lY;
  p_Mouse.z = (char)p_MouseArray.lZ;

  if(p_MouseArray.rgbButtons[0] != 0)
  {
    p_Mouse.mouse1 = true;
  }

  if(p_MouseArray.rgbButtons[1] != 0)
  {
    p_Mouse.mouse2 = true;
  }

  if(p_MouseArray.rgbButtons[2] != 0)
  {
    p_Mouse.mouse3 = true;
  }

  if(p_MouseArray.rgbButtons[3] != 0)
  {
    //p_Mouse.mouse1 = true;
  }

  return &p_Mouse;
}

short WinInput::GetMouseBufferCount()
{
  return (short)p_dMouseBufferCount;
}

void WinInput::GetMouseMove(short Index, _MOUSE *buffer)
{
  memset(buffer, 0, sizeof(_MOUSE));

  switch (p_MouseBuffer[Index].dwOfs) 
  {
      case DIMOFS_X: 
          buffer->x = (char)p_MouseBuffer[Index].dwData; 
          break;
      case DIMOFS_Y:
          buffer->y = (char)p_MouseBuffer[Index].dwData; 
          break; 
      case DIMOFS_Z:
          buffer->z = (char)p_MouseBuffer[Index].dwData; 
          break;
      case DIMOFS_BUTTON0:
          if (p_MouseBuffer[Index].dwData & 0x80)
          {
            buffer->mouse1 = TRUE; 
          }
          break;
      case DIMOFS_BUTTON1:
          if (p_MouseBuffer[Index].dwData & 0x80)
          {
            buffer->mouse2 = TRUE; 
          }
          break;
      case DIMOFS_BUTTON2:
          if (p_MouseBuffer[Index].dwData & 0x80)
          {
            buffer->mouse3 = TRUE; 
          }
          break;
  }
 
  return;
}

short WinInput::SnapshotJoystick()
{

  if(p_bAnalogJoystick == true)
  {
    return _NO_DIGITAL_JOYSTICK;
  }

  if(p_DiJoystick == NULL)
  {
    return _NO_JOYSTICK_FOUND;
  }

  HRESULT result;

  DWORD Items;

  Items = p_dSubBufferSize;

  result = p_DiJoystick->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), p_JoystickBuffer, &Items, 0); 

  if(result != 0)
  {
    Items = _INPUT_LOST;
    p_dJoystickBufferCount = 0;
  }
  else
  {
    p_dJoystickBufferCount = Items;
    Items = _A_OK;
  }

  return (short)Items;
}

// ---------------------------------------------
// GetJoystickBufferCount
// ----------------------------------------------
short WinInput::GetJoystickBufferCount()
{
  return (short)p_dJoystickBufferCount;
}

// ---------------------------------------------
// GetJoystickBufferCount
// ----------------------------------------------
void WinInput::GetJoystickMove(short Index, _JOYSTICK *buffer)
{
  memset(buffer, 0, sizeof(_JOYSTICK));

  switch (p_JoystickBuffer[Index].dwOfs) 
  {
      case DIJOFS_X: 
          buffer->x = (char)p_JoystickBuffer[Index].dwData; 
          break;
      case DIJOFS_Y:
          buffer->y = (char)p_JoystickBuffer[Index].dwData; 
          break; 
      case DIJOFS_RZ:
          buffer->z = (char)p_JoystickBuffer[Index].dwData; 
          break; 
      case DIJOFS_BUTTON0:
          if (p_JoystickBuffer[Index].dwData & 0x80)
          {
            buffer->button1 = TRUE; 
          }
          break;
      case DIJOFS_BUTTON1:
          if (p_JoystickBuffer[Index].dwData & 0x80)
          {
            buffer->button2 = TRUE; 
          }
          break;
      case DIJOFS_BUTTON2:
          if (p_JoystickBuffer[Index].dwData & 0x80)
          {
            buffer->button3 = TRUE; 
          }
          break;
      case DIJOFS_BUTTON3:
          if (p_JoystickBuffer[Index].dwData & 0x80)
          {
            buffer->button4 = TRUE; 
          }
          break;
      case DIJOFS_POV(0):
          if(p_JoystickBuffer[Index].dwData != -1)
          {
            buffer->hat = (short)p_JoystickBuffer[Index].dwData / 100;
          }
          else
          {
            buffer->hat = -1;
          }
          break;
      case DIJOFS_SLIDER(0): 
        buffer->throttle = (char)p_JoystickBuffer[Index].dwData; 
        break;
  }
  return;
}


// ---------------------------------------------
// GetJoystickAll
// Returns the current joystick. Automatically does analog/digital.
// ----------------------------------------------
_JOYSTICK* WinInput::GetJoystickInstant()
{
  
  memset(&p_Joystick, 0, sizeof(p_Joystick));
  memset(&p_DigitalBuffer, 0, sizeof(p_DigitalBuffer));

  if(p_DiJoystick == NULL)
  {
    p_Joystick.hat = -1;
    p_Joystick.throttle = 100;
    return &p_Joystick;
  }

  //if(p_bAnalogJoystick == true)
  //{ // poll and retrieve analog results.
    DIJOYSTATE2 js;
    memset(&js, 0, sizeof(js));

    p_DiJoystick->Poll();
    p_DiJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &js);

    p_Joystick.x = (char)js.lX;
    p_Joystick.y = (char)js.lY;
    p_Joystick.z = (char)js.lRz;

    if (js.rgbButtons[0] & 0x80)
    {
      p_Joystick.button1 = true; 
    }
    if (js.rgbButtons[1] & 0x80)
    {
      p_Joystick.button2 = true; 
    }
    if (js.rgbButtons[2] & 0x80)
    {
      p_Joystick.button3 = true; 
    }
    if (js.rgbButtons[3] & 0x80)
    {
      p_Joystick.button4 = true; 
    }
    if(js.rgdwPOV[0] != -1)
    {
      p_Joystick.hat = (short)js.rgdwPOV[0] / 100;
    }
    else
    {
      p_Joystick.hat = -1;
    }
    p_Joystick.throttle = (char)js.rglSlider[0];
  //}
  /*
  else
  {  // do the digital
    short num, indx;
    SnapshotJoystick();
    num = GetJoystickBufferCount();
    for(indx = 0; indx < num; indx++)
    {
      GetJoystickMove(indx, &p_DigitalBuffer);
      p_Joystick.x += p_DigitalBuffer.x;
      p_Joystick.y += p_DigitalBuffer.y;
      if(p_DigitalBuffer.button1 == true)
      {
        p_Joystick.button1 = true;
      }
      if(p_DigitalBuffer.button1 == true)
      {
        p_Joystick.button2 = true;
      }
      if(p_DigitalBuffer.button2 == true)
      {
        p_Joystick.button3 = true;
      }
      if(p_DigitalBuffer.button3 == true)
      {
        p_Joystick.button3 = true;
      }
      p_Joystick.hat += p_DigitalBuffer.hat;
    }
  }
  */

  return &p_Joystick;
}
void WinInput::Uninitialize()
{

  Unacquire();

  if(p_hMouseHandle != 0)
  {
    CloseHandle(p_hMouseHandle);
    p_hMouseHandle = 0;
  }
  if(p_hKeyboardHandle != 0)
  {
    CloseHandle(p_hKeyboardHandle);
    p_hKeyboardHandle = 0;
  }
  if(p_hJoystickHandle != 0)
  {
    CloseHandle(p_hJoystickHandle);
    p_hJoystickHandle = 0;
  }

  if(p_JoystickBuffer != 0)
  {
    delete []p_JoystickBuffer;
    p_JoystickBuffer = 0;
  }

  if(p_KeyboardBuffer != 0)
  {
    delete []p_KeyboardBuffer;
    p_KeyboardBuffer = 0;
  }

  if(p_MouseBuffer != 0)
  {
    delete []p_MouseBuffer;
    p_MouseBuffer = 0;
  }

  if(p_DiJoystick != 0)
  {
    p_DiJoystick->Release();
    p_DiJoystick = 0;
  }

  if(p_DiMouse != 0)
  {
    p_DiMouse->Release();
    p_DiMouse = 0;
  }

  if(p_DiKeyboard != 0)
  {
    p_DiKeyboard->Release();
    p_DiKeyboard = 0;
  }

  if(p_Di != 0)
  {
    p_Di->Release();
    p_Di = 0;
  }

  return;
}

void WinInput::InitializeKeyMap()
{
    memset(m_KeyMap, 0, 256);
    m_KeyMap['0'] = DIK_0;
    m_KeyMap['1'] = DIK_1;
    m_KeyMap['2'] = DIK_2;
    m_KeyMap['3'] = DIK_3;
    m_KeyMap['4'] = DIK_4;
    m_KeyMap['5'] = DIK_5;
    m_KeyMap['6'] = DIK_6;
    m_KeyMap['7'] = DIK_7;
    m_KeyMap['8'] = DIK_8;
    m_KeyMap['9'] = DIK_9;
	m_KeyMap['a'] = m_KeyMap['A'] = DIK_A;
	m_KeyMap['b'] = m_KeyMap['B'] = DIK_B;
	m_KeyMap['c'] = m_KeyMap['C'] = DIK_C;
	m_KeyMap['d'] = m_KeyMap['D'] = DIK_D;
	m_KeyMap['e'] = m_KeyMap['E'] = DIK_E;
	m_KeyMap['f'] = m_KeyMap['F'] = DIK_F;
	m_KeyMap['g'] = m_KeyMap['G'] = DIK_G;
	m_KeyMap['h'] = m_KeyMap['H'] = DIK_H;
	m_KeyMap['i'] = m_KeyMap['I'] = DIK_I;
	m_KeyMap['j'] = m_KeyMap['J'] = DIK_J;
	m_KeyMap['k'] = m_KeyMap['K'] = DIK_K;
	m_KeyMap['l'] = m_KeyMap['L'] = DIK_L;
	m_KeyMap['m'] = m_KeyMap['M'] = DIK_M;
	m_KeyMap['n'] = m_KeyMap['N'] = DIK_N;
	m_KeyMap['o'] = m_KeyMap['O'] = DIK_O;
	m_KeyMap['p'] = m_KeyMap['P'] = DIK_P;
	m_KeyMap['q'] = m_KeyMap['Q'] = DIK_Q;
	m_KeyMap['r'] = m_KeyMap['R'] = DIK_R;
	m_KeyMap['s'] = m_KeyMap['S'] = DIK_S;
	m_KeyMap['t'] = m_KeyMap['T'] = DIK_T;
	m_KeyMap['u'] = m_KeyMap['U'] = DIK_U;
	m_KeyMap['v'] = m_KeyMap['V'] = DIK_V;
	m_KeyMap['w'] = m_KeyMap['W'] = DIK_W;
	m_KeyMap['x'] = m_KeyMap['X'] = DIK_X;
	m_KeyMap['y'] = m_KeyMap['Y'] = DIK_Y;
	m_KeyMap['z'] = m_KeyMap['Z'] = DIK_Z;

}



/*
bool  WinInput::IsAnalogJoystick()
{
  return p_bAnalogJoystick;
}
*/


#endif
