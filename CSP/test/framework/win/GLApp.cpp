#include <stdio.h>
#include <iostream.h>
#include "windows.h"
#include "GLApp.h"
#include <graphics/win/TextWindow.h>

#include "mtxlib.h"
#include <graphics/win/GLPolygonListObject.h>
#include <graphics/win/Texture.h>
#include <graphics/win/TextureFormat.h>

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

//  plane_x=0.0;
//  plane_y=5.0;
//  plane_z=2000.0;
//  plane_ang_xaxis=0.0;
//  plane_ang_yaxis=0.0;
//  plane_ang_zaxis=0.0;
//  plane_dx=0.0;
//  plane_dy=0.0;
//  plane_dz=-0.25;

//  plane_nx=0;
//  plane_ny=0;
//  plane_nz=-1;

//  plane_upx=0;
//  plane_upy=1;
//  plane_upz=0;
//  plane_view_dir=0;

  planeOrientation = IdentityMatrix33();
  planeInitialDirection = vector3(0,0,-1);
  planeCurrentDirection = vector3(0,0,-1);
  planeInitialUp = vector3(0.0,1.0,0.0);
  planeCurrentPosition = vector3(0.0,20000.0,0.0);
  planeSpeed = 100.0;
  dt = 0.01;
//	font = new Font("Font.bmp");
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

short App::Initialize()
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
  m_sh1=audio->Load_Sound("test1.wav");
  audio->Cache_Sound(m_sh1,1);
  m_sh2=audio->Load_Sound("test2.wav");
  audio->Cache_Sound(m_sh2,1);

  // setup viewport
//  display->SetViewport(0.0f, 0.0f, 1.0f, 1.0f);
//  display->SetPerspective(60, (float)(globals->display.width)/
//	  (float)(globals->display.height), 0.5f, 25.0f);

  // setup world matrix
//  math->MatrixIdentity(&MatrixWorld);
  // setup perspective matrix with 90 FOV
//  math->MatrixPerspectiveFOV(&MatrixFOV, 90, globals->display.width, globals->display.height, 0.5f, 25000);
  // setup camera matrix
//  math->MatrixIdentity(&MatrixCamera);

  // tell the 3d system the new changes.
//  display->SetTransformWorld(&MatrixWorld);
//  display->SetTransformFOV(&MatrixFOV);
//  display->SetTransformCamera(&MatrixCamera);

  // setup body and head matrix
//  math->MatrixRotationX(&matHead, -0.2f);
//  math->MatrixIdentity(&matBody);
//  math->MatrixIdentity(&matPlane);

//  math->MatrixTranslation(&matPlane, 0, 400, 0);
//  math->MatrixInverse(&matPlane);

  // Create Sky points/triangles.
  
//  _NormalVertex ver2[4];

//  memset(ver2, 0, sizeof(ver2));

//  math->SetVector(&ver2[0].p, -20000, 0, 24000);
//  math->SetVector(&ver2[1].p, 20000, 0, 24000);
//  math->SetVector(&ver2[2].p, -20000, 1000, -1000);
//  math->SetVector(&ver2[3].p, 20000, 1000, -1000);

//  SkyBuff = display->CreateVertexBuffer(ver2, sizeof(ver2), sizeof(_NormalVertex));

//  _Index indx2[6];

//  indx2[0] = 1;
//  indx2[1] = 0;
//  indx2[2] = 2;
//  indx2[3] = 1;
//  indx2[4] = 2;
//  indx2[5] = 3;

//  SkyIndexBuff = display->CreateIndexBuffer(indx2, sizeof(indx2), sizeof(_Index));

  // setup sky material
//  _Color Diffuse, Ambient, Emissive;

//  Diffuse.r = 0;
//  Diffuse.g = 0;
//  Diffuse.b = 0;
//  Diffuse.a = 1;

//  Ambient.r = 0.19f;
//  Ambient.g = 0.35f;
//  Ambient.b = 0.94f;
//  Ambient.a = 1;

//  Emissive.r = 0;
//  Emissive.g = 0;
//  Emissive.b = 0;
//  Emissive.a = 1;

//  Sky = display->CreateMaterial(&Diffuse, &Ambient, &Emissive);

  // create light - sun
//  _Color SunColor;
//  _Vector SunVector;

//  SunColor.r = 1;
//  SunColor.g = 1;
//  SunColor.b = 1;
//  SunColor.a = 1;
  
//  math->SetVector(&SunVector, 0, -0.1f, 0.9f);

