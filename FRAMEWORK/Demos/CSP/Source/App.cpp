#include "App.h"

// ------ StandardApp Constructors/Deconstructors

App::App()
{
  p_bPaused = false;
  p_Framework = 0;

  p_Framework = 0;
  p_GUI = 0;
  p_3D = 0;
  p_Window3D = 0;
  p_Audio = 0;
  p_Camera = 0;
  p_Input = 0;

  p_Ground = 0;
  p_TextureGrass = 0;
  p_Material = 0;
  p_MaterialSky = 0;
  p_MaterialCloud = 0;
  p_SquareTop = 0;
  p_MainScreen = 0;
  p_ExitScreen = 0;
  p_PausedScreen = 0;
  p_ExitWindow = 0;
  p_PausedWindow = 0;
  p_InfoWindow = 0;
  p_Sky = 0;
  p_TextureTree = 0;

  p_NearTree = 0;
  p_MidTree = 0;
  p_FarTree = 0;

  p_Cloud = 0;
  p_Cloud2 = 0;
  p_TextureCloud = 0;

  for(unsigned short x=0;x<500;x++)
  {
    p_Tree[x] = 0;
  }

  p_Init = 0;
  p_fFPS = 30;
  p_fMultiplier = 1.0f;

}

App::~App()
{
  Uninitialize();
}

// ------ StandardApp Publics

