#include <stdio.h>
#include <iostream.h>
#ifdef WIN32
#include "windows.h"
#endif
#include "AppBase.h"

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
AppBase::AppBase()
{
  globals = 0;

  platform = 0;
  display = 0;
  audio = 0;
  network = 0;
  input = 0;
  math = 0;

  appInit = FALSE;
  appQuit = FALSE;
  appMessageWaiting = FALSE;
  appHasFocus = TRUE;
  appPaused = FALSE;

}

AppBase::~AppBase()
{

}

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void AppBase::SetSetup(void *setup)
{
  globals = (_APP_INIT_STRUCT *)setup;
}

short AppBase::GetDependencyCount()
{
  return 0;
}

void  AppBase::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short AppBase::SetDependencyData(short Index, void* Data)
{
  switch(Index)
  {
    case 0:
      return _NO_DEPENDENCY_TO_SET;
      break;
    default:
      return _NO_DEPENDENCY_TO_SET;
      break;
  }
  return 0;
}

void  AppBase::GetSetup(void*setup)
{
  setup = globals;
}

short AppBase::CheckStatus()
{
  if(globals == 0)
  {
    return _SETUP_NOT_SET;
  }

  return 0;
}

// -------------------------------------------
// STANDARD APP
// -------------------------------------------
short AppBase::CreateObjects()
{

  short number;
  short count;
  short mathtype = 1;

  FactoryPlatform       *PlatformFactory = 0;
  FactoryInput          *InputFactory = 0;
  FactoryNetwork        *NetworkFactory = 0;
  FactoryAudio          *AudioFactory = 0;
  FactoryMath           *MathFactory = 0;

  // -----
  // PLATFORM
  // -----
  _PLATFORM_DESCRIPTION *pd = 0;

  PlatformFactory = new FactoryPlatform;

  // You would normally check dependencies but screw it.
  // I already know there isn't any.
  
  Error.sError = PlatformFactory->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  number = PlatformFactory->GetCount();
  for(count = 0; count < number; count++)
  {
    PlatformFactory->GetDescription(count, &pd);
    if(pd->ID == DEFAULT_PLATFORM)
    {
      PlatformFactory->Create(pd->ID, &platform);
      // we found our platform we want,  lets break;
      break;
    }
  }

  _DEL(PlatformFactory);

  // Populate PlatformInit struct
  globals->platform.exename = globals->app.exename;
  globals->platform.displayname = globals->app.name;

  memcpy(&globals->platform.display, &globals->display, sizeof(_STANDARD_GRAPHICS));

  // Set platform init struct
  platform->SetSetup(&globals->platform);

  Error.sError = platform->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  // -----
  // AUDIO
  // -----
  _AUDIO_DESCRIPTION *ad = 0;

  AudioFactory = new FactoryAudio;

  // You would normally check dependencies but screw it.
  // I already know there isn't any.
  
  Error.sError = AudioFactory->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  number = AudioFactory->GetCount();
  for(count = 0; count < number; count++)
  {
    AudioFactory->GetDescription(count, &ad);
    if(ad->ID == DEFAULT_AUDIO)
    {
      AudioFactory->Create(ad->ID, &audio);
      // we found our platform we want,  lets break;
      break;
    }
  }

  _DEL(AudioFactory);

  audio_init_struct.this_inst = globals->platform.this_inst;
  audio->SetSetup(&audio_init_struct);
  dependencycount = audio->GetDependencyCount();
  if(dependencycount > 0)
  {
    // check which depedency we need.
    for(count = 0; count < dependencycount; count++)
    {
      audio->GetDependencyDescription(count, &dependency);
      switch(dependency.ID)
      {
        case _PLATFORM_DEPENDENCY:
          audio->SetDependencyData(dependency.ID, platform);
          break;
      }
    }
  }

  Error.sError = audio->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  // -----
  // Networking
  // -----
  _NETWORK_DESCRIPTION *nd = 0;

  NetworkFactory = new FactoryNetwork;

  // You would normally check dependencies but screw it.
  // I already know there isn't any.
  
  Error.sError = NetworkFactory->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  number = NetworkFactory->GetCount();
  for(count = 0; count < number; count++)
  {
    NetworkFactory->GetDescription(count, &nd);
    if(nd->ID == DEFAULT_NETWORK)
    {
      NetworkFactory->Create(nd->ID, &network);
      // we found our platform we want,  lets break;
      break;
    }
  }

  _DEL(NetworkFactory);

  Error.sError = network->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  // -----
  // Input
  // -----
  _INPUT_DESCRIPTION *id = 0;

  InputFactory = new FactoryInput;

  // You would normally check dependencies but screw it.
  // I already know there isn't any.
  
  Error.sError = InputFactory->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  number = InputFactory->GetCount();
  for(count = 0; count < number; count++)
  {
    InputFactory->GetDescription(count, &id);
    if(id->ID == DEFAULT_INPUT)
    {
      InputFactory->Create(id->ID, &input);
      // we found our platform we want,  lets break;
      break;
    }
  }

  _DEL(InputFactory);

  //Populate InputInitStruct
  input_init_struct.this_inst = globals->platform.this_inst;

  input->SetSetup(&input_init_struct);

  // ask object for dependency list
  dependencycount = input->GetDependencyCount();
  if(dependencycount > 0)
  {
    // check which depedency we need.
    for(count = 0; count < dependencycount; count++)
    {
      input->GetDependencyDescription(count, &dependency);
      switch(dependency.ID)
      {
        case _PLATFORM_DEPENDENCY:
          input->SetDependencyData(dependency.ID, platform);
          break;
      }
    }
  }

  Error.sError = input->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  // -----
  // Math
  // -----
  if( globals->platform.display.ID == _D3D8 )
	  mathtype = _DIRECTMATH;
  if( globals->platform.display.ID == _OGL )
	  mathtype = _OPENGLMATH;

  _MATH_DESCRIPTION *md = 0;

  MathFactory = new FactoryMath;

  // You would normally check dependencies but screw it.
  // I already know there isn't any.
  
  Error.sError = MathFactory->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  number = MathFactory->GetCount();
  for(count = 0; count < number; count++)
  {
    MathFactory->GetDescription(count, &md);
    if(md->ID == DEFAULT_MATH)
    {
      MathFactory->Create(md->ID, &math);
      // we found our platform we want,  lets break;
      break;
    }
  }
  _DEL(MathFactory);

  Error.sError = input->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  return 0;
}

