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
#include "InstantActionManager.h"
#include "StaticObject.h"
#include "DirVectorDrawable.h"

#include <osg/BoundingSphere>
#include <osg/NodeVisitor>
#include<osg/ShadeModel>

#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/Particle>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/PointPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>

#include <osgUtil/SmoothingVisitor>

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
	m_sObjectName = "";

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

  m_rpTransform->removeChild( m_rpSwitch.get() ); 


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


void BaseObject::setVelocity(StandardVector3 const & velocity)
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

osgParticle::ParticleSystem *setupParticleSystem(osg::Group *base, const osg::Vec4 &color, const osg::Vec3 &center, float lifetime)
{
	osgParticle::Particle ptemp;
	ptemp.setShape(osgParticle::Particle::HEXAGON);
	ptemp.setLifeTime(lifetime);
	ptemp.setSizeRange(osgParticle::rangef(0.025f, 0.025f));
	ptemp.setColorRange(osgParticle::rangev4(color, color));

	osgParticle::ParticleSystem *ps = osgNew osgParticle::ParticleSystem;
	ps->setDefaultAttributes();
	ps->setDefaultParticleTemplate(ptemp);
	ps->setFreezeOnCull(false);

	osgParticle::PointPlacer *pp = osgNew osgParticle::PointPlacer;
	pp->setCenter(center);

	osgParticle::RandomRateCounter *rrc = osgNew osgParticle::RandomRateCounter;
	rrc->setRateRange(30, 40);

	osgParticle::RadialShooter *rs = osgNew osgParticle::RadialShooter;
	rs->setPhiRange(0, osg::PI*2);
	rs->setThetaRange(osg::PI_2, osg::PI_2);
	rs->setInitialSpeedRange(0.25f, 0.5f);

	osgParticle::ModularEmitter *me = osgNew osgParticle::ModularEmitter;
	me->setParticleSystem(ps);
	me->setPlacer(pp);
	me->setCounter(rrc);
	me->setShooter(rs);
	base->addChild(me);

	osgParticle::AccelOperator *aop = osgNew osgParticle::AccelOperator;
	aop->setToGravity();

	osgParticle::ModularProgram *mp = osgNew osgParticle::ModularProgram;
	mp->setParticleSystem(ps);
	mp->addOperator(aop);
	base->addChild(mp);

	return ps;
}

void BaseObject::setNode( osg::Node * pNode )
{ 
    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::setNode() - ID: " << m_iObjectID);

    m_rpNode = pNode;
    
	osg::StateSet * stateSet = osgNew osg::StateSet;
	stateSet->setGlobalDefaults();
	m_rpNode->setStateSet(stateSet);
    
	osgUtil::SmoothingVisitor sv;
	m_rpNode->accept(sv);

	// to switch between various representants of same object (depending on views for example)
    m_rpSwitch = osgNew osg::Switch;
	m_rpSwitch->setValue(osg::Switch::ALL_CHILDREN_ON);
	m_rpSwitch->addChild(m_rpNode.get());
}

osg::Node* BaseObject::getNode()
{ 
    return m_rpTransform.get(); 
}


void BaseObject::addToScene()
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::addToScene() - ID: " << m_iObjectID);

	// master object to which all others ones are linked
    m_rpTransform = osgNew osg::MatrixTransform;
		
    m_rpTransform->addChild( m_rpSwitch.get() );

    StandardMatrix4 stdmat = TranslationMatrix4( m_LocalPosition ) * StandardMatrix4( m_Orientation );

	stdmat = stdmat.Transpose(); // pass it to opengl

    osg::Matrix worldMat;
    worldMat.set( stdmat[0][0], stdmat[0][1], stdmat[0][2], stdmat[0][3],
                  stdmat[1][0], stdmat[1][1], stdmat[1][2], stdmat[1][3],
                  stdmat[2][0], stdmat[2][1], stdmat[2][2], stdmat[2][3],
                  stdmat[3][0], stdmat[3][1], stdmat[3][2], stdmat[3][3] );


	//m_rpTransform->setReferenceFrame(osg::Transform::RELATIVE_TO_PARENTS);

    m_rpTransform->setMatrix(worldMat);

	setCullingActive(true);

	g_pBattlefield->addNodeToScene(m_rpTransform.get());

 	m_rpNode->setName( m_sObjectName );

    if (m_sObjectName == "PLAYER" )
	{
	osg::BoundingSphere s = m_rpNode.get()->getBound();
	float r = s.radius();
	osg::Vec3 c = s.center();

	//osgParticle::ParticleSystem *ps1 = setupParticleSystem(m_rpTransform.get(), 
	//	                                                   osg::Vec4(1, 0, 1, 1), c - osg::Vec3(0,-r, 0),1);
	//osgText::Text * ps1 = osgNew osgText::Text;
	//ps1->setText("Particle system");
	//ps1->setPosition( c - osg::Vec3(0,-r, 0));
	//osg::Geode *geode1 = osgNew osg::Geode;
	//geode1->addDrawable(ps1);
	//g_pBattlefield->addNodeToScene(geode1);
	}

	//CSP_LOG(CSP_APP, CSP_DEBUG, "NodeName: " << m_rpNode->getName() <<
	//	", BoundingPos: " << sphere.center() << ", BoundingRadius: " << 
	//	sphere.radius() );

}


int BaseObject::updateScene()
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::updateScene() ID:"  << m_iObjectID );

	osg::BoundingSphere s = m_rpNode.get()->getBound();
	//float r = s.radius();
	osg::Vec3 c = s.center();
    
	StandardMatrix4  stdmat = StandardMatrix4(StandardMatrix3::IDENTITY );

	// add testing model code here
	if (m_sObjectName == "PLAYER" )
	{
	//stdmat = TranslationMatrix4(-c.x(),-c.y(),-c.z()); 
	}

	stdmat =  TranslationMatrix4( m_LocalPosition ) * StandardMatrix4( m_Orientation ) * stdmat;

	stdmat = stdmat.Transpose(); // pass it to opengl
	
    osg::Matrix worldMat;
    worldMat.set( stdmat[0][0], stdmat[0][1], stdmat[0][2], stdmat[0][3],
                  stdmat[1][0], stdmat[1][1], stdmat[1][2], stdmat[1][3],
                  stdmat[2][0], stdmat[2][1], stdmat[2][2], stdmat[2][3],
                  stdmat[3][0], stdmat[3][1], stdmat[3][2], stdmat[3][3] );

    m_rpTransform->setMatrix(worldMat);

	CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::updateScene() - Position: " <<
		m_LocalPosition );
//	CSP_LOG(CSP_APP, CSP_DEBUG, "BaseObject::updateScene() - Bounding Sphere " 
//		<< c.x() << ", " << c.y() << ", " << c.z() << ", " << r );

	return 0;

}

void BaseObject::setCullingActive(bool flag)
{
    //if (m_rpNode.valid())
	//	m_rpNode->setCullingActive(flag);
	if (m_rpTransform.valid())
	{
		m_rpTransform->setCullingActive(flag);
	}
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
