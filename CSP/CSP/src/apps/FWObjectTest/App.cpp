#include <stdio.h>
#include <iostream.h>
#include <math.h>
#include "windows.h"
#include "App.h"
#include <graphics/win/TextWindow.h>

#include <standard/mtxlib.h>
//#include <graphics/win/GLPolygonListObject.h>
//#include <graphics/win/Texture.h>
//#include <graphics/win/TextureFormat.h>

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
App::App()
{


  sensativity = 0.3f;
  planesensx = 1.7f;
  planesensy = 1;

  plusplus = 1;

  SkyBuff = 0;
  SkyIndexBuff = 0;


  planeOrientation = IdentityMatrix33();
  planeInitialDirection = vector3(0,0,-1);
  planeCurrentDirection = vector3(0,0,-1);
  planeInitialNormDir = vector3(0.0,1.0,0.0);
  planeCurrentNormDir = vector3(0.0,1.0,0.0);
  planeCurrentPosition = vector3(0.0,2000.0,0.0);
  planeSpeed = 700.0;
  planeCurrentVelocity = planeSpeed*planeCurrentDirection;
  planeCurrentVelocityDir = planeCurrentVelocity.normalize();
  thrust = 0.7;
  thrustMin = 0;
  thrustMax = 1.0;
  aileron=0;
  elevator=0;
  aileronMin = -1.0;
  aileronMax = 1.0;
  elevatorMin = -1.0;
  elevatorMax = 1.0;

  planeGravityForce = vector3(0,-1,0);    // gravity force is always towards earth.
  planeThrustForce = vector3(0,0,0);
  planeLiftForce = vector3(0,0,0);
  planeDragForce = vector3(0,0,0);
  planeCurrentForceTotal = vector3(0,0,0);

  

  dt = 0.01;
//	font = new Font("Font.bmp");
}

App::~App()
{

}


short App::Initialize()
{
  AppBase::Initialize();

  camera = display->CreateCamera();

  m_pCube = CreateCube();
  CreateSceneObjects();

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

    // Calculate FPS - only update every 100 steps.
	static int updateCount=0;
	if (!updateCount%100)
	{
		etim = platform->GetTime();

		if(etim == stim)  etim++;
		rtim = etim - stim;
		fps = (short)(1000 / rtim);
		if(fps == 0) fps = 1;
		  plusplus = (float)(float)30 / (float)fps;
		updateCount=1;
	}

  }

  return;
}

void App::Uninitialize()
{
  audio->Discard_Sound(m_sh1);
  audio->Discard_Sound(m_sh2);
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
    aileron = (float)joystick.x/(float)1000;
  }
  if(joystick.y != 0)
  {
	elevator = (float)joystick.y/(float)1000;
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

  // update the orientation matrix based on control surfaces;
  matrix33 matZ = Matrix33RotationZ(-(aileron) * sensativity * planesensx * plusplus);
  planeOrientation = matZ*planeOrientation;

  matrix33 matX = Matrix33RotationX((elevator) * sensativity * planesensy * plusplus);
  planeOrientation = matX*planeOrientation;

  // calculate the vector plane directions.
  planeCurrentDirection = planeInitialDirection*planeOrientation;
  planeCurrentNormDir = planeInitialNormDir*planeOrientation;

  // calculate the forces;
  planeThrustForce = 100.0*thrust*planeCurrentDirection*100.0;
  planeLiftForce = 1.0e-5*planeSpeed*planeSpeed*planeCurrentNormDir;
  planeDragForce = -1.0e-9*planeSpeed*planeSpeed*planeCurrentDirection;

  // the resultant force is the sum of all the other forces;
  planeCurrentForceTotal = planeGravityForce+planeThrustForce+
						planeLiftForce+planeDragForce;

  // numerically integrate the velocity based on the forces.
//  planeCurrentVelocity += dt*planeCurrentForceTotal;
//  planeSpeed = planeCurrentVelocity.length();
//  planeCurrentVelocityDir = planeCurrentVelocity.normalize();

//  planeCurrentPosition +=  dt*planeCurrentVelocity;

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
				thrust += 0.05;
				if (thrust > thrustMax)
					thrust = thrustMax;
//				cout << "plane_dz = " << plane_dz << endl;
		  }
		break;
	  case 51:
		  if (key.down == TRUE)
		  {
			   thrust -= 0.05;
			   if (thrust < thrustMin)
				   thrust = thrustMin;
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
//    math->MatrixRotationY(&MatrixTemp, ((float)mouse.x / (float)100) * sensativity * -1 * plusplus);
//    math->MatrixMultiply(&matBody, &MatrixTemp);

//    math->MatrixRotationX(&MatrixTemp, ((float)mouse.y / (float)100) * sensativity * plusplus);
//    math->MatrixMultiply(&matHead, &MatrixTemp);

  }
  
  return;
}


