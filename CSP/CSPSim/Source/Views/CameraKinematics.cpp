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
 * @file CameraKinematics.cpp
 *
 **/

#include <SimData/Math.h>

#include "Views/CameraKinematics.h"
#include "CSPSim.h"
#include "DynamicObject.h"
#include "VirtualScene.h"
#include "Views/CameraCommand.h"


void CameraKinematics::scale(double dt) {
	float scale_factor = 1.0 + m_ZoomRate * dt;
	if ((m_DistanceToObject > m_MinimumDistance && scale_factor < 1.0) ||
		(m_DistanceToObject < m_AbsoluteMaximumDistance && scale_factor > 1.0) ) {
		m_DistanceToObject *= scale_factor;
	}
	m_DistanceToObject = simdata::clampTo<double>(m_DistanceToObject, m_MinimumDistance, m_AbsoluteMaximumDistance);
}
	
void CameraKinematics::update(double dt) {
	rotateTheta(dt);
	rotatePhi(dt);
	scale(dt);
}

float CameraKinematics::smooth(double value, float min_value, float max_value) const {
	float epsilon = 0.1f * fabs(max_value - min_value);
	float damping = std::min(value - min_value, max_value - value)/epsilon;
	if (damping > 0.0 && damping < 1.0) {
		return damping;
	} else {
		return 1.0;
	}
}

CameraKinematics::CameraKinematics():
// XXX: serialize those parameters
m_BaseRate(simdata::toRadians(30.0)),
	m_DisplacementCoefficient(0.001),
	m_MinimumDistanceOffset(10.0),
	m_AbsoluteMaximumDistance(80000.0) {
	reset();
}

void CameraKinematics::clampPhi(double& phi, float min_phi, float max_phi, bool smooth_on) {
	if (smooth_on && m_PanRatePhi != 0.0) {
		m_PanRatePhi = simdata::sign(m_PanRatePhi)*smooth(phi, min_phi, max_phi)*m_BaseRate;
	}
	phi = simdata::clampTo<double>(phi, min_phi, max_phi);
}

void CameraKinematics::clampTheta(double& theta, float min_theta, float max_theta, bool smooth_on) {
	if (smooth_on && m_PanRateTheta != 0.0) {
		m_PanRateTheta = simdata::sign(m_PanRateTheta)*smooth(theta, min_theta, max_theta)*m_BaseRate;
	}
	theta = simdata::clampTo<double>(theta, min_theta, max_theta);
}

void CameraKinematics::reset() {
	m_Phi = 0.0;
	m_Theta = 0.0;
	m_PanRatePhi = 0.0;
	m_PanRateTheta = 0.0;
	m_ZoomRate = 0.0;
	resetDistance();
}
	
void CameraKinematics::resetDistance() {
	const simdata::Ref<DynamicObject> active_object = CSPSim::theSim->getActiveObject();
	if (active_object.valid()) {
		m_MinimumDistance = 1.0 + active_object->getModel()->getBoundingSphereRadius() + CSPSim::theSim->getScene()->getNearPlane();
	} else {
		m_MinimumDistance = m_MinimumDistanceOffset;
	}
	m_DistanceToObject = m_MinimumDistance + m_MinimumDistanceOffset;
}

void CameraKinematics::panLeft() {
	m_PanRateTheta = m_BaseRate;
}

void CameraKinematics::panRight() {
	m_PanRateTheta = -m_BaseRate;
}

void CameraKinematics::panLeftRightStop() {
	m_PanRateTheta = 0.0;
}

void CameraKinematics::panUp() {
	m_PanRatePhi = m_BaseRate;
}

void CameraKinematics::panDown() {
	m_PanRatePhi = -m_BaseRate;
}

void CameraKinematics::panUpDownStop() {
	m_PanRatePhi = 0.0;
}

void CameraKinematics::zoomIn() {
	m_ZoomRate = -m_BaseRate;
}

void CameraKinematics::zoomOut() {
	m_ZoomRate = m_BaseRate;
}

void CameraKinematics::zoomStop() {
	m_ZoomRate = 0.0;
}

void CameraKinematics::zoomStepIn() {
	m_ZoomRate = -m_BaseRate;
	scale(100*m_DisplacementCoefficient);
	m_ZoomRate = 0.0;
}

void CameraKinematics::zoomStepOut() {
	m_ZoomRate = m_BaseRate;
	scale(100*m_DisplacementCoefficient);
	m_ZoomRate = 0.0;
}

void CameraKinematics::displacement(int x, int y, int dx, int dy) {
	m_PanRatePhi = 0.0;
	m_PanRateTheta = 0.0;
	m_Theta -= dx * m_DisplacementCoefficient;
	m_Phi -= dy * m_DisplacementCoefficient;
}

void CameraKinematics::accept(CameraCommand* cc) {
	if (cc) {
		cc->execute(this);
	}
}

