// Combat Simulator	Project	- FlightSim	Demo
// Copyright (C) 2004 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This	program	is free	software; you can redistribute it and/or
// modify it under the terms of	the	GNU	General	Public License
// as published	by the Free	Software Foundation; either	version	2
// of the License, or (at your option) any later version.
// 
// This	program	is distributed in the hope that	it will	be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A	PARTICULAR PURPOSE.	 See the
// GNU General Public License for more details.
// 
// You should have received	a copy of the GNU General Public License
// along with this program;	if not,	write to the Free Software
// Foundation, Inc., 59	Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file CameraAgent.cpp
 *
 **/

#include <SimData/Math.h>

#include "Views/CameraAgent.h"
#include "CSPSim.h"
#include "DynamicObject.h"
#include "VirtualScene.h"
#include "Views/CameraCommand.h"

CameraAgent::CameraAgent(const ViewFactory& vf):
	m_ViewMode(0) {
	vf.attachAllView(this);
	notifyCameraKinematicsToViews();
}

void CameraAgent::attach(size_t mode,View* vm){
	m_ViewList[mode] = vm;
}

CameraAgent::~CameraAgent() {
	deleteViews();
}

void CameraAgent::validate(double dt)	{
	VirtualScene* scene	= CSPSim::theSim->getScene();
	const simdata::Ref<TerrainObject> terrain =	scene->getTerrain();
	TerrainObject::IntersectionHint	camera_hint	= 0;
	double const SAFETY	= 2.0;
	float h	= SAFETY + terrain->getGroundElevation(m_EyePoint.x(),m_EyePoint.y(),camera_hint);
	if (m_EyePoint.z() < h) {
		double alpha_2 = simdata::toRadians(0.5*scene->getViewAngle());
		double near_dist = scene->getNearPlane();
		double aspect =	scene->getAspect();
		simdata::Vector3 eye_look =	m_LookPoint	- m_EyePoint;
		simdata::Vector3 eye_look_unit = eye_look.normalized();
		simdata::Vector3 up_vec_unit = m_UpVector.normalized();

		double tan_alpha_2 = tan(alpha_2);
		simdata::Vector3 right_unit	= eye_look_unit^up_vec_unit;
		double min_elev	= 0.0;
		// iterate on the pyramide edges
		for	(double	i =	-1.0; i	<= 1.0;	i += 2.0)
			for	(double	j =	-1.0;j <= 1.0; j +=	2.0) {
				simdata::Vector3 edge_vector = near_dist * (eye_look_unit 
					+ tan_alpha_2 *	( i	* up_vec_unit +	j *	aspect * right_unit));
				simdata::Vector3 edge =	m_EyePoint + edge_vector;
				double edge_elev = edge.z()-terrain->getGroundElevation(edge.x(),edge.y(),camera_hint);
				if (min_elev > edge_elev)
					min_elev = edge_elev;
			}
		double dh =	abs(h - m_LookPoint.z() - min_elev);
		double angle_x = std::max(simdata::toRadians(1.0),asin(dh/m_CameraKinematics.getDistance()));
		if (abs(m_CameraKinematics.getPhi()) < simdata::PI_2) 
			m_CameraKinematics.setPhi(angle_x);
		else
			m_CameraKinematics.setPhi(simdata::PI-angle_x);
		m_CameraKinematics.panUpDownStop();
		m_ViewList[m_ViewMode]->recalculate(m_EyePoint,m_LookPoint,m_UpVector,dt);
	}
}

void CameraAgent::set(size_t vm,CameraCommand* cc) {
	if (m_ViewMode != vm) {
		ViewList::iterator view_it = m_ViewList.find(vm);
		if (view_it != m_ViewList.end()) {
			view_it->second->activate();
			m_ViewMode = vm;
		}
	}
	m_CameraKinematics.accept(cc);
}

void CameraAgent::updateCamera(double dt)	{
	ViewList::iterator view = m_ViewList.find(m_ViewMode);
	if (view != m_ViewList.end()) {
		m_CameraKinematics.update(dt);
		view->second->update(m_EyePoint,m_LookPoint,m_UpVector,dt);
		if (!view->second->isInternal()) 
			validate(dt);
		view->second->cull();
	}
}

struct DestroyView {
	void operator()(std::pair<const size_t,View*>& vm) {
		delete vm.second;
	}
};

void CameraAgent::deleteViews() {
	std::for_each(m_ViewList.begin(),m_ViewList.end(),DestroyView());
}

class AcceptObject {
	simdata::Ref<DynamicObject> m_ActiveObject;
public:
	AcceptObject(const simdata::Ref<DynamicObject> object):m_ActiveObject(object){}
	void operator()(std::pair<const size_t,View*>& vm) {
		vm.second->accept(m_ActiveObject);
	}
};

void CameraAgent::setObject(simdata::Ref<DynamicObject> object) {
	std::for_each(m_ViewList.begin(),m_ViewList.end(),AcceptObject(object));
	m_CameraKinematics.reset();
}

class AcceptCameraKinematics {
	CameraKinematics* m_CameraKinematics;
public:
	AcceptCameraKinematics(CameraKinematics* ck):m_CameraKinematics(ck) {}
	void operator()(std::pair<const size_t,View*>& vm) {
		vm.second->accept(m_CameraKinematics);
	}
};

void CameraAgent::notifyCameraKinematicsToViews() {
	std::for_each(m_ViewList.begin(),m_ViewList.end(),AcceptCameraKinematics(&m_CameraKinematics));
}
