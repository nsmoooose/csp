#include "app.h"

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
App::App()
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

  sensativity = 0.3f;
  planesensx = 1.7f;
  planesensy = 1;

  plusplus = 1;

  SkyBuff = 0;
  SkyIndexBuff = 0;

}

App::~App()
{

}

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  App::SetSetup(void *setup)
{
  globals = (_APP_INIT_STRUCT *)setup;
}

short App::GetDependencyCount()
{
  return 0;
}

void  App::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short App::SetDependencyData(short Index, void* Data)
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

void  App::GetSetup(void*setup)
{
  setup = globals;
}

short App::CheckStatus()
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
short App::CreateObjects()
{

  short number;
  short count;

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
  fprintf(stderr,"Checking platforms: ");
  for(count = 0; count < number; count++)
  {
    PlatformFactory->GetDescription(count, &pd);
    fprintf(stderr, " \"%s\"", pd->Name);
    if(pd->ID == DEFAULT_PLATFORM)
    {
      PlatformFactory->Create(pd->ID, &platform);
      fprintf(stderr,"*");
      // we found our platform we want,  lets break;
      break;
    }
  }
  fprintf(stderr, "\n");

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
  fprintf(stderr,"Checking audio    : ");
  for(count = 0; count < number; count++)
  {
    AudioFactory->GetDescription(count, &ad);
      fprintf(stderr," \"%s\"",ad->Name);
    if(ad->ID == DEFAULT_AUDIO)
    {
      AudioFactory->Create(ad->ID, &audio);
      fprintf(stderr,"*");
      // we found our platform we want,  lets break;
      break;
    }
  }
  fprintf(stderr,"\n");

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
  fprintf(stderr,"Checking network  : ");
  for(count = 0; count < number; count++)
  {
    NetworkFactory->GetDescription(count, &nd);
    fprintf(stderr," \"%s\"", nd->Name);
    if(nd->ID == DEFAULT_NETWORK)
    {
      NetworkFactory->Create(nd->ID, &network);
      fprintf(stderr,"*");
      // we found our platform we want,  lets break;
      break;
    }
  }
  fprintf(stderr,"\n");

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
  fprintf(stderr,"Checking inputdev : ");
  for(count = 0; count < number; count++)
  {
    InputFactory->GetDescription(count, &id);
    fprintf(stderr," \"%s\"", id->Name);
    if(id->ID == DEFAULT_INPUT)
    {
      InputFactory->Create(id->ID, &input);
      fprintf(stderr,"*");
      // we found our platform we want,  lets break;
      break;
    }
  }
  fprintf(stderr,"\n");

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
  fprintf(stderr,"Checking mathemat : ");
  for(count = 0; count < number; count++)
  {
    MathFactory->GetDescription(count, &md);
    fprintf(stderr," \"%s\"",md->Name);
    if(md->ID == DEFAULT_MATH)
    {
      MathFactory->Create(md->ID, &math);
      fprintf(stderr,"*");
      // we found our platform we want,  lets break;
      break;
    }
  }
  fprintf(stderr,"\n");
  _DEL(MathFactory);

  Error.sError = input->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  return 0;
}

