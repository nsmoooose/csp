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

#include <csp/cspsim/Controller.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/ObjectUpdate.h>
#include <csp/cspsim/battlefield/BattlefieldMessages.h>
#include <csp/csplib/util/TimeStamp.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <map>
#include <set>

namespace csp {

CSP_XML_BEGIN(ChannelMirrorSet)
	CSP_DEF("mirrors", m_Mirrors, true)
CSP_XML_END

CSP_XML_BEGIN(RemoteController)
	CSP_DEF("channel_mirror_set", m_ChannelMirrorSet, false)
CSP_XML_END

CSP_XML_BEGIN(LocalController)
	CSP_DEF("channel_mirror_set", m_ChannelMirrorSet, false)
CSP_XML_END


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
	b_AngularVelocityBody = bus->getChannel(bus::Kinetics::AngularVelocityBody);
	b_Attitude = bus->getChannel(bus::Kinetics::Attitude);
	b_AccelerationBody = bus->getChannel(bus::Kinetics::AccelerationBody);
	for (ChannelMasters::iterator iter = m_ChannelMasters.begin(); iter != m_ChannelMasters.end(); ++iter) (*iter)->bind(bus);
}

void RemoteController::postCreate() {
	System::postCreate();
	if (m_ChannelMirrorSet.valid()) {
		Link<ChannelMirror>::vector const &mirrors = m_ChannelMirrorSet->mirrors();
		Link<ChannelMirror>::vector::const_iterator iter = mirrors.begin();
		m_ChannelMasters.reserve(mirrors.size());
		for (; iter != mirrors.end(); ++iter) {
			ChannelMaster *master = (*iter)->createMaster();
			assert(master);
			if (master) m_ChannelMasters.push_back(master);
		}
	}
}

bool RemoteAnimationUpdate::update(int lod) {
	if (m_Channel.valid() && lod >= m_LevelOfDetail) {
		double test = (m_Channel->value() - m_Limit0) * m_Scale;
		if (test < -5.0 || test > 260) {
			//static int XXX = 0; if ((++XXX % 1000) == 1) std::cout << m_Channel->getName() << " " << m_Channel->value() << " " << m_Limit0 << " " << m_Scale << "\n";
		}
		double value = clampTo((m_Channel->value() - m_Limit0) * m_Scale, 0.0, 255.0);
		uint8 x = static_cast<uint8>(value);
		if (x != m_LastValue) {
			// one unit of hysteresis to prevent jittering of the remote animation
			if ((x > m_LastValue) && (m_Increasing || (x > m_LastValue + 1))) {
				m_LastValue = x;
				m_Increasing = true;
				return true;
			} else if ((x < m_LastValue) && (!m_Increasing || (x + 1 < m_LastValue))) {
				m_LastValue = x;
				m_Increasing = false;
				return true;
			}
		}
	}
	return false;
}

void LocalAnimationUpdate::setTarget(uint8 value) {
	m_Target = m_Limit0 + (static_cast<double>(value) * m_Scale);
}

void LocalAnimationUpdate::update(double dt) {
	double value = m_Channel->value();
	if (value < m_Target - 0.001) {
		m_Channel->value() = std::min(value + m_RateLimit * dt, m_Target);  // ~60 deg/sec
	} else if (value > m_Target + 0.001) {
		m_Channel->value() = std::max(value - m_RateLimit * dt, m_Target);  // ~60 deg/sec
	}
}


