#include "SimpleFlightApp.h"

// Must create a global application object.
SimpleFlightApp myApp;

// ------ StandardApp Constructors/Deconstructors

SimpleFlightApp::SimpleFlightApp(): BaseApp()
{
  CSP_LOG(CSP_APP, CSP_TRACE, "SimpleFlightApp::SimpleFlightApp()");

}

SimpleFlightApp::~SimpleFlightApp()
{
  CSP_LOG(CSP_APP, CSP_TRACE, "SimpleFlightApp::~SimpleFlightApp()");

  Uninitialize();
}

// ------ StandardApp Publics

short SimpleFlightApp::Initialize(_APP_INIT_STRUCT *AppInit)
{
  CSP_LOG(CSP_APP, CSP_TRACE, "SimpleFlightApp::Initialize");

  int result;

  BaseApp::Initialize(AppInit);
	
  p_Camera = p_3D->CreateCamera();
  p_Camera->SetPerspectiveView(60, 640, 480, 0.5, 100000);
  p_Camera->SetCameraIdentity();


  p_Material = p_3D->CreateMaterial();
  p_Terrain = GeometryMeshLib::CreateRandomTerrainMesh(p_3D,
        25,25,50000.0f, 50000.0f, 5000.0f);

  p_Texture = p_3D->CreateTexture();
  p_Texture->Initialize("grass.bmp", 1);


  StandardColor Specular( 0,   0,    0,   0);
  StandardColor Diffuse ( 50,  50,  50, 255);
  StandardColor Ambient (255, 255, 255, 255);
  StandardColor Emissive(  0,   0,   0, 255);

  p_Material->Initialize(&Emissive,&Specular,&Diffuse,&Ambient,0);
      


  InitializeSim();

  p_WindowText->Uninitialize();
  p_Window3D->Show();

  p_3D->SetRenderState(FW_FOG, true);
  p_3D->SetRenderState(FW_FOGMODE, FW_FOG_LINEAR);
  float fogStart = 5000.0;
  p_3D->SetRenderState(FW_FOGSTART, *((int*)(&fogStart)));
  float fogEnd = 50000.0;
  p_3D->SetRenderState(FW_FOGEND, *((int*)(&fogEnd)));
  float fogDensity = 0.00001;
  p_3D->SetRenderState(FW_FOGDENSITY, *((int*)(&fogDensity)));
  StandardColor blue(0,0,255,0);
  p_3D->SetRenderState(FW_FOGCOLOR, (int)&blue);

  p_3D->SetRenderState(FW_WINDING, FW_CW);

  return _A_OK;
}



void SimpleFlightApp::Uninitialize()
{
  CSP_LOG(CSP_APP, CSP_TRACE, "SimpleFlightApp::Uninitialize()");


  if (p_Terrain != 0)
  {
	p_Terrain->Uninitialize();
	delete p_Terrain;
	p_Terrain=0;
  }


  if (p_Texture!=0)
  {
	p_Texture->Uninitialize();
	delete p_Texture;
	p_Texture=0;
  }


  if(p_Framework != 0)
  {
    p_Framework->Uninitialize();

    delete p_Framework;
    p_Framework = 0;
  }

  return;
}

