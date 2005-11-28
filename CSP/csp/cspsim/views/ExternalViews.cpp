// Combat Simulator Project
// Copyright (C) 2004-2005 The Combat Simulator Project
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
 * @file ExternalViews.cpp
 *
 **/


#include <csp/cspsim/views/ExternalViews.h>
#include <csp/cspsim/views/CameraAgent.h>
#include <csp/cspsim/views/CameraKinematics.h>

#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/DynamicObject.h>
// FIXME: why ObjetModel class definition is needed here (vc++ error)
#include <csp/cspsim/ObjectModel.h>

#include <csp/csplib/util/Noise.h>

#include <cmath>

CSP_NAMESPACE

void ExternalViewBody::activate() {
	View::activate();
}

void ExternalViewBody::updateView(Vector3& ep, Vector3& lp, Vector3& up, double /*dt*/) {
	updateBody(ep, lp, up);
}

void ExternalViewWorld::activate() {
	View::activate();
}

void ExternalViewWorld::updateView(Vector3& ep, Vector3& lp, Vector3& up, double /*dt*/) {
	/*  Very simplistic camera jitter (disabled).  Ideally should take wind speed/direction
	 *  and turbulence into account.
	static std::vector<float> turbulenceX;
	static std::vector<float> turbulenceY;
	static std::vector<float> turbulenceZ;
	static int i = 0, j = 512;
	if (turbulenceX.empty()) {
		Perlin1D noise(0.8, 8, Perlin1D::LINEAR);
		turbulenceX = noise.generate(1024, true, 4.0, 2.0);
		turbulenceY = noise.generate(1024, true, 4.0, 2.0);
		turbulenceZ = noise.generate(1024, true, 4.0, 1.0);
		for (int k = 0; k < 1000; k++) std::cout << turbulenceX[k] << " ";
	}
	if (++i >= 1024) i = 0;
	if (++j >= 1024) j = 0;
	*/
	updateWorld(ep, lp, up);
	/*
	ep += Vector3(turbulenceX[i], turbulenceY[i], turbulenceZ[i]) * 0.03;
	lp += Vector3(turbulenceX[j], turbulenceY[j], turbulenceZ[j]) * 0.015 * m_CameraKinematics->getDistance() ;
	*/
}

void FixedFlybyView::newFixedCamPos(SimObject* target) {
	if (m_Initialized) return;
	m_Initialized = true;
	Vector3 object_pos = target->getGlobalPosition();
	DynamicObject* dynamic = dynamic_cast<DynamicObject*>(target);
	if (dynamic) {
		Vector3 up = dynamic->getUpDirection();
		Vector3 object_dir = dynamic->getDirection();
		//double speed_level = dynamic->getSpeed()/50.0;
		m_FixedCameraPosition =  object_pos - 20.0 * object_dir;
	} else {
		m_FixedCameraPosition = object_pos + 100.0 * Vector3::ZAXIS + 100.0 * Vector3::XAXIS;
	}
}

void FixedFlybyView::activate() {
	m_Initialized = false;
	FlybyView::activate();
}

void FlybyView::newFixedCamPos(SimObject* target) {
	Vector3 object_pos = target->getGlobalPosition();
	DynamicObject* dynamic = dynamic_cast<DynamicObject*>(target);
	if (dynamic) {
		Vector3 up = dynamic->getUpDirection();
		Vector3 object_dir = dynamic->getDirection();
		//double speed_level = dynamic->getSpeed()/50.0;
		m_FixedCameraPosition =  object_pos + 900.0* object_dir + ( 12.0 - (rand() % 5) ) * (object_dir^up) + ( 6.0 + (rand () % 3) ) * up;
	} else {
		m_FixedCameraPosition = object_pos + 100.0 * Vector3::ZAXIS + 100.0 * Vector3::XAXIS;
	}
}

void FlybyView::activate() {
	View::activate();
	newFixedCamPos(m_ActiveObject.get());
}


void FlybyView::updateView(Vector3& ep, Vector3& lp, Vector3& up, double /*dt*/) {
	lp = m_ActiveObject->getGlobalPosition();
	ep = m_FixedCameraPosition;
	if ((lp - ep).length() > 1000.0) {
		newFixedCamPos(m_ActiveObject.get());
	}
	up = Vector3::ZAXIS;
}

void FlybyView::recalculate(Vector3& ep, Vector3& /*lp*/, Vector3& /*up*/, double /*dt*/) {
	TerrainObject* terrain = CSPSim::theSim->getTerrain();
	if (terrain) {
		const double SAFETY = 2.0f;
		TerrainObject::IntersectionHint camera_hint = 0;
		double h = SAFETY + terrain->getGroundElevation(ep.x(), ep.y(), camera_hint);
		double d = ep.z() - h;
		if (d<0) ep.z() -= d;
	}
}

void SatelliteView::activate() {
	View::activate();
	m_CameraKinematics->setTheta(0.0);
	m_CameraKinematics->setPhi(PI_2);
	m_CameraKinematics->setDistance(500.0);
}

void SatelliteView::updateView(Vector3& ep, Vector3& lp, Vector3& up, double /*dt*/) {
	updateWorld(ep, lp, up);
}

CSP_NAMESPACE_END

