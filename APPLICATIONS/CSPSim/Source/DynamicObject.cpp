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


#include "DynamicObject.h"
#include "BaseController.h"
#include "LogStream.h"
#include "VirtualBattlefield.h"
#include "VirtualScene.h"
#include "TerrainObject.h"
#include "SmokeEffects.h"
#include "CSPSim.h"

#include <osg/Geode>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>

#include <SimData/Quaternion.h>



DynamicObject::DynamicObject(): SimObject()
{
	m_Mass = 1.0;
	m_Speed = 0.0;

	m_GroundZ = 0.0;
	m_GroundN = simdata::Vector3::ZAXIS;

	setAttitude(simdata::Quaternion::IDENTITY);
	setGlobalPosition(simdata::Vector3::ZERO);

	m_Controller = NULL;

	setLocal(true);
	setHuman(false);

	m_PrevPosition = simdata::Vector3::ZERO;

	m_LinearVelocity = simdata::Vector3::ZERO;
	m_AngularVelocity = simdata::Vector3::ZERO;

	m_Inertia = simdata::Matrix3::IDENTITY;
	m_InertiaInv = simdata::Matrix3::IDENTITY;

	m_SmokeTrails = NULL;
	m_Smoke = false;
}

DynamicObject::~DynamicObject()
{
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

void DynamicObject::setGlobalPosition(simdata::Vector3 const & position)
{
	setGlobalPosition(position.x, position.y, position.z);
}

void DynamicObject::setGlobalPosition(double x, double y, double z)
{
	// if the object is on the ground ignore the z component and use the elevation at
	// the x,y point.
	m_GlobalPosition.x = x;
	m_GlobalPosition.y = y;

	if (isGrounded())
	{
		float offset = 0.0;
		/* FIXME FIXME FIXME XXX XXX
		if (m_rpNode.valid())
		{
			// FIXME this is not a very good way to put a vehicle on the ground...
			osg::BoundingSphere sphere = m_rpNode->getBound();
			offset = sphere.radius() - sphere.center().z();
		}
		else 
			offset = 0.0;
		if (m_Battlefield) {
			offset += m_Battlefield->getElevation(x,y);
		}
		*/
		m_GlobalPosition.z = offset; 
	}
	else
	{
		m_GlobalPosition.z = z; 
	}

/*
	CSP_LOG(CSP_APP, CSP_DEBUG, "SimObject::setPosition - ID: " << m_ObjectID 
	        << ", Name: " << m_ObjectName << ", Position: " << m_GlobalPosition );
*/
}

void DynamicObject::setLocalFrame(simdata::Vector3 const &origin, osg::Group *group) {
	m_LocalOrigin = origin;
	m_LocalGroup = group;
}

void DynamicObject::setVelocity(simdata::Vector3 const &velocity)
{
	m_LinearVelocity = velocity;
	m_Speed = m_LinearVelocity.Length();

/*
	CSP_LOG(CSP_APP, CSP_DEBUG, "SimObject::setVelocity - ID: " << m_ObjectID 
	<< ", Name: " << m_ObjectName << ", Velocity: " << m_LinearVelocity );
*/
}

void DynamicObject::setVelocity(double Vx, double Vy, double Vz)
{
	m_LinearVelocity = simdata::Vector3(Vx, Vy, Vz);
	m_Speed = m_LinearVelocity.Length();

/*
	CSP_LOG(CSP_APP, CSP_DEBUG, "SimObject::setVelocity - ID: " << m_ObjectID 
	<<  ", Name: " << m_ObjectName << ", Velocity: " << m_LinearVelocity );
*/
}

void DynamicObject::updateGroundPosition()
{
	float offset;
	if (m_Model.valid()) {
		//offset = m_Model->groundOffset();
		offset = 0.0;
	}
	else {
		offset = 0.0;
	}
	m_GlobalPosition.z = CSPSim::theSim->getBattlefield()->getElevation(m_GlobalPosition.x, m_GlobalPosition.y) + offset; 
	m_GlobalPosition.z = offset;
}



// move
void DynamicObject::doMovement(double dt)
{
	// Save the objects old position
	m_PrevPosition = m_GlobalPosition;
}

// update 
double DynamicObject::onUpdate(double dt)
{
	doMovement(dt);
	if (m_Controller) {
		m_Controller->onUpdate(dt);
	}
	postMotion(dt);
	return 0.0;
}

// update variables that depend on position
void DynamicObject::postMotion(double dt)
{
	if (m_SmokeTrails.valid()) {
		m_SmokeTrails->update(dt, m_GlobalPosition, m_Attitude);
	}
	if (isGrounded()) {
		updateGroundPosition();
	} else {
		VirtualBattlefield const *battlefield = CSPSim::theSim->getBattlefield();
		assert(battlefield);
		m_GroundZ = battlefield->getElevation(m_GlobalPosition.x, m_GlobalPosition.y);
		float h1, h2;
		h1 = battlefield->getElevation(m_GlobalPosition.x+1, m_GlobalPosition.y) - m_GroundZ;
		h2 = battlefield->getElevation(m_GlobalPosition.x, m_GlobalPosition.y+1) - m_GroundZ;
		m_GroundN = simdata::Vector3(-h1,-h2, 1.0);
		m_GroundN.Normalize();
		/*
		float nx, ny, nz;
		m_Battlefield->getNormal(m_GlobalPosition.x, m_GlobalPosition.y, nx, ny, nz);
		m_GroundN = simdata::Vector3(nx, ny, nz);
		*/
	}
}

simdata::Vector3 DynamicObject::getDirection() const
{
	return m_Attitude.GetRotated(simdata::Vector3::YAXIS);
}

simdata::Vector3 DynamicObject::getUpDirection() const
{
	return m_Attitude.GetRotated(simdata::Vector3::ZAXIS);
}

void DynamicObject::setAttitude(simdata::Quaternion const &attitude)
{
	m_Attitude = attitude;
}

simdata::Vector3 DynamicObject::getViewPoint() const {
	 return m_Attitude.GetRotated(m_Model->getViewPoint());
}

// FIXME move the osg related code out of here!
bool DynamicObject::AddSmoke()
{
	if (m_SmokeTrails.valid()) return true;
	if (!m_SceneModel) return false;

	fx::SmokeTrail *trail = new fx::SmokeTrail();
	trail->setEnabled(false);
	trail->setTexture("Images/white-smoke.rgb");
	trail->setColorRange(osg::Vec4(0.9, 0.9, 1.0, 1.0), osg::Vec4(1.0, 1.0, 1.0, 0.5));
	trail->setSizeRange(0.2, 4.0);
	trail->setLight(true);
	trail->setLifeTime(5.5);
	trail->setExpansion(1.2);
	trail->addOperator(new fx::SmokeThinner);

	// FIXME position should be specified in ObjectModel XML
	double radius = m_Model->getBoundingSphereRadius();
	trail->setOffset(simdata::Vector3(0.0, -0.70 * radius, 0.0));

	m_SmokeTrails = new fx::SmokeTrailSystem;
	m_SmokeTrails->addSmokeTrail(trail);

	m_Smoke = false;

	return true;
}


bool DynamicObject::isSmoke() {
	return m_Smoke;
}

void DynamicObject::DisableSmoke()
{
	if (m_Smoke) {
		m_SmokeTrails->setEnabled(false);
		m_Smoke = false;
	}
}

void DynamicObject::EnableSmoke()
{
	if (!m_Smoke) {
		if (!AddSmoke()) return;
		m_SmokeTrails->setEnabled(true);
		m_Smoke = true;
	}
}