short App::Initialize(_APP_INIT_STRUCT *AppInit)
{

  // declarations
  StandardColor     Color;

  StandardTLVertex  TLVertex[14];
  StandardNTVertex  NTVertex[40];

  _Index            Indx[60];

  StandardColor     Emissive;
  StandardColor     Ambient;
  StandardColor     Diffuse;
  StandardColor     Specular;

  short             result = _A_OK;

  unsigned short    x;
  unsigned short    y;
  float             z;
  float             x1;
  float             y1;
  float             z1[11][11];

  _Index            Indx2[10 * 10 * 6];
  StandardNTVertex  Points[10 * 10 * 4];

  // ------- wipe struct

  memcpy(&p_AppInit, AppInit, sizeof(p_AppInit));

  // ------- Create Objects

  p_Factory = new StandardFactory();

  p_Factory->Initialize();

  p_Factory->CreateFramework(&p_Framework);

  // ------ Lets Rock
  result = p_Framework->Initialize();

  if (result == _A_OK)
  {

    p_Factory->CreateWindowText(&p_WindowText);
    
    result = p_WindowText->Initialize(this, p_Framework);
    if(result != _A_OK)
    {
        // return because we can't display error in WindowText.
        return result;
    }

    p_WindowText->PrintText("Initializing\n");

    p_WindowText->PrintText("    Objects....");

    p_Factory->CreateGraphics(&p_3D);
    p_Factory->CreateNetwork(&p_Network);
    p_Factory->CreateAudio(&p_Audio);
    p_Factory->CreateInput(&p_Input);
    p_Factory->CreateGUI(&p_GUI);

    p_WindowText->PrintText("OK\n");

    // --- meat and potatoes

    p_Factory->CreateWindow3D(&p_Window3D);

    p_Init = new _GRAPHICS_INIT_STRUCT;

    p_Init->Depth = 16;
    p_Init->Width = 800;
    p_Init->Height = 600;
    p_Init->Fullscreen = false;
    p_Init->HardwardTL = false;
    p_Init->VSync = false;
    p_Init->Wireframe = false;
    p_Init->NearZ = 0.5f;
    p_Init->FarZ = 25000.0f;
    p_Init->MipLevels = 5;

    p_WindowText->PrintText("    Window.....");

    result = p_Window3D->Initialize(this, p_Framework, &p_Init);

    if(result != _A_OK)
    {
       goto EARLY_EXIT;
    }
  
    p_WindowText->PrintText("OK\n");

    p_WindowText->PrintText("    Input......");

    result = p_Input->Initialize(p_Framework, p_Init->Handle, p_AppInit.Instance);
    if(result != _A_OK)
    {
      goto EARLY_EXIT;
    }

    p_WindowText->PrintText("OK\n");


    p_WindowText->PrintText("    Graphics...");
    result = p_3D->Initialize(this, p_Init);
    if(result != _A_OK)
    {
      goto EARLY_EXIT;
    }

    p_WindowText->PrintText("OK\n");


    // Setup Complete, Create Scene

    p_WindowText->PrintText("    Scene......");

    // Setup Colors

    Color.Initialize(240,230,210,255);
    p_3D->SetBackgroundColor(&Color);

    Color.Initialize(240,230,210,255);
    p_3D->SetFogColor(&Color);

    Color.Initialize(128,128,128,255);
    p_3D->SetAmbientColor(&Color);

    // Create Camera

	  p_Camera  = p_3D->CreateCamera();
	  p_Camera->SetFOVPerspectiveView(90, p_Init->Width, p_Init->Height, p_Init->NearZ, p_Init->FarZ); 
    p_Camera->SetPosition(500,50,500);
    p_3D->SetCamera(p_Camera);

    // Create texture
    p_TextureCloud = p_3D->CreateTexture();
    Color.Initialize(255,255,255,255);
    p_TextureCloud->Initialize("CLOUD.bmp", p_Init->MipLevels, &Color);

    // Create texture
    p_TextureGrass = p_3D->CreateTexture();
    p_TextureGrass->Initialize("grass.bmp");

    // create tree texture
    p_TextureTree = p_3D->CreateTexture();
    p_TextureTree->Initialize("tree.bmp", p_Init->MipLevels, &Color);

    // Create material for clouds (lighting on texture)

    p_MaterialCloud = p_3D->CreateMaterial();
    Specular.Initialize(  0,   0,   0,   0);
    Diffuse.Initialize (  0,   0,   0,  32);
    Ambient.Initialize (255, 255, 255,  32);
    Emissive.Initialize(  0,   0,   0,  32);

    p_MaterialCloud->Initialize(&Emissive, &Specular,&Diffuse,&Ambient,0);

    // Create material (lighting on texture)

    p_Material = p_3D->CreateMaterial();

    Specular.Initialize(  0,   0,   0,   0);
    Diffuse.Initialize (100, 100, 100, 255);
    Ambient.Initialize (128, 128, 128, 255);
    Emissive.Initialize(  0,   0,   0, 255);

    p_Material->Initialize(&Emissive, &Specular,&Diffuse,&Ambient,0);

    // Create Near Tree
    p_NearTree = p_3D->CreatePolygonMesh();

    NTVertex[ 0].Initialize(    0,  5,    0,  .5f, .1f);

    NTVertex[ 1].Initialize(-2.0f,  1, 2.0f,  .5f,.75f);
    NTVertex[ 2].Initialize( 2.0f,  1, 2.0f,  .5f,.75f);
    NTVertex[ 3].Initialize(-2.0f,  1,-2.0f,  .4f,.75f);
    NTVertex[ 4].Initialize( 2.0f,  1,-2.0f,  .7f,.75f);

    NTVertex[ 5].Initialize(-.25f,  1,-.25f,  .50f,.85f);
    NTVertex[ 6].Initialize( .25f,  1,-.25f,  .55f,.85f);
    NTVertex[ 7].Initialize(-.25f,  1, .25f,  .50f,.85f);
    NTVertex[ 8].Initialize( .25f,  1, .25f,  .55f,.85f);

    NTVertex[ 9].Initialize(-.25f,  0,-.25f,  .50f, 1);
    NTVertex[10].Initialize( .25f,  0,-.25f,  .55f, 1);
    NTVertex[11].Initialize(-.25f,  0, .25f,  .50f, 1);
    NTVertex[12].Initialize( .25f,  0, .25f,  .55f, 1);

    p_NearTree->Initialize(FW_TRIANGLES, 13, NTVertex);

    Indx[ 0] = 0;
    Indx[ 1] = 4;
    Indx[ 2] = 3;

    Indx[ 3] = 0;
    Indx[ 4] = 2;
    Indx[ 5] = 4;

    Indx[ 6] = 0;
    Indx[ 7] = 1;
    Indx[ 8] = 2;

    Indx[ 9] = 0;
    Indx[10] = 3;
    Indx[11] = 1;

    Indx[12] = 5;
    Indx[13] = 6;
    Indx[14] = 9;

    Indx[15] = 6;
    Indx[16] = 10;
    Indx[17] = 9;

    Indx[18] = 6;
    Indx[19] = 8;
    Indx[20] = 10;

    Indx[21] = 8;
    Indx[22] = 12;
    Indx[23] = 10;

    Indx[24] = 8;
    Indx[25] = 7;
    Indx[26] = 12;

    Indx[27] = 7;
    Indx[28] = 11;
    Indx[29] = 12;

    Indx[30] = 7;
    Indx[31] = 5;
    Indx[32] = 11;

    Indx[33] = 5;
    Indx[34] = 9;
    Indx[35] = 11;

    p_NearTree->SetIndexBuffer(Indx, 36);

    // Create Mid Tree
    p_MidTree = p_3D->CreatePolygonBillboard();
    p_MidTree->Initialize(p_Camera, 5, 5, true);

    // Create Far Tree
    p_FarTree = p_3D->CreatePolygonMesh();

    NTVertex[0].Initialize(  0,  5,   0,    .5f,.70f);
    NTVertex[1].Initialize(  0,  0,   0,    .5f,.90f);

    p_FarTree->Initialize(FW_LINES, 2, NTVertex);

    Indx[0] = 0;
    Indx[1] = 1;

    p_FarTree->SetIndexBuffer(Indx, 2);

    // Create SQUARE ground

    p_WindowText->PrintText("    Creating terrain");
    srand(1);

    p_Ground = p_3D->CreatePolygonMesh();

    for(y = 0; y < 11; y++)
    {
      for(x = 0; x < 11; x++)
      {
        z1[x][y] = ((float)rand() / (float)RAND_MAX) * 50;
      }
    }

    for(y = 0; y < 10; y++)
    {
      for(x = 0; x < 10; x++)
      {

        Points[(y * 40) + (x * 4) + 0].Initialize(x * 100,        z1[x][y+1],     y * 100 + 100,   0,0);
        Points[(y * 40) + (x * 4) + 1].Initialize(x * 100 + 100,  z1[x+1][y+1],   y * 100 + 100,   1,0);
        Points[(y * 40) + (x * 4) + 2].Initialize(x * 100,        z1[x][y],       y * 100,         0,1);
        Points[(y * 40) + (x * 4) + 3].Initialize(x * 100 + 100,  z1[x+1][y],     y * 100,         1,1);

        StandardVector3 tempVec1, tempVec2, tempVecR;

        tempVec1 = Points[(y * 40) + (x * 4) + 0].p - Points[(y * 40) + (x * 4) + 1].p;
        tempVec2 = Points[(y * 40) + (x * 4) + 0].p - Points[(y * 40) + (x * 4) + 2].p;
        tempVecR = Cross(tempVec1, tempVec2);
        tempVecR.Normalize();
        Points[(y * 40) + (x * 4) + 0].n = tempVecR;

        tempVec1 = Points[(y * 40) + (x * 4) + 1].p - Points[(y * 40) + (x * 4) + 0].p;
        tempVec2 = Points[(y * 40) + (x * 4) + 1].p - Points[(y * 40) + (x * 4) + 3].p;
        tempVecR = Cross(tempVec2, tempVec1);
        tempVecR.Normalize();
        Points[(y * 40) + (x * 4) + 1].n = tempVecR;

        tempVec1 = Points[(y * 40) + (x * 4) + 2].p - Points[(y * 40) + (x * 4) + 0].p;
        tempVec2 = Points[(y * 40) + (x * 4) + 2].p - Points[(y * 40) + (x * 4) + 3].p;
        tempVecR = Cross(tempVec1, tempVec2);
        tempVecR.Normalize();
        Points[(y * 40) + (x * 4) + 2].n = tempVecR;

        tempVec1 = Points[(y * 40) + (x * 4) + 3].p - Points[(y * 40) + (x * 4) + 1].p;
        tempVec2 = Points[(y * 40) + (x * 4) + 3].p - Points[(y * 40) + (x * 4) + 2].p;
        tempVecR = Cross(tempVec2, tempVec1);
        tempVecR.Normalize();
        Points[(y * 40) + (x * 4) + 3].n = tempVecR;

        Indx2[(y * 60) + (x * 6) + 0] = (y * 40) + (x * 4) + 0;
        Indx2[(y * 60) + (x * 6) + 1] = (y * 40) + (x * 4) + 1;
        Indx2[(y * 60) + (x * 6) + 2] = (y * 40) + (x * 4) + 2;
        Indx2[(y * 60) + (x * 6) + 3] = (y * 40) + (x * 4) + 2;
        Indx2[(y * 60) + (x * 6) + 4] = (y * 40) + (x * 4) + 1;
        Indx2[(y * 60) + (x * 6) + 5] = (y * 40) + (x * 4) + 3;
      }
    }

    p_Ground->Initialize(FW_TRIANGLES, 10 * 10 * 4, Points);
    p_Ground->SetIndexBuffer(Indx2, 10 * 10 * 6);

    p_WindowText->PrintText("Generating LOD for trees.");

    // Create LOD for trees.
    for(x = 0; x < 500; x++)
    {
      p_Tree[x] = new StandardLOD;

      p_Tree[x]->Initialize(p_Camera);

      p_Tree[x]->SetMidStart(200);
      p_Tree[x]->SetMidEnd(500);
      p_Tree[x]->SetFarEnd(1000);

      p_Tree[x]->SetNear(p_NearTree);
      p_Tree[x]->SetMid(p_MidTree);
      p_Tree[x]->SetFar(p_FarTree);

      x1 = ((float)rand() / (float)RAND_MAX) * 1000;
      y1 = ((float)rand() / (float)RAND_MAX) * 1000;

      StandardVector3 tempOrig;
      StandardVector3 tempDir;

      tempOrig.x = x1;
      tempOrig.y = 100;
      tempOrig.z = y1;

      tempDir.x = 0;
      tempDir.y = -1;
      tempDir.z = 0;

      float u,v;

      StandardVector3 v0,v1,v2,v3;

      v0.x = ((int)(x1/1000 * 10) * 100);
      v0.y = z1[(int)(x1/100)][(int)(y1/100) + 1];
      v0.z = ((int)(y1/1000 * 10) * 100) + 100;

      v1.x = ((int)(x1/1000 * 10) * 100) + 100;
      v1.y = z1[(int)(x1/100) + 1][(int)(y1/100) + 1];
      v1.z = ((int)(y1/1000 * 10) * 100) + 100;

      v2.x = ((int)(x1/1000 * 10) * 100);
      v2.y = z1[(int)(x1/100)][(int)(y1/100)];
      v2.z = ((int)(y1/1000 * 10) * 100);

      if (RayIntersectTriangle(tempOrig, tempDir, v0,v1,v2, &z,&u,&v) == false)
      {
        
        v0.x = ((int)(x1/1000 * 10) * 100);
        v0.y = z1[(int)(x1/100)][(int)(y1/100)];
        v0.z = ((int)(y1/1000 * 10) * 100);

        v1.x = ((int)(x1/1000 * 10) * 100) + 100;
        v1.y = z1[(int)(x1/100) + 1][(int)(y1/100) + 1];
        v1.z = ((int)(y1/1000 * 10) * 100) + 100;

        v2.x = ((int)(x1/1000 * 10) * 100) + 100;
        v2.y = z1[(int)(x1/100) + 1][(int)(y1/100)];
        v2.z = ((int)(y1/1000 * 10) * 100);
        
        RayIntersectTriangle(tempOrig, tempDir, v0,v1,v2, &z,&u,&v);

      }

      p_Tree[x]->SetPos(x1, 100 - z, y1);

    }
    // Create material (lighting on texture) for sky

    p_MaterialSky = p_3D->CreateMaterial();

    Specular.Initialize(  0,   0,   0,   0);
    Diffuse.Initialize (  0,   0,   0, 255);
    Ambient.Initialize (  0,   0,   0, 255);
    Emissive.Initialize(110, 170, 240, 255);

    p_MaterialSky->Initialize(&Emissive, &Specular,&Diffuse,&Ambient,0);

    // Create SKY
    p_Sky = p_3D->CreatePolygonMesh();

    NTVertex[0].Initialize(-p_Init->FarZ,  p_Init->FarZ * .2f, -p_Init->FarZ,  0,0);
    NTVertex[1].Initialize( p_Init->FarZ,  p_Init->FarZ * .2f, -p_Init->FarZ,  1,0);
    NTVertex[2].Initialize(-p_Init->FarZ,  0,  p_Init->FarZ,                   0,1);
    NTVertex[3].Initialize( p_Init->FarZ,  0,  p_Init->FarZ,                   1,1);

    p_Sky->Initialize(FW_TRIANGLES, 4, NTVertex);

    Indx[0] = 0;
    Indx[1] = 3;
    Indx[2] = 1;
    Indx[3] = 0;
    Indx[4] = 2;
    Indx[5] = 3;

    p_Sky->SetIndexBuffer(Indx, 6);

    p_WindowText->PrintText("Creating Cloud");

    // Create Cloud
    p_Cloud = p_3D->CreatePolygonMesh();

    NTVertex[0].Initialize(-50000,  1500, 50000,  0,0);
    NTVertex[1].Initialize( 50000,  1500, 50000,  9,0);
    NTVertex[2].Initialize(-50000,  1500,-50000,  0,9);
    NTVertex[3].Initialize( 50000,  1500,-50000,  9,9);

    p_Cloud->Initialize(FW_TRIANGLES, 4, NTVertex);

    Indx[0] = 0;
    Indx[1] = 2;
    Indx[2] = 1;
    Indx[3] = 2;
    Indx[4] = 3;
    Indx[5] = 1;

    p_Cloud->SetIndexBuffer(Indx, 6);

    // Create Cloud
    p_Cloud2 = p_3D->CreatePolygonMesh();

    NTVertex[0].Initialize(-50000,  1500, 50000,  9,9);
    NTVertex[1].Initialize( 50000,  1500, 50000,  0,9);
    NTVertex[2].Initialize(-50000,  1500,-50000,  9,0);
    NTVertex[3].Initialize( 50000,  1500,-50000,  0,0);

    p_Cloud2->Initialize(FW_TRIANGLES, 4, NTVertex);

    Indx[0] = 0;
    Indx[1] = 2;
    Indx[2] = 1;
    Indx[3] = 2;
    Indx[4] = 3;
    Indx[5] = 1;

    p_Cloud2->SetIndexBuffer(Indx, 6);


    // dark top
    p_SquareTop = p_3D->CreatePolygonFlat();

    TLVertex[0].Initialize(  0.0f,  0.0f,    0,  0,  0,100);
    TLVertex[1].Initialize(  1.0f,  0.0f,    0,  0,  0,100);
    TLVertex[2].Initialize(  0.0f,  0.025f,  0,  0,  0, 50);
    TLVertex[3].Initialize(  1.0f,  0.025f,  0,  0,  0, 50);

    p_SquareTop->Initialize(FW_TRIANGLES, false, 4, TLVertex);

    Indx[0] = 0;
    Indx[1] = 1;
    Indx[2] = 2;
    Indx[3] = 1;
    Indx[4] = 3;
    Indx[5] = 2;

    p_SquareTop->SetIndexBuffer(Indx, 6);

    p_WindowText->PrintText("OK\n");

    // Create GUI device
    p_WindowText->PrintText("    GUI........");

    result = p_GUI->Initialize(this, p_Window3D, p_Framework, p_Input, p_3D);

    // Setup main screen
    p_MainScreen = p_GUI->CreateScreen();
    p_MainScreen->Initialize(0,0);

    // Setup Exit Screen.
    Color.Initialize(0,0,0,150);

    p_ExitScreen = p_GUI->CreateScreen();
    p_ExitScreen->Initialize(1,&Color);

    _Rect Rect;

    Rect.x1 = 0.1f;
    Rect.y1 = 0.1f;
    Rect.x2 = 0.4f;
    Rect.y2 = 0.4f;

    p_ExitWindow = p_ExitScreen->CreateGUIWindow("Exit?", &Rect);

    Rect.x1 = 0.10f;
    Rect.y1 = 0.35f;
    Rect.x2 = 0.45f;
    Rect.y2 = 0.65f;

    p_ExitWindow->CreateButton(0,"Yes", &Rect);

    Rect.x1 = 0.55f;
    Rect.y1 = 0.35f;
    Rect.x2 = 0.90f;
    Rect.y2 = 0.65f;

    p_ExitWindow->CreateButton(1,"No", &Rect);

    Rect.x1 = 0.6f;
    Rect.y1 = 0.6f;
    Rect.x2 = 1.0f;
    Rect.y2 = 1.0f;

    p_InfoWindow = p_ExitScreen->CreateGUIWindow("Credits", &Rect);


    Rect.x1 = 0.0f;
    Rect.y1 = 0.0f;
    Rect.x2 = 1.0f;
    Rect.y2 = 1.0f;

    p_InfoWindow->CreateLabel("(C) 2001 by Combat Simulator Project\n"
                              "\n"
                              "http://csp.homeip.net\n",
                              &Rect);


    // setup paused screen
    Color.Initialize(100,0,0,150);

    p_PausedScreen = p_GUI->CreateScreen();
    p_PausedScreen->Initialize(2,&Color);

    Rect.x1 = 0.1f;
    Rect.y1 = 0.1f;
    Rect.x2 = 0.9f;
    Rect.y2 = 0.2f;

    p_PausedWindow = p_PausedScreen->CreateGUIWindow("Paused", &Rect);

    p_WindowText->PrintText("OK\n");

EARLY_EXIT:

    if(result != _A_OK)
    {
      char ach[256];
      sprintf(ach, "Failed! Error [%d]\n", result);

      p_WindowText->PrintText(ach);
      p_WindowText->Run();
    }
  }

  return result;
}