Ref<NetworkMessage> RemoteController::getUpdate(TimeStamp current_timestamp, SimTime interval, int detail) {
	// TODO (in subclass) compare interval to current_timestamp - last_timestamp to decide if
	// we need to generate a new message.  better, need to find a simple way to cache the
	// encoded packet data if no update is needed!
	// ^^^^^
	// Note that this is currently done, at least to some extent.   LocalBattlefield caches
	// messages by detail level, and messages sent to multiple targets are serialized only
	// once per frame (by caching in MessageQueue).  In the current scheme, message
	// contents must *not* depend on interval (which is always zero now), since the same
	// message may be sent to peers at different update intervals.
	//
	// Assuming the current caching scheme is sufficient, the interval parameter should be
	// removed from this method.  On the other hand, interval may provide an important
	// optimization by allowing the controlling host to suppress updates to specific peers
	// based on expected error.  For example, there is no need to send fast updates for
	// vehicles moving at nearly constant velocity.  This optimization needs further
	// investigation.
	// ^^^^^
	// Note: a rudementary form of error estimation is implemented below.  This approach
	// does not attempt to estimate the actual error on each host, but instead uses
	// linear acceleration and angular velocity to obtain a rough upper bound on the
	// error and throttle updates accordingly.  This is significantly simpler and less
	// CPU intensive than maintaining per-host error budgets, and potentially just as
	// effective.

	interval = 0; // silence warnings for now

	// occasionally force updates even when nothing has changed, in case packets were
	// dropped and we are in an inconsistent state.
	const bool force = ((++m_UpdateDelay & 31) == 1);

	// XXX test to reduce the update rate at low acceleration / rotation rates.  the values and
	// formulas here are ad-hoc and require tuning.  the lower bound of the clamp forces the
	// update rate to be at least 1/5 the nominal update rate for this detail level.  the rate
	// increases toward the nominal rate as the linear acceleration and/or rotation rate increase.
	// with the current parameters, the rate quickly saturates at the nominal update rate.  the
	// maximum error at which we skip packets depends exponentially on the detail level (ie
	// linearly with separation), so that lower detail levels will skip packets at significantly
	// higher acceleration/rotation rates.  as it stands this optimization will be most useful
	// for large numbers of AI (mostly ground) vehicles moving at constant speed, however with
	// proper tweaking it may prove more widely beneficial.
#define CSP_ERROR_THROTTLING  // experimental
#ifdef CSP_ERROR_THROTTLING
	double acceleration_error = b_AccelerationBody->value().length2();
	double rotation_error = b_AngularVelocity->value().length2();
	double error = clampTo(sqrt(acceleration_error + 350 * rotation_error) * (1 << detail) * 0.001, 0.2, 1.0);
	int skip_count = static_cast<int>(1.0 / error + 0.5);
#else
	int skip_count = 1;
#endif

	//if (m_UpdateDelay % 20 == 0) {
	//	CSPLOG(ERROR, APP) << "skip: " << acceleration_error << " " << rotation_error << " " << skip_count;
	//}

	Ref<ObjectUpdate> update = new ObjectUpdate();

	std::vector<uint8> &flags = update->set_animation_flags();
	std::vector<uint8> &values = update->set_animation_values();
	bool has_animation_updates = false;
	for (unsigned i = 0; i < m_ChannelMasters.size(); ++i) {
		if (m_ChannelMasters[i]->send(detail, values, force)) {
			has_animation_updates = true;
			unsigned byte = i >> 3;
			unsigned bit = i & 7;
			flags.resize(byte + 1, 0);
			flags[byte] |= (1 << bit);
		}
	}

	// skip packets only if the error is low _and_ the animated control surfaces are stationary.
	if ((m_UpdateDelay % skip_count > 0) && !has_animation_updates) {
		return 0;
	}

	if (!has_animation_updates) {
		update->clear_animation_flags();
		update->clear_animation_values();
	}
	update->set_timestamp(current_timestamp);
	update->set_position(GlobalPosition(position()));
	update->set_velocity(Vector3f(velocity()));
	if (detail > 1) {
		update->set_attitude(Vector4f(attitude()));
	}
	return update;
}

void LocalController::setTargetPosition(Vector3 const &position) {
	m_TargetPosition = position;
	m_Correction = m_TargetPosition - b_Position->value();
}

void LocalController::setTargetVelocity(Vector3 const &velocity) {
	m_TargetVelocity = velocity;
}
void LocalController::setTargetAttitude(Quat const &attitude) {
	m_TargetAttitude = attitude;
}

LocalController::LocalController(): m_LastStamp(0) {
}

LocalController::~LocalController() {
}

void LocalController::postCreate() {
	System::postCreate();
	if (m_ChannelMirrorSet.valid()) {
		Link<ChannelMirror>::vector const &mirrors = m_ChannelMirrorSet->mirrors();
		Link<ChannelMirror>::vector::const_iterator iter = mirrors.begin();
		m_ChannelSlaves.reserve(mirrors.size());
		for (; iter != mirrors.end(); ++iter) {
			ChannelSlave *slave = (*iter)->createSlave();
			assert(slave);
			if (slave) m_ChannelSlaves.push_back(slave);
		}
	}
}

void LocalController::registerChannels(Bus *bus) {
	CSPLOG(INFO, APP) << "LocalController::registerChannels";
	for (ChannelSlaves::iterator iter = m_ChannelSlaves.begin(); iter != m_ChannelSlaves.end(); ++iter) (*iter)->bind(bus);
}

