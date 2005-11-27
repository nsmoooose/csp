// Combat Simulator Project
// Copyright (C) 2002, 2004 The Combat Simulator Project
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
 * @file Controller.h
 *
 **/


#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <ChannelMirror.h>
#include <System.h>

#include <csp/lib/data/Vector3.h>
#include <csp/lib/data/Quat.h>
#include <csp/lib/data/Date.h>
#include <csp/lib/util/TimeStamp.h>

namespace simnet { class NetworkMessage; }


class ChannelMirrorSet: public simdata::Object {
	simdata::Link<ChannelMirror>::vector m_Mirrors;
public:
	SIMDATA_DECLARE_OBJECT(ChannelMirrorSet)
	simdata::Link<ChannelMirror>::vector const &mirrors() const { return m_Mirrors; }
};


class RemoteAnimationUpdate: public simdata::Referenced {
	DataChannel<double>::CRef m_Channel;
	simdata::uint8 m_LastValue;
	int m_LevelOfDetail;
	double m_Limit0;
	double m_Limit1;
	double m_Scale;
	bool m_Increasing;

public:
	typedef simdata::Ref<RemoteAnimationUpdate> Ref;
	RemoteAnimationUpdate(): m_LastValue(0), m_LevelOfDetail(100), m_Limit0(-1.0), m_Limit1(1.0), m_Scale(255.0/2.0), m_Increasing(true) { }
	void bind(DataChannel<double>::CRef const &channel) { m_Channel = channel; }
	bool update(int lod);
	void setLevelOfDetail(unsigned lod) { m_LevelOfDetail = lod; }
	void setLimits(double lo, double hi) { m_Limit0 = lo; m_Limit1 = hi; m_Scale = 255.0 / (m_Limit1 - m_Limit0); }
	void expandLimits(double lo, double hi) { setLimits(std::min(lo, m_Limit0), std::max(hi, m_Limit1)); }
	inline int lod() const { return m_LevelOfDetail; }
	inline simdata::uint8 value() const { return m_LastValue; }
	inline static double convert(simdata::uint8 value);
};


class LocalAnimationUpdate: public simdata::Referenced {
	DataChannel<double>::Ref m_Channel;
	double m_Target;
	double m_Limit0;
	double m_Limit1;
	double m_Scale;
	double m_RateLimit;

public:
	typedef simdata::Ref<LocalAnimationUpdate> Ref;
	LocalAnimationUpdate(): m_Target(0), m_Limit0(-1.0), m_Limit1(1.0), m_Scale(2.0/255.0), m_RateLimit(1.0) { }
	void setLimits(double lo, double hi) { m_Limit0 = lo; m_Limit1 = hi; m_Scale = (hi - lo) / 255.0; }
	void setRateLimit(double limit) { m_RateLimit = limit; }
	void lowerRateLimit(double limit) { m_RateLimit = std::min(limit, m_RateLimit); }
	void expandLimits(double lo, double hi) { setLimits(std::min(lo, m_Limit0), std::max(hi, m_Limit1)); }
	void bind(DataChannel<double>::Ref const &channel) { m_Channel = channel; }
	void setTarget(simdata::uint8 value);
	void update(double dt);
};


/** Interface for controlling remote copies of an object.
 */
class RemoteController: public System {
	typedef std::vector< simdata::Ref<ChannelMaster> > ChannelMasters;

	ChannelMasters m_ChannelMasters;
	simdata::Link<ChannelMirrorSet> m_ChannelMirrorSet;

	DataChannel<simdata::Vector3>::CRef b_Position;
	DataChannel<simdata::Vector3>::CRef b_Velocity;
	DataChannel<simdata::Vector3>::CRef b_AngularVelocity;
	DataChannel<simdata::Vector3>::CRef b_AngularVelocityBody;
	DataChannel<simdata::Vector3>::CRef b_AccelerationBody;
	DataChannel<simdata::Quat>::CRef b_Attitude;
	int m_UpdateDelay;

protected:
	inline simdata::Vector3 const &position() const { return b_Position->value(); }
	inline simdata::Vector3 const &velocity() const { return b_Velocity->value(); }
	inline simdata::Vector3 const &angularVelocity() const { return b_AngularVelocity->value(); }
	inline simdata::Quat const &attitude() const { return b_Attitude->value(); }
	virtual void postCreate();

public:
	SIMDATA_DECLARE_OBJECT(RemoteController)

	RemoteController();
	virtual ~RemoteController();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus *);
	virtual simdata::Ref<simnet::NetworkMessage> getUpdate(simcore::TimeStamp current_timestamp, simdata::SimTime interval, int detail);
};


/** Interface for controlling a local object based on updates from
 *  a remote controller.
 */
class LocalController: public System {
	simdata::Link<ChannelMirrorSet> m_ChannelMirrorSet;

	typedef std::vector< simdata::Ref<ChannelSlave> > ChannelSlaves;
	ChannelSlaves m_ChannelSlaves;

	// dynamic properties
	DataChannel<simdata::Vector3>::Ref b_Position;
	DataChannel<simdata::Vector3>::Ref b_Velocity;
	DataChannel<simdata::Vector3>::Ref b_AngularVelocity;
	DataChannel<simdata::Quat>::Ref b_Attitude;
	simdata::Vector3 m_TargetVelocity;
	simdata::Quat m_TargetAttitude;
	simdata::Vector3 m_TargetPosition;
	simdata::Vector3 m_Correction;
	simcore::TimeStamp m_LastStamp;

protected:
	void setTargetPosition(simdata::Vector3 const &position);
	void setTargetVelocity(simdata::Vector3 const &velocity);
	void setTargetAttitude(simdata::Quat const &attitude);
	bool sequentialUpdate(simcore::TimeStamp stamp, simcore::TimeStamp now, simdata::SimTime &dt);
	virtual void postCreate();

public:
	SIMDATA_DECLARE_OBJECT(LocalController)

	LocalController();
	virtual ~LocalController();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus *);
	virtual void onUpdate(simdata::Ref<simnet::NetworkMessage> const &msg, simcore::TimeStamp now);
	double onUpdate(double dt);
};


#endif // __CONTROLLER_H__