void App::Run()
{

  p_WindowText->PrintText("Run\n");

  // Show the 3D window
  p_WindowText->PrintText("    Show Window......");
  p_Window3D->Show();
  p_WindowText->PrintText("OK\n");

  // Activate Screen 0
  p_WindowText->PrintText("    Set GUI Screen...");
  p_GUI->SetScreen(p_MainScreen);
  p_WindowText->PrintText("OK\n");

  // RUN
  p_WindowText->PrintText("    Run GUI..........");
  p_GUI->Run();
  p_WindowText->PrintText("OK\n");

  return;
}

void App::OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy)
{

  if(p_bPaused == false)
  {
    if(joy != 0)
    {
      p_Camera->Rotate('x', -(joy->y * 0.001f) * p_fMultiplier);
      p_Camera->Rotate('y', (joy->z * 0.0001f) * p_fMultiplier);
      p_Camera->Rotate('z', -(joy->x * 0.001f) * p_fMultiplier);

      p_Camera->Move('z', ((100 - joy->throttle) * 0.01f) * p_fMultiplier);
    }
    if(mouse != 0)
    {
      if (mouse->mouse2 == true)
      {
        p_Camera->Rotate('y', ((mouse->x * 0.01f) * p_fMultiplier) / p_fMultiplier);
        p_Camera->Rotate('x', -((mouse->y * 0.01f) * p_fMultiplier) / p_fMultiplier);
        if(mouse->mouse1 == true)
        {
          p_Camera->Move('z', ((0.9f) * p_fMultiplier) / p_fMultiplier);
        }
      }
    }
  }

  return;
}

