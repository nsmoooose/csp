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

#include <System.h>
#include <SimData/Vector3.h>
#include <SimData/Quat.h>
#include <SimData/Date.h>
#include <SimCore/Util/TimeStamp.h>

namespace simnet { class NetworkMessage; }


class RemoteAnimationUpdate {
	DataChannel<double>::CRef m_Channel;
	simdata::uint8 m_LastValue;

public:
	RemoteAnimationUpdate(): m_LastValue(0) { }
	void bind(DataChannel<double>::CRef const &channel) { m_Channel = channel; }
	bool update();
	inline simdata::uint8 value() const { return m_LastValue; }
	inline static double convert(simdata::uint8 value);
};

class LocalAnimationUpdate {
	DataChannel<double>::Ref m_Channel;
	double m_Target;

public:
	LocalAnimationUpdate(): m_Target(0) { }
	void bind(DataChannel<double>::Ref const &channel) { m_Channel = channel; }
	void setTarget(simdata::uint8 value);
	void update(double dt);
};


/** Interface for controlling remote copies of an object.
 */
class RemoteController: public System {
	DataChannel<simdata::Vector3>::CRef b_Position;
	DataChannel<simdata::Vector3>::CRef b_Velocity;
	DataChannel<simdata::Vector3>::CRef b_AngularVelocity;
	DataChannel<simdata::Vector3>::CRef b_AngularVelocityBody;
	DataChannel<simdata::Vector3>::CRef b_AccelerationBody;
	DataChannel<simdata::Quat>::CRef b_Attitude;
	RemoteAnimationUpdate m_GearExtension;
	RemoteAnimationUpdate m_AileronDeflection;
	RemoteAnimationUpdate m_ElevatorDeflection;
	RemoteAnimationUpdate m_RudderDeflection;
	RemoteAnimationUpdate m_AirbrakeDeflection;
	int m_UpdateDelay;

protected:
	inline simdata::Vector3 const &position() const { return b_Position->value(); }
	inline simdata::Vector3 const &velocity() const { return b_Velocity->value(); }
	inline simdata::Vector3 const &angularVelocity() const { return b_AngularVelocity->value(); }
	inline simdata::Quat const &attitude() const { return b_Attitude->value(); }

public:
	SIMDATA_OBJECT(RemoteController, 0, 0)
	BEGIN_SIMDATA_XML_INTERFACE(RemoteController)
	END_SIMDATA_XML_INTERFACE

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
	// animation state
	DataChannel<double>::Ref b_GearExtension;
	DataChannel<double>::Ref b_AileronDeflection;
	DataChannel<double>::Ref b_ElevatorDeflection;
	DataChannel<double>::Ref b_RudderDeflection;
	DataChannel<double>::Ref b_AirbrakeDeflection;
	LocalAnimationUpdate m_GearExtension;
	LocalAnimationUpdate m_AileronDeflection;
	LocalAnimationUpdate m_ElevatorDeflection;
	LocalAnimationUpdate m_RudderDeflection;
	LocalAnimationUpdate m_AirbrakeDeflection;

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

public:
	SIMDATA_OBJECT(LocalController, 0, 0)
	BEGIN_SIMDATA_XML_INTERFACE(LocalController)
	END_SIMDATA_XML_INTERFACE

	LocalController();
	virtual ~LocalController();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus *);
	virtual void onUpdate(simdata::Ref<simnet::NetworkMessage> const &msg, simcore::TimeStamp now);
	double onUpdate(double dt);
};


#endif // __CONTROLLER_H__

