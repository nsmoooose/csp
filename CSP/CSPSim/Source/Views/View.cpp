// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file View.cpp
 *
 **/

#include "CSPSim.h"
#include "DynamicObject.h"
#include "VirtualBattlefield.h"

#include "Views/View.h"
#include "Views/CameraAgent.h"



void View::accept(const simdata::Ref<DynamicObject> object) {
	m_ActiveObject = object;
}

View::~View() { }

void View::updateBody(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up) {
	simdata::Vector3 object_up = m_ActiveObject->getUpDirection();
	simdata::Vector3 object_dir = m_ActiveObject->getDirection();
	simdata::Quat q = simdata::Quat(-m_CameraKinematics->getTheta(),object_up,-m_CameraKinematics->getPhi(),
					object_dir^object_up,0.0,object_dir);
	simdata::Vector3 object_pos = m_ActiveObject->getGlobalPosition();
	ep = object_pos + m_CameraKinematics->getDistance() * q.rotate(-object_dir);
	lp = object_pos;
	up = q.rotate(object_up);
}

void View::updateWorld(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up) {
	simdata::Quat q = simdata::Quat(-m_CameraKinematics->getTheta(),simdata::Vector3::ZAXIS,-m_CameraKinematics->getPhi(),
					 simdata::Vector3::XAXIS,0.0,simdata::Vector3::YAXIS);
	simdata::Vector3 object_pos = m_ActiveObject->getGlobalPosition();
	ep = object_pos + m_CameraKinematics->getDistance() * q.rotate(-simdata::Vector3::YAXIS);
	lp = object_pos;
	up = q.rotate(simdata::Vector3::ZAXIS);
}

View::View(size_t vm):
	m_ViewMode(vm),
	m_Internal(false) {
}

void View::cull() {
	VirtualScene* scene = CSPSim::theSim->getScene();
	if (scene && m_ActiveObject.valid()) {
		bool isNear = m_ActiveObject->getNearFlag();
		if (isNear && !m_Internal) {
			scene->setNearObject(m_ActiveObject, false);
		} else
		if (!isNear && m_Internal) {
			scene->setNearObject(m_ActiveObject, true);		
		}
	}
}

void InternalView::constrain() {
	float limit = simdata::PI_2;
	m_CameraKinematics->clampPhi(m_CameraKinematics->getPhi(),-limit,limit);
	m_CameraKinematics->clampTheta(m_CameraKinematics->getTheta(),-limit,limit);
}

void InternalView::update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt) {
	constrain();
	simdata::Vector3 object_up = m_ActiveObject->getUpDirection();
	simdata::Vector3 object_dir = m_ActiveObject->getDirection();
	simdata::Quat q = simdata::Quat(m_CameraKinematics->getTheta(),object_up,m_CameraKinematics->getPhi(),
					object_dir^object_up,0.0,object_dir);
	simdata::Vector3 object_pos = m_ActiveObject->getGlobalPosition();
	ep = object_pos + m_ActiveObject->getViewPoint();
	lp = ep + m_CameraKinematics->getDistance() * q.rotate(object_dir);
	up = q.rotate(object_up);
}

void InternalView::activate() {
	m_CameraKinematics->reset();
}

void InternalViewHist::update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt) {
	const float c = 0.001;
	simdata::Vector3 displ = c*dt*m_ActiveObject->getVelocity();
	static simdata::Vector3 prev_displ = displ;
	
	InternalView::update(ep,lp,up,dt);
	
	ep += 0.5*(prev_displ + displ);
	prev_displ = displ;
}

void ExternalViewBody::activate() {
	m_CameraKinematics->resetDistance();
}

void ExternalViewBody::update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt) {
	updateBody(ep,lp,up);
}

void ExternalViewWorld::activate() {
	m_CameraKinematics->resetDistance();
}

void ExternalViewWorld::update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt) {
	updateWorld(ep,lp,up);
}


void FlybyView::newFixedCamPos(SimObject* target) {
	simdata::Vector3 object_pos = target->getGlobalPosition();
	DynamicObject* dynamic = dynamic_cast<DynamicObject*>(target);
	if (dynamic) {
		simdata::Vector3 up = dynamic->getUpDirection();
		simdata::Vector3 object_dir = dynamic->getDirection();
		//double speed_level = dynamic->getSpeed()/50.0;
		m_FixedCameraPosition =  object_pos + 900.0* object_dir + ( 12.0 - (rand() % 5) ) * (object_dir^up) 
			 + ( 6.0 + (rand () % 3) ) * up;
	} else {
		m_FixedCameraPosition = object_pos + 100.0 * simdata::Vector3::ZAXIS + 100.0 * simdata::Vector3::XAXIS;
	}
}

void FlybyView::activate() {
	newFixedCamPos(m_ActiveObject.get());
}


