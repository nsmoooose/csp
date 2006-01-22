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

#include <csp/cspsim/ChannelMirror.h>
#include <csp/cspsim/System.h>

#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Date.h>
#include <csp/csplib/util/TimeStamp.h>

CSP_NAMESPACE

class NetworkMessage;


class ChannelMirrorSet: public Object {
	Link<ChannelMirror>::vector m_Mirrors;
public:
	CSP_DECLARE_OBJECT(ChannelMirrorSet)
	Link<ChannelMirror>::vector const &mirrors() const { return m_Mirrors; }
};


class RemoteAnimationUpdate: public Referenced {
	DataChannel<double>::CRefT m_Channel;
	uint8 m_LastValue;
	int m_LevelOfDetail;
	double m_Limit0;
	double m_Limit1;
	double m_Scale;
	bool m_Increasing;

public:
	RemoteAnimationUpdate(): m_LastValue(0), m_LevelOfDetail(100), m_Limit0(-1.0), m_Limit1(1.0), m_Scale(255.0/2.0), m_Increasing(true) { }
	void bind(DataChannel<double>::CRefT const &channel) { m_Channel = channel; }
	bool update(int lod);
	void setLevelOfDetail(unsigned lod) { m_LevelOfDetail = lod; }
	void setLimits(double lo, double hi) { m_Limit0 = lo; m_Limit1 = hi; m_Scale = 255.0 / (m_Limit1 - m_Limit0); }
	void expandLimits(double lo, double hi) { setLimits(std::min(lo, m_Limit0), std::max(hi, m_Limit1)); }
	inline int lod() const { return m_LevelOfDetail; }
	inline uint8 value() const { return m_LastValue; }
	inline static double convert(uint8 value);
};


class LocalAnimationUpdate: public Referenced {
	DataChannel<double>::RefT m_Channel;
	double m_Target;
	double m_Limit0;
	double m_Limit1;
	double m_Scale;
	double m_RateLimit;

public:
	LocalAnimationUpdate(): m_Target(0), m_Limit0(-1.0), m_Limit1(1.0), m_Scale(2.0/255.0), m_RateLimit(1.0) { }
	void setLimits(double lo, double hi) { m_Limit0 = lo; m_Limit1 = hi; m_Scale = (hi - lo) / 255.0; }
	void setRateLimit(double limit) { m_RateLimit = limit; }
	void lowerRateLimit(double limit) { m_RateLimit = std::min(limit, m_RateLimit); }
	void expandLimits(double lo, double hi) { setLimits(std::min(lo, m_Limit0), std::max(hi, m_Limit1)); }
	void bind(DataChannel<double>::RefT const &channel) { m_Channel = channel; }
	void setTarget(uint8 value);
	void update(double dt);
};


/** Interface for controlling remote copies of an object.
 */
class RemoteController: public System {
	typedef std::vector< Ref<ChannelMaster> > ChannelMasters;

	ChannelMasters m_ChannelMasters;
	Link<ChannelMirrorSet> m_ChannelMirrorSet;

	DataChannel<Vector3>::CRefT b_Position;
	DataChannel<Vector3>::CRefT b_Velocity;
	DataChannel<Vector3>::CRefT b_AngularVelocity;
	DataChannel<Vector3>::CRefT b_AngularVelocityBody;
	DataChannel<Vector3>::CRefT b_AccelerationBody;
	DataChannel<Quat>::CRefT b_Attitude;
	int m_UpdateDelay;

protected:
	inline Vector3 const &position() const { return b_Position->value(); }
	inline Vector3 const &velocity() const { return b_Velocity->value(); }
	inline Vector3 const &angularVelocity() const { return b_AngularVelocity->value(); }
	inline Quat const &attitude() const { return b_Attitude->value(); }
	virtual void postCreate();

public:
	CSP_DECLARE_OBJECT(RemoteController)

	RemoteController();
	virtual ~RemoteController();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus *);
	virtual Ref<NetworkMessage> getUpdate(TimeStamp current_timestamp, SimTime interval, int detail);
};


/** Interface for controlling a local object based on updates from
 *  a remote controller.
 */
class LocalController: public System {
	Link<ChannelMirrorSet> m_ChannelMirrorSet;

	typedef std::vector< Ref<ChannelSlave> > ChannelSlaves;
	ChannelSlaves m_ChannelSlaves;

	// dynamic properties
	DataChannel<Vector3>::RefT b_Position;
	DataChannel<Vector3>::RefT b_Velocity;
	DataChannel<Vector3>::RefT b_AngularVelocity;
	DataChannel<Quat>::RefT b_Attitude;
	Vector3 m_TargetVelocity;
	Quat m_TargetAttitude;
	Vector3 m_TargetPosition;
	Vector3 m_Correction;
	TimeStamp m_LastStamp;

protected:
	void setTargetPosition(Vector3 const &position);
	void setTargetVelocity(Vector3 const &velocity);
	void setTargetAttitude(Quat const &attitude);
	bool sequentialUpdate(TimeStamp stamp, TimeStamp now, SimTime &dt);
	virtual void postCreate();

public:
	CSP_DECLARE_OBJECT(LocalController)

	LocalController();
	virtual ~LocalController();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus *);
	virtual void onUpdate(Ref<NetworkMessage> const &msg, TimeStamp now);
	double onUpdate(double dt);
};

CSP_NAMESPACE_END

#endif // __CONTROLLER_H__

