#ifndef __STATICOBJECT_H__
#define __STATICOBJECT_H__


#include "BaseObject.h"



/**
 * class StaticObject
 *
 * @author unknown
 */
class StaticObject : public BaseObject
{
 public:
  StaticObject();
  virtual ~StaticObject();
  virtual void dump();
  virtual void OnUpdate(double dt);
  virtual void initialize();
  virtual unsigned int OnRender();
  virtual int updateScene();


 protected:

  virtual void doMovement(double dt);

  

};











#endif