void App::OnStartCycle()
{
  stim = p_Framework->GetTime();
}

void App::OnEndCycle()
{
  // Calculate FPS
  etim = p_Framework->GetTime();

  if(etim == stim)  etim++;
  rtim = etim - stim;

  //  --- uncomment when not in testing phase to make a 200 max FPS.
  if(rtim < 5)
  {
    p_Framework->Nap(5 - rtim);
    rtim = 5;
  }
  
  p_fFPS = (short)(1000 / rtim);
  if(p_fFPS == 0) p_fFPS = 1;
	p_fMultiplier = (float)(float)30 / (float)p_fFPS;

}

void App::OnRender()
{

  unsigned long TPF = 0;

  unsigned short x;

  // draw scene
  p_3D->BeginScene();
  p_3D->Clear();

  // draw sky
  p_3D->SetRenderState(FW_ZBUFFERWRITE, false);
  p_3D->SetRenderState(FW_ZBUFFER,false);

  p_3D->SetTexture(0);
  p_3D->SetMaterial(p_MaterialSky);

  StandardMatrix4 tempCamera;
  tempCamera = *p_Camera->GetCameraMatrix();

  StandardMatrix4 tempSky;

  tempSky = StandardMatrix4::IDENTITY;

  float headingt;

  headingt = (float)atan2(tempCamera.rowcol[2][2], tempCamera.rowcol[0][2]);

  tempSky.rowcol[0][2] = (float)cos(headingt);
  tempSky.rowcol[1][2] = 0;
  tempSky.rowcol[2][2] = (float)sin(headingt);
  tempSky.rowcol[0][1] = 0;
  tempSky.rowcol[1][1] = 1;
  tempSky.rowcol[2][1] = 0;

  StandardVector3 vec1, vec2, vecr;

  vec1.x = tempSky.rowcol[0][2];
  vec1.y = tempSky.rowcol[1][2];
  vec1.z = tempSky.rowcol[2][2];

  vec2.x = tempSky.rowcol[0][1];
  vec2.y = tempSky.rowcol[1][1];
  vec2.z = tempSky.rowcol[2][1];

  vecr = Cross(vec1, vec2);

  tempSky.rowcol[0][0] = vecr.x;
  tempSky.rowcol[1][0] = vecr.y;
  tempSky.rowcol[2][0] = vecr.z;

  tempCamera.Inverse();

  tempSky.rowcol[3][0] = tempCamera.rowcol[3][0];
  tempSky.rowcol[3][1] = tempCamera.rowcol[3][1];
  tempSky.rowcol[3][2] = tempCamera.rowcol[3][2];

  p_Sky->SetMatrix(&tempSky);

  TPF += p_Sky->Draw();

  // draw clouds
  p_3D->SetMaterial(p_MaterialCloud);
  p_3D->SetTexture(p_TextureCloud);
  TPF += p_Cloud2->Draw();
  TPF += p_Cloud->Draw();

  p_3D->SetRenderState(FW_ZBUFFERWRITE, true);
  p_3D->SetRenderState(FW_ZBUFFER,true);

  // draw ground
  p_3D->SetMaterial(p_Material);
  //p_3D->SetTexture(0);
  p_3D->SetTexture(p_TextureGrass);
  TPF += p_Ground->Draw();

  // draw tree
  //p_3D->SetTexture(0);
  p_3D->SetTexture(p_TextureTree);

  for(x=0;x<500;x++)
  {
    TPF += p_Tree[x]->Draw();
  }

  // draw rest
  p_3D->SetRenderState(FW_ZBUFFERWRITE, false);
  p_3D->SetRenderState(FW_ZBUFFER,false);

  p_3D->SetTexture(0);
  p_3D->SetMaterial(0);

  TPF += p_SquareTop->Draw();
  TPF += p_GUI->Draw();

  // draw stats
  _Rect rect;
  StandardColor color;

  color.r = 100;
  color.g = 255;
  color.b = 100;
  color.a = 255;

  rect.x1 = 0;
  rect.y1 = 0;
  rect.x2 = 1;
  rect.y2 = 0.025f;

  char ach[256];

  // LEFT
  sprintf(ach, "TPF - %4.4d", TPF);
  p_3D->DrawTextF(ach, rect, 0, 1, &color);

  // MIDDLE
  sprintf(ach, "FPS - %4.4f", p_fFPS);
  p_3D->DrawTextF(ach, rect, 1, 1, &color);

  // RIGHT SIDE
  _Point pt;

  pt = p_Window3D->GetMouse();

  sprintf(ach, "MouseXY - %4.4f %4.4f", pt.x, pt.y);
  p_3D->DrawTextF(ach, rect, 2, 1, &color);

  // END
  p_3D->EndScene();
  p_3D->Flip();

  return;
}