void SimpleFlightApp::OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy)
{
 //---

  CSP_LOG(CSP_APP, CSP_TRACE, "SimpleFlightApp::OnMovement");


  a_key = key[97];
  s_key = key[115];
  d_key = key[100];
  f_key = key[102];

  left_key = key[276];
  right_key = key[275];
  up_key = key[273];
  down_key = key[274];

  //  mouse_x = 0;
  //  mouse_y = 0;

  mouse_x = mouse->x;
  mouse_y = mouse->y;

  // use this for joystick control
  elevator = joy->y;
  aileron = joy->x;

  //if (down_key)
  //    {
  //      elevatorKey+=0.01;
  //      if (elevatorKey > 1.0) elevatorKey = 1.0;
  //    }
  //  if (up_key)
  //    {
  //      elevatorKey-=0.01;
  //      if (elevatorKey < -1.0) elevatorKey = -1.0;
  //    }

  //  if (left_key)
  //    {
  //      aileronKey -= 0.01;
  //      if (aileronKey < -1.0) aileronKey = -1.0;
  //    }
  //  if (right_key)
  //    {
  //      aileronKey += 0.01;
  //  if (aileronKey > 1.0) aileronKey = 1.0;
  //    }

#ifdef HAVE_OPENAL
  if (s_key)
     {
       fprintf(stdout, "App::OnMovement - starting sound\n");
        p_Audio->Play_Sound(sound_a, 255,1);
    }
  if (a_key)
     {
       fprintf(stdout, "App::OnMovement - stopping sound\n");
        p_Audio->Stop_Sound(sound_a);
    }
  if (d_key)
     {
	p_Audio->Stop_Sound(sound_a);
	p_Audio->Play_Sound(sound_a, 128,1);
   }
#endif
  // elevator = elevatorKey*100;
  //  aileron = aileronKey*100;

  //elevator =0;
  //aileron =0;

  // update the orientation matrix based on control surfaces;
  StandardMatrix3 matZ = RotationZMatrix3(-(aileron) * sensativity * planesensx * plusplus);
  planeOrientation = matZ*planeOrientation;

  StandardMatrix3 matX = RotationXMatrix3((elevator) * sensativity * planesensy * plusplus);
  planeOrientation = matX*planeOrientation;

  // calculate the vector plane directions.
  planeCurrentDirection = planeInitialDirection*planeOrientation;
  planeCurrentNormDir = planeInitialNormDir*planeOrientation;

  // calculate the forces;
//  planeThrustForce = 100.0*thrust*planeCurrentDirection*100.0;
//  planeLiftForce = 1.0e-5*planeSpeed*planeSpeed*planeCurrentNormDir;
//  planeDragForce = -1.0e-9*planeSpeed*planeSpeed*planeCurrentDirection;

  // the resultant force is the sum of all the other forces;
//  planeCurrentForceTotal = planeGravityForce+planeThrustForce+
//						planeLiftForce+planeDragForce;

  // numerically integrate the velocity based on the forces.
//  planeCurrentVelocity += dt*planeCurrentForceTotal;
//  planeSpeed = planeCurrentVelocity.length();
//  planeCurrentVelocityDir = planeCurrentVelocity.normalize();

//  planeCurrentPosition +=  dt*planeCurrentVelocity;

//  planeCurrentPosition += dt/p_fFPS*100*planeSpeed*planeCurrentDirection;
    planeCurrentPosition += dt*planeSpeed*planeCurrentDirection;

//  math->MatrixTranslation(&MatrixTemp, 0,0, ((float)(100 - joystick.throttle) / (float)100) * plusplus);
//  math->MatrixInverse(&MatrixTemp);
//  math->MatrixMultiply(&matPlane, &MatrixTemp);

//  heading = (float)atan2(matPlane._fx, matPlane._fz);
//  heading = math->ToDegree(heading);
//  if(heading < 0) heading = 360 + heading;
//  headingx = (float)cos(heading);
//  headingz = (float)sin(heading);

  return;
}


