#include "stdinc.h"

#include "BaseObject.h"
#include "VirtualBattlefield.h"
#include <osg/Quat>
#include <osg/Geode>
#include "LogStream.h"
#include "TankObject.h"
#include "AirplaneObject.h"
#include "MissileObject.h"
#include "ProjectileObject.h"
#include "AAAObject.h"
#include "instantActionManager.h"
#include "StaticObject.h"
#include "DirVectorDrawable.h"
#include <osg/NodeVisitor>
#include <osg/BoundingSphere>

extern VirtualBattlefield * g_pBattlefield;
extern double g_LatticeXDist;
extern double g_LatticeYDist;

using namespace std;

BaseObject::BaseObject()
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::BaseObject()" );
    m_rpTransform = NULL;
    m_rpNode = NULL;
    m_bFreezeFlag = false;
	m_bOnGround = false;

	m_XLatticePos = 0;
	m_YLatticePos = 0;

	m_Mass = 1;
	m_heading = 0;
	m_roll = 0;
	m_pitch = 0;

}

BaseObject::~BaseObject()
{
  CSP_LOG(CSP_APP, CSP_INFO, "BaseObject::~BaseObject()" );

  m_rpTransform->removeChild( m_rpNode.get() );


}

void BaseObject::setNamedParameter(const char * name, const char * value)
{

}

unsigned int BaseObject::getClassTypeFromName(const string & name)
{
    if (name == "TANK")
    {
        return TANK_OBJECT_TYPE;
    }
    else if (name == "AIRPLANE")
    {
        return AIRPLANE_OBJECT_TYPE;
    }
    else if (name == "PROJECTILE")
    {
        return PROJECTILE_OBJECT_TYPE;
    }
    else if (name == "MISSILE")
    {
        return MISSILE_OBJECT_TYPE;
    }
    else if (name == "AAA")
    {
        return AAA_OBJECT_TYPE;
    }
    else if (name == "INSTANTACTIONMAN")
    {
        return INSTACTMAN_OBJECT_TYPE;
    }
    else if (name == "STATIC")
    {
        return STATIC_OBJECT_TYPE;
    }
    else 
    {
        return UNKNOWN_TYPE;
    }
}

BaseObject * BaseObject::createObject(unsigned int type)
{
    switch (type)
    {
    case TANK_OBJECT_TYPE:
        return new TankObject;
        break;
    case AIRPLANE_OBJECT_TYPE:
        return new AirplaneObject;
        break;
    case PROJECTILE_OBJECT_TYPE:
        return new ProjectileObject;
        break;
    case MISSILE_OBJECT_TYPE:
        return new MissileObject;
        break;
    case STATIC_OBJECT_TYPE:
        return new StaticObject;
        break;
//    case AAA_OBJECT_TYPE:
//        return new AAAObject;
//        break;
//    case INSTACTMAN_OBJECT_TYPE:
//        return new InstantActionManager;
//        break;
    default:
        return NULL;
    }
}




void BaseObject::setLocalPosition(StandardVector3 const & position)
{
	setLocalPosition(position.x, position.y, position.z);
}

void BaseObject::setLocalPosition(double x, double y, double z)
{
	// if the object is on the ground ignore the z component and use the elevation at
	// the x,y point.

	m_LocalPosition.x = x;
	m_LocalPosition.y = y;

	m_GlobalPosition.x = g_LatticeXDist*m_XLatticePos + x;
	m_GlobalPosition.y = g_LatticeYDist*m_YLatticePos + y;

	if(m_bOnGround)
	{
		m_GlobalPosition.z = g_pBattlefield->getElevation(x,y); 
	    m_LocalPosition.z = m_GlobalPosition.z;
	}
	else
	{
		m_GlobalPosition.z = z; 
	    m_LocalPosition.z = z;
	}

    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::setPosition - ID: " << m_iObjectID 
       << ", Name: " << m_sObjectName << ", LocalPosition: " << m_LocalPosition );

}


void BaseObject::setGlobalPosition(StandardVector3 const & position)
{
	setGlobalPosition(position.x, position.y, position.z);

}

