#ifndef _BASECONTROLLER_H__
#define _BASECONTROLLER_H__


class BaseObject;
class Message;

class BaseController
{
 public:
  unsigned int object_id;      // the game object this controller is in charge of.
  unsigned int controller_id;
  unsigned int controller_type;

 public:
  BaseController();
  virtual ~BaseController();

  virtual void OnMessage(const Message *) = 0;
  virtual void OnUpdate(unsigned int dt) = 0;
  virtual void OnEnter() = 0;
  virtual void OnExit() = 0;
  virtual void Initialize() = 0;

};

#endif
