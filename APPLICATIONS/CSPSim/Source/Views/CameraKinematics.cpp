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
	float scale_factor	= 1.0 +	m_ZoomRate * dt;
	if ((m_DistanceToObject	> m_MinimumDistance	&& scale_factor	< 1.0) || 
		(m_DistanceToObject < m_AbsoluteMaximumDistance && scale_factor > 1.0) ) {
			m_DistanceToObject *= scale_factor;
	}
	m_DistanceToObject = simdata::clampTo<double>(m_DistanceToObject,m_MinimumDistance,m_AbsoluteMaximumDistance);
}
	
void CameraKinematics::update(double dt) {
	rotateAboutZ(dt);
	rotateAboutX(dt);
	scale(dt);
}

float CameraKinematics::smooth(double value, float min_value,float max_value) const {
	float epsilon = 0.1 * abs(max_value - min_value);
	float damping = std::min(value - min_value, max_value - value)/epsilon;
	if (damping > 0.0 && damping < 1.0)
		return damping;
	else 
		return 1.0;
}

CameraKinematics::CameraKinematics():
// XXX: serialize those parameters
m_BaseRate(simdata::toRadians(30.0)),
	m_DisplacementCoefficient(0.001),
	m_MinimumDistanceOffset(10.0),
	m_AbsoluteMaximumDistance(2000.0) {
	reset();
}

void CameraKinematics::clampX(double& value,float min_value,float max_value, bool smooth_on) {
	if (smooth_on && m_PanRateX != 0.0) {
		m_PanRateX = simdata::sign(m_PanRateX)*smooth(value,min_value,max_value)*m_BaseRate;
	}
	value = simdata::clampTo<double>(value,min_value,max_value);
}

void CameraKinematics::clampZ(double& value,float min_value,float max_value, bool smooth_on) {
	if (smooth_on && m_PanRateZ != 0.0) {
		m_PanRateZ = simdata::sign(m_PanRateZ)*smooth(value,min_value,max_value)*m_BaseRate;
	}
	value = simdata::clampTo<double>(value,min_value,max_value);
}

void CameraKinematics::reset() {
	m_AngleRotX	= 0.0;
	m_AngleRotZ	= 0.0;
	m_PanRateX = 0.0;
	m_PanRateZ = 0.0;
	m_ZoomRate = 0.0;
	resetDistance();
}
	
void CameraKinematics::resetDistance() {
	const simdata::Ref<DynamicObject> active_object = CSPSim::theSim->getActiveObject();
	if (active_object.valid())
		m_MinimumDistance =	active_object->getModel()->getBoundingSphereRadius()+CSPSim::theSim->getScene()->getNearPlane();
	else
		m_MinimumDistance = m_MinimumDistanceOffset;
	m_DistanceToObject = m_MinimumDistance + m_MinimumDistanceOffset;
}
	
void CameraKinematics::panLeft()	{
	m_PanRateZ = m_BaseRate;
}
	
void CameraKinematics::panRight() {
	m_PanRateZ = -m_BaseRate;
}

void CameraKinematics::panLeftRightStop() {
	m_PanRateZ = 0.0;
}

void CameraKinematics::panUp() {
	m_PanRateX = m_BaseRate;
}

void CameraKinematics::panDown()	{
	m_PanRateX = -m_BaseRate;
}

void CameraKinematics::panUpDownStop() { 
	m_PanRateX = 0.0;
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

void CameraKinematics::displacement(int x, int y, int dx, int dy) {
	m_PanRateX = 0.0;
	m_PanRateZ = 0.0;
	m_AngleRotZ	-= dx *	m_DisplacementCoefficient;
	m_AngleRotX	-= dy *	m_DisplacementCoefficient;
}

void CameraKinematics::accept(CameraCommand* cm) {
	if (cm) {
		if (std::find(m_CameraCommandList.begin(),m_CameraCommandList.end(),cm) == m_CameraCommandList.end()) {
			m_CameraCommandList.push_back(cm);
			cm->setCameraKinematics(this);
		}
		cm->execute();
	}
}
