#ifndef __AIRPLANE_H__
#define __AIRPLANE_H__

#include <Framework.h>
#include <BaseObject.h>
#include <Aircraft.h>
#include <Message.h>
#include <ObjectData.h>

class Airplane : public Aircraft
{
 public:

  Airplane();
  virtual ~Airplane();

  Airplane(const Airplane &);
  Airplane & operator=(const Airplane &);

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
  virtual void SetTokenizedValue(int token, const char * value);

  virtual BaseObject * CreateNamedObject(const char * str);



  StandardVector3 position;
  StandardVector3 velocity;
  int army;

 protected:
  void doMovement(unsigned int dt);


  StandardMatrix3 planeOrientation;
  StandardVector3  planeInitialDirection;
  StandardVector3  planeCurrentDirection;
  StandardVector3  planeCurrentPosition;
  StandardVector3  planeInitialNormDir;
  StandardVector3  planeCurrentNormDir;
  StandardVector3  planeCurrentVelocity;
  StandardVector3  planeCurrentVelocityDir;

  float planeSpeed;
  float thrust;
  float thrustMin;
  float thrustMax;
  float aileron;
  float elevator;
  float aileronMin;
  float aileronMax;
  float elevatorMin;
  float elevatorMax;
  StandardVector3 planeCurrentForceTotal;
  StandardVector3 planeGravityForce;
  StandardVector3 planeThrustForce;
  StandardVector3 planeLiftForce;
  StandardVector3 planeDragForce;
  
  float plusplus;
  float sensativity;
  float planesensx, planesensy;

  float elevatorKey;
  float aileronKey;



};

#endif