void BaseObject::setGlobalPosition(double x, double y, double z)
{
	// if the object is on the ground ignore the z component and use the elevation at
	// the x,y point.
	m_GlobalPosition.x = x;
	m_GlobalPosition.y = y;

	m_XLatticePos = x / g_LatticeXDist;
	m_YLatticePos = y / g_LatticeYDist;

	m_LocalPosition.x = m_GlobalPosition.x - g_LatticeXDist*m_XLatticePos;
	m_LocalPosition.y = m_GlobalPosition.y - g_LatticeYDist*m_YLatticePos;

	if(m_bOnGround)
	{
		float offset;
		if (m_rpNode.valid())
		{
			osg::BoundingSphere sphere = m_rpNode->getBound();
			offset = sphere.radius() - sphere.center().z();
		}
		else 
			offset = 0.0;
		m_GlobalPosition.z = g_pBattlefield->getElevation(x,y) + offset; 
	    m_LocalPosition.z = m_GlobalPosition.z;
	}
	else
	{
		m_GlobalPosition.z = z; 
	    m_LocalPosition.z = z;
	}

    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::setPosition - ID: " << m_iObjectID 
       << ", Name: " << m_sObjectName << ", Position: " << m_GlobalPosition );
}

void BaseObject::setLatticePosition(int x, int y)
{
	m_XLatticePos = x;
	m_YLatticePos = y;
}

void BaseObject::getLatticePosition(int & x, int & y)
{
	x = m_XLatticePos;
	y = m_YLatticePos;
}


void BaseObject::setOrientation(StandardMatrix3 & mOrientation)
{
    m_Orientation = mOrientation;
    m_Direction = m_Orientation*m_InitialDirection;
    m_CurrentNormDir = m_Orientation*m_InitialNormDir;
}

void BaseObject::setOrientation(StandardQuaternion & qOrientation)
{
    qOrientation.ToRotationMatrix(m_Orientation);
    m_Direction = m_Orientation*m_InitialDirection;
    m_CurrentNormDir = m_Orientation*m_InitialNormDir;

}

void BaseObject::setOrientation(double heading, double pitch, double roll)
{
    m_Orientation.FromEulerAnglesZXY (DegreesToRadians(heading),
                                      DegreesToRadians(-pitch), 
                                      DegreesToRadians(roll) );
    m_Direction = m_Orientation*m_InitialDirection;
    m_CurrentNormDir = m_Orientation*m_InitialNormDir;

}


void BaseObject::setVelocity(StandardVector3 & velocity)
{
    m_LinearVelocity = velocity;
    m_Speed = m_LinearVelocity.Length();

    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::setVelocity - ID: " << m_iObjectID 
       << ", Name: " << m_sObjectName << ", Velocity: " << m_LinearVelocity );

}

void BaseObject::setVelocity(double Vx, double Vy, double Vz)
{
    m_LinearVelocity = StandardVector3(Vx, Vy, Vz);
    m_Speed = m_LinearVelocity.Length();

    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::setVelocity - ID: " << m_iObjectID 
      <<  ", Name: " << m_sObjectName << ", Velocity: " << m_LinearVelocity );

}

void BaseObject::setNode( osg::Node * pNode )
{ 
    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::setNode() - ID: " << m_iObjectID);

    if (!m_rpTransform)
    {
        m_rpTransform = new osg::Transform;
    }

    m_rpNode = pNode;
    //m_rpOffsetTransform->addChild(m_rpNode.get() );
//	DirVectorDrawable * pDrawable = new DirVectorDrawable;
//	osg::Geode * pGeode = new osg::Geode;
//	pGeode->addDrawable(pDrawable);

	m_rpTransform->addChild ( m_rpNode.get() );


}

osg::Node* BaseObject::getNode()
{ 
    return m_rpTransform.get(); 
}