// -------------------------------------
// Render
// Renders the screen.
// -------------------------------------
void App::Render(void)
{
	float altitude = planeCurrentPosition.y;
	float horizon = sqrt(2.0*6.370e6*altitude+altitude*altitude);
//	camera->SetPerspectiveView(60, 1, horizon);
	camera->SetPerspectiveView(90, .1, 100);
//	camera->SetOrthoView( 1, 100);
	display->BeginScene();
	display->ClearSurface();
//	display->SetForeground(FALSE);

//	display->SetPerspective(60, 1.3,1, planeCurrentPosition.y*333.0);
//	_Rect rect = _Rect(0,0,640,480);
//	font->Test(rect);
//	display->EndScene();
	
//	display->SetModelViewMatrix();
//	display->SetColor(1.0,0.0,0.0);
//	display->SetIdentity();

	// use euler angles to orient plane
//	display->Rotate(plane_ang_zaxis, 0.0, 0.0, 1.0);
//	display->Rotate(plane_ang_xaxis, 1.0, 0.0, 0.0);
//	display->Rotate(plane_ang_yaxis, 0.0, 1.0, 0.0);

	// position camera at head of plane.
//	display->Translate(-plane_x,-plane_y,-plane_z);
//	display->DrawTriangle(0,0,0,10,0,0,5,5,0);

// display->Translate(0,60,50);

    camera->Reset();
	camera->Translate(0,0,10);
	static RotAngle=0;
	camera->Rotate(RotAngle++, 1,0.,0);

    //camera->SetPosition(vector3(0,0,-10), vector3(0,0,0), vector3(0,-1,0));

//	camera->SetPosition(planeCurrentPosition,
//		                planeCurrentPosition+planeCurrentDirection,    /* needs to be target instead of direction */
//						planeCurrentNormDir);

//	camera->Translate(0, 10000, 30000);
//	camera->Rotate(45, 1.0, 0,0);
//	camera->SetPosition(vector3(0,30000,100000),
//		                vector3(0,0,0),    /* needs to be target instead of direction */
//						vector3(0,1,0));

//	display->Look(plane_x, plane_y, plane_z,plane_nx, plane_ny, plane_nz,
//		plane_upx, plane_upy, plane_upz);

	m_pSkyMaterial->Apply();
//	skyObject->Draw();
    
//	grassTexture->Apply();
//	pTerrainMaterialObject->Apply();
//	terrainObject->Draw();

	m_pCube->Draw();
//	display->Translate(0.0f, 5.0e5f, 0.0f);

//	skyTexture->Apply();
//	skyObject->Draw();

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
//  rect.y1 = rect.y2 - 10;
//  rect.y2 = 480;
//  rect.x2 = 640;
  display->DrawTextF(ach, strlen(ach), rect, 1, &RGB);
//  printf("%s\n", ach);

  int topy = rect.y2;
  // do left side output

  //rect.y1 -= 10;
  rect.y2 -= 10;
  rect.y1 = rect.y2 - 20;
  sprintf(ach, "Position: [%.3f, %.3f, %.3f]\n", planeCurrentPosition.x,
		planeCurrentPosition.y, planeCurrentPosition.z);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);

  //rect.y2 -= 10;
  //rect.y1 -= 10;
  sprintf(ach, "Direction: [%6.3f, %6.3f, %6.3f]\n", planeCurrentDirection.x,
	    planeCurrentDirection.y, planeCurrentDirection.z);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);

  //rect.y2 -= 10;
  //rect.y1 -= 10;
  sprintf(ach, "Up Direction: [%6.3f, %6.3f, %6.3f]\n", planeCurrentNormDir.x,
	  planeCurrentNormDir.y, planeCurrentNormDir.z);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);

  //rect.y2 -= 10;
  sprintf(ach, "Plane Speed: %.5f\n", planeSpeed);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);
  
  //rect.y2 -= 10;
  sprintf(ach, "Plane Velocity: [%10.4e, %10.4e, %10.4e]\n",
	  planeCurrentVelocity.z, planeCurrentVelocity.y, planeCurrentVelocity.z);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);

  //rect.y2 -= 10;
  sprintf(ach, "Plane Velocity Direction: [%.3f, %.3f, %.3f]\n",
	  planeCurrentVelocityDir.x, planeCurrentVelocityDir.y, planeCurrentVelocityDir.z);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);
  
  //rect.y2 -= 10;
  sprintf(ach, "Gravity Force [%10.4e, %10.4e, %10.4e]\n", 
	  planeGravityForce.x, planeGravityForce.y, planeGravityForce.z);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);