void LocalController::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Position = bus->getSharedChannel(bus::Kinetics::Position, true, true);
	b_Velocity = bus->getSharedChannel(bus::Kinetics::Velocity, true, true);
	b_AngularVelocity = bus->getSharedChannel(bus::Kinetics::AngularVelocity, true, true);
	b_Attitude = bus->getSharedChannel(bus::Kinetics::Attitude, true, true);
}

bool LocalController::sequentialUpdate(TimeStamp stamp, TimeStamp now, SimTime &dt) {
#if 0
	static double hack_offset = 0.0;

	//-----------------
	std::cerr << "update " << stamp << " " << m_LastStamp << " i=" << timeStampDelta(stamp, m_LastStamp) << " o=" << timeStampDelta(now, stamp) << " h=" << hack_offset << "\n";
	//-----------------
#endif

	if (m_LastStamp != 0) {
		SimTime sequence = timeStampDelta(stamp, m_LastStamp);
		if (sequence <= 0) {
			//static int XXX = 0; if (++XXX % 10 == 0) std::cout << "dropped 10 stale updates (" << stamp << ", " << m_LastStamp << ")\n";
			return false;
		}
	}

	dt = timeStampDelta(now, stamp);

	if (std::abs(dt) > 0.200) {
		//static int XXX = 0; if (++XXX % 5 == 0) std::cout << "WARNING: large update skew (" << (dt * 1e+3) << " ms)\n";
	}

#if 0
	dt = timeStampDelta(now, stamp) + hack_offset;
	// if dt < -200 ms the clocks are badly skewed.  may need a recovery mode
	// note that we need to allow negative time differences, since "now"
	// corresponds to the start of this message processing cycle rather than
	// the current time.
	// XXX hack
	if (dt < -30) {
		hack_offset += -dt;
		std::cout << "huge hack offset correction: " << hack_offset << " (" << dt << ")\n";
		dt = 0.0;
		m_LastStamp = stamp;
		return true;
	}
	// XXX ^^^ hack
	if (dt < -0.200) {
		// XXX hack
		hack_offset += 0.025;  // XXX temporary bandaid for clock skew problems
		std::cout << "hack offset now " << hack_offset << " (" << dt << ")\n";
		dt = 0.0;
		m_LastStamp = stamp;
		return true;
		// XXX ^^^ hack
		return false;
	}
#endif
	m_LastStamp = stamp;
	return true;
}

void LocalController::onUpdate(Ref<NetworkMessage> const &msg, TimeStamp now) {
	CSPLOG(INFO, OBJECT) << "received state: " << *msg;
	Ref<ObjectUpdate> update = NetworkMessage::FastCast<ObjectUpdate>(msg);
	if (!update) return;
	SimTime dt;
	if (sequentialUpdate(update->timestamp(), now, dt)) {
		CSPLOG(INFO, OBJECT) << "updating kinetics";
		setTargetVelocity(update->velocity().asVector3());
		if (update->has_attitude()) {
			setTargetAttitude(update->attitude().asQuat());
		}
		setTargetPosition(update->position().asVector3() + m_TargetVelocity * dt);

		if (update->has_animation_flags()) {
			std::vector<uint8> const &flags = update->animation_flags();
			std::vector<uint8> const &values = update->animation_values();
			const unsigned n = std::min(flags.size() * 8, m_ChannelSlaves.size());
			unsigned idx = 0;
			for (unsigned i = 0; i < n; ++i) {
				const uint8 flag = flags[i>>3];
				if ((flag & (1<<(i&7))) != 0) {
					m_ChannelSlaves[i]->receive(values, idx);
				}
			}
			assert(idx == values.size());
		}
	} else {
		CSPLOG(INFO, OBJECT) << "non sequential update: msg=" << update->timestamp() << " local=" << now;
	}
}

double LocalController::onUpdate(double dt) {
	CSPLOG(INFO, OBJECT) << "local controller update";
	b_Position->value() = b_Position->value() + b_Velocity->value() * dt;
	{
		double dx = m_Correction.length() - 50.0;
		double f = dt * 1.0;
		if (dx > 0.0) f += dx * dx * 0.0001;
		if (f > 1.0) f = 1.0;
		Vector3 partial_correction = f * m_Correction;
		b_Position->value() += partial_correction;
		m_Correction -= partial_correction;
	}
	{
		Vector3 dv = m_TargetVelocity - b_Velocity->value();
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
		// TODO can skip if LOD is too low for any animations
		for (unsigned i = 0; i < m_ChannelSlaves.size(); ++i) {
			m_ChannelSlaves[i]->update(dt);
		}
	}
	return 0.0;
}

} // namespace csp

