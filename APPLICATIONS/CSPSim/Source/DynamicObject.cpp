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
#include "Log.h"
#include "VirtualBattlefield.h"
#include "VirtualScene.h"
#include "TerrainObject.h"
#include "CSPSim.h"

#include <SimData/Quat.h>



DynamicObject::DynamicObject(): SimObject()
{
	m_Mass = 1.0;
	m_Speed = 0.0;

	m_GroundZ = 0.0;
	m_GroundN = simdata::Vector3::ZAXIS;
	m_GroundHint = 0;

	setAttitude(simdata::Quat::IDENTITY);
	setGlobalPosition(simdata::Vector3::ZERO);

	m_Controller = NULL;

	setLocal(true);
	setHuman(false);

	m_PrevPosition = simdata::Vector3::ZERO;

	m_LinearVelocity = simdata::Vector3::ZERO;
	m_AngularVelocity = simdata::Vector3::ZERO;

	m_Inertia = simdata::Matrix3::IDENTITY;
	m_InertiaInv = simdata::Matrix3::IDENTITY;
}

DynamicObject::~DynamicObject()
{
}

void DynamicObject::pack(simdata::Packer& p) const {
	SimObject::pack(p);
	p.pack(m_Model);
	p.pack(m_Mass);
	p.pack(m_Inertia);
}

void DynamicObject::unpack(simdata::UnPacker& p) {
	SimObject::unpack(p);
	p.unpack(m_Model);
	p.unpack(m_Mass);
	p.unpack(m_Inertia);
}

void DynamicObject::createSceneModel() {
	if (!m_SceneModel) {
		m_SceneModel = new SceneModel(m_Model);
		assert(m_SceneModel.valid());
		bindAnimations();
	}
}

void DynamicObject::destroySceneModel() {
	m_SceneModel = NULL;
}

osg::Node* DynamicObject::getOrCreateModelNode() {
	if (!m_SceneModel) createSceneModel();
	return m_SceneModel->getRoot();
}

osg::Node* DynamicObject::getModelNode() { 
	if (!m_SceneModel) return 0;
	return m_SceneModel->getRoot();
}


void DynamicObject::setGlobalPosition(simdata::Vector3 const & position)
{
	setGlobalPosition(position.x(), position.y(), position.z());
}

void DynamicObject::setGlobalPosition(double x, double y, double z)
{
	// if the object is on the ground ignore the z component and use the elevation at
	// the x,y point.
	m_GlobalPosition.x() = x;
	m_GlobalPosition.y() = y;

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
		m_GlobalPosition.z() = offset; 
	}
	else
	{
		m_GlobalPosition.z() = z; 
	}

/*
	CSP_LOG(APP, DEBUG, "SimObject::setPosition - ID: " << m_ObjectID 
	        << ", Name: " << m_ObjectName << ", Position: " << m_GlobalPosition );
*/
}

void DynamicObject::setVelocity(simdata::Vector3 const &velocity)
{
	m_LinearVelocity = velocity;
	m_Speed = m_LinearVelocity.length();

/*
	CSP_LOG(APP, DEBUG, "SimObject::setVelocity - ID: " << m_ObjectID 
	<< ", Name: " << m_ObjectName << ", Velocity: " << m_LinearVelocity );
*/
}

void DynamicObject::setVelocity(double Vx, double Vy, double Vz)
{
	m_LinearVelocity = simdata::Vector3(Vx, Vy, Vz);
	m_Speed = m_LinearVelocity.length();

/*
	CSP_LOG(APP, DEBUG, "SimObject::setVelocity - ID: " << m_ObjectID 
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
	m_GlobalPosition.z() = CSPSim::theSim->getBattlefield()->getGroundElevation(m_GlobalPosition.x(), m_GlobalPosition.y(), m_GroundHint) + offset; 
	m_GlobalPosition.z() = offset;
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
	onRender();
	return 0.0;
}

unsigned int DynamicObject::onRender() {
	return 0;
}

// update variables that depend on position
void DynamicObject::postMotion(double dt) {
	if (m_SceneModel.valid() && isSmoke()) {
		m_SceneModel->updateSmoke(dt, m_GlobalPosition, m_Attitude);
	}
	// FIXME GroundZ/GroundN should be set for all vehicles
	if (isGrounded()) {
		updateGroundPosition();
	} else {
		VirtualBattlefield const *battlefield = CSPSim::theSim->getBattlefield();
		assert(battlefield);
		m_GroundZ = battlefield->getGroundElevation(m_GlobalPosition.x(), m_GlobalPosition.y(), m_GroundN, m_GroundHint);
		/*
		float h1, h2;
		h1 = battlefield->getElevation(m_GlobalPosition.x()+1, m_GlobalPosition.y()) - m_GroundZ;
		h2 = battlefield->getElevation(m_GlobalPosition.x(), m_GlobalPosition.y()+1) - m_GroundZ;
		m_GroundN = simdata::Vector3(-h1,-h2, 1.0);
		m_GroundN.normalize();
		*/
		/*
		float nx, ny, nz;
		m_Battlefield->getNormal(m_GlobalPosition.x(), m_GlobalPosition.y(), nx, ny, nz);
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

void DynamicObject::setAttitude(simdata::Quat const &attitude)
{
	m_Attitude = attitude;
}

simdata::Vector3 DynamicObject::getViewPoint() const {
	 return m_Attitude.GetRotated(m_Model->getViewPoint());
}

bool DynamicObject::isSmoke() {
	return m_SceneModel.valid() && m_SceneModel->isSmoke();
}

void DynamicObject::disableSmoke() {
	if (m_SceneModel.valid() && isSmoke()) {
		m_SceneModel->disableSmoke();
	}
}

void DynamicObject::enableSmoke() {
	if (m_SceneModel.valid() && !isSmoke()) {
		m_SceneModel->enableSmoke();
	}
}

void DynamicObject::setDataRecorder(DataRecorder *recorder) {
	m_DataRecorder = recorder;
	initDataRecorder();
}

void DynamicObject::initDataRecorder() {
}