short App::Initialize()
{

  // Create the platform window.
  Error.sError = platform->CreateAppWindow();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }
  fprintf(stderr,"AppWindow..\n");

  // Create the display variable.
  Error.sError = platform->CreateRenderingDevice(&display);
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }
  fprintf(stderr,"RenderDev..\n");

  // Turn the platform window into a 3D window.
  Error.sError = display->Create3DWindow();//"OpenGL", 0,0,640,480,16,false);
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  // Ask if display API can flip without assistance.
  appCanFlip = display->CanFlip();

  // Create Input.
  input->CreateInput(16);
  input->Acquire();
  fprintf(stderr,"Input..\n");

  // Create Sound
  audio->Init();
  m_sh1=audio->Load_Sound("test1.wav");
  audio->Cache_Sound(m_sh1,1);
  m_sh2=audio->Load_Sound("test2.wav");
  audio->Cache_Sound(m_sh2,1);
  fprintf(stderr,"Audio..\n");

  // setup world matrix
  math->MatrixIdentity(&MatrixWorld);
  // setup perspective matrix with 90 FOV
  math->MatrixPerspectiveFOV(&MatrixFOV, 90, globals->display.width, globals->display.height, 0.5f, 25000);
  // setup camera matrix
  math->MatrixIdentity(&MatrixCamera);

  // tell the 3d system the new changes.
  display->SetTransformWorld(&MatrixWorld);
  display->SetTransformFOV(&MatrixFOV);
  display->SetTransformCamera(&MatrixCamera);

  // setup body and head matrix
  math->MatrixRotationX(&matHead, -0.2f);
  math->MatrixIdentity(&matBody);
  math->MatrixIdentity(&matPlane);

  math->MatrixTranslation(&matPlane, 0, 400, 0);
  math->MatrixInverse(&matPlane);

  // Create Sky points/triangles.
  
  _NormalVertex ver2[4];

  memset(ver2, 0, sizeof(ver2));

  math->SetVector(&ver2[0].p, -20000, 0, 24000);
  math->SetVector(&ver2[1].p, 20000, 0, 24000);
  math->SetVector(&ver2[2].p, -20000, 1000, -1000);
  math->SetVector(&ver2[3].p, 20000, 1000, -1000);

  SkyBuff = display->CreateVertexBuffer(ver2, sizeof(ver2), sizeof(_NormalVertex));

  _Index indx2[6];

  indx2[0] = 1;
  indx2[1] = 0;
  indx2[2] = 2;
  indx2[3] = 1;
  indx2[4] = 2;
  indx2[5] = 3;

  SkyIndexBuff = display->CreateIndexBuffer(indx2, sizeof(indx2), sizeof(_Index));

  // setup sky material
  _Color Diffuse, Ambient, Emissive;

  Diffuse.r = 0;
  Diffuse.g = 0;
  Diffuse.b = 0;
  Diffuse.a = 1;

  Ambient.r = 0.19f;
  Ambient.g = 0.35f;
  Ambient.b = 0.94f;
  Ambient.a = 1;

  Emissive.r = 0;
  Emissive.g = 0;
  Emissive.b = 0;
  Emissive.a = 1;

  Sky = display->CreateMaterial(&Diffuse, &Ambient, &Emissive);

  // create light - sun
  _Color SunColor;
  _Vector SunVector;

  SunColor.r = 1;
  SunColor.g = 1;
  SunColor.b = 1;
  SunColor.a = 1;
  
  math->SetVector(&SunVector, 0, -0.1f, 0.9f);

  Sun = display->CreateDirectionalLight(&SunVector, &SunColor); 

  // tell the 3d system what sun we are using.
  //display->SetLight(0, &Sun);
  // turn the light on.
  //display->LightEnabled(0, TRUE);

  return _A_OK;
}

