#ifndef __MISSILEOBJECT_H__
#define __MISSILEOBJECT_H__

#include "BaseObject.h"

/**
 * class MissileObject
 *
 * @author unknown
 */
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

  void doMovement(double dt);

 protected:


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
