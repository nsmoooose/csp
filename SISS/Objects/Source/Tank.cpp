#include <BaseObject.h>
#include <Tank.h>
#include <ObjectManager.h>
#include <ControllerManager.h>
#include <TankController.h>
#include <BaseController.h>
#include <VectorHolder.h>
#include <iostream.h>

Tank::Tank() : GroundVehicle()
{

  cout << "Tank::Tank() - Creating tank object" << endl;

  object_type = 1;
  controller_id = 0;
  object_name = "UNNAMEDTANK";
  deleteFlag = false;

  movement_state = 0;

  position = StandardVector3(0.0f, 0.0f, 0.0f);
  velocity = StandardVector3(0.0f, 0.0f, 0.0f);
  direction = StandardVector3(0.0f, 0.0f, 0.0f);
  gun_angle = 45.0f;
  max_viewing_range = 2000.0;
  max_viewing_angle = 60.0;
  max_firing_range = 1000.0;
  forward_speed = 5.0;
  backward_speed = 2.0;
  turn_speed = 5.0;
  army = 0;

}

Tank::~Tank()
{

}

void Tank::Initialize()
{
  TankController * controller = (TankController*)ControllerManager::getControllerManager().getControllerFromID(controller_id);
  if (controller)
    controller->Initialize();
}

void Tank::SetNamedValue(const char * name, const char * value)
{
  cout << "Tank::SetNamedParameter Name: " << name << ", Value: " << value << endl;
  // handle tank specific values
  if (0)
    {
      //TODO
    }
  // call base class if not already handled
  else
    {
      GroundVehicle::SetNamedValue(name, value);
    }

}

void Tank::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "Tank::SetNamedParameter Name: " << name <<  endl;

  // handle tank specific objects.
  if (0)
    {
      //TODO
    }
  // call base class if not already handled
  else
    {
      GroundVehicle::SetNamedObject(name, value);
    }
}

BaseObject * Tank::CreateNamedObject(const char * name)
{
  cout << "Tank::CreateNamedParameter Name: " << name <<  endl;
  if (0)
    {
      //TODO
    }
  else
    return GroundVehicle::CreateNamedObject(name);
}

void Tank::SetTokenizedValue(int token, const char * value)
{
  cout << "Tank::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // tank specific values

      // call base class
    default:
      {
	GroundVehicle::SetTokenizedValue(token, value);
	break;
      }
    }
}

void Tank::SetValue(const char * value)
{

}


void Tank::dump()
{
  //  cout << "ID: " << object_id
  //           << ", TYPE: " << object_type
  //           << ", ARMY: " << army
  //           << ", POSX: " << position.x
  //           << ", POSY: " << position.y
  //           << ", POSZ: " << position.z
  //           << ", DIRX: " << direction.x
  //           << ", DIRY: " << direction.y
  //           << ", DIRZ: " << direction.z
  //           << ", VELX: " << velocity.x
  //	   << ", VELY: " << velocity.y
  //           << ", VELZ: " << velocity.z
  //           << ", GUNANGLE: " << gun_angle
  //           << endl;      

  cout << "Dumping Tank" << endl;
  // TODO Dump tank
  GroundVehicle::dump();

}

void Tank::OnUpdate(unsigned int dt)
{
  doMovement(dt);
  BaseController * controller = ControllerManager::getControllerManager().getControllerFromID(controller_id);
  controller->OnUpdate(dt);
}

void Tank::OnPhysics(unsigned int dt)
{

}

void Tank::OnController(unsigned int dt)
{

}


void Tank::doMovement(unsigned int dt)
{
  float step_size = 0.001*(float)dt;

  switch (movement_state)
    {
    case 0:
      // do nothing this is the stop state.
      break;
    case 1:
      // forward state
      {
	//	cout << "Tank " << tankObject->object_id << " moving forward." << endl;
	position = position + step_size*forward_speed*direction;
      }
      break;
    case 2:
      // backward state
      {
	//	    cout << "Tank " << tankObject->object_id << " moving backward." << endl;
	position = position - step_size*backward_speed*direction;
      }
      break;
    case 3:
      // right turn state
      {
	//	    cout << "Tank " << tankObject->object_id << " turning right." << endl;
	    
	float cur_x = direction.x; float cur_y = direction.y;
	direction.x = direction.x + step_size*turn_speed*cur_y;
	direction.y = direction.y - step_size*turn_speed*cur_x;
      }
      break;
    case 4:
      // left turn state
      {
	//	    cout << "Tank " << tankObject->object_id << " turning left." << endl;

	float cur_x = direction.x; float cur_y = direction.y;
	direction.x = direction.x - step_size*turn_speed*cur_y;
	direction.y = direction.y + step_size*turn_speed*cur_x;	
      }
      break;
      
    }

}

void Tank::ReceiveUpdate(const ObjectData &)
{

}
 
void Tank::ReceiveMessage(const Message & )
{

}
