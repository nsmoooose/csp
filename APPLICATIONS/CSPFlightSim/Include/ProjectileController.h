#ifndef PROJECTILECONTROLLER_H__
#define PROJECTILECONTROLLER_H__

#include "BaseController.h"
#include "ProjectileObject.h"

class ProjectileController : public BaseController
{
 public:
  
 

 public:
  ProjectileController();
  virtual ~ProjectileController();

  virtual void OnUpdate(unsigned int dt);
  virtual void OnMessage(const Message * message);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void Initialize();
  


};

#endif
