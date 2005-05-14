// Combat Simulator Project - FlightSim Demo
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
 * @file View.cpp
 *
 **/


#include "Views/View.h"
#include "Views/CameraAgent.h"
#include "Views/CameraKinematics.h"
#include "Views/InternalView.h"
#include "Views/ExternalViews.h"

#include "CSPSim.h"
#include "DynamicObject.h"
#include "VirtualScene.h"

#include <SimCore/Battlefield/LocalBattlefield.h>
#include <SimData/Quat.h>

#include <cmath>
#include <queue>

namespace {
struct ContactSorter {
	typedef simdata::Ref<SimObject> Unit;
	Unit unit;
	double priority;
	double distance;
	inline bool operator<(ContactSorter const &other) const { return priority < other.priority; }
	ContactSorter(Unit const &unit_, double priority_, double distance_): unit(unit_), priority(priority_), distance(distance_) {}
};
}

void View::findContacts(simdata::Vector3 const &dir, double cutoff_angle, std::vector<Contact> &contacts) const {
	contacts.clear();
	LocalBattlefield* battlefield = CSPSim::theSim->getBattlefield();
	if (battlefield) {
		std::vector<SimObject::ObjectId> const &bf_contacts = m_ActiveObject->getContacts();
		if (!bf_contacts.empty()) {
			const simdata::Vector3 dir_unit = dir.normalized();
			const simdata::Vector3 pos = m_ActiveObject->getGlobalPosition();
			const double cos_cutoff = cos(cutoff_angle);
			std::priority_queue<ContactSorter> contact_queue;
			for (unsigned i = 0; i < bf_contacts.size(); ++i) {
				Battlefield::Unit unit = battlefield->getUnitById(bf_contacts[i]);
				if (!unit) continue;
				simdata::Vector3 relpos = unit->getGlobalPosition() - pos;
				double distance = relpos.normalize();
				double priority = relpos * dir_unit;
				if (priority > cos_cutoff) {
					contact_queue.push(ContactSorter(unit, priority, distance));
				}
			}
			contacts.reserve(contact_queue.size());
			while (!contact_queue.empty()) {
				contacts.push_back(contact_queue.top().unit);
				contact_queue.pop();
			}
		}
	}
}


void View::setActiveObject(const simdata::Ref<DynamicObject> object) {
	if (m_ActiveObject.valid()) {
		m_ActiveObject->internalView(false);
	}
	m_ActiveObject = object;
	if (m_ActiveObject.valid()) {
		m_ActiveObject->internalView(isInternal());
	}
}

View::~View() { }

void View::update(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt) {
	assert(m_CameraKinematics.valid());
	m_CameraKinematics->update(dt);
	updateView(ep, lp, up, dt);
}

void View::updateBody(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up) {
	simdata::Vector3 object_up = m_ActiveObject->getUpDirection();
	simdata::Vector3 object_dir = m_ActiveObject->getDirection();
	simdata::Quat q = simdata::Quat(-m_CameraKinematics->getTheta(), object_up, -m_CameraKinematics->getPhi(),
                                    object_dir^object_up, 0.0, object_dir);
	simdata::Vector3 object_pos = m_ActiveObject->getGlobalPosition();
	ep = object_pos + m_CameraKinematics->getDistance() * q.rotate(-object_dir);
	lp = object_pos;
	up = q.rotate(object_up);
}

void View::updateWorld(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up) {
	simdata::Quat q = simdata::Quat(-m_CameraKinematics->getTheta(), simdata::Vector3::ZAXIS, -m_CameraKinematics->getPhi(),
                                    simdata::Vector3::XAXIS, 0.0, simdata::Vector3::YAXIS);
	simdata::Vector3 object_pos = m_ActiveObject->getGlobalPosition();
	ep = object_pos + m_CameraKinematics->getDistance() * q.rotate(-simdata::Vector3::YAXIS);
	lp = object_pos;
	up = q.rotate(simdata::Vector3::ZAXIS);
}

View::View(size_t vm, bool internal):
	m_Internal(internal),
	m_ViewMode(vm),
	m_CameraKinematics(new CameraKinematics) {
}

void View::cull() {
	VirtualScene* scene = CSPSim::theSim->getScene();
	if (scene && m_ActiveObject.valid()) {
		bool isNear = m_ActiveObject->isNearField();
		if (isNear && !isInternal()) {
			scene->setNearObject(m_ActiveObject, false);
		} else
		if (!isNear && isInternal()) {
			scene->setNearObject(m_ActiveObject, true);
		}
	}
}

void View::activate() {
	if (m_ActiveObject.valid()) {
		m_ActiveObject->internalView(isInternal());
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
	return new FixedFlybyView(4);
}

View* ViewFactory::createView_5() const {
	return new FixedFlybyView(5);
}
	
View* ViewFactory::createView_7() const {
	return new SatelliteView(7);
}
	
View* ViewFactory::createView_8() const {
	return new FlybyView(8);
}

View* ViewFactory::createView_9() const {
	return new FlybyView(9);
}

void ViewFactory::attachAllView(CameraAgent* ca) const {
	ca->attach(1, createView_1());
	ca->attach(2, createView_2());
	ca->attach(3, createView_3());
	ca->attach(4, createView_4());
	ca->attach(5, createView_5());
	ca->attach(7, createView_7());
	ca->attach(8, createView_8());
	ca->attach(9, createView_9());
}