void SimpleFlightApp::OnRender()
{

  CSP_LOG(CSP_APP, CSP_TRACE, "SimpleFlightApp::OnRender()");

  unsigned long stim, etim, rtim;

  //stim = p_Framework->GetTime();
  unsigned long TPF = 0;
  _Rect rect;
  StandardColor color;
  
  color.r = 255;
  color.g = 255;
  color.b = 255;
  color.a = 255;

  rect.x1 = 0;
  rect.y1 = 0;
  rect.x2 = 1;
  rect.y2 = 0.025f;

  char ach[256];
  char ach2[256];


  sprintf(ach, "FPS - %f", p_fFPS);

  float altitude = planeCurrentPosition.y;
  float horizon = sqrt(2.0*6.370e6*altitude+altitude*altitude);

//  p_Camera->SetPerspectiveView(60, 800, 600, 0.5, horizon); 

  StandardVector3 target = planeCurrentPosition+planeCurrentDirection;

//  p_Camera->SetPosition(0,200,-10,0,0,0,0,1,0);
  p_Camera->SetPosition(&planeCurrentPosition,
	                    &target,    /* needs to be target instead of direction */
						&planeCurrentNormDir);
  p_3D->SetCamera(p_Camera);
  StandardMatrix4 * pCameraMat = p_Camera->GetCameraMatrix();
  StandardMatrix4 * pProjMat = p_Camera->GetProjectionMatrix();

  p_3D->BeginScene();
  p_3D->Clear();

//  p_3D->SetViewport(p_Viewport_right);
//  p_3D->SetTexture(p_Viewport->GetSurface());
//    p_3D->DrawPolygonMesh(p_Model);


	
  //p_3D->SetViewport(p_Viewport_left);

  p_3D->SetMaterial(p_Material);
  p_3D->SetTexture(p_Texture);
  TPF += p_3D->DrawPolygonMesh(p_Terrain);


	//  p_3D->SetTexture(p_Viewport->GetSurface());
//  p_3D->SetViewport(p_Viewport_full);

  rect.x1 = 0.0;
  rect.y1 = 0.0;
  rect.x2 = 1.0;
  rect.y2 = 0.05;

  // LEFT
  sprintf(ach, "TPF - %4.4d", TPF);
  p_3D->DrawTextF(ach, rect, 0, 0, &color);

  // MIDDLE
  sprintf(ach, "FPS - %4.4f", p_fFPS);
  p_3D->DrawTextF(ach, rect, 1, 0, &color);

  // RIGHT SIDE

  //  _Point pt;

  //  pt = p_Window3D->GetMouse();

  sprintf(ach, "MouseXY - %d %d", mouse_x, mouse_y);
  p_3D->DrawTextF(ach, rect, 2, 0, &color);

  sprintf(ach,"\n\na_key - %d, s_key - %d, d_key - %d, f_key - %d", a_key, s_key, d_key, f_key);
  p_3D->DrawTextF(ach, rect, 2, 0, &color);

  sprintf(ach,"\n\n\n\nleft_key - %d, right_key - %d, down_key - %d, up_key - %d", left_key, right_key, down_key, up_key);
  p_3D->DrawTextF(ach, rect, 2, 0, &color);

//  rect.y1 += 0.1;
//    rect.y2 = 0.9;
//  rect.y1 = rect.y2 - 20;
  sprintf(ach, "\nPosition: [%.3f, %.3f, %.3f]\n", planeCurrentPosition.x,
		planeCurrentPosition.y, planeCurrentPosition.z);
  p_3D->DrawTextF(ach, rect, 0, 0, &color);

  sprintf(ach, "\n\nDirection: [%6.3f, %6.3f, %6.3f]\n", planeCurrentDirection.x,
	    planeCurrentDirection.y, planeCurrentDirection.z);
  p_3D->DrawTextF(ach, rect, 0, 0, &color);

  sprintf(ach, "\n\n\nUp Direction: [%6.3f, %6.3f, %6.3f]\n", planeCurrentNormDir.x,
	  planeCurrentNormDir.y, planeCurrentNormDir.z);
  p_3D->DrawTextF(ach, rect, 0, 0, &color);

    sprintf(ach, "\n\n\n\n\nOrientation:");
  p_3D->DrawTextF(ach, rect, 0, 0, &color);

  planeOrientation.Print(ach);
  sprintf(ach2,"\n\n\n\n\n\n%s",ach);
  p_3D->DrawTextF(ach2, rect, 0, 0, &color);

//  sprintf(ach, "\n\n\n\n\n\n\n\n\n\n\n\nHorizon: %f", horizon);
//  p_3D->DrawTextF(ach, rect, 0, &color);

  p_3D->EndScene();
  p_3D->Flip();

  return;
}


void SimpleFlightApp::OnNetwork()
{
  CSP_LOG(CSP_APP, CSP_TRACE, "SimpleFlightApp::OnNetwork()");

  return;
}

void SimpleFlightApp::On2D(char *InputLine)
{
  CSP_LOG(CSP_APP, CSP_TRACE, "SimpleFlightApp::On2D");



//  if (stricmp(InputLine, "config") == 0)
//  {
//    p_Framework->GetTextWindow()->PrintText("Not implemented.\n");
//  }
  
  return;
}

void SimpleFlightApp::OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy, _GUI * GUI)
{
  CSP_LOG(CSP_APP, CSP_TRACE, "SimpleFlightApp::OnGUI");

  return;
}




void SimpleFlightApp::InitializeSim()
{
#ifdef TRACE_APP
  fprintf(stdout, "App::InitializeSim()\n");
#endif

  planeOrientation = StandardMatrix3::IDENTITY;
  planeInitialDirection = StandardVector3(0,0,-1);
  planeCurrentDirection = StandardVector3(0,0,-1);
  planeInitialNormDir = StandardVector3(0.0,1.0,0.0);
  planeCurrentNormDir = StandardVector3(0.0,1.0,0.0);
  planeCurrentPosition = StandardVector3(0.0,10000.0,0.0);
  planeSpeed = 500.0;
  planeCurrentVelocity = planeSpeed*planeCurrentDirection;
  planeCurrentVelocityDir = planeCurrentVelocity.Normalize();
  thrust = 0.7;
  thrustMin = 0.0;
  thrustMax = 1.0;
  aileron=0.0;
  elevator=0.0;
  aileronMin = -1.0;
  aileronMax = 1.0;
  elevatorMin = -1.0;
  elevatorMax = 1.0;

  aileronKey = 0.0;
  elevatorKey = 0.0;

  planeGravityForce = StandardVector3(0,-1,0);    // gravity force is always towards earth.
  planeThrustForce = StandardVector3(0,0,0);
  planeLiftForce = StandardVector3(0,0,0);
  planeDragForce = StandardVector3(0,0,0);
  planeCurrentForceTotal = StandardVector3(0,0,0);

  dt = 0.01;

  sensativity = 0.01;
  planesensx = 0.1;
  planesensy = 0.1;
  plusplus = 0.1;
}