void App::Run()
{

  unsigned long stim, etim, rtim;

  //MAINLOOP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

void App::Uninitialize()
{
  audio->Discard_Sound(m_sh1);
  audio->Discard_Sound(m_sh2);
  return;
}

void App::DeleteObjects()
{
  
  input->DeleteInput();
  _DEL(input);

  audio->Shutdown();

  _DEL(audio);
  _DEL(network);
  _DEL(math);

  display->DeleteIndexBuffer(SkyIndexBuff);
  display->DeleteVertexBuffer(SkyBuff);
  display->DeleteLight(Sun);
  display->DeleteMaterial(Sky);

  platform->DeleteRenderingDevice(display);
  platform->DeleteAppWindow();

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
void App::Movement(void)
{
  
  joystick = input->GetJoystickAll();

  if(joystick.x != 0)
  {
    math->MatrixRotationZ(&MatrixTemp, ((float)joystick.x / (float)1000) * sensativity * planesensx * plusplus);
    math->MatrixMultiply(&matPlane, &MatrixTemp);
  }
  if(joystick.y != 0)
  {
    math->MatrixRotationX(&MatrixTemp, ((float)joystick.y / (float)1000) * sensativity * planesensy * plusplus);
    math->MatrixMultiply(&matPlane, &MatrixTemp);
  }

  if (joystick.button1)
  {
    audio->Play_Sound(m_sh1);
  }
  if (joystick.button2)
  {
    audio->Play_Sound(m_sh2);
  }
  switch(joystick.hat)
  {
    case 0:
      math->MatrixRotationX(&MatrixTemp, 0.1f * sensativity * plusplus);
      math->MatrixMultiply(&matHead, &MatrixTemp);

      break;
    case 90:
      math->MatrixRotationY(&MatrixTemp, -0.1f * sensativity * plusplus);
      math->MatrixMultiply(&matBody, &MatrixTemp);

      break;
    case 180:
      math->MatrixRotationX(&MatrixTemp, -0.1f * sensativity * plusplus);
      math->MatrixMultiply(&matHead, &MatrixTemp);

      break;
    case 270:
      math->MatrixRotationY(&MatrixTemp, 0.1f * sensativity * plusplus);
      math->MatrixMultiply(&matBody, &MatrixTemp);

      break;
  }

  //---

  math->MatrixTranslation(&MatrixTemp, 0,0, ((float)(100 - joystick.throttle) / (float)100) * plusplus);
  math->MatrixInverse(&MatrixTemp);
  math->MatrixMultiply(&matPlane, &MatrixTemp);

  heading = (float)atan2(matPlane._fx, matPlane._fz);
  heading = math->ToDegree(heading);
  if(heading < 0) heading = 360 + heading;
  headingx = (float)cos(heading);
  headingz = (float)sin(heading);

  return;
}

// -------------------------------------
// Process GUI Message
// This gets a message from the GUI.
// -------------------------------------
void App::ProcessInputMessage(void)
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
        break;
    }
  }

  //---
  count = input->GetMouseBufferCount();
  if (count == _INPUT_LOST)
  {
      appHasFocus = FALSE;
      return;
  }

  for(index = 0; index <= count - 1; index++)
  {
    input->GetMouseMove(index, &mouse);

    // ---
    // Move the camera depending on mouse move
    // ---
    math->MatrixRotationY(&MatrixTemp, ((float)mouse.x / (float)100) * sensativity * -1 * plusplus);
    math->MatrixMultiply(&matBody, &MatrixTemp);

    math->MatrixRotationX(&MatrixTemp, ((float)mouse.y / (float)100) * sensativity * plusplus);
    math->MatrixMultiply(&matHead, &MatrixTemp);

  }
  
  return;
}


