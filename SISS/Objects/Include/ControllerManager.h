#ifndef _CONTROLLERMANAGER_H__
#define _CONTROLLERMANAGER_H__

#include <BaseController.h>
#include <list>
#include <queue>

class ControllerManager
{
 protected:
  list<BaseController *> controllerList;
  static ControllerManager * ms_manager;

  ControllerManager() { ms_manager = this; }
  
 public:
  ~ControllerManager() { ms_manager = 0; }

  static ControllerManager& getControllerManager() 
    { if (ms_manager == NULL) 
         ms_manager = new ControllerManager; 
      return *ms_manager; 
    }

  void addController(BaseController *);

  BaseController * getControllerFromID(unsigned int ID);

};

#endif
