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
#include <Animation.h>
#include <KineticsChannels.h>
#include <ObjectModel.h>
#include <ObjectUpdate.h>
#include <SimCore/Battlefield/BattlefieldMessages.h>
#include <SimCore/Util/TimeStamp.h>

#include <map>
#include <set>

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
	b_AngularVelocityBody = bus->getChannel(bus::Kinetics::AngularVelocityBody);
	b_Attitude = bus->getChannel(bus::Kinetics::Attitude);
	b_AccelerationBody = bus->getChannel(bus::Kinetics::AccelerationBody);
	importAnimations(bus);
}

void RemoteController::importAnimations(Bus *bus) {
	CSP_LOG(APP, INFO, "RemoteController::importAnimations");
	DataChannel<simdata::Ref<ObjectModel> >::CRef model_channel = bus->getChannel("Internal.ObjectModel");
	if (model_channel.valid()) {
		typedef std::pair<unsigned, unsigned> LodIndex;
		typedef std::map<std::string, LodIndex> ChannelMap;
		ChannelMap channels;
		simdata::Ref<const ObjectModel> model = model_channel->value();
		const unsigned num_animations = model->numAnimations();
		CSP_LOG(APP, INFO, "Scanning " << num_animations << " animations");
		for (unsigned i = 0; i < num_animations; ++i) {
			const Animation &animation = model->animation(i);
			if (animation.getLOD() < 10) {
				const unsigned lod = animation.getLOD();
				const std::string name = animation.getChannelName();
				const double limit0 = animation.getExportLimit0();
				const double limit1 = animation.getExportLimit1();
				ChannelMap::iterator iter = channels.find(name);
				if (iter == channels.end()) {
					CSP_LOG(APP, INFO, "Adding animation channel " << name << " at lod " << lod);
					DataChannel<double>::CRef channel;
					try {
						channel = bus->getChannel(name);
					} catch (simdata::ConversionError &e) {
						e.clear();
						CSP_LOG(APP, ERROR, "Animation channel " << name << " is not of type double; skipping!");
						continue;
					}
					assert(channel.valid());
					channels[name] = LodIndex(lod, m_AnimationUpdates.size());
					RemoteAnimationUpdate *update = new RemoteAnimationUpdate();
					update->setLevelOfDetail(lod);
					update->setLimits(limit0, limit1);
					update->bind(channel);
					m_AnimationUpdates.push_back(update);
				} else if (lod < iter->second.first) {
					const unsigned idx = iter->second.second;
					CSP_LOG(APP, INFO, "Updating animation channel " << name << " lod to " << lod);
					m_AnimationUpdates[idx]->setLevelOfDetail(lod);
					m_AnimationUpdates[idx]->expandLimits(limit0, limit1);
					iter->second.first = lod;
				}
			}
		}
		CSP_LOG(APP, INFO, "Added " << m_AnimationUpdates.size() << " animation channels");
	} else {
		CSP_LOG(APP, WARNING, "3D model not found");
	}
}