//  Sun = display->CreateDirectionalLight(&SunVector, &SunColor); 

  // tell the 3d system what sun we are using.
  //display->SetLight(0, &Sun);
  // turn the light on.
  //display->LightEnabled(0, TRUE);

  _NormalVertexStruct triVect[400];
  int count=0;
//	_NormalVertexStruct triVect[6];

//	triVect[0].p.x = -5000.0;   triVect[0].p.y = 0.0;    triVect[0].p.z = -5000.0;
//	triVect[0].n.x = 0.0;    triVect[0].n.y = 1.0;    triVect[0].n.z = 0.0;
//	triVect[0].tu = 1.0;     triVect[0].tv = 1.0;

//	triVect[1].p.x = -5000.0;    triVect[1].p.y = 0.0;   triVect[1].p.z = 5000.0;
//	triVect[1].n.x = 0.0;     triVect[1].n.y = 1.0;   triVect[1].n.z = 0.0;
//	triVect[1].tu = 1.0;      triVect[1].tv = 0.0;

//	triVect[2].p.x = 5000.0;     triVect[2].p.y = 0.0;   triVect[2].p.z = 5000.0;
//	triVect[2].n.x = 0.0;     triVect[2].n.y = 1.0;   triVect[2].n.z = 0.0;
//	triVect[2].tu = 0.0;      triVect[2].tv = 0.0;

//	triVect[3].p.x = -5000.0;   triVect[3].p.y = 0.0;    triVect[3].p.z = -5000.0;
//	triVect[3].n.x = 0.0;    triVect[3].n.y = 1.0;    triVect[3].n.z = 0.0;
//	triVect[3].tu = 1.0;     triVect[3].tv = 1.0;

//	triVect[4].p.x = 5000.0;     triVect[4].p.y = 0.0;   triVect[4].p.z = 5000.0;
//	triVect[4].n.x = 0.0;     triVect[4].n.y = 1.0;   triVect[4].n.z = 0.0;
//	triVect[4].tu = 0.0;      triVect[4].tv = 0.0;

//	triVect[5].p.x = 5000.0;     triVect[5].p.y = 0.0;   triVect[5].p.z = -5000.0;
//	triVect[5].n.x = 0.0;     triVect[5].n.y = 1.0;   triVect[5].n.z = 0.0;
//	triVect[5].tu = 0.0;      triVect[5].tv = 1.0;

  double scalefactor = 1000000.0/10;
  double texturescale = 1.0/10;
  for(int i=0;i<10;i++)
	  for(int j=0;j<10;j++)
	  {
		triVect[count].p.x = -500000.0+i*scalefactor;       triVect[count].p.y = 0.0; triVect[count].p.z = -500000.0+j*scalefactor;
		triVect[count].n.x = 0.0;          triVect[count].n.y = 1.0; triVect[count].n.z = 0.0;
		triVect[count].tu = 0.0;           triVect[count].tv = 0.0;

		count++;

		triVect[count].p.x = -500000.0+(i+1.0)*scalefactor;   triVect[count].p.y = 0.0; triVect[count].p.z = -500000.0+j*scalefactor;
		triVect[count].n.x = 0.0;          triVect[count].n.y = 1.0; triVect[count].n.z = 0.0;
		triVect[count].tu = 0.0;           triVect[count].tv = 1.0;

		count++;

		triVect[count].p.x = -500000.0+(i+1.0)*scalefactor;   triVect[count].p.y = 0.0; triVect[count].p.z = -500000.0+(j+1.0)*scalefactor;
		triVect[count].n.x = 0.0;          triVect[count].n.y = 1.0; triVect[count].n.z = 0.0;
		triVect[count].tu = 1.0;           triVect[count].tv = 1.0;

		count++;

		triVect[count].p.x = -500000.0+i*scalefactor;       triVect[count].p.y = 0.0; triVect[count].p.z = -500000.0+(j+1.0)*scalefactor;
    	triVect[count].n.x = 0.0;          triVect[count].n.y = 1.0; triVect[count].n.z = 0.0;
		triVect[count].tu = 1.0;           triVect[count].tv = 0.0;

		count++;
		
	  }
//	display->CreateVertexBuffer(triVect, sizeof(triVect), sizeof(_NormalVertex));
	terrainObject = display->CreatePolygonListObject();
	terrainObject->Create(FW_QUADS, 400, triVect);

  camera = new Camera();


  grassTexture = new Texture( "grass.bmp", Texture::DIB );
//  grassTextureFormat = new TextureFormat(grassTexture, 0, 0);
  grassTexture->genTexObject();
//  textWindow = new TextWindow(display, 0.0,0.0,1.0,1.0);

  skyTexture = new Texture("sky.bmp", Texture::DIB);
