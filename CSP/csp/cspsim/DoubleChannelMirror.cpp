// Combat Simulator Project - CSPSim
// Copyright (C) 2005 The Combat Simulator Project
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


/** Channel mirror specializations for double-precision floating point
 *  channels.
 */


#include <ChannelMirror.h>
#include <Bus.h>
#include <csp/lib/util/Math.h>
#include <csp/lib/data/ObjectInterface.h>


class DoubleChannelSlave: public ChannelSlave {
	DataChannel<double>::Ref m_Channel;
	double m_Limit0;
	double m_RateLimit;
	double m_Scale;
	double m_Target;

public:
	DoubleChannelSlave(std::string const &channel_name, int lod, double limit0, double limit1, double rate_limit):
		ChannelSlave(channel_name, lod),
		m_Limit0(limit0),
		m_RateLimit(rate_limit),
		m_Scale(0.0),
		m_Target(0.0)
	{
		assert(limit1 > limit0);
		m_Scale = (limit1 - limit0) / 255.0;
	}

	virtual void bind(Bus *bus) {
		m_Channel = bus->getSharedChannel(getChannelName(), /*required=*/false, /*override=*/true);
		if (!m_Channel) {
			m_Channel = bus->registerSharedDataChannel<double>(getChannelName(), 0.0);
		} else {
			if (!m_Channel->isShared()) {
				CSP_LOG(OBJECT, ERROR, "Cannot drive nonshared data channel " << getChannelName());
				m_Channel = 0;  // be nice
			}
		}
		if (!m_Channel) CSP_LOG(OBJECT, WARNING, "Unable to bind or create shared data channel " << getChannelName());
	}

	virtual bool receive(ValueSet const &values, unsigned &idx) {
		assert(idx < values.size());
		if (idx >= values.size()) return false;
		const simdata::uint8 value = values[idx++];
		m_Target = m_Limit0 + static_cast<double>(value) * m_Scale;
		return true;
	}

	virtual void update(double dt) {
		if (!m_Channel) return;
		const double value = m_Channel->value();
		if (value < m_Target - 0.001) {
			m_Channel->value() = std::min(value + m_RateLimit * dt, m_Target);
		} else if (value > m_Target + 0.001) {
			m_Channel->value() = std::max(value - m_RateLimit * dt, m_Target);
		}
	}

};

class DoubleChannelMaster: public ChannelMaster {
	DataChannel<double>::CRef m_Channel;
	double m_Limit0;
	double m_RateLimit;
	double m_Scale;
	simdata::uint8 m_LastValue;
	bool m_Increasing;

public:
	DoubleChannelMaster(std::string const &channel_name, int lod, double limit0, double limit1, double rate_limit):
		ChannelMaster(channel_name, lod),
		m_Limit0(limit0),
		m_RateLimit(rate_limit),
		m_Scale(0.0),
		m_LastValue(0),
		m_Increasing(false)
	{
		assert(limit1 > limit0);
		m_Scale = 255.0 / (limit1 - limit0);
	}

	virtual void bind(Bus *bus) {
		m_Channel = bus->getChannel(getChannelName(), true);
	}

	virtual bool send(int lod, ValueSet &values, bool force) {
		if (m_Channel.valid() && lod >= getLod()) {
			const simdata::uint8 value = static_cast<simdata::uint8>(simdata::clampTo((m_Channel->value() - m_Limit0) * m_Scale, 0.0, 255.0));
			bool add = force;
			if (value != m_LastValue) {
				// one unit of hysteresis to prevent jittering of the remote animation
				if ((value > m_LastValue) && (m_Increasing || (value > m_LastValue + 1))) {
					m_Increasing = true;
					add = true;
				} else if ((value < m_LastValue) && (!m_Increasing || (value + 1 < m_LastValue))) {
					m_Increasing = false;
					add = true;
				}
			}
			if (add) {
				values.push_back(value);
				m_LastValue = value;
				return true;
			}
		}
		return false;
	}

};


/** Channel mirror for double-precision floating point data channels.
 */
class DoubleChannelMirror: public ChannelMirror {
	std::string m_ChannelName;
	int m_Lod;
	double m_Limit0;
	double m_Limit1;
	double m_RateLimit;
public:
	SIMDATA_DECLARE_STATIC_OBJECT(DoubleChannelMirror)

	virtual ChannelMaster *createMaster() const { return new DoubleChannelMaster(m_ChannelName, m_Lod, m_Limit0, m_Limit1, m_RateLimit); }
	virtual ChannelSlave *createSlave() const { return new DoubleChannelSlave(m_ChannelName, m_Lod, m_Limit0, m_Limit1, m_RateLimit); }
};

SIMDATA_XML_BEGIN(DoubleChannelMirror)
	SIMDATA_DEF("channel_name", m_ChannelName, true)
	SIMDATA_DEF("lod", m_Lod, true)
	SIMDATA_DEF("limit_0", m_Limit0, true)
	SIMDATA_DEF("limit_1", m_Limit1, true)
	SIMDATA_DEF("rate_limit", m_RateLimit, true)
SIMDATA_XML_END