bool RemoteAnimationUpdate::update(int lod) {
	if (m_Channel.valid() && lod >= m_LevelOfDetail) {
		double test = (m_Channel->value() - m_Limit0) * m_Scale;
		if (test < -5.0 || test > 260) {
			static int XXX = 0;
			if ((++XXX % 1000) == 1) {
				std::cout << m_Channel->getName() << " " << m_Channel->value() << " " << m_Limit0 << " " << m_Scale << "\n";
			}
		}
		double value = simdata::clampTo((m_Channel->value() - m_Limit0) * m_Scale, 0.0, 255.0);
		simdata::uint8 x = static_cast<simdata::uint8>(value);
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

void LocalAnimationUpdate::setTarget(simdata::uint8 value) {
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
	double error = simdata::clampTo(sqrt(acceleration_error + 350 * rotation_error) * (1 << detail) * 0.001, 0.2, 1.0);
	int skip_count = static_cast<int>(1.0 / error + 0.5);
#else
	int skip_count = 1;
#endif

	//if (m_UpdateDelay % 20 == 0) {
	//	CSP_LOG(APP, ERROR, "skip: " << acceleration_error << " " << rotation_error << " " << skip_count);
	//}

	ObjectUpdate::Ref update = new ObjectUpdate();

	std::vector<simdata::uint8> &flags = update->set_animation_flags();
	std::vector<simdata::uint8> &values = update->set_animation_values();
	bool has_animation_updates = false;
	for (unsigned i = 0; i < m_AnimationUpdates.size(); ++i) {
		const bool add = force || m_AnimationUpdates[i]->update(detail);
		if (add) {
			has_animation_updates = true;
			unsigned byte = i >> 3;
			unsigned bit = i & 7;
			flags.resize(byte + 1, 0);
			flags[byte] |= (1 << bit);
			values.push_back(m_AnimationUpdates[i]->value());
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
	update->set_position(simcore::GlobalPosition(position()));
	update->set_velocity(simcore::Vector3f(velocity()));
	if (detail > 1) {
		update->set_attitude(simcore::Vector4f(attitude()));
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
	CSP_LOG(APP, INFO, "LocalController::registerChannels");
	// Relies on the fact that DynamicObject registers its channels before the subsystems.
	DataChannel<simdata::Ref<ObjectModel> >::CRef model_channel = bus->getChannel("Internal.ObjectModel", false);
	if (model_channel.valid()) {
		typedef std::map<std::string, unsigned> ChannelMap;
		ChannelMap channels;
		simdata::Ref<const ObjectModel> model = model_channel->value();
		const unsigned num_animations = model->numAnimations();
		CSP_LOG(APP, INFO, "Scanning " << num_animations << " animations");
		for (unsigned i = 0; i < num_animations; ++i) {
			const Animation &animation = model->animation(i);
			if (animation.getLOD() < 10) {
				const std::string name = animation.getChannelName();
				const double limit0 = animation.getExportLimit0();
				const double limit1 = animation.getExportLimit1();
				const double rate_limit = animation.getExportRate();
				ChannelMap::const_iterator iter = channels.find(name);
				if (iter == channels.end()) {
					CSP_LOG(APP, INFO, "Adding animation channel " << name);
					LocalAnimationUpdate *update = new LocalAnimationUpdate();
					assert(!bus->hasChannel(name));
					DataChannel<double>::Ref channel;
					channel = bus->registerLocalDataChannel<double>(name, 0.0);
					assert(channel.valid());
					update->bind(channel);
					update->setLimits(limit0, limit1);
					update->setRateLimit(rate_limit);
					channels[name] = m_AnimationUpdates.size();
					m_AnimationUpdates.push_back(update);
				} else {
					m_AnimationUpdates[iter->second]->expandLimits(limit0, limit1);
					m_AnimationUpdates[iter->second]->lowerRateLimit(rate_limit);
				}
			}
		}
		CSP_LOG(APP, INFO, "Added " << m_AnimationUpdates.size() << " animation channels");
	} else {
		CSP_LOG(APP, WARNING, "3D model not found");
	}

}

void LocalController::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Position = bus->getSharedChannel(bus::Kinetics::Position, true, true);
	b_Velocity = bus->getSharedChannel(bus::Kinetics::Velocity, true, true);
	b_AngularVelocity = bus->getSharedChannel(bus::Kinetics::AngularVelocity, true, true);
	b_Attitude = bus->getSharedChannel(bus::Kinetics::Attitude, true, true);
}

bool LocalController::sequentialUpdate(simcore::TimeStamp stamp, simcore::TimeStamp now, simdata::SimTime &dt) {
#if 0
	static double hack_offset = 0.0;

	//-----------------
	std::cerr << "update " << stamp << " " << m_LastStamp << " i=" << simcore::timeStampDelta(stamp, m_LastStamp) << " o=" << simcore::timeStampDelta(now, stamp) << " h=" << hack_offset << "\n";
	//-----------------
#endif

	if (m_LastStamp != 0) {
		simdata::SimTime sequence = simcore::timeStampDelta(stamp, m_LastStamp);
		if (sequence <= 0) {
			static int XXX = 0; if (++XXX % 10 == 0) std::cout << "dropped 10 stale updates (" << stamp << ", " << m_LastStamp << ")\n";
			return false;
		}
	}

	dt = simcore::timeStampDelta(now, stamp);

	if (std::abs(dt) > 0.200) {
		static int XXX = 0;
		if (++XXX % 5 == 0) std::cout << "WARNING: large update skew (" << (dt * 1e+3) << " ms)\n";
	}

#if 0
	dt = simcore::timeStampDelta(now, stamp) + hack_offset;
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

		if (update->has_animation_flags()) {
			std::vector<simdata::uint8> const &flags = update->animation_flags();
			std::vector<simdata::uint8> const &values = update->animation_values();
			const unsigned n = std::min(flags.size() * 8, m_AnimationUpdates.size());
			unsigned idx = 0;
			for (unsigned i = 0; i < n; ++i) {
				const simdata::uint8 flag = flags[i>>3];
				if ((flag & (1<<(i&7))) != 0) {
					assert(idx < values.size());
					m_AnimationUpdates[i]->setTarget(values[idx++]);
				}
			}
			assert(idx == values.size());
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
		// TODO can skip if LOD is too low for any animations
		for (unsigned i = 0; i < m_AnimationUpdates.size(); ++i) {
			m_AnimationUpdates[i]->update(dt);
		}
		/*
		m_ElevatorDeflection.update(dt);
		m_AirbrakeDeflection.update(dt);
		m_AileronDeflection.update(dt);
		m_RudderDeflection.update(dt);
		*/
	}
	return 0.0;
}