//  skyTextureFormat = new TextureFormat(skyTexture, 0, 0);
  skyTexture->genTexObject();

  count = 0;
  triVect[count].p.x = -2000000; triVect[count].p.y = 0.0;     triVect[count].p.z = -2000000.;
  triVect[count].n.x = 0.0;     triVect[count].n.y = -1.0;       triVect[count].n.z = 0.0;
  triVect[count].tu = 0.0;      triVect[count].tv = 0.0;

  count++;

  triVect[count].p.x = -2000000; triVect[count].p.y = 0.0;     triVect[count].p.z = 2000000.0;
  triVect[count].n.x = 0.0;     triVect[count].n.y = -1.0;       triVect[count].n.z = 0.0;
  triVect[count].tu = 0.0;      triVect[count].tv = 1.0;

  count++;

  triVect[count].p.x = 2000000; triVect[count].p.y = 0.0;     triVect[count].p.z = 2000000.0;
  triVect[count].n.x = 0.0;     triVect[count].n.y = -1.0;       triVect[count].n.z = 0.0;
  triVect[count].tu = 1.0;      triVect[count].tv = 1.0;
  
  count++;
  
  triVect[count].p.x = -2000000; triVect[count].p.y = 0.0;     triVect[count].p.z = -2000000.0;
  triVect[count].n.x = 0.0;     triVect[count].n.y = -1.0;       triVect[count].n.z = 0.0;
  triVect[count].tu = 0.0;      triVect[count].tv = 0.0;

  count++;

  triVect[count].p.x = 2000000; triVect[count].p.y = 0.0;     triVect[count].p.z = 2000000.0;
  triVect[count].n.x = 0.0;     triVect[count].n.y = -1.0;       triVect[count].n.z = 0.0;
  triVect[count].tu = 1.0;      triVect[count].tv = 1.0;

  count++;

  triVect[count].p.x = 2000000; triVect[count].p.y = 0.0;     triVect[count].p.z = -2000000.0;
  triVect[count].n.x = 0.0;     triVect[count].n.y = -1.0;       triVect[count].n.z = 0.0;
  triVect[count].tu = 1.0;      triVect[count].tv = 0.0;
  

  skyObject = display->CreatePolygonListObject();
  skyObject->Create(FW_TRIANGLES, 6, triVect);

//  textureObject = display->CreateTexture("grass.bmp");
//  display->SetTexture(4,textureObject);
//  display->SetTextureMode(TRUE);

  return _A_OK;
}

void App::Run()
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
  double dt = 0.1;   // this needs to be determine from the simulation
  
  // using simple 1st order integration move plane.
//  plane += dt*plane_dx;
//  plane_y += dt*plane_dy;
//  plane_z += dt*plane_dz;

//  _KEY_STRUCT buffer;
//
//  short index = input->GetKeyboardBufferCount();
//  if (index > 0)
//  {
//	input->GetKeyboardKeyPress(index, &buffer);
//    if (buffer.down)
//	{
//	  fprintf(stdout, "Key is down\n");
//	}
//  }

  joystick = input->GetJoystickAll();

  if(joystick.x != 0)
  {
    matrix33 matZ = Matrix33RotationZ(-((float)joystick.x / (float)1000) * sensativity * planesensx * plusplus);
	planeOrientation = matZ*planeOrientation;
//    math->MatrixRotationZ(&MatrixTemp, ((float)joystick.x / (float)1000) * sensativity * planesensx * plusplus);
//    math->MatrixMultiply(&matPlane, &MatrixTemp);
  }
  if(joystick.y != 0)
  {
    matrix33 matX = Matrix33RotationX(((float)joystick.y / (float)1000) * sensativity * planesensy * plusplus);
	planeOrientation = matX*planeOrientation;
//    math->MatrixRotationX(&MatrixTemp, ((float)joystick.y / (float)1000) * sensativity * planesensy * plusplus);
//    math->MatrixMultiply(&matPlane, &MatrixTemp);
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
//      math->MatrixRotationX(&MatrixTemp, 0.1f * sensativity * plusplus);
//      math->MatrixMultiply(&matHead, &MatrixTemp);

      break;
    case 90:
//      math->MatrixRotationY(&MatrixTemp, -0.1f * sensativity * plusplus);
//      math->MatrixMultiply(&matBody, &MatrixTemp);

      break;
    case 180:
//      math->MatrixRotationX(&MatrixTemp, -0.1f * sensativity * plusplus);
//      math->MatrixMultiply(&matHead, &MatrixTemp);

      break;
    case 270:
//      math->MatrixRotationY(&MatrixTemp, 0.1f * sensativity * plusplus);
//      math->MatrixMultiply(&matBody, &MatrixTemp);

      break;
  }

  //---

  planeCurrentDirection = planeInitialDirection*planeOrientation;
  planeCurrentPosition = planeCurrentPosition + dt*planeSpeed*planeCurrentDirection;


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
	  case 203:
		  if(key.down == TRUE)
		  {
	//		  plane_view_dir -= 5;
//			  cout << "View Direction = " << plane_view_dir << endl;
		  }
		  break;
	  case 205:
		  if (key.down == TRUE)
		  {
	//		  plane_view_dir += 5;
//			  cout << "View Direction = " << plane_view_dir << endl;
		  }
		  break;
	  case 200:
		  if (key.down == TRUE)
		  {
	//		  plane_y += 1;
//			  cout << "y = " << plane_y << endl;
		  }
		  break;
	  case 208:
		  if (key.down == TRUE)
		  {
	//		  plane_y -= 1;
//			  cout << "y = " << plane_y << endl;
		  }
		  break;
	  case 52:
		  if (key.down == TRUE)
		  {
				planeSpeed += 5.0;
//				cout << "plane_dz = " << plane_dz << endl;
		  }
		break;
	  case 51:
		  if (key.down == TRUE)
		  {
			   planeSpeed -= 5.0;
//			   cout << "plane_dz = " << plane_dz << endl;
		  }
		break;
	  default:
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

	display->SetPerspective(33, 1.3,1, planeCurrentPosition.y*333.0);
	display->BeginScene();
