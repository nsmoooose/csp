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
 * @file Controller.cpp
 *
 **/


#include <Controller.h>
#include <KineticsChannels.h>
#include <SimCore/Battlefield/BattlefieldMessages.h>
#include <ObjectUpdate.h>
#include <SimCore/Util/TimeStamp.h>

SIMDATA_REGISTER_INTERFACE(LocalController)
SIMDATA_REGISTER_INTERFACE(RemoteController)


RemoteController::RemoteController(): m_UpdateDelay(0) {
}

RemoteController::~RemoteController() {
}

void RemoteController::registerChannels(Bus *) {
}

void RemoteController::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Position = bus->getChannel(bus::Kinetics::Position);
	b_Velocity = bus->getChannel(bus::Kinetics::Velocity);
	b_AngularVelocity = bus->getChannel(bus::Kinetics::AngularVelocity);
	b_Attitude = bus->getChannel(bus::Kinetics::Attitude);
	m_GearExtension.bind(bus->getChannel("LandingGear.GearExtended"));
	m_AileronDeflection.bind(bus->getChannel("ControlSurfaces.AileronDeflection"));
	m_ElevatorDeflection.bind(bus->getChannel("ControlSurfaces.ElevatorDeflection"));
	m_RudderDeflection.bind(bus->getChannel("ControlSurfaces.RudderDeflection"));
	m_AirbrakeDeflection.bind(bus->getChannel("ControlSurfaces.AirbrakeDeflection"));
}

bool RemoteAnimationUpdate::update() {
	if (m_Channel.valid()) {
		simdata::uint8 x = static_cast<simdata::uint8>(100.0 * m_Channel->value() + 128);
		if (x != m_LastValue) {
			m_LastValue = x;
			return true;
		}
	}
	return false;
}

void LocalAnimationUpdate::setTarget(simdata::uint8 value) {
	m_Target = 0.01 * (static_cast<double>(value) - 128.0);
}

void LocalAnimationUpdate::update(double dt) {
	double value = m_Channel->value();
	if (value < m_Target - 0.001) {
		m_Channel->value() = std::min(value + 1.0 * dt, m_Target);  // ~60 deg/sec
	} else if (value > m_Target + 0.001) {
		m_Channel->value() = std::max(value - 1.0 * dt, m_Target);  // ~60 deg/sec
	}
}


simdata::Ref<simnet::NetworkMessage> RemoteController::getUpdate(simcore::TimeStamp current_timestamp, simdata::SimTime interval, int detail) {
	// TODO (in subclass) compare interval to current_timestamp - last_timestamp to decide if
	// we need to generate a new message.  better, need to find a simple way to cache the
	// encoded packet data if no update is needed!
	// ^^^^^
	// Note that this is currently done, at least to some extent.   LocalBattlefield caches
	// messages by detail level, and messages sent to multiple targets are serialized only
	// once per frame (by caching in simnet::MessageQueue).  In the current scheme, message
	// contents must *not* depend on interval (which is always zero now), since the same
	// message may be sent to peers at different update intervals.
	//
	// Assuming the current caching scheme is sufficient, the interval parameter should be
	// removed from this method.  On the other hand, interval may provide an important
	// optimization by allowing the controlling host to suppress updates to specific peers
	// based on expected error.  For example, there is no need to send fast updates for
	// vehicles moving at nearly constant velocity.  This optimization needs further
	// investigation.

	interval = 0; // silence warnings for now

	// occasionally force updates even when nothing has changed, in case packets were
	// dropped and we are in an inconsistent state.
	const bool force = ((++m_UpdateDelay & 15) == 1);

	ObjectUpdate::Ref update = new ObjectUpdate();
	update->set_timestamp(current_timestamp);
	update->set_position(simcore::GlobalPosition(position()));
	update->set_velocity(simcore::Vector3f(velocity()));
	if (detail > 1) {
		update->set_attitude(simcore::Vector4f(attitude()));
	}
	if (detail > 4) {
		// only a single bit for now.  if we don't set the bit every time it will
		// assume its default value (gear up).
		m_GearExtension.update();
		const bool gear_up = m_GearExtension.value() == 128;  // 0.0 -> 128
		if (!gear_up || force) {
			// by only setting the non-default state, we save a couple bytes when
			// the gear is up.  there are probably better ways to optimize this.
			update->set_gear_up(gear_up);
		}
	}
	if (detail > 7) {
		if (m_ElevatorDeflection.update() || force) {
			update->set_elevator_deflection(m_ElevatorDeflection.value());
		}
		if (m_AileronDeflection.update() || force) {
			update->set_aileron_deflection(m_AileronDeflection.value());
		}
		if (m_RudderDeflection.update() || force) {
			update->set_rudder_deflection(m_RudderDeflection.value());
		}
		if (m_AirbrakeDeflection.update() || force) {
			update->set_airbrake_deflection(m_AirbrakeDeflection.value());
		}
	}
	return update;
}

void LocalController::setTargetPosition(simdata::Vector3 const &position) {
	m_TargetPosition = position;
	m_Correction = m_TargetPosition - b_Position->value();
}

void LocalController::setTargetVelocity(simdata::Vector3 const &velocity) {
	m_TargetVelocity = velocity;
}
void LocalController::setTargetAttitude(simdata::Quat const &attitude) {
	m_TargetAttitude = attitude;
}

