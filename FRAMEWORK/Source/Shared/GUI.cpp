#include "GUI.h"

/** GUI()
 * Default Contructor the the GUI class.
 * Class members are initialized.
 */
GUI::GUI()
{
  p_Input = 0;
  p_3D = 0;

  p_ActiveScreen = 0;

  p_NoGUI.Button = -1;
  p_NoGUI.Window = -1;
  p_NoGUI.Screen = -1;

  p_MouseDown = false;

  p_Shift = false;
  p_Alt = false;
  p_Ctrl = false;

}

/** ~GUI()
 * Destructor for the GUI class.
 */
GUI::~GUI()
{
  Uninitialize();
}

/** Initialize()
 * Initializer for the GUI class. 
 * This routine takes pointers to App, Window3D,
 * Framework, Input and Graphics objects and 
 * stores them for the object.
 */
short GUI::Initialize(StandardApp *App, 
                      StandardWindow3D *Window3D, 
                      StandardFramework *Framework,
                      StandardInput *Input,
                      StandardGraphics *Graphics)
{

  p_App = App;
  p_Window3D = Window3D;
  p_Framework = Framework;
  p_Input = Input;
  p_3D = Graphics;
  p_Init = p_3D->GetInit();

  return 0;
}

/** Uninitialize()
 * Uninitializes/Cleans up the object.
 */
void GUI::Uninitialize()
{  

  p_ActiveScreen = 0;

  return;
}

/**
 * Run()
 * The main GUI loop. The main logic is as follows
 * This routine continues until the shutdown flag
 * in the framework is set. The main loop
 * A. Call Apps OnStartCycle
 * B. Call Window3D's HandleSystemMessages
 * C. See if the Window3D has the focus if so 
 *    1. acquire input.
 *    2. preprocess input.
 *    3. call the apps Render routine.
 * D. If the Window does not have focus then nap.
 * E. Call the Apps OnEndCycle routine.
 */
void GUI::Run()
{

  bool focus = false;

  while(p_Framework->GetShutdown() == false)
  {

    p_App->OnStartCycle();

    p_Window3D->HandleSystemMessages();

    if(p_Window3D->GetFocused() == true)
    {
      if(focus == false)
      {
        focus=true;
        p_Input->Acquire();
      }

      PreprocessInput();
      
      p_App->OnRender();

    }
    else
    {
      focus = false;
      p_Framework->Nap(100);
    }

    p_App->OnEndCycle();

  }

  return;
}

bool GUI::PreprocessInput()
{

  bool result = false;

  _KEY_UNBUFFERED     *key;
  _KEY_BUFFERED        key2;

  _MOUSE              *mouse;
  _MOUSE               mouse2;

  _JOYSTICK           *joystick;
  _JOYSTICK            joystick2;

  short                x;
  
  // handled unbuffered

  key = p_Input->GetKeyboardInstant(); 
  mouse = p_Input->GetMouseInstant();
  joystick = p_Input->GetJoystickInstant();

  p_App->OnMovement(key, mouse, joystick);

  // handled buffered
  p_Input->SnapshotKeyboard();
  p_Input->SnapshotMouse();
  p_Input->SnapshotJoystick();

  for(x = 0; x < p_Input->GetKeyboardBufferCount() ; x++)
  {
    p_Input->GetKeyboardKeyPress(x, &key2);

    PreprocessKeyboard(&key2);
  }

  for(x = 0; x < p_Input->GetMouseBufferCount() ; x++)
  {
    p_Input->GetMouseMove(x, &mouse2);

    PreprocessMouse(&mouse2);
  }

  for(x = 0; x < p_Input->GetJoystickBufferCount() ; x++)
  {
    p_Input->GetJoystickMove(x, &joystick2);

    PreprocessJoystick(&joystick2);
  }

  return result;
}

void GUI::PreprocessKeyboard(_KEY_BUFFERED *key)
{

  _GUI_KEY tempKey;

  if(key->down == true)
  {

    switch(key->key)
    {
      case 29:
        p_Ctrl = true;
        break;
      case 42:
        p_Shift = true;
        break;
      case 54:
        p_Shift = true;
        true;
      case 56:
        p_Alt = true;
        break;
      case 157:
        p_Ctrl = true;
        break;
      case 184:
        p_Alt = true;
        break;
      default:

        tempKey.key = key->key;
        tempKey.ctrl = p_Ctrl;
        tempKey.alt = p_Alt;
        tempKey.shift = p_Shift;

		if (p_ActiveScreen)
			p_ActiveScreen->Process(&tempKey, 0);
    }
  }
  else
  {
    switch(key->key)
    {
      case 29:
        p_Ctrl = false;
        break;
      case 42:
        p_Shift = false;
        break;
      case 54:
        p_Shift = false;
        break;
      case 56:
        p_Alt = false;
        break;
      case 157:
        p_Ctrl = false;
        break;
      case 184:
        p_Alt = false;
        break;
      default:
        int a = 0;
    }
  }

  p_App->OnGUI(key,0,0, &p_NoGUI);

  return;
}

void GUI::PreprocessMouse(_MOUSE *mouse)
{

  _Point pt;

  //pt = p_Window3D->GetMouse();
  pt.x = 0;
  pt.y = 0;

  
  if(mouse->mouse1 == false &&
    mouse->x == 0 && mouse->y == 0 && mouse->z == 0)
  {
    p_MouseDown = false;
  }

  if(mouse->mouse1 == true &&
    mouse->x == 0 && mouse->y == 0 && mouse->z == 0)
  {
    p_MouseDown = true;
  }

  _GUI_MOUSE tempMouse;

  tempMouse.down = p_MouseDown;
  tempMouse.pt = pt;


  if(p_ActiveScreen != 0)
  {
    if(p_MouseDown == true)
    {
      p_MouseDGUI = p_ActiveScreen->Process(0, &tempMouse);
    }
    else
    {
      p_MouseUGUI = p_ActiveScreen->Process(0, &tempMouse);

      if(p_MouseDGUI.Button == p_MouseUGUI.Button &&
        p_MouseDGUI.Window == p_MouseUGUI.Window &&
        p_MouseDGUI.Screen == p_MouseUGUI.Screen)
      {
        p_App->OnGUI(0, 0, 0, &p_MouseUGUI); 

        p_MouseDGUI.Button = -1;
        p_MouseDGUI.Window = -1;
        p_MouseDGUI.Screen = -1;

        p_MouseUGUI.Button = -1;
        p_MouseUGUI.Window = -1;
        p_MouseUGUI.Screen = -1;

      }
    }
  }

  return;
}

void GUI::PreprocessJoystick(_JOYSTICK *joystick)
{
  
  p_App->OnGUI(0,0,joystick, &p_NoGUI);

  return;
}

StandardScreen* GUI::CreateScreen()
{
 
  StandardScreen *Screen;
  
  Screen = new StandardScreen(p_3D);

  return Screen;
}

void GUI::SetScreen(StandardScreen *Screen)
{

  p_ActiveScreen = Screen;

  p_ActiveScreen->Apply();

}

unsigned long GUI::Draw()
{
    if (p_ActiveScreen)
        return p_ActiveScreen->Draw();
    else 
        return 0;
}

