#ifndef AIRPLANECONTROLLER_H__
#define AIRPLANECONTROLLER_H__

#include "BaseController.h"
#include "ObjectRangeInfo.h"
#include "AirplaneObject.h"


class Message;


/**
 * class AirplaneController - Describe me!
 *
 * @author unknown
 */
class AirplaneController : public BaseController
{

 public:

 

 public: 
  AirplaneController();
  virtual ~AirplaneController();

  virtual void OnUpdate(unsigned int dt);
  virtual void OnMessage(const Message * message);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void Initialize();


 protected:

  
};

#endif
