#include <BaseObject.h>
#include <Airplane.h>
#include <BaseController.h>
#include <ObjectManager.h>
#include <ControllerManager.h>

#include <iostream>

using namespace std;

Airplane::Airplane()
{

  object_type = 3;
  object_id = ObjectManager::getObjectManager().getNewObjectID();
  controller_id = 0;
  object_name = "UNNAMEDPLANE";
  deleteFlag = false;


  planeOrientation = StandardMatrix3::IDENTITY;
  planeInitialDirection = StandardVector3(0,-1,0);
  planeCurrentDirection = StandardVector3(0,-1,0);
  planeInitialNormDir = StandardVector3(0.0,0.0,1.0);
  planeCurrentNormDir = StandardVector3(0.0,0.0,1.0);
  planeCurrentPosition = StandardVector3(0.0,0.0,10000.0);
  planeSpeed = 500.0;      // meters per sec
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

  planeGravityForce = StandardVector3(0,0,-1);    // gravity force is always towards earth.
  planeThrustForce = StandardVector3(0,0,0);
  planeLiftForce = StandardVector3(0,0,0);
  planeDragForce = StandardVector3(0,0,0);
  planeCurrentForceTotal = StandardVector3(0,0,0);

  sensativity = 0.01;
  planesensx = 0.1;
  planesensy = 0.1;
  plusplus = 0.1;

}

Airplane::~Airplane()
{

}

Airplane::Airplane(const Airplane &)
{

}

Airplane & Airplane::operator=(const Airplane &)
{
  return *this;
}


void Airplane::Initialize()
{

}

void Airplane::dump()
{
  // most of these need to be moved elsewhere
  //  cout << "ID: " << object_id
  //       << ", TYPE: " << object_type
  //       << ", ARMY: " << army
  //       << ", POSX: " << planeCurrentPosition.x
  //       << ", POSY: " << planeCurrentPosition.y
  //       << ", POSZ: " << planeCurrentPosition.z
  //       << ", DIRX: " << planeCurrentDirection.x
  //       << ", DIRY: " << planeCurrentDirection.y
  //       << ", DIRZ: " << planeCurrentDirection.z
  //       << ", VELX: " << planeCurrentVelocity.x
  //   << ", VELY: " << planeCurrentVelocity.y
  //       << ", VELZ: " << planeCurrentVelocity.z
  //       << endl;     
 
  cout << "Dumping Airplane" << endl;
  // TODO Dump Airplane
  Aircraft::dump();
}

void Airplane::OnPhysics(unsigned int dt)
{

}

void Airplane::OnController(unsigned int dt)
{

}

void Airplane::OnUpdate(unsigned int dt)
{
  doMovement(dt);
  BaseController * controller = ControllerManager::getControllerManager().getControllerFromID(controller_id);
  if (controller)
    controller->OnUpdate(dt);
}


void Airplane::ReceiveUpdate(const ObjectData &)
{

}

void Airplane::ReceiveMessage(const Message & )
{

}

void Airplane::SetNamedValue(const char * name, const char * value)
{
  cout << "Airplane::SetNamedParameter Name: " << name << ", Value: " << value << endl;
  // handle airplane specific values
  if (0)
    {
      //TODO
    }
  // call base class if not already handled
  else
    {
      Aircraft::SetNamedValue(name, value);
    }

}

void Airplane::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "Airplane::SetNamedParameter Name: " << name <<  endl;

  // handle airplane specific objects.
  if (0)
    {
      //TODO
    }
  // call base class if not already handled
  else
    {
      Aircraft::SetNamedObject(name, value);
    }

}

void Airplane::SetValue(const char * value)
{

}

void Airplane::SetTokenizedValue(int token, const char * value)
{
  cout << "Airplane::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // airplane specific values

      // call base class
    default:
      {
	Aircraft::SetTokenizedValue(token, value);
	break;
      }
    }
}

BaseObject * Airplane::CreateNamedObject(const char * name)
{
  cout << "Aircraft::CreateNamedParameter Name: " << name <<  endl;
  if (0)
    {
      //TODO
    }
  else
    return Aircraft::CreateNamedObject(name);
}



void Airplane::doMovement(unsigned int dt)
{
  float   step_size = 0.001*dt;

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
    planeCurrentPosition += step_size*planeSpeed*planeCurrentDirection;

//  math->MatrixTranslation(&MatrixTemp, 0,0, ((float)(100 - joystick.throttle) / (float)100) * plusplus);
//  math->MatrixInverse(&MatrixTemp);
//  math->MatrixMultiply(&matPlane, &MatrixTemp);

//  heading = (float)atan2(matPlane._fx, matPlane._fz);
//  heading = math->ToDegree(heading);
//  if(heading < 0) heading = 360 + heading;
//  headingx = (float)cos(heading);
//  headingz = (float)sin(heading);

}