void App::OnNetwork()
{

  return;
}

void App::On2D(char *InputLine)
{

  //if (stricmp(InputLine, "config") == 0)
  //{
  //  p_WindowText->PrintText("Not implemented.\n");
  //}
  
  return;
}

void App::OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,
                _GUI *GUI)
{

  if(key != 0)
  {
    if(key->down == true)
    {
      switch(key->key)
      {
        case 1:
          if(p_bPaused == true)
          {
            p_GUI->SetScreen(p_MainScreen);
            p_bPaused = false;
            //p_Framework->TriggerShutdown();
          }
          else
          {
            p_GUI->SetScreen(p_ExitScreen);
            p_bPaused = true;
          }
          break;
        case 197:
          if(p_bPaused == true)
          {
            p_GUI->SetScreen(p_MainScreen);
            p_bPaused = false;
          }
          else
          {
            p_GUI->SetScreen(p_PausedScreen);
            p_bPaused = true;
          }
          break;
      }
    }
  }


  switch(GUI->Screen)
  {
    case 1:
      switch(GUI->Window)
      {
        case 0:
          switch(GUI->Button)
          {
            case 0:
              p_Framework->TriggerShutdown();
              break;
            case 1:
              p_GUI->SetScreen(p_MainScreen);
              p_bPaused = false;
              break;
          }
          break;
      }
      break;
  }

  return;
}