//	_Rect rect = _Rect(0,0,640,480);
//	font->Test(rect);
//	display->EndScene();
	
	display->SetModelViewMatrix();
	display->SetColor(1.0,0.0,0.0);
	display->SetIdentity();

	// use euler angles to orient plane
//	display->Rotate(plane_ang_zaxis, 0.0, 0.0, 1.0);
//	display->Rotate(plane_ang_xaxis, 1.0, 0.0, 0.0);
//	display->Rotate(plane_ang_yaxis, 0.0, 1.0, 0.0);

	// position camera at head of plane.
//	display->Translate(-plane_x,-plane_y,-plane_z);
//	display->DrawTriangle(0,0,0,10,0,0,5,5,0);

//	display->Translate(0,60,50);

	camera->SetPosition(planeCurrentPosition.x, 
		                planeCurrentPosition.y, 
						planeCurrentPosition.z);
//	double angle=plane_view_dir/360.0*2*3.1415;
	camera->SetDirection(planeCurrentDirection.x,
		                 planeCurrentDirection.y,
						 planeCurrentDirection.z);

	vector3 vectorUp = planeInitialUp*planeOrientation;

	camera->SetUp(vectorUp.x, vectorUp.y, vectorUp.z);
	camera->Position();

//	display->Look(plane_x, plane_y, plane_z,plane_nx, plane_ny, plane_nz,
//		plane_upx, plane_upy, plane_upz);

	grassTexture->Apply();
	terrainObject->Draw();

	display->Translate(0.0f, 5.0e4f, 0.0f);

	skyTexture->Apply();
	skyObject->Draw();

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
  _Rect rect = display->GetCoordsRect();
//  rect.y1 = rect.y2 - 40;
//  rect.y2 = 480;
//  rect.x2 = 640;
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 20;
  sprintf(ach, "Position: [%f, %f, %f]", planeCurrentPosition.x,
		planeCurrentPosition.y, planeCurrentPosition.z);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 20;
  sprintf(ach, "Direction: [%f, %f, %f]", planeCurrentDirection.x,
	    planeCurrentDirection.y, planeCurrentDirection.z);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 20;
  sprintf(ach, "Plane Speed: %f", planeSpeed);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 20;
  sprintf(ach, "Orientation:");
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 20;
  sprintf(ach, "%f, %f, %f", planeOrientation.col[0].x,
	  planeOrientation.col[1].x, planeOrientation.col[2].x);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 20;
  sprintf(ach, "%f, %f, %f", planeOrientation.col[0].y,
	  planeOrientation.col[1].y, planeOrientation.col[2].y);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);
  
  rect.y2 -= 20;
  sprintf(ach, "%f, %f, %f", planeOrientation.col[0].z,
	  planeOrientation.col[1].z, planeOrientation.col[2].z);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);


  display->EndScene();

//  if(appCanFlip)
//  {
//    display->Flip();
//  }
//  else
//  {
//    platform->Flip();
//  }

	return;

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