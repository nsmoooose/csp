#ifndef __TANK_H__
#define __TANK_H__

#include <Framework.h>
#include <BaseObject.h>
#include <GroundVehicle.h>
#include <ObjectData.h>
#include <Message.h>

// movement states for the tank object
// 0 - stopped
// 1 - forward
// 2 - backward
// 3 - turning right
// 4 - turning left


class Tank : public GroundVehicle
{
 public:

  Tank();
  virtual ~Tank();

  Tank(const Tank &);
  Tank & operator=(const Tank &);

  virtual void dump();
  virtual void Initialize();

  // BaseEntity specific methods
  virtual void OnPhysics(unsigned int dt);
  virtual void OnController(unsigned int dt);

  // Object communication methods
  virtual void OnUpdate(unsigned int dt);
  virtual void ReceiveUpdate(const ObjectData &);
  virtual void ReceiveMessage(const Message & );

  // XML handler methods
  virtual void SetNamedValue(const char * name, const char * value);
  virtual void SetNamedObject(const char * name, const BaseObject * value);
  virtual void SetValue(const char * value);
  virtual BaseObject * CreateNamedObject(const char * str);
  virtual void SetTokenizedValue(int token, const char * value);

  StandardVector3 position;
  StandardVector3 velocity;
  StandardVector3 direction;
  int army;
  float gun_angle;

  float max_viewing_range;
  float max_viewing_angle;
  float max_firing_range;
 
  float forward_speed;
  float backward_speed;
  float turn_speed;

  unsigned int movement_state;

  unsigned int object_type;
  unsigned int controller_id;


 protected:
  void doMovement(unsigned int dt);


};

#endif
