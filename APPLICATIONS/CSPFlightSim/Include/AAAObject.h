#ifndef __AAAOBJECT_H__
#define __AAAOBJECT_H__

using namespace std;

#include "BaseObject.h"
#include "AirplaneObject.h"
#include "AAAController.h"


/**
 * class AAAObject - Describe me!
 *
 * @author unknown
 */
class AAAObject : public BaseObject
{
 public:

  friend class AAAController;

  AAAObject();
  virtual ~AAAObject();

  virtual void dump();

  virtual void OnUpdate( float dt);
  virtual void initialize();
  virtual unsigned int OnRender();
  virtual int updateScene();


  virtual void setNamedParameter(const char * name, const char * value);

  void setPlayerPlane(AirplaneObject * pPlane);
  void checkForPlayerInRange();
  void fireWeapon();

 protected:
   AirplaneObject * p_PlayerPlane;
   int refire_time;
   bool weapon_ready;
   bool in_range;
};

#endif

