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
 * @file SimObject.cpp
 *
 **/


#include "SimObject.h"
#include "Log.h"
#include "SmokeEffects.h"
#include "VirtualBattlefield.h"

#include <osg/NodeVisitor>
#include <osg/Quat>

#include <SimData/InterfaceRegistry.h>
#include <SimData/Math.h>


SIMDATA_REGISTER_INTERFACE(SimObject)


SimObject::SimObject()
{
	CSP_LOG(APP, DEBUG, "SimObject::SimObject()" );

	m_SceneModel = NULL;

	m_Flags = 0;

	///m_ObjectID = 0;
	///m_ObjectType = 0;
	///m_ObjectName = "";
}


SimObject::~SimObject()
{
	CSP_LOG(APP, INFO, "SimObject::~SimObject()" );
}


void SimObject::pack(simdata::Packer& p) const {
	Object::pack(p);
	p.pack(m_Model);
}


void SimObject::unpack(simdata::UnPacker& p) {
	Object::unpack(p);
	p.unpack(m_Model);
}


void SimObject::createSceneModel() {
	if (m_SceneModel) return;
	m_SceneModel = new SceneModel(m_Model);
	assert(m_SceneModel);
}

void SimObject::destroySceneModel() {
	if (m_SceneModel) {
		delete m_SceneModel;
		m_SceneModel = NULL;
	}
}

osg::Node* SimObject::getOrCreateModelNode() {
	if (!m_SceneModel) createSceneModel();
	return m_SceneModel->getRoot();
}

osg::Node* SimObject::getModelNode() { 
	if (!m_SceneModel) return 0;
	return m_SceneModel->getRoot();
}



////////////////////////////////////////////////////////////////////////////////////////////
//  UNUSED, LEFTOVER CRUFT... maybe useful down the road, who knows?
////////////////////////////////////////////////////////////////////////////////////////////


#if 0
void SimObject::setLocalPosition(simdata::Vector3 const & position)
{
	setLocalPosition(position.x, position.y, position.z);
}

void SimObject::setLocalPosition(double x, double y, double z)
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

	CSP_LOG(APP, DEBUG, "SimObject::setPosition - ID: " << m_ObjectID 
	        << ", Name: " << m_ObjectName << ", LocalPosition: " << m_LocalPosition );

}

void SimObject::setLatticePosition(int x, int y)
{
	m_XLatticePos = x;
	m_YLatticePos = y;
}
#endif

/*
extern osgParticle::ParticleSystem *setupParticleSystem(osg::MatrixTransform *base, 
                                                        const std::string & p_textureFile,
                                                        const osg::Vec4 &colorMin,
                                                        const osg::Vec4 &colorMax,
                                                        const osg::Vec3 &center, 
							osgParticle::SegmentPlacer *&placer,
                                                        float lifetime);
*/

/*
void SimObject::AddSmoke()
{
	if (!m_Battlefield) return;

	osg::BoundingSphere s = m_rpNode.get()->getBound();
	float r = s.radius();
	unsigned short i;

	osgParticle::ParticleSystemUpdater *psu = new osgParticle::ParticleSystemUpdater;
	osgParticle::ParticleSystem *ps;
	ps = setupParticleSystem(m_rpTransform.get(),
	                         "Images/white-smoke.rgb",
	                         osg::Vec4(0.9, 0.9, 1.0, 1.0), 
	                         osg::Vec4(1.0, 1.0, 1.0, 0.5), 
	                         osg::Vec3(0.0, 0.8 * r, 0.0),
				 m_Placer, 
				 5.0);
	osg::Geode *geode = new osg::Geode;
	geode->setName("PlayerParticleSystem");
	geode->addDrawable(ps);
	m_Battlefield->addNodeToScene(geode);
	psu->addParticleSystem(ps);
    
	m_rpTransform.get()->addChild(psu);
}
*/

