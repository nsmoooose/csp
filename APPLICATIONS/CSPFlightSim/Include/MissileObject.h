#ifndef __MISSILEOBJECT_H__
#define __MISSILEOBJECT_H__

#include "BaseObject.h"

class MissileObject : public BaseObject
{
 public:

  MissileObject();
  virtual ~MissileObject();
  virtual void dump();
  virtual void OnUpdate(double dt);
  virtual void initialize();
  virtual unsigned int OnRender();
  void doSimplePhysics(double dt);

  virtual int updateScene();


//  StandardVector3 m_Position;
//  StandardVector3 m_Velocity;

  void doMovement(double dt);

//  StandardMatrix3  m_Orientation;
//  StandardVector3  m_InitialDirection;
//  StandardVector3  m_CurrentDirection;
//  StandardVector3  m_CurrentPosition;
//  StandardVector3  m_InitialNormDir;
//  StandardVector3  m_CurrentNormDir;
//  StandardVector3  m_CurrentVelocity;
//  StandardVector3  m_CurrentVelocityDir;

 protected:


  double m_Speed;
  double m_Thrust;
  double m_ThrustMin;
  double m_ThrustMax;
  double m_Aileron;
  double m_Elevator;
  double m_AileronMin;
  double m_AileronMax;
  double m_ElevatorMin;
  double m_ElevatorMax;
  StandardVector3 m_CurrentForceTotal;
  StandardVector3 m_GravityForce;
  StandardVector3 m_ThrustForce;
  StandardVector3 m_LiftForce;
  StandardVector3 m_DragForce;
  
  double plusplus;
  double sensativity;
  double planesensx, planesensy;

  double m_ElevatorKey;
  double m_AileronKey;

  double m_Active;
  double m_MaxFlightTime;
  double m_FlightTime;

  int particleCount;
  int particleMax;
  int particlePos;
  StandardVector3 * m_ParticlePosition;

};

#endif
