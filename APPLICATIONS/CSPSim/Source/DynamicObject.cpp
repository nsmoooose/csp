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
 * @file DynamicObject.cpp
 *
 **/

#include <osg/Geode>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>

#include <SimData/Quaternion.h>

#include "DynamicObject.h"
#include "LogStream.h"
#include "VirtualBattlefield.h"
#include "SmokeEffects.h"

extern double g_LatticeXDist;
extern double g_LatticeYDist;


DynamicObject::DynamicObject(): SimObject()
{
	m_Mass = 1.0;
	m_Speed = 0.0;

	m_pController = NULL;
	m_iControllerID = 0;

	m_Local = true;
	m_Human = false;

	m_PrevPosition = simdata::Vector3::ZERO;

	m_LinearVelocity = simdata::Vector3::ZERO;
	m_LinearVelocityDirection = simdata::Vector3::YAXIS;

	m_AngularVelocity = simdata::Vector3::ZERO;

	m_Inertia = simdata::Matrix3::IDENTITY;
	m_InertiaInv = simdata::Matrix3::IDENTITY;

	m_SmokeSegments = NULL;
	m_SmokeUpdater = NULL;
	m_SmokeEmitter = NULL;
	m_Smoke = false;
}

DynamicObject::~DynamicObject()
{
	if (m_SmokeSegments) {
		delete m_SmokeSegments;
	}
}

void DynamicObject::pack(simdata::Packer& p) const {
	SimObject::pack(p);
	p.pack(m_Mass);
	p.pack(m_Inertia);
}

void DynamicObject::unpack(simdata::UnPacker& p) {
	SimObject::unpack(p);
	p.unpack(m_Mass);
	p.unpack(m_Inertia);
}


void DynamicObject::setVelocity(simdata::Vector3 const &velocity)
{
	m_LinearVelocity = velocity;
	m_Speed = m_LinearVelocity.Length();

	CSP_LOG(CSP_APP, CSP_DEBUG, "SimObject::setVelocity - ID: " << m_iObjectID 
	<< ", Name: " << m_sObjectName << ", Velocity: " << m_LinearVelocity );
}

void DynamicObject::setVelocity(double Vx, double Vy, double Vz)
{
	m_LinearVelocity = simdata::Vector3(Vx, Vy, Vz);
	m_Speed = m_LinearVelocity.Length();

	CSP_LOG(CSP_APP, CSP_DEBUG, "SimObject::setVelocity - ID: " << m_iObjectID 
	<<  ", Name: " << m_sObjectName << ", Velocity: " << m_LinearVelocity );
}

void DynamicObject::updateGroundPosition()
{
	float offset;
	if (m_rpNode.valid()) {
		osg::BoundingSphere sphere = m_rpNode->getBound();
		offset = sphere.radius() - sphere.center().z();
	}
	else {
		offset = 0.0;
	}
	assert(m_Battlefield);
	m_GlobalPosition.z = m_Battlefield->getElevation(m_GlobalPosition.x, m_GlobalPosition.y) + offset; 
	m_LocalPosition.z = m_GlobalPosition.z;
}

// Update Local Position to account for lattice crossings.
void DynamicObject::updateLocalPosition()
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
void DynamicObject::updateGlobalPosition()
{
	m_GlobalPosition.x = m_LocalPosition.x + m_XLatticePos*g_LatticeXDist;
	m_GlobalPosition.y = m_LocalPosition.y + m_YLatticePos*g_LatticeYDist;
	m_GlobalPosition.z = m_LocalPosition.z;
	// simple testing of smoke trails... improve later
	if (m_SmokeSegments) {
		simdata::Vector3 motion = m_LocalPosition - m_PrevPosition;
//		simdata::Vector3 motionBody = simdata::QVRotate(m_qOrientation.Bar(), motion);
//		osg::Quat q = osg::Quat(m_qOrientation.x, m_qOrientation.y, m_qOrientation.z, m_qOrientation.w);
		//m_SmokeSegments->update(osg::Vec3(motionBody.x, motionBody.y, motionBody.z), q);
//		osg::Matrix m, minv;
	//	m.makeRotate(q);
	//	minv.makeRotate(q.inverse());
		m_SmokeSegments->update(motion, m_Orientation, m_OrientationInverse);
	}
}

void DynamicObject::updateOrientation() 
{
	m_qOrientation.ToRotationMatrix(m_Orientation);
	m_qOrientation.Inverse().ToRotationMatrix(m_OrientationInverse);
}

// move
void DynamicObject::doMovement(double dt)
{
	// Save the objects old position
	m_PrevPosition = m_LocalPosition;
}

// update 
void DynamicObject::onUpdate(double dt)
{
	doMovement(dt);
	if (m_pController) {
		m_pController->onUpdate(dt);
	}
	postMotion(dt);
}

// update variables that depend on position
void DynamicObject::postMotion(double dt)
{
	updateLocalPosition();
	updateGlobalPosition();
	if (m_bOnGround) {
		updateGroundPosition();
	}
}

bool DynamicObject::AddSmoke()
{
	if (!m_Battlefield) return false;
	if (m_SmokeSegments) return true;

	osg::BoundingSphere s = m_rpNode.get()->getBound();
	m_SmokeSegments = new fx::smoke::SmokeSegments;
	m_SmokeSegments->addSource(simdata::Vector3(0.0, -0.8 * s.radius(), 0.0));

	fx::smoke::Trail trail;
	trail.setTexture("Images/white-smoke.rgb");
	trail.setColorRange(osg::Vec4(0.9, 0.9, 1.0, 1.0), osg::Vec4(1.0, 1.0, 1.0, 0.5));
	trail.setSizeRange(0.2, 4.0);
	trail.setPlacer(m_SmokeSegments->getSegment(0));

	trail.setOperator(new fx::smoke::Thinner);
	trail.setLight(true);

	trail.setLifeTime(5.5);
	trail.setExpansion(1.2);
	osgParticle::ParticleSystemUpdater *psu = 0;
	m_SmokeEmitter = trail.create(m_rpTransform.get(), psu);
	m_SmokeUpdater = psu;
	m_SmokeEmitter->setEnabled(false);
	
	m_SmokeGeode = new osg::Geode;
	m_SmokeGeode->setName("SmokeParticleSystem");
	m_SmokeGeode->addDrawable(m_SmokeEmitter->getParticleSystem());
	m_Battlefield->addNodeToScene(m_SmokeGeode.get());
	m_rpTransform->addChild(m_SmokeUpdater.get());

	m_Smoke = false;

	return true;
}

bool DynamicObject::isSmoke() {
	return m_Smoke;
}

void DynamicObject::DisableSmoke()
{
	if (m_Smoke) {
		m_SmokeEmitter->setEnabled(false);
		m_Smoke = false;
	}
}

void DynamicObject::EnableSmoke()
{
	if (!m_Smoke) {
		if (!AddSmoke()) return;
		m_SmokeEmitter->setEnabled(true);
		m_Smoke = true;
	}
}

