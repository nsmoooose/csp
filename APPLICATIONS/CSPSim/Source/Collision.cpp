// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file Collision.cpp
 *
 **/
#include <algorithm>

#include <SimData/Quaternion.h>
#include <SimData/Ref.h>

#include "Collision.h"

using std::min;
using std::max;

ContactData::ContactData(simdata::Vector3 const& pb, simdata::Vector3 const& vb, simdata::Vector3 const& avb, 
				  simdata::Vector3 const& nb, double deep):
	m_pb(pb),
	m_vb(vb),
	m_avb(avb),
	m_normalBody(nb),
	m_deep(deep) {
		m_vtdirb = simdata::Normalized(m_vb - simdata::Dot(m_vb,m_normalBody) * m_normalBody);
}

simdata::Vector3 ContactData::getTangentVelocityDir() const {
	return m_vtdirb;
}

void ContactData::setForce(simdata::Vector3 const &force) {
	static const double staticFriction = 0.5;
	static const double dynamicFriction = 0.7;

	double speed = m_vb.Length();
	double fnorm = fabs(simdata::Dot(force,m_normalBody));

	m_Force = force;

	if (speed > 2.0)
		m_Force -= (0.8 * speed + dynamicFriction * fnorm) * m_vtdirb;
		
	else
		m_Force -= (0.7 * speed + staticFriction * fnorm) * m_vtdirb;
	m_Moment = m_pb^m_Force;
}


HardContactData::HardContactData(simdata::Vector3 const& pb, simdata::Vector3 const& vb, simdata::Vector3 const& avb, 
				  simdata::Vector3 const& nb, double deep):
	ContactData(pb,vb,avb,nb,deep) {
}
	
void HardContactData::setImpulsion(simdata::Vector3 const& linearImpulsion, simdata::Vector3 const& angularImpulsion) {
	m_LinearImpulsion = linearImpulsion;
	m_AngularImpulsion = angularImpulsion;
}


ContactList::ContactList():
	m_Deep(0.0),
	m_NormalMaxDeep(simdata::Vector3::ZERO),
	m_ForceBody(simdata::Vector3::ZERO),
	m_MomentBody(simdata::Vector3::ZERO),
	m_Nhcd(0),
	m_LinearImpulsion(simdata::Vector3::ZERO),
	m_AngularImpulsion(simdata::Vector3::ZERO) {
}

void ContactList::deleteElements() {
	std::vector<ContactData *>::iterator i = m_contactDataList.begin();
	std::vector<ContactData *>::const_iterator iEnd = m_contactDataList.end();
	for (; i != iEnd; ++i)
		delete *i;
}

ContactList::~ContactList() {
	if (!m_contactDataList.empty())
		deleteElements();
}

void ContactList::addContact(ContactData *cd) {
	m_contactDataList.push_back(cd);
	double d = max(cd->m_deep, m_Deep);
	if (m_Deep < d) {
		m_NormalMaxDeep = cd->m_normalBody;
		m_Deep = d;
	}
	m_ForceBody += cd->m_Force;
	m_MomentBody += cd->m_Moment;
	HardContactData *hcd = dynamic_cast<HardContactData *>(cd);
	if (hcd) {
		m_Nhcd++;
		m_LinearImpulsion += hcd->m_LinearImpulsion;
		m_AngularImpulsion += hcd->m_AngularImpulsion;
	}
}

void ContactList::reset() {
	if (!m_contactDataList.empty()) {
		deleteElements();
		m_Deep = 0.0;
		m_NormalMaxDeep = simdata::Vector3::ZERO;
		m_contactDataList.clear();
		m_ForceBody = simdata::Vector3::ZERO;
		m_MomentBody = simdata::Vector3::ZERO;
		m_Nhcd = 0;
		m_LinearImpulsion = simdata::Vector3::ZERO;
		m_AngularImpulsion = simdata::Vector3::ZERO;
	}
}

unsigned short ContactList::size() const { 
	return m_contactDataList.size();
}

bool ContactList::hasImpulsion() const { 
	return m_Nhcd != 0;
}

simdata::Vector3 ContactList::zCorrection() const {
	return m_Deep * m_NormalMaxDeep;
}

simdata::Vector3 ContactList::linearImpulsion() const {
	simdata::Vector3 linearImpulsion = simdata::Vector3::ZERO;
	if (m_Nhcd > 0)
		linearImpulsion = m_LinearImpulsion / m_Nhcd;
	return linearImpulsion;
}

simdata::Vector3 ContactList::angularImpulsion() const {
	simdata::Vector3 angularImpulsion = simdata::Vector3::ZERO;
	if (m_Nhcd > 0)
		angularImpulsion = m_AngularImpulsion / m_Nhcd;
	return angularImpulsion;
}

