// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file BaseObject.h
 *
 **/

#ifndef __BASEOBJECT_H__
#define __BASEOBJECT_H__

#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Switch>

#include "BaseController.h"
#include "TypesMath.h"

#define UNKNOWN_TYPE 0
#define TANK_OBJECT_TYPE 1
#define AIRPLANE_OBJECT_TYPE 2
#define PROJECTILE_OBJECT_TYPE 10
#define MISSILE_OBJECT_TYPE 11
#define AAA_OBJECT_TYPE 20
#define STATIC_OBJECT_TYPE 30
#define INSTACTMAN_OBJECT_TYPE 100



/**
 * class BaseObject - Describe me!
 *
 * @author unknown
 */
class BaseObject
{
 public:

  BaseObject();
  virtual ~BaseObject();

  virtual void dump() = 0;

  virtual void OnUpdate( double dt) = 0;

  virtual void initialize() = 0;
  virtual unsigned int OnRender() = 0;

  virtual void setNamedParameter(const char * name, const char * value);
  static unsigned int getClassTypeFromName(const std::string & name);
  static BaseObject * createObject(unsigned int type);


  StandardMatrix3 const & getOrientation() const { return m_Orientation; }
  StandardQuaternion const & getqOrientation() const { return m_qOrientation; };
  void setOrientation(StandardMatrix3 & mOrientation);
  void setOrientation(StandardQuaternion const & qOrientation);
  void setOrientation(double heading, double pitch, double roll);
  void setVelocity(StandardVector3 const & velocity);
  void setVelocity(double Vx, double Vy, double Vz);
  void addToScene();
  virtual int updateScene();
  void setController(BaseController * pController) 
    {   m_pController = pController;
        m_iControllerID = pController->controller_id;
    }
  unsigned int getObjectID() { return m_iObjectID; }
  unsigned int getObjectType() { return m_iObjectType; }
  unsigned int getControllerID() { return m_iControllerID; }
  void setFreezeState( bool flag ) { m_bFreezeFlag = flag; }
  bool getFreezeState() { return m_bFreezeFlag; }

  StandardVector3 const & getLocalPosition() const { return m_LocalPosition; }
  StandardVector3 const & getGlobalPosition() const { return m_GlobalPosition; }
  void setLocalPosition(StandardVector3 const & position);
  void setLocalPosition(double x, double y, double z);
  void setGlobalPosition(StandardVector3 const & position);
  void setGlobalPosition(double x, double y, double z);
  void setLatticePosition(int x, int y);
  void getLatticePosition(int & x, int & y);

  void updateLocalPosition();
  void updateGlobalPosition();
  void updateGroundPosition();

  double getPitch() { return m_pitch; }
  double getHeading() { return m_heading; }
  double getRoll() { return m_roll; } 

  double getSpeed() const { return m_Speed; }
  StandardVector3 const & getVelocity() const { return m_LinearVelocity; }

  void setCullingActive(bool flag);

  void setDeleteFlag( bool flag ) { m_bDeleteFlag = flag; }
  bool getDeleteFlag() { return m_bDeleteFlag; }

  void setArmy(int army) { m_iArmy = army; }
  int getArmy() { return m_iArmy; }

  double getBoundingRadius() { return m_fBoundingSphereRadius; }

  StandardVector3 & getDirection() { return m_Direction; }

  StandardVector3 & getUpDirection() { return m_CurrentNormDir; }

  void setNode( osg::Node * pNode );
  osg::Node * getNode();

  // this needs to see if this name is already in use.
  void setObjectName(std::string name) { m_sObjectName = name; }
  std::string getObjectName() { return m_sObjectName; }

  void ShowRepresentant(unsigned short const p_usflag)
  {
	//m_rpSwitch->setAllChildrenOff();
    m_rpSwitch->setValue(1-p_usflag, false);
    m_rpSwitch->setValue(p_usflag, true);
  }

  virtual double getGForce() {return 1.0;};

 protected:

  virtual void doMovement(double dt) = 0;
  void AddSmoke();

  StandardQuaternion m_qOrientation;
  StandardMatrix3 m_Orientation;
  StandardMatrix3 m_OrientationInverse;
  StandardVector3 m_Direction;
  StandardVector3 m_InitialDirection;

  StandardMatrix3 m_Ibody;
  StandardMatrix3 m_IbodyInv;
  StandardMatrix3 m_Iinv;

  StandardVector3 m_GlobalPosition;
  int m_XLatticePos;
  int m_YLatticePos;

  StandardVector3 m_LocalPosition;

  StandardVector3 m_PrevPosition;
  StandardVector3 m_LinearVelocity;
  StandardVector3 m_LinearVelocityDirection;
  StandardVector3 m_InitialNormDir;
  StandardVector3 m_CurrentNormDir;

  StandardVector3 m_Force;
  StandardVector3 m_Torque;
  StandardVector3 m_Omega;

  double m_Mass;

  double m_Speed;

  float m_fBoundingSphereRadius;

  double m_roll, m_pitch, m_heading;

  BaseController * m_pController;
  unsigned int m_iControllerID;

  int m_iArmy;

  osg::ref_ptr<osg::Node>      m_rpNode;
  osg::ref_ptr<osg::MatrixTransform> m_rpTransform;
  osg::ref_ptr<osg::Switch> m_rpSwitch;

  unsigned int m_iObjectID;
  unsigned int m_iObjectType;
  std::string m_sObjectName;
  bool m_bDeleteFlag;
  bool m_bFreezeFlag;
  bool m_bOnGround;
};

#endif // __BASEOBJECT_H__