short AppBase::Initialize()
{

  // Create the platform window.
  Error.sError = platform->CreateAppWindow();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  // Create the display variable.
  Error.sError = platform->CreateRenderingDevice(&display);
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  // Turn the platform window into a 3D window.
  Error.sError = display->Create3DWindow();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  // Ask if display API can flip without assistance.
  appCanFlip = display->CanFlip();

  // Create Input.
  input->CreateInput(16);
  input->Acquire();

  // Create Sound
  audio->Init();

  return _A_OK;
}



void AppBase::Run()
{

  unsigned long stim, etim, rtim;

  //MAINLOOP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // This is the main simulation loop. The basic logic:
  // 1. get the time
  // 2. handle system messages
  // 3. get input and process
  // 4. do movement
  // 5. render scene
  // 6. calculate time again and determine frames/second
  while(appQuit == FALSE)
  {
    stim = platform->GetTime();

    platform->HandleSystemMessages();

    if(appHasFocus == TRUE)
    {
      appMessageWaiting = input->GetInputMessage(FALSE);
      if(appMessageWaiting == TRUE)
      {
        ProcessInputMessage();
      }

      if(appPaused == FALSE)
      {
        Movement();
      }
      if(appQuit == FALSE)
      {
        Render();
      }
    }
    else
    {
      appHasFocus = platform->GetRegainedFocus();
      if(appHasFocus == TRUE)
      {
        input->Acquire();
      }
      else
      {
        platform->Nap(50);
      }
    }

    // Did we get a system quit?
    if(platform->GetQuit() == TRUE || appQuit == TRUE)
    {
      appQuit = TRUE;
    }
    else
    {
      // do other things like networking whatever...
    }
    // Let system rest
    //Sleep(5);

    // Calculate FPS
    etim = platform->GetTime();

    if(etim == stim)  etim++;
    rtim = etim - stim;
    fps = (short)(1000 / rtim);
    if(fps == 0) fps = 1;
	  plusplus = (float)(float)30 / (float)fps;

  }

  return;
}

void AppBase::Uninitialize()
{
  return;
}

void AppBase::DeleteObjects()
{
  
  input->DeleteInput();
  _DEL(input);

  audio->Shutdown();

  _DEL(audio);
  _DEL(network);
  _DEL(math);

  _DEL(platform);

  return;
}

// -------------------------------------------
// App Specific
// -------------------------------------------
// -------------------------------------
// Movement
// Calculate Movements
// -------------------------------------
void AppBase::Movement(void)
{

}

// -------------------------------------
// Process GUI Message
// This gets a message from the GUI.
// -------------------------------------
void AppBase::ProcessInputMessage(void)
{
  short count;
  short index;

  //---
  count = input->GetKeyboardBufferCount();
  if (count == _INPUT_LOST)
  {
      appHasFocus = FALSE;
      return;
  }

  for(index = 0; index <= count - 1; index++)
  {
    input->GetKeyboardKeyPress(index, &key);
    switch(key.key)
    {
      case 1:   //escape
        appQuit=TRUE;
        break;
      case 197: // paused
        if(key.down == TRUE)
        {
          if(appPaused == TRUE)
          {
            appPaused = FALSE;
          }
          else
          {
            appPaused = TRUE;
          }
        }
	  default:
        break;
    }
  }

  return;
}


// -------------------------------------
// Render
// Renders the screen.
// -------------------------------------
void AppBase::Render(void)
{


  return;
}