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
 * @file CameraAgent.cpp
 *
 **/

#include <SimData/Math.h>

#include "Views/CameraAgent.h"
#include "Views/CameraCommand.h"

#include "Animation.h"
#include "CSPSim.h"
#include "DynamicObject.h"
#include "ObjectModel.h"
#include "VirtualScene.h"



CameraAgent::CameraAgent(const ViewFactory& vf):
	m_ViewMode(0) {
	vf.attachAllView(this);
	notifyCameraKinematicsToViews();
}

void CameraAgent::attach(size_t mode, View* vm){
	m_ViewList[mode] = vm;
}

CameraAgent::~CameraAgent() {
	deleteViews();
}

void CameraAgent::validate(double dt) {
	VirtualScene* scene = CSPSim::theSim->getScene();
	const simdata::Ref<TerrainObject> terrain = scene->getTerrain();
	TerrainObject::IntersectionHint camera_hint = 0;
	double const SAFETY = 3.0;
	float h = SAFETY + terrain->getGroundElevation(m_EyePoint.x(), m_EyePoint.y(), camera_hint);
	// if the eyepoint is near the ground, check more carefully that the terrain isn't
	// clipped by the near-clipping plane.
	if (m_EyePoint.z() < h) {
		double alpha_2 = simdata::toRadians(0.5*scene->getViewAngle());
		double near_dist = scene->getNearPlane();
		double aspect = scene->getAspect();
		simdata::Vector3 eye_look = m_LookPoint - m_EyePoint;
		simdata::Vector3 eye_look_unit = eye_look.normalized();
		simdata::Vector3 up_vec_unit = m_UpVector.normalized();

		double tan_alpha_2 = tan(alpha_2);
		simdata::Vector3 right_unit = eye_look_unit^up_vec_unit;
		simdata::Vector3 min_edge;
		double min_elev = 1.0;
		// iterate over the corners of the rectangle defined by the intersection
		// of the near clipping plane and the view frustum; find the lowest
		// corner.
		for (double i = -1.0; i <= 1.0; i += 2.0)
			for (double j = -1.0;j <= 1.0; j += 2.0) {
				simdata::Vector3 edge_vector = near_dist * (eye_look_unit
					+ tan_alpha_2 * ( i * up_vec_unit + j * aspect * right_unit));
				simdata::Vector3 edge = m_EyePoint + edge_vector;
				double edge_elev = edge.z()-terrain->getGroundElevation(edge.x(), edge.y(), camera_hint);
				if (min_elev > edge_elev) {
					min_elev = edge_elev;
					min_edge = edge;
				}
			}
		// if the lowest corner is near or below the terrain, the terrain may be
		// clipped (revealing the void underneath); rotate the view up to prevent
		// this from happening.
		if (min_elev < 0.1) {
			simdata::Vector3 rotvec = m_LookPoint - min_edge;
			// project back onto the plane defined by L-E and Up
			rotvec -= dot(rotvec, right_unit) * right_unit;
			double R = rotvec.normalize();
			// distance to raise the lowest clip point (to 10cm above the terrain).
			double dh = 0.1 - min_elev;
			// angle from the lookpoint to the lowest clip point, relative to x-y plane.
			double phi = asin(-rotvec.z());
			// angle to rotate the view in order to bring the lowest clip point up to
			// the terrain height (+10cm).
			double alpha = std::max(0.0, acos(rotvec.z() - dh/R) - simdata::PI_2 - phi);
			if (alpha > 0.0) {
				phi = m_CameraKinematics.getPhi();
				// first bring phi into the range -pi..pi
				if (dot(rotvec ^ simdata::Vector3::ZAXIS, right_unit) > 0) {
					m_CameraKinematics.setPhi(phi + alpha);
				} else {
					m_CameraKinematics.setPhi(phi - alpha);
				}
				m_CameraKinematics.panUpDownStop();
				m_ViewList[m_ViewMode]->recalculate(m_EyePoint, m_LookPoint, m_UpVector, dt);
			}
		}
	}
}

void CameraAgent::setViewMode(size_t vm) {
	ViewList::iterator view_it = m_ViewList.find(vm);
	if (view_it != m_ViewList.end()) {
		if (m_ViewMode == vm) {
			CSP_LOG(APP, INFO, "reactivate view");
			view_it->second->reactivate();
		} else {
			CSP_LOG(APP, INFO, "selected new view");
			view_it->second->activate();
			m_ViewMode = vm;
		}
	}
}

void CameraAgent::setCameraCommand(CameraCommand *cc) {
	m_CameraKinematics.accept(cc);
}

void CameraAgent::updateCamera(double dt) {
	ViewList::iterator view = m_ViewList.find(m_ViewMode);
	if (view != m_ViewList.end()) {
		m_CameraKinematics.update(dt);
		view->second->update(m_EyePoint, m_LookPoint, m_UpVector, dt);
		if (!view->second->isInternal()) {
			validate(dt);
		}
		view->second->cull();
	}
}

void CameraAgent::deleteViews() {
	std::for_each(m_ViewList.begin(), m_ViewList.end(), DestroyView());
}

void CameraAgent::setObject(simdata::Ref<DynamicObject> object) {
	std::for_each(m_ViewList.begin(), m_ViewList.end(), Accept<simdata::Ref<DynamicObject> >(object));
	m_CameraKinematics.reset();
}

void CameraAgent::notifyCameraKinematicsToViews() {
	std::for_each(m_ViewList.begin(), m_ViewList.end(), Accept<CameraKinematics*>(&m_CameraKinematics));
}
