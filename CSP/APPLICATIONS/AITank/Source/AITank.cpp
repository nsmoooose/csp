#include <Framework.h>

#include <ConfigReader.h>
#include <ObjectManager.h>
#include <ControllerManager.h>
#include <BaseObject.h>
#include <BaseController.h>
#include <Tank.h>
#include <TankController.h>
#include <Projectile.h>
#include <ProjectileController.h>
#include <Airplane.h>
#include <ObjectRangeInfo.h>



#include "AITank.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>
#include <queue>

using namespace std;

const int gun_max_angle=20;
const int gun_min_angle=0;

// set the default values for the applications parameters.
float board_x_min = 0.0f;
float board_y_min = 0.0f;
float board_x_max = 10000.0f;
float board_y_max = 10000.0f;
unsigned int sim_step_time = 1;

unsigned int local_sim_time = 0;
unsigned int max_sim_time = 50;

bool game_over = false;

string object_data="";


// This is a simulation of a tank battle between
// two tanks from opposing armies. Each tank can
// move or fire a projectile. If the tank misses it
// will be know if the shot was long or short to help
// position the next shot.

// The board will be a 1000-1000 2d square.

//static bool                     doNamespaces    = false;
//static bool                     doSchema        = false;
//static const char*              encodingName    = "LATIN1";
//static XMLFormatter::UnRepFlags unRepFlags      = XMLFormatter::UnRep_CharRef;
//static char*                    xmlFile         = 0;
//static SAXParser::ValSchemes    valScheme       = SAXParser::Val_Auto;




void createGameObjects()
{
  cout << "Creating game objects" << endl;
  Tank * tank1 = new Tank;
  tank1->object_name = "TankOne";
  tank1->position = StandardVector3(100.0f, 100.0f, 0.0f);
  tank1->velocity = StandardVector3(0.50f, 0.0f, 0.0f);
  tank1->direction = StandardVector3(0.0f, 1.0f, 0.0f);
  tank1->army = 1;
  tank1->SetController(1);

  TankController * tankController1 = new TankController;
  tankController1->controller_id = 1;
  tankController1->object_id = tank1->object_id;
  tankController1->state = 0;
  tankController1->next_state = 0;
  tankController1->force_state_change = 0;

  ObjectManager::getObjectManager().addObject(tank1);

  ControllerManager::getControllerManager().addController(tankController1);

  Tank * tank2 = new Tank;
  tank2->position = StandardVector3(900.0f, 900.0f, 0.0f);
  tank2->velocity = StandardVector3(0.0f, 1.0f, 0.0f);
  tank2->direction = StandardVector3(-1.0f, 0.0f, 0.0f);
  tank2->object_name = "TankTwo";
  tank2->army = 2;

  tank2->SetController(2);
  TankController * tankController2 = new TankController;
  tankController2->controller_id =2;
  tankController2->object_id = tank2->object_id;
  tankController2->state = 0;
  tankController2->next_state = 0;
  tankController2->force_state_change = 0;

  ObjectManager::getObjectManager().addObject(tank2);
  ControllerManager::getControllerManager().addController(tankController2);

  Projectile * projectile = new Projectile;
  projectile->position = StandardVector3(905.0f, 905.0f, 5.0f);
  float projectileAngle = 1.0/360.0*2.0*3.1415;
  projectile->velocity = StandardVector3(750.0*cos(projectileAngle), 0.0, 
					 750.0*sin(projectileAngle));

  projectile->SetController(3);

  ProjectileController * projectileController = new ProjectileController;
  projectileController->object_id = projectile->object_id;
  projectileController->controller_id = 3;

  ObjectManager::getObjectManager().addObject(projectile);
  ControllerManager::getControllerManager().addController(projectileController);

  Airplane * airplaneObject = new Airplane;
  ObjectManager::getObjectManager().addObject(airplaneObject);
  

}

// this calls the OnEnter method for each objects controller.
void initializeGameObjects()
{
  cout << "Initializing Game Objects" << endl;
  ObjectManager::getObjectManager().initializeAllObjects();

}




// process the OnUpdate of all game objects.
void processOnUpdate(unsigned int dt)
{
  ObjectManager::getObjectManager().updateAllObjects(dt);
}



int main(int argc, char * argv[])
{
  cout << "Starting Tank Demo" << endl;

  readXMLConfigurationFile(argv[1]);

  ObjectManager::getObjectManager().dumpAllObjects();

  cout << "Max Sim Time: " << max_sim_time << endl;
  cout << "Sim Step Time: " << sim_step_time << endl;
  cout << "Board X Min: " << board_x_min << endl;
  cout << "Board Y Min: " << board_y_min << endl;
  cout << "Board X Max: " << board_x_max << endl;
  cout << "Board Y Max: " << board_y_max << endl;
  cout << "ObjectData: " << object_data << endl;

  //  readObjectData(object_data.data());

  //  createGameObjects();
  initializeGameObjects();

  ObjectManager::getObjectManager().dumpAllObjects();

  while(!game_over)
    {
      // increment the simulation time.
      local_sim_time += sim_step_time;

      // process the delayed message list.
      ObjectManager::getObjectManager().processDelayedMsgList();

      // process the OnUpdate list.
      processOnUpdate(sim_step_time);

      // remove marked objects, this should be done at the end of the main loop.
      ObjectManager::getObjectManager().removeObjectsMarkedForDelete();

      if (local_sim_time > max_sim_time)
	game_over = true;

      cout << "Simulation Time " << local_sim_time << "ms" << endl;
      ObjectManager::getObjectManager().dumpAllObjects();

	    
    }

  cout << "Game Over" << endl;

  ObjectManager::getObjectManager().dumpAllObjects();
  ObjectManager::getObjectManager().dumpObjectHistory();
	
}


