#ifndef MISSILECONTROLLER_H__
#define MISSILECONTROLLER_H__

#include "BaseController.h"
#include "ObjectRangeInfo.h"
#include "MissileObject.h"
#include <list>


class Message;


/**
 * class MissileController - Describe me!
 *
 * @author unknown
 */
class MissileController : public BaseController
{

 public:

 

 public: 
  MissileController();
  virtual ~MissileController();

  virtual void OnUpdate(unsigned int dt);
  virtual void OnMessage(const Message * message);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void Initialize();


 protected:

  
};

#endif