LocalController::LocalController(): m_LastStamp(0) {
}

LocalController::~LocalController() {
}

void LocalController::registerChannels(Bus *bus) {
	b_GearExtension = bus->registerLocalDataChannel<double>("LandingGear.GearExtended", 0.0);
	b_AileronDeflection = bus->registerLocalDataChannel<double>("ControlSurfaces.AileronDeflection", 0.0);
	b_ElevatorDeflection = bus->registerLocalDataChannel<double>("ControlSurfaces.ElevatorDeflection", 0.0);
	b_RudderDeflection = bus->registerLocalDataChannel<double>("ControlSurfaces.RudderDeflection", 0.0);
	b_AirbrakeDeflection = bus->registerLocalDataChannel<double>("ControlSurfaces.AirbrakeDeflection", 0.0);
}

void LocalController::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Position = bus->getSharedChannel(bus::Kinetics::Position, true, true);
	b_Velocity = bus->getSharedChannel(bus::Kinetics::Velocity, true, true);
	b_AngularVelocity = bus->getSharedChannel(bus::Kinetics::AngularVelocity, true, true);
	b_Attitude = bus->getSharedChannel(bus::Kinetics::Attitude, true, true);
	m_GearExtension.bind(b_GearExtension);
	m_AileronDeflection.bind(b_AileronDeflection);
	m_ElevatorDeflection.bind(b_ElevatorDeflection);
	m_RudderDeflection.bind(b_RudderDeflection);
	m_AirbrakeDeflection.bind(b_AirbrakeDeflection);
}

bool LocalController::sequentialUpdate(simcore::TimeStamp stamp, simcore::TimeStamp now, simdata::SimTime &dt) {
	static double hack_offset = 0.0;
	if (m_LastStamp != 0) {
		simdata::SimTime sequence = simcore::timeStampDelta(stamp, m_LastStamp);
		if (sequence <= 0) return false;
	}
	dt = simcore::timeStampDelta(now, stamp) + hack_offset;
	// if dt < -200 ms the clocks are badly skewed.  may need a recovery mode
	// note that we need to allow negative time differences, since "now"
	// corresponds to the start of this message processing cycle rather than
	// the current time.
	if (dt < -0.200) {
		// XXX hack
		hack_offset += 0.025;  // XXX temporary bandaid for clock skew problems
		std::cout << "hack offset now " << hack_offset << "\n";
		dt = 0.0;
		m_LastStamp = stamp;
		return true;
		// XXX ^^^ hack
		return false;
	}
	m_LastStamp = stamp;
	return true;
}

void LocalController::onUpdate(simdata::Ref<simnet::NetworkMessage> const &msg, simcore::TimeStamp now) {
	CSP_LOG(OBJECT, INFO, "received state: " << *msg);
	ObjectUpdate::Ref update = simnet::NetworkMessage::FastCast<ObjectUpdate>(msg);
	if (!update) return;
	simdata::SimTime dt;
	if (sequentialUpdate(update->timestamp(), now, dt)) {
		CSP_LOG(OBJECT, INFO, "updating kinetics");
		setTargetVelocity(update->velocity().Vector3());
		if (update->has_attitude()) {
			setTargetAttitude(update->attitude().Quat());
		}
		setTargetPosition(update->position().Vector3() + m_TargetVelocity * dt);
		b_GearExtension->value() = (update->gear_up() ? 0.0 : 1.0);
		if (update->has_elevator_deflection()) {
			m_ElevatorDeflection.setTarget(update->elevator_deflection());
		}
		if (update->has_airbrake_deflection()) {
			m_AirbrakeDeflection.setTarget(update->airbrake_deflection());
		}
		if (update->has_aileron_deflection()) {
			m_AileronDeflection.setTarget(update->aileron_deflection());
		}
		if (update->has_rudder_deflection()) {
			m_RudderDeflection.setTarget(update->rudder_deflection());
		}
	} else {
		CSP_LOG(OBJECT, INFO, "non sequential update: msg=" << update->timestamp() << " local=" << now);
	}
}

double LocalController::onUpdate(double dt) {
	CSP_LOG(OBJECT, INFO, "local controller update");
	b_Position->value() = b_Position->value() + b_Velocity->value() * dt;
	{
		double dx = m_Correction.length() - 50.0;
		double f = dt * 1.0;
		if (dx > 0.0) f += dx * dx * 0.0001;
		if (f > 1.0) f = 1.0;
		simdata::Vector3 partial_correction = f * m_Correction;
		b_Position->value() += partial_correction;
		m_Correction -= partial_correction;
	}
	{
		simdata::Vector3 dv = m_TargetVelocity - b_Velocity->value();
		double dx = dv.length() - 10;
		double f = dt * 10.0;
		if (dx > 0.0) f += dx * dx * 0.01;
		if (f > 1.0) f = 1.0;
		b_Velocity->value() = b_Velocity->value() + dv * f;
	}
	{
		double f = dt * 10.0;
		if (f > 1.0) f = 1.0;
		b_Attitude->value().slerp(f, b_Attitude->value(), m_TargetAttitude);
	}
	{
		m_ElevatorDeflection.update(dt);
		m_AirbrakeDeflection.update(dt);
		m_AileronDeflection.update(dt);
		m_RudderDeflection.update(dt);
	}
	return 0.0;
}