// -------------------------------------
// Render
// Renders the screen.
// -------------------------------------
void App::Render(void)
{

  tris = 0;

  math->MatrixIdentity(&MatrixCamera);

  math->MatrixMultiply2(&MatrixTemp, &matBody, &matHead);
  math->MatrixMultiply2(&MatrixCamera, &matPlane, &MatrixTemp);

  display->SetTransformCamera(&MatrixCamera);

  //---
  // Start rendering.
  //---
  display->BeginScene();

  display->ClearSurface();

  // ------------------------------------------------------------
  // Draw Sky
  // ------------------------------------------------------------
  display->SetForeground(FALSE);

  _Matrix tempmat, tempmat1, tempmat2, tempmat3, tempmat4, tempmat5;

  math->MatrixIdentity(&tempmat);

  tempmat1 = matPlane;
  tempmat2 = matBody;
  tempmat3 = matHead;

  math->MatrixMultiply2(&tempmat5, &tempmat2, &tempmat3);

  math->MatrixIdentity(&tempmat4);
  math->MatrixMultiply2(&tempmat4, &tempmat1, &tempmat5);

  float headingt;

  headingt = (float)atan2(tempmat4._fz, tempmat4._fx);

  math->MatrixIdentity(&tempmat2);

  tempmat2._fx = (float)cos(headingt);
  tempmat2._fy = 0;
  tempmat2._fz = (float)sin(headingt);
  tempmat2._ux = 0;
  tempmat2._uy = 1;
  tempmat2._uz = 0;
  
  _Vector vec1, vec2, vecr;

  vec1.x = tempmat2._fx;
  vec1.y = tempmat2._fy;
  vec1.z = tempmat2._fz;

  vec2.x = tempmat2._ux;
  vec2.y = tempmat2._uy;
  vec2.z = tempmat2._uz;

  vecr = math->CrossProduct(&vec2, &vec1);

  tempmat2._rx = vecr.x;
  tempmat2._ry = vecr.y;
  tempmat2._rz = vecr.z;

  math->MatrixMultiply(&tempmat, &tempmat2);

  math->MatrixInverse(&tempmat);

  math->MatrixInverse(&tempmat1);
  math->MatrixTranslation(&tempmat2, tempmat1._px, tempmat1._py, tempmat1._pz);
  math->MatrixMultiply(&tempmat, &tempmat2);

  display->SetTransformWorld(&tempmat);

  display->SetTexture(0, NULL);
  display->SetMaterial(Sky);

  display->SetVertexBuffer(SkyBuff, sizeof(_NormalVertex));
  display->SetIndexBuffer(SkyIndexBuff);

  display->DrawTriangleListBuffer(4, 2);
  tris += 2;

  /*
  // ------------------------------------------------------------
  // Draw Land
  // ------------------------------------------------------------
  display->SetForeground(TRUE);

  display->SetTransformWorld(&MatrixWorld);

  display->SetTexture(0, Tex);
  display->SetMaterial(Mat);

  display->SetVertexBuffer(Buff, sizeof(_NormalVertex));
  display->SetIndexBuffer(IndexBuff);

  display->DrawTriangleListBuffer((30 * 30) * 4, (30 * 30) * 2);
  tris+= (30 * 30) * 2;

  // ------------------------------------------------------------
  // Draw Hud
  // ------------------------------------------------------------
  math->MatrixInverse2(&MatrixTemp, &matPlane);
  display->SetTransformWorld(&MatrixTemp);

  display->SetTexture(0, Hud);
  display->SetMaterial(HudMat);

  display->SetVertexBuffer(HudBuff, sizeof(_NormalVertex));
  display->SetIndexBuffer(HudIndexBuff);

  display->DrawTriangleListBuffer(4, 2);
  tris+= 2;

  // ------------------------------------------------------------
  // Draw Cockpit
  // ------------------------------------------------------------
  math->MatrixInverse2(&MatrixTemp, &matPlane);
  display->SetTransformWorld(&MatrixTemp);

  display->SetTexture(0, NULL);
  display->SetMaterial(CockpitMat);

  display->SetVertexBuffer(CockpitBuff, sizeof(_NormalVertex));
  display->SetIndexBuffer(CockpitIndexBuff);

  display->DrawTriangleListBuffer(22, 22);
  tris+= 22;
*/

  // -------------------
  // Draw Stats
  // -------------------
  _Color RGB;
  RGB.r = 1;
  RGB.g = 1;
  RGB.b = 1;
  RGB.a = 1;

  char ach[256];

  sprintf(ach, "FPS - %.3d", fps);
  display->DrawTextF(ach, strlen(ach), display->GetCoordsRect(), 2, &RGB);

  sprintf(ach, "\nTPF - %d", tris);
  display->DrawTextF(ach, strlen(ach), display->GetCoordsRect(), 2, &RGB);

  sprintf(ach, "Press ESC to quit");
  display->DrawTextF(ach, strlen(ach), display->GetCoordsRect(), 1, &RGB);

  if(appPaused == TRUE)
  {
    sprintf(ach, "\nPaused");
    display->DrawTextF(ach, strlen(ach), display->GetCoordsRect(), 1, &RGB);
  }

  sprintf(ach, "Sample Framework Test");
  display->DrawTextF(ach, strlen(ach), display->GetCoordsRect(), 0, &RGB);

  MatrixTemp = MatrixCamera;

  math->MatrixInverse(&MatrixTemp);

  sprintf(ach, "\n\n%.0f", heading);

  /*
  sprintf(ach, "\n\n%.0f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f", heading,
    MatrixTemp.m[0][0], MatrixTemp.m[0][1], MatrixTemp.m[0][2], MatrixTemp.m[0][3],
    MatrixTemp.m[1][0], MatrixTemp.m[1][1], MatrixTemp.m[1][2], MatrixTemp.m[1][3],
    MatrixTemp.m[2][0], MatrixTemp.m[2][1], MatrixTemp.m[2][2], MatrixTemp.m[2][3], 
    MatrixTemp.m[3][0], MatrixTemp.m[3][1], MatrixTemp.m[3][2], MatrixTemp.m[3][3]
    );
*/

  display->DrawTextF(ach, strlen(ach), display->GetCoordsRect(), 1, &RGB);

  // ---
  // End rendering
  // ---
  display->EndScene();

  if(appCanFlip)
  {
    display->Flip();
  }
  else
  {
    platform->Flip();
  }

  return;
}