void FlybyView::update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt) {
	lp = m_ActiveObject->getGlobalPosition();
	ep = m_FixedCameraPosition;
	if ((lp - ep).length() > 1000.0)
		 newFixedCamPos(m_ActiveObject.get());
	up = simdata::Vector3::ZAXIS;
}

void FlybyView::recalculate(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt) {
	VirtualScene* scene	= CSPSim::theSim->getScene();
	const simdata::Ref<TerrainObject> terrain =	scene->getTerrain();
	const float SAFETY	= 2.0f;
	TerrainObject::IntersectionHint	camera_hint	= 0;
	float h	= SAFETY + terrain->getGroundElevation(ep.x(),ep.y(),camera_hint);
	float d = ep.z() - h;
	if (d<0)
		ep.z() -= d;
}

void SatelliteView::activate() {
	m_CameraKinematics->setTheta(0.0);
	m_CameraKinematics->setPhi(simdata::PI_2);
	m_CameraKinematics->setDistance(500.0);
}
	
void SatelliteView::update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt) {
	updateWorld(ep,lp,up);
}


PadlockView::PadlockView(size_t vm):
	View(vm), 
	m_Padlock(m_ActiveObject) {
	m_Internal = true;
}

void PadlockView::activate() {
	m_CameraKinematics->reset();
	VirtualBattlefield* battlefield = CSPSim::theSim->getBattlefield();
	m_Padlock = battlefield->getNextUnit(m_ActiveObject, -1, -1, -1);
	if (m_Padlock == m_ActiveObject) {
		m_Padlock = battlefield->getNextUnit(m_Padlock, -1, -1, -1);
	}
	if (m_Padlock != m_ActiveObject) {
		m_NeckTheta = m_CameraKinematics->getPhi() - simdata::PI_2;
		m_NeckPhi = m_CameraKinematics->getTheta();
	}
}

void PadlockView::constrain(simdata::Vector3& ep,simdata::Vector3&	lp,simdata::Vector3& up,double dt) {
	if (m_Padlock.valid()) {
		ep = m_ActiveObject->getGlobalPosition();
		m_Attitude = m_ActiveObject->getAttitude();
		ep += m_ActiveObject->getViewPoint();
		simdata::Vector3 dir = (m_Padlock->getGlobalPosition() - ep).normalized();
		dir	= m_Attitude.invrotate(dir);
		float phi =	atan2(-dir.x(),	dir.y());
		float theta	= acos(dir.z());
		float phi_max =	2.6	- std::max(0.0,	1.57 - 2.0*theta);
		if (phi	> phi_max) {
			phi	= phi_max; 
			m_NeckLimit	= true;
		} else 
			if (phi	< -phi_max)	{
				phi	= -phi_max;
				m_NeckLimit	= true;
			} else {
				m_NeckLimit	= false;
			}
			float motion = std::min(3.0*dt,	0.3);
			phi	= m_NeckPhi	* (1.0-motion) + phi * motion;
			m_psi =	phi	* std::max(0.0,	std::min(1.0, 2.0*theta));
			m_NeckTheta	= theta;
			m_NeckPhi =	phi;
	}
}

void PadlockView::update(simdata::Vector3& ep,simdata::Vector3&	lp,simdata::Vector3& up,double dt) {
	if (m_Padlock.valid()) {
		constrain(ep,lp,up,dt);
		simdata::Vector3 dir(-sin(m_NeckTheta)*sin(m_NeckPhi), sin(m_NeckTheta)*cos(m_NeckPhi), cos(m_NeckTheta));
		simdata::Vector3 d(sin(m_psi), -cos(m_psi),	0.0);
		up.set(d.x()*cos(m_NeckTheta), d.y()*cos(m_NeckTheta), sin(m_NeckTheta));
		up = m_Attitude.rotate(up);
		float offset = std::max(0.0, std::min(0.4, 0.3*(abs(m_psi) - 1.57)));
		if (m_psi	> 0.0) offset =	-offset;
		ep += m_Attitude.rotate(offset * simdata::Vector3::XAXIS);
		dir	= m_Attitude.rotate(dir);
		lp = ep	+ 100.0	* dir;
	}
}


View* ViewFactory::createView_1() const {
	return new InternalView(1);
}
	
View* ViewFactory::createView_2() const {
	return new ExternalViewBody(2);
}
	
View* ViewFactory::createView_3() const {
	return new ExternalViewWorld(3);
}

View* ViewFactory::createView_4() const {
	return new InternalViewHist(4);
}
	
View* ViewFactory::createView_7() const {
	return new SatelliteView(7);
}
	
View* ViewFactory::createView_8() const {
	return new FlybyView(8);
}

View* ViewFactory::createView_9() const {
	return new PadlockView(9);
}

void ViewFactory::attachAllView(CameraAgent* ca) const {
	ca->attach(1,createView_1());
	ca->attach(2,createView_2());
	ca->attach(3,createView_3());
	ca->attach(4,createView_4());
	ca->attach(7,createView_7());
	ca->attach(8,createView_8());
	ca->attach(9,createView_9());
}
