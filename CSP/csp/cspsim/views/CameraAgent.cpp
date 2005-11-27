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
 * @file CameraAgent.cpp
 *
 **/

#include <csp/csplib/util/Math.h>

#include "Views/CameraAgent.h"
#include "Views/CameraCommand.h"

#include "CSPSim.h"
#include "DynamicObject.h"
#include "ObjectModel.h"
#include "VirtualScene.h"


CameraAgent::CameraAgent(const ViewFactory& vf, ViewMode default_view):
	m_EyePoint(simdata::Vector3::ZERO), 
	m_LookPoint(simdata::Vector3::XAXIS),
	m_UpVector(simdata::Vector3::ZAXIS),
	m_ViewMode(10000) {
	vf.attachAllView(this);
	setViewMode(default_view);
}

void CameraAgent::attach(ViewMode mode, View* view){
	assert(view);
	m_ViewList[mode] = view;
}

CameraAgent::~CameraAgent() { }

void CameraAgent::validate(double dt) {
	if (!m_ActiveView) return;
	VirtualScene* scene = CSPSim::theSim->getScene();
	const simdata::Ref<TerrainObject> terrain = scene->getTerrain();
	TerrainObject::IntersectionHint camera_hint = 0;
	double const SAFETY = 3.0;
	double h = SAFETY + terrain->getGroundElevation(m_EyePoint.x(), m_EyePoint.y(), camera_hint);
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
				CameraKinematics *kinematics = m_ActiveView->kinematics();
				phi = kinematics->getPhi();
				// first bring phi into the range -pi..pi
				if (dot(rotvec ^ simdata::Vector3::ZAXIS, right_unit) > 0) {
					kinematics->setPhi(phi + alpha);
				} else {
					kinematics->setPhi(phi - alpha);
				}
				kinematics->panUpDownStop();
				m_ActiveView->recalculate(m_EyePoint, m_LookPoint, m_UpVector, dt);
			}
		}
	}
}

void CameraAgent::setViewMode(ViewMode vm) {
	if (vm == m_ViewMode) {
		CSP_LOG(APP, INFO, "reactivate view");
		assert(m_ActiveView.valid());
		m_ActiveView->reactivate();
	} else {
		ViewList::iterator iter = m_ViewList.find(vm);
		if (iter != m_ViewList.end()) {
			assert(iter->second.valid());
			m_ViewMode = vm;
			m_ActiveView = iter->second;
			m_ActiveView->setActiveObject(m_ActiveObject);
			m_ActiveView->activate();
		}
	}
}

void CameraAgent::select() {
	if (m_ActiveView.valid()) m_ActiveView->select();
}

void CameraAgent::reselect() {
	if (m_ActiveView.valid()) m_ActiveView->reselect();
}

void CameraAgent::deselect() {
	if (m_ActiveView.valid()) m_ActiveView->deselect();
}

void CameraAgent::setCameraCommand(CameraCommand *cc) {
	if (m_ActiveView.valid()) {
		m_ActiveView->kinematics()->accept(cc);
	}
}

void CameraAgent::updateCamera(double dt) {
	if (m_ActiveView.valid()) {
		m_ActiveView->update(m_EyePoint, m_LookPoint, m_UpVector, dt);
		if (!m_ActiveView->isInternal()) {
			validate(dt);
		}
		m_ActiveView->cull();
	}
}

void CameraAgent::setObject(simdata::Ref<DynamicObject> object) {
	m_ActiveObject = object;
	if (m_ActiveView.valid()) m_ActiveView->setActiveObject(object);
}