void BaseObject::addToScene()
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::addToScene() - ID: " << m_iObjectID);


    if(!m_rpTransform)
    {
        m_rpTransform = new osg::Transform;
		m_rpTransform->addChild( m_rpNode.get() );
    }


    StandardMatrix4 stdmat = TranslationMatrix4( m_LocalPosition ) * StandardMatrix4( m_Orientation );

	stdmat = stdmat.Transpose(); // pass it to opengl

    osg::Matrix worldMat;
    worldMat.set( stdmat[0][0], stdmat[0][1], stdmat[0][2], stdmat[0][3],
                  stdmat[1][0], stdmat[1][1], stdmat[1][2], stdmat[1][3],
                  stdmat[2][0], stdmat[2][1], stdmat[2][2], stdmat[2][3],
                  stdmat[3][0], stdmat[3][1], stdmat[3][2], stdmat[3][3] );


	//m_rpTransform->setReferenceFrame(osg::Transform::RELATIVE_TO_PARENTS);

    m_rpTransform->setMatrix(worldMat);

	m_rpSwitch = new osg::Switch;
	m_rpSwitch->setValue(osg::Switch::ALL_CHILDREN_ON);
	m_rpSwitch->addChild(m_rpTransform.get());

    g_pBattlefield->addNodeToScene(m_rpSwitch.get());
	m_rpNode->setName( getObjectName() );

	osg::BoundingSphere sphere = m_rpNode->getBound();

	CSP_LOG(CSP_APP, CSP_DEBUG, "NodeName: " << m_rpNode->getName() <<
		", BoundingPos: " << sphere.center() << ", BoundingRadius: " << 
		sphere.radius() );

}

int BaseObject::updateScene()
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::updateScene() ID:"  << m_iObjectID );

    if (!m_rpTransform)
    {
        CSP_LOG(CSP_APP, CSP_WARN, "BaseObject::updateScene() warning object has null OSG transform" );
        return 0;
    }
     
	// add testing model.osg code here
    //stdmat *= RotationZMatrix4( PI );
	//stdmat *= StandardMatrix4(0.5 * StandardMatrix3::IDENTITY);
	//stdmat *= TranslationMatrix4(0,0,-100);

	StandardMatrix4 stdmat = TranslationMatrix4( m_LocalPosition ) * StandardMatrix4( m_Orientation );

	stdmat = stdmat.Transpose(); // pass it to opengl
	
    osg::Matrix worldMat;
    worldMat.set( stdmat[0][0], stdmat[0][1], stdmat[0][2], stdmat[0][3],
                  stdmat[1][0], stdmat[1][1], stdmat[1][2], stdmat[1][3],
                  stdmat[2][0], stdmat[2][1], stdmat[2][2], stdmat[2][3],
                  stdmat[3][0], stdmat[3][1], stdmat[3][2], stdmat[3][3] );

    m_rpTransform->setMatrix(worldMat);


//	osg::BoundingSphere sphere = m_rpNode->getBound();
//	osg::Vec3 cen = sphere.center();
//	float radius = sphere.radius();
	CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::updateScene() - Position: " <<
		m_LocalPosition );
//	CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::updateScene() - Bounding Sphere " 
//		<< cen.x() << ", " << cen.y() << ", " << cen.z() << ", " << radius );

	return 0;

}

void BaseObject::setCullingActive(bool flag)
{
    if (m_rpTransform.valid())
        m_rpTransform->setCullingActive(flag);
}

void BaseObject::updateGroundPosition()
{
	float offset;
	if (m_rpNode.valid())
	{
		osg::BoundingSphere sphere = m_rpNode->getBound();
		offset = sphere.radius() - sphere.center().z();
	}
	else 
		offset = 0.0;
	m_GlobalPosition.z = g_pBattlefield->getElevation(m_GlobalPosition.x,
		m_GlobalPosition.y) + offset; 
	m_LocalPosition.z = m_GlobalPosition.z;

}

// Update Local Position to account for lattice crossings.
void BaseObject::updateLocalPosition()
{
	if (m_LocalPosition.x >= g_LatticeXDist)
	{
		m_LocalPosition.x -= g_LatticeXDist;
		m_XLatticePos++;
	}
	else if (m_LocalPosition.x < 0)
	{
		m_LocalPosition.x += g_LatticeXDist;
		m_XLatticePos--;
	}

	if (m_LocalPosition.y >= g_LatticeYDist)
	{
		m_LocalPosition.y -= g_LatticeYDist;
		m_YLatticePos++;
	}
	else if (m_LocalPosition.y < 0)
	{
		m_LocalPosition.y += g_LatticeYDist;
		m_YLatticePos--;
	}

}

// update global position based on local position.
void BaseObject::updateGlobalPosition()
{
	m_GlobalPosition.x = m_LocalPosition.x + m_XLatticePos*g_LatticeXDist;
	m_GlobalPosition.y = m_LocalPosition.y + m_YLatticePos*g_LatticeYDist;
	m_GlobalPosition.z = m_LocalPosition.z;

}