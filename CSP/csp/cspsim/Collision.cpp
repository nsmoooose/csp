// Combat Simulator Project
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

#include <Collision.h>
#include <Animation.h>
#include <KineticsChannels.h>
#include <ObjectModel.h>
#include <csp/csplib/util/Log.h>

#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/util/Ref.h>

using bus::Kinetics;


CSP_XML_BEGIN(GroundCollisionDynamics)
CSP_XML_END


GroundCollisionDynamics::GroundCollisionDynamics():
	m_ContactSpring(1e+5),
	m_SpringConstant(5e+6),
	m_Friction(1.2),
	m_ImpactDamping(1e+6),
	m_ImpactSpeedTolerance(3.0),
	m_NeedsImpulse(false),
	m_HasContact(false)
{
}

void GroundCollisionDynamics::setGroundProperties(double spring_constant,
                                                  double friction,
                                                  double impact_damping)
{
	m_SpringConstant = spring_constant;
	m_Friction = friction;
	m_ImpactDamping = impact_damping;
}


void GroundCollisionDynamics::registerChannels(Bus *) {
}

void GroundCollisionDynamics::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Mass = bus->getChannel(Kinetics::Mass);
	b_GroundN = bus->getChannel(Kinetics::GroundN);
	b_GroundZ = bus->getChannel(Kinetics::GroundZ);
	b_NearGround = bus->getChannel(Kinetics::NearGround);
	DataChannel<simdata::Ref<ObjectModel> >::CRef model = bus->getChannel("Internal.ObjectModel");
	m_Contacts = model->value()->getContacts();
	m_Forces.resize(m_Contacts.size());
	m_Extension.resize(m_Contacts.size());
}


void GroundCollisionDynamics::computeForceAndMoment(double) {
	m_HasContact = false;
	m_NeedsImpulse = false;
	m_Force = simdata::Vector3::ZERO;
	m_Moment = simdata::Vector3::ZERO;

	if (!b_NearGround->value()) return;

	const simdata::Vector3 model_position = getModelPositionLocal();
	const double height = model_position.z() - b_GroundZ->value();
	simdata::Quat const &q = *m_Attitude;
	simdata::Vector3 const &velocityBody = *m_VelocityBody;
	simdata::Vector3 const &angularVelocityBody = *m_AngularVelocityBody;
	simdata::Vector3 const &normalGroundLocal = b_GroundN->value();
	simdata::Vector3 normalGroundBody = q.invrotate(normalGroundLocal);
	simdata::Vector3 origin(0.0, 0.0, height);

	for (size_t i = 0; i < m_Contacts.size(); ++i) {
		simdata::Vector3 forceBody = simdata::Vector3::ZERO;
		simdata::Vector3 const &contactBody = m_Contacts[i];
		simdata::Vector3 contactLocal = q.rotate(contactBody) + origin;
		double depth = -simdata::dot(contactLocal, normalGroundLocal);
		if (depth >= 0.0) { // contact
			m_HasContact = true;
			simdata::Vector3 contactVelocityBody = velocityBody + (angularVelocityBody^contactBody);
			double impactSpeed = -simdata::dot(contactVelocityBody, normalGroundBody);
			if (impactSpeed > m_ImpactSpeedTolerance) { // hard impact (not used)
			}
			// normal spring force plus damping
			double normalForce = depth * m_SpringConstant + m_ImpactDamping * impactSpeed;
			// sanity check
			normalForce = std::max(normalForce, 0.0);
			forceBody += normalForce * normalGroundBody;
			// sliding frictional force
			simdata::Vector3 slidingVelocityBody = contactVelocityBody + impactSpeed * normalGroundBody;
			double slidingSpeed = slidingVelocityBody.length();
			if (slidingSpeed > 0.0) {
				// in principle we should have both sliding and static friction coefficients,
				// but this is probably adequate for now.
				double frictionLimit = std::max(0.1, m_Friction * normalForce);
				// i'd like to do damped spring extension relative to a fixed point when not
				// sliding, but not sure if this will work with rk since dt doesn't accumulate
				// monotonic time.
				/*
				m_Extension[i] += slidingVelocityBody * dt;
				simdata::Vector3 slidingFriction = -m_Extension[i] * m_ContactSpring;
				double frictionScale = slidingFriction.length() / frictionLimit;
				if (frictionScale > 1.0) {  // free sliding
					m_Extension[i] /= frictionScale;
					slidingFriction /= frictionScale;
				} else {
					double beta = 0.2*m_ContactSpring;
					slidingFriction -= beta * slidingVelocityBody;
				}
				*/
				// instead just use sliding friction, and attenuate it to zero at low velocity
				// this generally results in a perpetual slide at low speed.
				double friction = frictionLimit / (slidingSpeed + 0.1);
				simdata::Vector3 slidingFriction = -friction * slidingVelocityBody;
				forceBody += slidingFriction;
			}
			m_Force += forceBody;
		} else {
			// release sliding spring tension
			m_Extension[i] = simdata::Vector3::ZERO;
		}
		m_Forces[i] = forceBody;
	}

	if (m_HasContact) {
		double acceleration = m_Force.length() / b_Mass->value();
		double limit = 20.0 * 9.8; // 20 G limit for basic response
		double scale = 1.0;
		// extreme impact, supply addition reactive impulse
		if (acceleration > limit) {
			scale = limit / acceleration;
			m_NeedsImpulse = true;
		}
		// compute normalized forces and moments
		m_Force = simdata::Vector3::ZERO;
		for (size_t i = 0; i < m_Contacts.size(); ++i) {
			m_Force += m_Forces[i] * scale;
			m_Moment += m_Contacts[i] ^ m_Forces[i] * scale;
		}
	}
}