#if 0
void SimObject::AddSmoke()
{
	if (!m_Battlefield) return;

	osg::BoundingSphere s = m_rpNode.get()->getBound();
	float r = s.radius();
	unsigned short i;

	osg::Vec3Array* pl = new osg::Vec3Array;
	
	for (i = 0; i<1; ++i) {
		pl->push_back(osg::Vec3(0.0,-(0.8+i/10.0) * r,0.0));
	}
    

	osgParticle::ParticleSystemUpdater *psu = new osgParticle::ParticleSystemUpdater;

	for (i = 0; i<pl->size();++i) {
		osgParticle::ParticleSystem *ps;
		/*
		ps = setupParticleSystem(m_rpTransform.get(),
		                         "Images/particle.rgb",
		                         osg::Vec4(0.2, 0.5, 1, 0.8), 
		                         osg::Vec4(0,0.5,1,1), 
		                         (*pl)[i],0.05);
		*/
		ps = setupParticleSystem(m_rpTransform.get(),
		                         "Images/white-smoke.rgb",
		                         osg::Vec4(0.9, 0.9, 1.0, 1.0), 
		                         osg::Vec4(1.0, 1.0, 1.0, 0.5), 
		                         //"Images/white-smoke.rgb",
		                         //osg::Vec4(0.5, 0.5, 0.6, 0.1), 
		                         //osg::Vec4(0.6, 0.6, 0.6, 0.3), 
		                         (*pl)[i],
					 m_Placer, 
					 5.0);
		osg::Geode *geode = new osg::Geode;
		geode->setName("PlayerParticleSystem");
		geode->addDrawable(ps);
		m_Battlefield->addNodeToScene(geode);
		psu->addParticleSystem(ps);
	}
    
	/*
	for (i = 0; i<pl->size();++i) {
		osgParticle::ParticleSystem *ps;
		ps = setupParticleSystem(m_rpTransform.get(),
		                         "Images/smoke.rgb",
		                         osg::Vec4(0,1,0,1), 
		                         osg::Vec4(0.2, 0.5, 1, 0.8), 
		                         (*pl)[i],0.01);
		osg::Geode *geode = new osg::Geode;
		geode->setName("PlayerParticleSystem");
		geode->addDrawable(ps);
		g_pBattlefield->addNodeToScene(geode);
		psu->addParticleSystem(ps);
	}
	*/
    
	//g_pBattlefield->addNodeToScene(psu);
	m_rpTransform.get()->addChild(psu);
}
#endif

/*
<<<<<<< SimObject.cpp

void SimObject::addToScene(VirtualBattlefield *battlefield)
{
	CSP_LOG(APP, DEBUG, "SimObject::addToScene() - ID: " << m_iObjectID);

	if (!m_ModelInit) {
		initModel();
	}

	osg::Matrix worldMat;
	simdata::Matrix3::M_t (&R)[3][3] = m_Orientation.rowcol;
	worldMat.set(R[0][0], R[1][0], R[2][0], 0.0,
	             R[0][1], R[1][1], R[2][1], 0.0,
		     R[0][2], R[1][2], R[2][2], 0.0,
		     m_LocalPosition.x, m_LocalPosition.y, m_LocalPosition.z, 1.0);

	//m_rpTransform->setReferenceFrame(osg::Transform::RELATIVE_TO_PARENTS);

	m_rpTransform->setMatrix(worldMat);
	
	m_Battlefield = battlefield;
	m_Battlefield->addNodeToScene(m_rpTransform.get());

	setCullingActive(true);

	//CSP_LOG(APP, DEBUG, "NodeName: " << m_rpNode->getName() <<
	//	", BoundingPos: " << sphere.center() << ", BoundingRadius: " << 
	//	sphere.radius() );

}


void SimObject::removeFromScene() {
	// FIXME what else?
	m_Battlefield = NULL;
}


int SimObject::updateScene() { 
	// this needs 2 upgrades; 
	// first one is: working with quat and only quat; 
	// second is: make an osg update()/draw() callback
	

	CSP_LOG(APP, DEBUG, "SimObject::updateScene() ID:"  << m_iObjectID );

	osg::Quat q = osg::Quat(m_qOrientation.x, m_qOrientation.y, m_qOrientation.z, m_qOrientation.w);
	osg::Matrix R = osg::Matrix::rotate(q);
	osg::Matrix T = osg::Matrix::translate(m_LocalPosition.x, m_LocalPosition.y, m_LocalPosition.z);
	
	m_rpTransform->setMatrix(R * T);

	// onRender();

	CSP_LOG(APP, DEBUG, "SimObject::updateScene() - Position: " <<
		m_LocalPosition );
//	CSP_LOG(APP, DEBUG, "SimObject::updateScene() - Bounding Sphere " 
//		<< c.x() << ", " << c.y() << ", " << c.z() << ", " << r );

	return 0;
}

void SimObject::setCullingActive(bool flag)
{
	//if (m_rpNode.valid())
	//	m_rpNode->setCullingActive(flag);
	if (m_rpTransform.valid())
	{
		m_rpTransform->setCullingActive(flag);
	}
}


void SimObject::getLatticePosition(int & x, int & y) const
{
	x = m_XLatticePos;
	y = m_YLatticePos;
}


void SimObject::setOrientation(simdata::Matrix3 const &mOrientation)
{
	m_Orientation = mOrientation;
	m_qOrientation.FromRotationMatrix(m_Orientation);
	m_Direction = m_Orientation*simdata::Vector3::XAXIS;
	m_NormalDirection = m_Orientation*simdata::Vector3::ZAXIS;
}

void SimObject::setOrientation(simdata::Quaternion const &qOrientation)
{
	simdata::Matrix3 Orientation;
	qOrientation.ToRotationMatrix(Orientation);
	SimObject::setOrientation(Orientation);
}

=======
>>>>>>> 1.7
*/