simdata::Vector3 ContactList::forceBody() const {
	simdata::Vector3 force = simdata::Vector3::ZERO;
	if (!m_contactDataList.empty())
		force = m_ForceBody / m_contactDataList.size();
	return force; 
}

simdata::Vector3 ContactList::momentBody() const {
	simdata::Vector3 moment = simdata::Vector3::ZERO;
	if (!m_contactDataList.empty())
		moment = m_MomentBody / m_contactDataList.size();
	return moment; 
}


double const GroundCollisionDynamics::TOL = 0.01;

GroundCollisionDynamics::GroundCollisionDynamics(double mass, simdata::Matrix3 const &inertia, std::vector<simdata::Vector3> const &contacts):
	BaseDynamics(),
	m_MassInverse(1.0/mass),
	m_InertiaInverse(inertia.Inverse()),
	m_Contacts(contacts),
	m_Elastic(0.6),
	m_Friction(0.8),
	m_Vtol(-3.0) {
}

void GroundCollisionDynamics::reset(double dt) {
	m_ContactList.reset();
}

void GroundCollisionDynamics::bindWeight(simdata::Vector3 const &weightBody) {
	m_WeightBody = &weightBody;
}

void GroundCollisionDynamics::update(double dt) {
	simdata::Quaternion const &q = *m_qOrientation;
	simdata::Vector3 const &velocity_body = *m_VelocityBody;
	double const velocity_body_length = velocity_body.Length();
	simdata::Vector3 const &angular_velocity_body = *m_AngularVelocityBody;
	double const angular_velocity_body_length = angular_velocity_body.Length();
	simdata::Vector3 const groundReactionBody = - *m_WeightBody;
	simdata::Vector3 const &p = *m_PositionLocal;
	
	simdata::Vector3 const &normalGroundLocal =*m_NormalGround;
	simdata::Vector3 origin(0.0, 0.0, *m_Height);
	simdata::Vector3 normalGroundBody = simdata::QVRotate(q.Bar(),normalGroundLocal);

	std::vector<simdata::Vector3>::const_iterator contact = m_Contacts.begin();
	std::vector<simdata::Vector3>::const_iterator cEnd =  m_Contacts.end();
	
	for (; contact != cEnd; ++contact) {
		simdata::Vector3 contact_local = simdata::QVRotate(q,*contact) + origin;
		double height = simdata::Dot(contact_local, normalGroundLocal);
		if (height <= 0.0) {
			simdata::Vector3 Vb = velocity_body + (angular_velocity_body^(*contact));
			double vbn = simdata::Dot(Vb,normalGroundBody);
			if (vbn < m_Vtol) {
				HardContactData *hcd = new HardContactData(*contact,Vb,angular_velocity_body,normalGroundBody, -height);
				simdata::Vector3 angularImpulsionContact = m_InertiaInverse * (*contact^normalGroundBody);
				double li = -(1.0 + m_Elastic) * vbn / (m_MassInverse + simdata::Dot(normalGroundBody,angularImpulsionContact));
				simdata::Vector3 linearImpulsion =  m_MassInverse * li * (normalGroundBody - m_Friction * hcd->getTangentVelocityDir());
				double normLI = linearImpulsion.Length();
				double limiter = min(velocity_body_length, normLI);
				linearImpulsion = (limiter / normLI) * linearImpulsion;
				simdata::Vector3 angularImpulsion = 
					li * ( angularImpulsionContact - m_InertiaInverse * (*contact^(m_Friction * hcd->getTangentVelocityDir())));
				double normAI = angularImpulsion.Length();
				limiter = min(angular_velocity_body_length, normAI);
				angularImpulsion = (limiter / normAI) * angularImpulsion;
				hcd->setImpulsion(linearImpulsion,angularImpulsion);
				hcd->setForce(groundReactionBody);
				m_ContactList.addContact(hcd);
			}
			else {
				ContactData *cd = new ContactData(*contact,Vb,angular_velocity_body,normalGroundBody, -height);
				cd->setForce(groundReactionBody);
				m_ContactList.addContact(cd);
			}
		} //if (height <= 0.0) 
	} //for (contact)
	m_Force = m_ContactList.forceBody();
	m_Moment = m_ContactList.momentBody();
}

simdata::Vector3 GroundCollisionDynamics::getLinearImpulsion() const {
	return m_ContactList.linearImpulsion();
}

simdata::Vector3 GroundCollisionDynamics::getAngularImpulsion() const {
	return m_ContactList.angularImpulsion();
}
	
simdata::Vector3 GroundCollisionDynamics::getZCorrection() const {
	return m_ContactList.zCorrection();
}

bool GroundCollisionDynamics::hasImpulsion() const {
	return m_ContactList.hasImpulsion();
}

bool GroundCollisionDynamics::hasContact() const {
	return m_ContactList.size() != 0;
}

