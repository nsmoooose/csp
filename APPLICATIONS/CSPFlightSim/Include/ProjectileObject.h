#ifndef __PROJECTILEOBJECT_H__
#define __PROJECTILEOBJECT_H__

#include "BaseObject.h"

class ProjectileObject : public BaseObject
{
 public:
  ProjectileObject();
  virtual ~ProjectileObject();
  virtual void dump();
  virtual void OnUpdate(double dt);
  virtual void initialize();
  virtual unsigned int OnRender();
  void doMovement(double dt);


//  StandardVector3 position;
//  StandardVector3 velocity;

 protected:
  void DoMovement(double dt);
  

};

#endif
