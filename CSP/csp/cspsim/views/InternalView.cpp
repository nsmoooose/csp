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
 * @file InternalView.cpp
 *
 **/

#include <cmath>
#include <queue>

#include "Views/InternalView.h"
#include "Views/CameraAgent.h"
#include "Views/CameraKinematics.h"

#include "CSPSim.h"
#include "DynamicObject.h"
#include "ObjectModel.h"
#include "VirtualScene.h"

#include <SimCore/Battlefield/LocalBattlefield.h>
#include <SimData/Noise.h>
#include <csp/lib/data/Quat.h>


InternalView::InternalView(size_t vm):
	View(vm, true /*internal*/),
	m_Padlocked(false),
	m_EyePhi(0),
	m_EyeTheta(0),
	m_NeckPhi(0),
	m_NeckTheta(0),
	m_TorsoTheta(0),
	m_TorsoSideLean(0),
	m_FlipTime(0),
	m_EyeRate(simdata::toRadians(90.0)),
	m_NeckRate(simdata::toRadians(90.0)),
	m_TorsoRate(simdata::toRadians(50.0)),
	m_EyeTurnLimit(simdata::toRadians(15.0)),
	m_EyeTiltLimit(simdata::toRadians(15.0)),
	m_NeckTurnLimit(simdata::toRadians(80.0)),
	m_NeckTiltLimit(simdata::toRadians(75.0)),
	m_TorsoTurnLimit(simdata::toRadians(60.0)),
	m_NeckPivotOffset(0.0, -0.10, -0.15)
{ }

// TODO add static_cast helpers to dynamicobject for dynamicobject/featuregroup casts.

// TODO
//  - object bounding sphere diameter, distance, aspect
//  - background contrast (sky, ground, camoflage, motion)
//  - atmospheric conditions
//  - light level and target lights (strobe, spot, intensity)
//  - sun angle (sun blindness, glint)
//  - ownship occlusion
//  - terrain occlusion
bool InternalView::inVisualRange(Contact contact) const {
	if (contact->isStatic()) {
		return true; // FIXME
	}
	simdata::Ref<DynamicObject> object = static_cast<DynamicObject*>(contact.get());
	double size = 2.0 * object->getModel()->getBoundingSphereRadius();
	simdata::Vector3 contact_pos = object->getGlobalPosition();
	simdata::Vector3 ownship_pos = m_ActiveObject->getGlobalPosition();
	simdata::Vector3 dir = contact_pos - ownship_pos;
	double distance = dir.normalize();
	double aspect_factor = 1.0 - 0.5 * std::abs(object->getDirection() * dir);
	double angular_size = size / distance;
	if (angular_size * aspect_factor < simdata::PI * 0.001) {
		return false;
	}
	return true;
}

struct InternalView::SortById {
	bool operator()(Contact const &a, Contact const &b) const { return a->id() < b->id(); }
};

// TODO
// for tightly grouped objects near fov center select() should cycle through them.
// construct a list of candidates, find the next highest id from the currently
// padlocked one.
//
// also, need a hit test function to check los, angle, and distance to padlocked
// target.  of out of view, go to predictive mode for a few seconds.  if not
// reacquired, go to forward view.
//
void InternalView::select() {
	std::vector<Contact> contacts;
	simdata::Vector3 look = m_DirLocal.normalized();
	findContacts(look, simdata::toRadians(30.0), contacts);
	// for select() we set last_id=0 if not padlocked so that we will choose the
	// contact nearest the center of the fov.  for reselect() will want to prefer
	// m_Padlock if it is valid and anywhere in the fov.  (it might be desirable
	// for select() to start with m_Padlock if it is valid and in the selection
	// list, but this behavior is probably too confusing.)
	const SimObject::ObjectId last_id = (!m_Padlocked) ? 0 : m_Padlock->id();
	m_Padlock = 0;
	Contact nearest = 0;
	simdata::Vector3 origin = m_ActiveObject->getGlobalPosition();
	std::vector<Contact> selection;
	const double select_limit = cos(simdata::toRadians(5.0));
	for (unsigned i = 0; i < contacts.size(); ++i) {
		Contact &contact = contacts[i];
		if (inVisualRange(contact)) {
			if (!nearest) nearest = contact;
			simdata::Vector3 pos = contact->getGlobalPosition();
			simdata::Vector3 dir = (pos - origin).normalized();
			if ((look * dir) >= select_limit) {
				selection.push_back(contact);
			}
		}
	}
	// cycle through selection
	if (last_id > 0 && !selection.empty()) {
		Contact first = 0;
		Contact next = 0;
		SimObject::ObjectId first_id = 0;
		SimObject::ObjectId next_id = 0;
		for (unsigned i = 0; i < selection.size(); ++i) {
			const SimObject::ObjectId id = selection[i]->id();
			if (id < first_id || !first) {
				first_id = id;
				first = selection[i];
			}
			if (id > last_id && (id < next_id || !next)) {
				next_id = id;
				next = selection[i];
			}
		}
		m_Padlock = !next ? first : next;
	} else {
		m_Padlock = nearest;
	}
	setPadlocked();
}