void App::RequestCommands()
{
  //p_WindowText->PrintText("config           - This will launch the config screen.\n");

  return;
}

_APP_INIT_STRUCT* App::GetInit()
{
  return &p_AppInit;
}

void App::Uninitialize()
{

  for(unsigned short x=0;x<500;x++)
  {
    if(p_Tree[x] != 0)
    {
      p_Tree[x]->Uninitialize();
      delete p_Tree[x];
      p_Tree[x] = 0;
    }
  }

  if(p_NearTree != 0)
  {
    p_NearTree->Uninitialize();
    delete p_NearTree;
    p_NearTree = 0;
  }

  if(p_MidTree != 0)
  {
    p_MidTree->Uninitialize();
    delete p_MidTree;
    p_MidTree = 0;
  }

  if(p_FarTree != 0)
  {
    p_FarTree->Uninitialize();
    delete p_FarTree;
    p_FarTree = 0;
  }

  if(p_WindowText != 0)
  {
    p_WindowText->Uninitialize();
    delete p_WindowText;
    p_WindowText = 0;
  }

  if(p_Input != 0)
  {
    p_Input->Uninitialize();
    delete p_Input;
    p_Input = 0;
  }

  if(p_ExitWindow != 0)
  {
    p_ExitWindow->Uninitialize();
    delete p_ExitWindow;
    p_ExitWindow = 0;
  }

  if(p_PausedWindow != 0)
  {
    p_PausedWindow->Uninitialize();
    delete p_PausedWindow;
    p_PausedWindow = 0;
  }

  if(p_InfoWindow != 0)
  {
    p_InfoWindow->Uninitialize();
    delete p_InfoWindow;
    p_InfoWindow = 0;
  }

  if(p_PausedScreen != 0)
  {
    p_PausedScreen->Uninitialize();
    delete p_PausedScreen;
    p_PausedScreen = 0;
  }

  if(p_ExitScreen != 0)
  {
    p_ExitScreen->Uninitialize();
    delete p_ExitScreen;
    p_ExitScreen = 0;
  }

  if(p_MainScreen != 0)
  {
    p_MainScreen->Uninitialize();
    delete p_MainScreen;
    p_MainScreen = 0;
  }

  if(p_TextureCloud != 0)
  {
    p_TextureCloud->Uninitialize();

    delete p_TextureCloud;
    p_TextureCloud = 0;
  }

  if(p_Cloud != 0)
  {
    p_Cloud->Uninitialize();

    delete p_Cloud;
    p_Cloud = 0;
  }

  if(p_Cloud2 != 0)
  {
    p_Cloud2->Uninitialize();

    delete p_Cloud2;
    p_Cloud2 = 0;
  }

  if(p_Ground != 0)
  {
    p_Ground->Uninitialize();

    delete p_Ground;
    p_Ground = 0;
  }

  if(p_TextureTree != 0)
  {
    p_TextureTree->Uninitialize();

    delete p_TextureTree;
    p_TextureTree = 0;
  }

  if(p_Sky != 0)
  {
    p_Sky->Uninitialize();

    delete p_Sky;
    p_Sky = 0;
  }

  if(p_SquareTop != 0)
  {
    p_SquareTop->Uninitialize();

    delete p_SquareTop;
    p_SquareTop = 0;
  }

  if (p_Material != 0)
  {
	  p_Material->Uninitialize();

	  delete p_Material;
	  p_Material=0;
  }
 
  if (p_MaterialSky != 0)
  {
	  p_MaterialSky->Uninitialize();

	  delete p_MaterialSky;
	  p_MaterialSky=0;
  }

  if (p_MaterialCloud != 0)
  {
	  p_MaterialCloud->Uninitialize();

	  delete p_MaterialCloud;
	  p_MaterialCloud=0;
  }

  if(p_TextureGrass != 0)
  {
    p_TextureGrass->Uninitialize();

    delete p_TextureGrass;
    p_TextureGrass = 0;
  }

  if(p_GUI != 0)
  {
    p_GUI->Uninitialize();

    delete p_GUI;
    p_GUI = 0;
  }

  if(p_3D != 0)
  {
    p_3D->Uninitialize();

    delete p_3D;
    p_3D = 0;
  }

  if(p_Window3D != 0)
  {
    p_Window3D->Uninitialize();

    delete p_Window3D;
    p_Window3D = 0;
  }

  if(p_Framework != 0)
  {
    p_Framework->Uninitialize();

    delete p_Framework;
    p_Framework = 0;
  }

  if(p_Factory != 0)
  {
    p_Factory->Uninitialize();

    delete p_Factory;
    p_Factory = 0;
  }

  if(p_Init != 0)
  {
    delete p_Init;
    p_Init = 0;
  }

  return;
}
