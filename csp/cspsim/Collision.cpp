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

#include <csp/cspsim/Collision.h>
#include <csp/cspsim/Animation.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/ObjectModel.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Quat.h>

#include <algorithm>

namespace csp {

using bus::Kinetics;

CSP_XML_BEGIN(GroundCollisionDynamics)
CSP_XML_END


GroundCollisionDynamics::GroundCollisionDynamics():
	m_ContactSpring(1e+5),
	m_SpringConstant(5e+6),
	m_Friction(1.2),
	m_ImpactDamping(1e+5),
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
	b_CenterOfMassOffset = bus->getChannel(Kinetics::CenterOfMassOffset);
	b_InertiaInverse = bus->getChannel(Kinetics::InertiaInverse);
	DataChannel<Ref<ObjectModel> >::CRefT model = bus->getChannel("Internal.ObjectModel");
	m_Contacts = model->value()->getContacts();
	m_Forces.resize(m_Contacts.size());
	m_Extension.resize(m_Contacts.size());
}


// WARNING: there is a lot of ad-hoc voodoo in this routine that has been
// hand tuned to provide somewhat visually plausible impact behavior without
// triggering numerical instabilities at low frame rates.  if you make changes,
// be sure to test the behavior of objects of different mass (e.g., wing tanks
// and aircraft) at low and high frame rates (20Hz to 100Hz).  in particular,
// hard impact, sliding impact, and stationary support (upright and inverted)
// should be checked.  perhaps a lot of the voodoo can go away once explosions
// that mask the impact details are modeled/rendered! :)
void GroundCollisionDynamics::computeForceAndMoment(double) {
	m_HasContact = false;
	m_NeedsImpulse = false;
	m_Force = Vector3::ZERO;
	m_Moment = Vector3::ZERO;

	if (!b_NearGround->value()) return;

	const Vector3 model_position = getModelPosition();
	const double height = model_position.z() - b_GroundZ->value();
	Quat const &q = *m_Attitude;
	Vector3 const &velocityBody = *m_VelocityBody;
	Vector3 const &angularVelocityBody = *m_AngularVelocityBody;
	Vector3 const &normalGroundLocal = b_GroundN->value();
	Vector3 normalGroundBody = q.invrotate(normalGroundLocal);
	Vector3 origin(0.0, 0.0, height);

	// adjust spring constant and damping in relation to vehicle mass
	// so that the response is not too stiff for light objects.  this
	// helps to reduce numerical instability.  the response is nominally
	// overdamped, although this depends on the number and distribution
	// of contact points.
	m_SpringConstant = 100.0 * b_Mass->value();
	m_ImpactDamping = 3.0 * sqrt(m_SpringConstant * b_Mass->value());

	for (size_t i = 0; i < m_Contacts.size(); ++i) {
		Vector3 forceBody = Vector3::ZERO;
		Vector3 const &contactBody = m_Contacts[i];
		Vector3 contactLocal = q.rotate(contactBody) + origin;
		double depth = -dot(contactLocal, normalGroundLocal);
		if (depth >= 0.0) { // contact
			m_HasContact = true;
			Vector3 contactVelocityBody = velocityBody + (angularVelocityBody^contactBody);
			double impactSpeed = -dot(contactVelocityBody, normalGroundBody);
			// if the contact goes far below the surface, stiffen the response until the
			// impact velocity is reversed.
			double stiffening = (impactSpeed > 0) ? std::max(1.0, depth * 10.0) : 1.0;
			// normal spring force plus damping
			double normalForce = depth * m_SpringConstant * stiffening + m_ImpactDamping * impactSpeed;
			// sanity check
			normalForce = std::max(normalForce, 0.0);

			forceBody += normalForce * normalGroundBody;

			// sliding frictional force
			Vector3 slidingVelocityBody = contactVelocityBody + impactSpeed * normalGroundBody;
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
				Vector3 slidingFriction = -m_Extension[i] * m_ContactSpring;
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
				Vector3 slidingFriction = -friction * slidingVelocityBody;
				forceBody += slidingFriction;
			}

			m_Force += forceBody;
			m_Moment += contactBody ^ forceBody;
		} else {
			// release sliding spring tension
			m_Extension[i] = Vector3::ZERO;
		}
		m_Forces[i] = forceBody;
	}

	if (m_HasContact) {
		double acceleration = m_Force.length() / b_Mass->value();
		double impactSpeed = -dot(velocityBody, normalGroundBody);
		double limit = std::max(5.0, (impactSpeed * 4)) * 9.8;
		// hard impact; limit the force and moment as the impact speed is
		// reduced to prevent big rebounds and reduce numerical instability.
		// at high accelerations, have PhysicsModel directly dampen the
		// velocity.
		if (acceleration > limit) {
			double scale = limit / acceleration;
			m_Force *= scale;
			m_Moment *= scale;
		}
		// todo: consider making the impulse adjustable (effectively force
		// v to zero if the acceleration is extreme, but just dampen it under
		// more moderate impacts above ~20G.
		if (acceleration > 20 * 9.8) m_NeedsImpulse = true;
		// not sure if this is still needed, but it seemed to help stability
		// when testing various ad-hoc tweaks to the reaction force at low
		// frame rates.
		acceleration = (b_InertiaInverse->value() * m_Moment).length();
		if (acceleration > 50) {
			m_Moment *= 50 / acceleration;
		}
	}
}

} // namespace csp

