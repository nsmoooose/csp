#include <ControllerManager.h>
#include <BaseController.h>


ControllerManager * ControllerManager::ms_manager = 0;

void ControllerManager::addController(BaseController * controller)
{
  controllerList.push_front(controller);
}


// find the controller for this ID. return a null if the
// object is not found.
BaseController *  ControllerManager::getControllerFromID(unsigned int ID)
{
  list<BaseController *>::iterator i;
  for (i = controllerList.begin() ; i != controllerList.end() ; ++i)
    {
      BaseController * controller = *i;
      if (ID == controller->controller_id)
	return controller;
    }
  return NULL;
}
