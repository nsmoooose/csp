#ifndef __TANKOBJECT_H__
#define __TANKOBJECT_H__

#include "BaseObject.h"

// movement states for the tank object
// 0 - stopped
// 1 - forward
// 2 - backward
// 3 - turning right
// 4 - turning left


/**
 * class TankObject
 *
 * @author unknown
 */
class TankObject : public BaseObject
{
 public:

  TankObject();
  virtual ~TankObject();
  virtual void dump();
  virtual void OnUpdate(double dt);
  virtual unsigned int OnRender();

  virtual int updateScene();


  virtual void setNamedParameter(const char * name, const char * value);
  virtual void initialize();
  double getMaxViewingRange() { return max_viewing_range; }
  double getMaxViewingAngle() { return max_viewing_angle; }
  double getMaxFiringRange() { return max_firing_range; }

  void setMovementState(unsigned int state) { movement_state = state; }

 protected:
  void doMovement(double dt);

  double gun_angle;

  double max_viewing_range;
  double max_viewing_angle;
  double max_firing_range;
 
  double forward_speed;
  double backward_speed;
  double turn_speed;

  unsigned int movement_state;

};

#endif
