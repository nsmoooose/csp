#ifndef AIAIRPLANECONTROLLER_H__
#define AIAIRPLANECONTROLLER_H__

#include "BaseController.h"
#include "ObjectRangeInfo.h"
#include "AirplaneObject.h"

// alitude states
// 0 level flight
// 1 climb
// 2 dive

class Message;

class AIAirplaneController : public BaseController
{

 public:

     enum {LEVEL, CLIMB, DIVE};

 public: 
  AIAirplaneController();
  virtual ~AIAirplaneController();

  virtual void OnUpdate(unsigned int dt);
  virtual void OnMessage(const Message * message);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void Initialize();

  virtual void  SetAltitude(float alt)
  { targetAltitude = alt; }

 protected:

     float targetAltitude;
     float altitudeTol;

    int AltitudeState;
    // pitch angles in degrees
    float divePitch;      
    float climbPitch;
    float pitchTol;
    float targetPitch;


};

#endif