void InternalView::reselect() {
	// TODO when padlock is lost, the view should pan to keep the expected position in the fov.
	// reselect() should then only need to consider if the padlock target is within the fov and
	// visible.
	if (m_Padlock.valid() && !m_Padlocked) {
		simdata::Vector3 expected_position = m_PadlockPosition + m_PadlockVelocity * m_PadlockLossTime;
		simdata::Vector3 expected_direction = (expected_position - m_ActiveObject->getGlobalPosition()).normalized();
		simdata::Vector3 actual_direction = (m_Padlock->getGlobalPosition() - m_ActiveObject->getGlobalPosition());
		actual_direction.normalize();
		if ((expected_direction * actual_direction > cos(simdata::toRadians(30.0))) && inVisualRange(m_Padlock)) {
			setPadlocked();
		} else {
			m_Padlock = 0;
		}
	}
}

void InternalView::deselect() {
	m_Padlock = 0;
	m_Padlocked = false;
}

void InternalView::activate() {
	View::activate();
	select();
}

void InternalView::setPadlocked() {
	assert(m_Padlock != m_ActiveObject);
	m_Padlocked = m_Padlock.valid();
	m_CameraKinematics->resetExternalPan();
}

bool InternalView::checkPadlocked() {
	if (!m_Padlocked) return false;
	m_Padlocked = !m_CameraKinematics->externalPan();
	if (!m_Padlocked) {
		CSP_LOG(APP, ERROR, "moved out of padlock");
	}
	return m_Padlocked;
}