//  rect.y2 -= 10;
  sprintf(ach, "Thrust Force [%10.4e, %10.4e, %10.4e]\n",
	  planeThrustForce.x, planeThrustForce.y, planeThrustForce.z);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);

//  rect.y2 -= 10;
  sprintf(ach, "Lift Force [%10.4e, %10.4e, %10.4e]\n",
	  planeLiftForce.x, planeLiftForce.y, planeLiftForce.z);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);

//  rect.y2 -= 10;
  sprintf(ach, "Drag Force [%10.4e, %10.4e, %10.4e]\n",
	  planeDragForce.x, planeDragForce.y, planeDragForce.z);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);

//  rect.y2 -= 10;
  sprintf(ach, "Total Force [%10.4e, %10.4e, %10.4e]\n",
	  planeCurrentForceTotal.x, planeCurrentForceTotal.y, 
	  planeCurrentForceTotal.z);
  display->DrawTextF(ach, strlen(ach), rect, 0, &RGB);

  // do right hand output
  rect.y2 = topy - 10;

    sprintf(ach, "Orientation:");
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 10;
  sprintf(ach, "%6.3f, %6.3f, %6.3f", planeOrientation.col[0].x,
      planeOrientation.col[1].x, planeOrientation.col[2].x);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 10;
  sprintf(ach, "%6.3f, %6.3f, %6.3f", planeOrientation.col[0].y,
	  planeOrientation.col[1].y, planeOrientation.col[2].y);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);
  
  rect.y2 -= 10;
  sprintf(ach, "%6.3f, %6.3f, %6.3f", planeOrientation.col[0].z,
	  planeOrientation.col[1].z, planeOrientation.col[2].z);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 10;
  sprintf(ach, "Aileron %.3f", aileron);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 10;
  sprintf(ach, "Elevator %.3f", elevator);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  rect.y2 -= 10;
  sprintf(ach, "Thrust %.3f", thrust);
  display->DrawTextF(ach, strlen(ach), rect, 2, &RGB);

  display->Flip();
  display->EndScene();


	return;


}

