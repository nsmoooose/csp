// Combat Simulator Project
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

#include <csp/csplib/util/Math.h>

#include "Views/CameraKinematics.h"
#include "Views/CameraCommand.h"

#include "CSPSim.h"
#include "DynamicObject.h"
#include "ObjectModel.h"
#include "VirtualScene.h"


void CameraKinematics::scale(double dt) {
	double scale_factor = 1.0 + m_ZoomRate * dt;
	if ((m_DistanceToObject > m_MinimumDistance && scale_factor < 1.0) ||
		(m_DistanceToObject < m_AbsoluteMaximumDistance && scale_factor > 1.0) ) {
		m_DistanceToObject *= scale_factor;
	}
	m_DistanceToObject = simdata::clampTo<double>(m_DistanceToObject, m_MinimumDistance, m_AbsoluteMaximumDistance);
}

void CameraKinematics::update(double dt) {
	if (m_PanRateTheta != 0.0 || m_PanRatePhi != 0.0) {
		m_Accel = std::min(3.0, m_Accel + 1.0 * dt);
	}
	m_FOVScale = std::min(CSPSim::theSim->getScene()->getViewAngle() / 40.0, 1.0);
	rotateTheta(dt);
	rotatePhi(dt);
	scale(dt);
	m_Accel = 1.0 + (m_Accel - 1.0) *  std::max(0.0, 1.0 - dt);
}

double CameraKinematics::smooth(double value, double min_value, double max_value) const {
	double epsilon = 0.1f * fabs(max_value - min_value);
	double damping = std::min(value - min_value, max_value - value)/epsilon;
	if (damping > 0.0 && damping < 1.0) {
		return damping;
	} else {
		return 1.0;
	}
}

CameraKinematics::CameraKinematics():
// XXX: serialize these parameters
	m_BaseRate(simdata::toRadians(30.0)),
	m_DisplacementCoefficient(0.001),
	m_MinimumDistanceOffset(10.0),
	m_AbsoluteMaximumDistance(80000.0),
	m_FOVScale(1.0),
	m_Accel(1.0),
	m_ExternalPan(false) {
	reset();
}
void CameraKinematics::clampPhi(double min_phi, double max_phi, bool smooth_on) {
	if (smooth_on && m_PanRatePhi != 0.0) {
		m_PanRatePhi = simdata::sign(m_PanRatePhi) * smooth(m_Phi, min_phi, max_phi) * m_BaseRate;
	}
	m_Phi = simdata::clampTo<double>(m_Phi, min_phi, max_phi);
}

void CameraKinematics::clampTheta(double min_theta, double max_theta, bool smooth_on) {
	if (smooth_on && m_PanRateTheta != 0.0) {
		m_PanRateTheta = simdata::sign(m_PanRateTheta)*smooth(m_Theta, min_theta, max_theta)*m_BaseRate;
	}
	m_Theta = simdata::clampTo<double>(m_Theta, min_theta, max_theta);
}

void CameraKinematics::reset() {
	m_Phi = 0.0;
	m_Theta = 0.0;
	m_PanRatePhi = 0.0;
	m_PanRateTheta = 0.0;
	m_ZoomRate = 0.0;
	m_Accel = 1.0;
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
	m_ExternalPan = true;
}

void CameraKinematics::panRight() {
	m_PanRateTheta = -m_BaseRate;
	m_ExternalPan = true;
}

void CameraKinematics::panLeftRightStop() {
	m_PanRateTheta = 0.0;
}

void CameraKinematics::panUp() {
	m_PanRatePhi = m_BaseRate;
	m_ExternalPan = true;
}

void CameraKinematics::panDown() {
	m_PanRatePhi = -m_BaseRate;
	m_ExternalPan = true;
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

void CameraKinematics::displacement(int /*x*/, int /*y*/, int dx, int dy) {
	m_PanRatePhi = 0.0;
	m_PanRateTheta = 0.0;
	m_Accel = std::min(3.0, m_Accel + 0.1 * (dx * dx + dy * dy));
	m_Theta -= dx * m_Accel * m_DisplacementCoefficient * m_FOVScale;
	m_Phi -= dy * m_Accel * m_DisplacementCoefficient * m_FOVScale;
	m_ExternalPan = true;
}

void CameraKinematics::accept(CameraCommand* cc) {
	if (cc) cc->execute(this);
}

