#ifndef _AAACONTROLLER_H__
#define _AAACONTROLLER_H__

#include "BaseController.h"

class BaseObject;
class Message;


/**
 * class AAAController - Describe me!
 *
 * @author unknown
 */
class AAAController : public BaseController
{
 public:

 public:
  AAAController();
  virtual ~AAAController();

  virtual void OnMessage(const Message *);
  virtual void OnUpdate(unsigned int dt);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void Initialize();

};

#endif