PolygonListObject * App::CreateCube()
{
   PolygonListObject * pCube = GetDisplay()->CreatePolygonListObject();
   _NormalVertexStruct cubeVert[8];
   _Index Indices[36];
   cubeVert[0].p.x = -1.0; cubeVert[0].p.y = -1.0; cubeVert[0].p.z = -1.0;
   cubeVert[0].n.x = -1.0; cubeVert[0].n.y = -1.0; cubeVert[0].n.z = -1.0;
   cubeVert[0].tu = 0.0;   cubeVert[0].tv = 0.0;

   cubeVert[1].p.x = -1.0; cubeVert[1].p.y = -1.0; cubeVert[1].p.z = 1.0;
   cubeVert[1].n.x = -1.0; cubeVert[1].n.y = -1.0; cubeVert[1].n.z = 1.0;
   cubeVert[1].tu = 0.0;   cubeVert[1].tv = 1.0;

   cubeVert[2].p.x = 1.0; cubeVert[2].p.y = -1.0; cubeVert[2].p.z = 1.0;
   cubeVert[2].n.x = 1.0; cubeVert[2].n.y = -1.0; cubeVert[2].n.z = 1.0;
   cubeVert[2].tu = 1.0; cubeVert[2].tv = 1.0;

   cubeVert[3].p.x = 1.0; cubeVert[3].p.y = -1.0; cubeVert[3].p.z = -1.0;
   cubeVert[3].n.x = 1.0; cubeVert[3].n.y = -1.0; cubeVert[3].n.z = -1.0;
   cubeVert[3].tu = 1.0; cubeVert[3].tv = 0.0;

   cubeVert[4].p.x = -1.0; cubeVert[4].p.y = 1.0; cubeVert[4].p.z = -1.0;
   cubeVert[4].n.x = -1.0; cubeVert[4].n.y = 1.0; cubeVert[4].n.z = -1.0;
   cubeVert[4].tu = 1.0;   cubeVert[4].tv = 0.0;

   cubeVert[5].p.x = -1.0; cubeVert[5].p.y = 1.0; cubeVert[5].p.z = 1.0;
   cubeVert[5].n.x = -1.0; cubeVert[5].n.y = 1.0; cubeVert[5].n.z = 1.0;
   cubeVert[5].tu = 0.0;   cubeVert[5].tv = 1.0;

   cubeVert[6].p.x = 1.0; cubeVert[6].p.y = 1.0; cubeVert[6].p.z = 1.0;
   cubeVert[6].n.x = 1.0; cubeVert[6].n.y = 1.0; cubeVert[6].n.z = 1.0;
   cubeVert[6].tu = 0.0; cubeVert[6].tv = 0.0;

   cubeVert[7].p.x = 1.0; cubeVert[7].p.y = 1.0; cubeVert[7].p.z = -1.0;
   cubeVert[7].n.x = 1.0; cubeVert[7].n.y = 1.0; cubeVert[7].n.z = -1.0;
   cubeVert[7].tu = 1.0; cubeVert[7].tv = 0.0;

   short IndexCount = 0;
   // bottom face
   Indices[IndexCount++] = 0; Indices[IndexCount++] = 2; Indices[IndexCount++] = 1;
   Indices[IndexCount++] = 0; Indices[IndexCount++] = 3; Indices[IndexCount++] = 2;

   // back face
   Indices[IndexCount++] = 0; Indices[IndexCount++] = 4; Indices[IndexCount++] = 7;
   Indices[IndexCount++] = 0; Indices[IndexCount++] = 7; Indices[IndexCount++] = 3;

   // left face
   Indices[IndexCount++] = 0; Indices[IndexCount++] = 1; Indices[IndexCount++] = 5;
   Indices[IndexCount++] = 0; Indices[IndexCount++] = 5; Indices[IndexCount++] = 4;

   // front face
   Indices[IndexCount++] = 1; Indices[IndexCount++] = 2; Indices[IndexCount++] = 6;
   Indices[IndexCount++] = 2; Indices[IndexCount++] = 5; Indices[IndexCount++] = 1;

   // right face
   Indices[IndexCount++] = 3; Indices[IndexCount++] = 7; Indices[IndexCount++] = 6;
   Indices[IndexCount++] = 3; Indices[IndexCount++] = 6; Indices[IndexCount++] = 2;

   // top face
   Indices[IndexCount++] = 4; Indices[IndexCount++] = 5; Indices[IndexCount++] = 6;
   Indices[IndexCount++] = 4; Indices[IndexCount++] = 6; Indices[IndexCount++] = 7;

   pCube->Create(FW_TRIANGLES, 8, cubeVert);
   pCube->SetIndices(IndexCount, sizeof(_Index), Indices);

   return pCube;
}