void InternalView::moveViewpoint(simdata::Vector3 &ep, simdata::Vector3 &lp, simdata::Vector3 &up, double dt) {
	simdata::Quat attitude = m_ActiveObject->getAttitude();

	if (m_Padlock.valid() && m_Padlocked) {
		m_PadlockPosition = m_Padlock->getGlobalPosition();
		m_PadlockVelocity = m_Padlock->getVelocity();
		m_PadlockLossTime = 0.0;
		simdata::Vector3 dir = (m_PadlockPosition - ep).normalized();
		dir = attitude.invrotate(dir);
		double pad_theta = atan2(-dir.x(), dir.y());
		double pad_phi = std::max(asin(dir.z()), -30.0 / 90.0 * simdata::PI_2);
		m_CameraKinematics->setTheta(pad_theta);
		m_CameraKinematics->setPhi(pad_phi);
	}

	simdata::Vector3 accel_g = (m_ActiveObject->getAccelerationBody() / 9.8) + attitude.invrotate(simdata::Vector3::ZAXIS);
	// TODO accel should include a contribution from the angular acceleration at the pilot's position
	double G = accel_g.z();

	// TODO
	// + limit torso twist depending on G, possibly limit neck motion too
	// + limit head swing at 90 deg (hysteresis)
	// + adjust hud viewpoint
	// + don't padlock into cockpit -- and lean to the side if the target is below!
	// lose lock if out of view for too long -- could extrapolate linear velocity, reacquire if nearby
	// + don't padlock outside of fov
	// + padlock nearest to fov center, cycle outward
	// model neck motion by system, export for remote animation!
	
	double theta = m_CameraKinematics->getTheta();

	// Hysteresis around branch cut.
	if (std::abs(theta) > 0.8 * simdata::PI && theta * m_EyeTheta < -0.01) {
		m_FlipTime += dt;
		if (m_FlipTime < 3.0) {
			theta -= 2.0 * simdata::PI * simdata::sign(theta);
		}
	} else {
		m_FlipTime = 0.0;
	}

	double phi = m_CameraKinematics->getPhi();
	const double sign_theta = simdata::sign(theta);
	const double sign_phi = simdata::sign(phi);

	// Turn torso only if necessary
	{
		double excess = std::max(0.0, std::abs(theta) - m_EyeTurnLimit - m_NeckTurnLimit);
		double limit = m_TorsoRate * dt;
		double delta = simdata::clampTo(excess * sign_theta - m_TorsoTheta, -limit, limit);
		double turn_limit = m_TorsoTurnLimit * simdata::clampTo(3.0 - G, 0.0, 1.0);
		m_TorsoTheta = simdata::clampTo(m_TorsoTheta + delta, -turn_limit, turn_limit);
	}
	theta -= m_TorsoTheta;

	// Lean slightly when looking down and to the side in order to see around obstacles.
	{
		const double max_side_lean = 0.12; // meters
		double side_lean = simdata::clampTo((-0.24 - phi) * 10.0, 0.0, 1.0) * std::max(0.0, sin(std::abs(2.0 * theta)));
		side_lean = - sign_theta * std::max(0.0, side_lean - 2.0 * std::abs(m_TorsoTheta)) * max_side_lean;
		double limit = m_TorsoRate * dt;
		double delta = simdata::clampTo(side_lean - m_TorsoSideLean, -limit, limit);
		m_TorsoSideLean = simdata::clampTo(m_TorsoSideLean + delta, -max_side_lean, max_side_lean);
	}


	// Degrade neck turn rate above 3 G.
	const double g_rate_factor = simdata::clampTo(2.0 - 0.33 * G, 0.0, 1.0);

	// Turn neck only if necessary
	{
		double excess = std::max(0.0, std::abs(theta) - m_EyeTurnLimit);
		double limit = m_NeckRate * g_rate_factor * dt;
		double delta = simdata::clampTo(excess * sign_theta - m_NeckTheta, -limit, limit);
		m_NeckTheta = simdata::clampTo(m_NeckTheta + delta, -m_NeckTurnLimit, m_NeckTurnLimit);
	}

	// Tilt neck only if necessary
	{
		double excess = std::max(0.0, std::abs(phi) - m_EyeTiltLimit);
		double limit = m_NeckRate * g_rate_factor * dt;
		double delta = simdata::clampTo(excess * sign_phi - m_NeckPhi, -limit, limit);
		m_NeckPhi = simdata::clampTo(m_NeckPhi + delta, -m_NeckTiltLimit, m_NeckTiltLimit);
	}

	// Neck axis coupling
	if (m_NeckPhi > 0) {
		double limit = std::max(simdata::PI_2 - 2.0 * m_NeckPhi, 0.0);
		double excess = std::max(0.0, std::abs(m_NeckTheta) - limit);
		m_NeckTheta -= simdata::sign(m_NeckTheta) * excess * std::min(5*dt, 1.0);
		//m_NeckTheta = simdata::clampTo(m_NeckTheta, -limit, limit);
	}

	theta -= m_NeckTheta;
	// Turn eyes
	{
		double limit = m_EyeRate * dt;
		double delta = simdata::clampTo(theta - m_EyeTheta, -limit, limit);
		m_EyeTheta = simdata::clampTo(m_EyeTheta + delta, -m_EyeTurnLimit, m_EyeTurnLimit);
	}

	phi -= m_NeckPhi;
	// Tilt eyes
	{
		double limit = m_EyeRate * dt;
		double delta = simdata::clampTo(phi - m_EyePhi, -limit, limit);
		m_EyePhi = simdata::clampTo(m_EyePhi + delta, -m_EyeTiltLimit, m_EyeTiltLimit);
	}

	simdata::Vector3 eyes(-m_NeckPivotOffset);
	simdata::Matrix3 neck_rotation = simdata::Matrix3::rotate(m_NeckTheta + m_TorsoTheta, 0, 0, 1) * simdata::Matrix3::rotate(m_NeckPhi, 1, 0, 0);
	eyes = neck_rotation * eyes;
	const double lean_angle = (3.14 * 60.0 / 180.0);
	const double lean_distance = std::abs(0.2 * m_TorsoTheta);
	simdata::Vector3 lean_offset(lean_distance * cos(lean_angle) * simdata::sign(-m_TorsoTheta) + m_TorsoSideLean, lean_distance * sin(lean_angle), 0.0);
	const double total_phi = m_EyePhi + m_NeckPhi;
	const double total_theta = m_EyeTheta + m_NeckTheta + m_TorsoTheta;
	simdata::Vector3 dir(-cos(total_phi)*sin(total_theta), cos(total_phi)*cos(total_theta), sin(total_phi));

	double dir_f = std::min(1.0, 8.0 * dt);
	m_Dir = (1.0 - dir_f) * m_Dir + dir_f * dir;
	m_DirLocal = attitude.rotate(m_Dir);

	//m_CameraKinematics->setTheta(total_theta);
	//m_CameraKinematics->setPhi(total_phi);
	m_CameraKinematics->clampTheta(-simdata::PI, simdata::PI, false);
	m_CameraKinematics->clampPhi(-simdata::PI_2, simdata::PI_2, false);

	simdata::Vector3 bobbing(-0.010 * simdata::clampTo(accel_g.x(), -10.0, 10.0),
	                         -0.010 * simdata::clampTo(accel_g.y(), -10.0, 10.0),
	                         -0.005 * simdata::clampTo(accel_g.z(), -10.0, 10.0));
	double bob_f = std::min(1.0, 6.0 * dt);
	m_NeckBobbing = (1.0 - bob_f) * m_NeckBobbing + bob_f * bobbing;
	ep = m_ActiveObject->getNominalViewPointBody() + (lean_offset + eyes + m_NeckPivotOffset) + m_NeckBobbing;
	m_ActiveObject->setViewPointBody(ep);
	ep = attitude.rotate(ep) + m_ActiveObject->getGlobalPosition();
	lp = ep + m_DirLocal * 100.0;

	double up_f = std::min(1.0, 8.0 * dt);
	up = (1.0 - up_f) * m_Up + up_f * attitude.rotate(neck_rotation * simdata::Vector3(0.0, -sin(m_EyePhi), cos(m_EyePhi)));
	m_Up = up;
}


void InternalView::updateView(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt) {
	checkPadlocked();
	moveViewpoint(ep, lp, up, dt);
	if (!m_Padlocked) m_PadlockLossTime += dt;
}