void App::CreateSceneObjects()
{

  int i, j;
  const int n=25; 
  const int m=25;
  static _NormalVertexStruct triVect[n*m];
  static _Index Indices[(n-1)*(m-1)*2*3];
  int vertexCount=0;
  double magfactor=500000.0;
  double scalefactor = 2*magfactor/n;
  double heightfactor=500.0;
  double texturescale = 1.0/n;
  for(i=0;i<n;i++)
	  for(j=0;j<m;j++)
	  {
		triVect[vertexCount].p.x = -magfactor+i*scalefactor; 
		//triVect[vertexCount].p.y = (float)rand()/RAND_MAX*heightfactor; 
		triVect[vertexCount].p.y = 0.0f;
		triVect[vertexCount].p.z = -magfactor+j*scalefactor;
		triVect[vertexCount].n.x = 0.0;
		triVect[vertexCount].n.y = 1.0;
		triVect[vertexCount].n.z = 0.0;
		triVect[vertexCount].tu = i*texturescale;           
		triVect[vertexCount].tv = j*texturescale;

		vertexCount++;
		
	  }

	int indexCount=0;
	for (i=0;i<m-1;i++)
		for (j=0;j<n-1;j++)
		{
			// right hand triangles
			Indices[indexCount++] = i*m+j;
			Indices[indexCount++] = i*m+j+1;
			Indices[indexCount++] = (i+1)*m+j;

			Indices[indexCount++] = (i+1)*m+j;
			Indices[indexCount++] = i*m+j+1;
			Indices[indexCount++] = (i+1)*m+j+1;

			// left hand triangles
//			Indices[indexCount++] = i*m+j;
//			Indices[indexCount++] = (i+1)*m+j;
//			Indices[indexCount++] = i*m+j+1;

//			Indices[indexCount++] = (i+1)*m+j;
//			Indices[indexCount++] = (i+1)*m+j+1;
//			Indices[indexCount++] = i*m+j+1;
		
		}
//	display->CreateVertexBuffer(triVect, sizeof(triVect), sizeof(_NormalVertex));
	terrainObject = GetDisplay()->CreatePolygonListObject();
	terrainObject->Create(FW_TRIANGLES, vertexCount, triVect);
	terrainObject->SetIndices(indexCount, sizeof(_Index), Indices);
	grassTexture = GetDisplay()->CreateTexture2D();
	grassTexture->Create("grass.bmp", Texture::DIB );
//    grassTexture->genTexObject();


	  // Create Sky points/triangles.
  
  _NormalVertex ver2[4];
  ver2[0].p.x = -20000;  ver2[0].n.x = 0;  ver2[0].tu = 0;
  ver2[0].p.y = 0;       ver2[0].n.y = 0;  ver2[0].tv = 0;
  ver2[0].p.z = 20000;   ver2[0].n.z = 1; 
  ver2[1].p.x = 20000;   ver2[1].n.x = 0;  ver2[1].tu = 0;
  ver2[1].p.y = 0;       ver2[1].n.y = 0;  ver2[1].tv = 0;
  ver2[1].p.z = 20000;   ver2[1].n.z = 1;  
  ver2[2].p.x = -20000;  ver2[2].n.x = 0;  ver2[2].tu = 0;
  ver2[2].p.y = 2000;    ver2[2].n.y = 0;  ver2[2].tv = 0;
  ver2[2].p.z = -20000;   ver2[2].n.z = 1;
  ver2[3].p.x = 20000;   ver2[3].n.x = 0;  ver2[3].tu = 0;
  ver2[3].p.y = 2000;    ver2[3].n.y = 0;  ver2[3].tv = 0;
  ver2[3].p.z = -20000;   ver2[3].n.z = 1;

  _Index indx2[6];

  indx2[0] = 1;
  indx2[1] = 0;
  indx2[2] = 2;
  indx2[3] = 1;
  indx2[4] = 2;
  indx2[5] = 3;

  skyObject = GetDisplay()->CreatePolygonListObject();
  skyObject->Create(FW_TRIANGLES, 4, ver2);
  skyObject->SetIndices(6, sizeof(_Index), indx2);

  // setup sky material
  _Color Diffuse, Ambient, Emissive, Specular;

  Specular.r = 0;
  Specular.g = 0;
  Specular.b = 0;
  Specular.a = 0;

  Diffuse.r = 1;
  Diffuse.g = 0;
  Diffuse.b = 0;
  Diffuse.a = 1;

  Ambient.r = 1.0f;
  Ambient.g = 0.0f;
  Ambient.b = 0.0f;
  Ambient.a = 1;

  Emissive.r = 1;
  Emissive.g = 0;
  Emissive.b = 0;
  Emissive.a = 1;



  m_pSkyMaterial = GetDisplay()->CreateMaterialObject();
  m_pSkyMaterial->Create(&Emissive,&Specular,&Diffuse,&Ambient,0);

}
