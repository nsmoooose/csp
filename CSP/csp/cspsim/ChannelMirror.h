// Combat Simulator Project
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


/** Interfaces for replicating data channels between remote clients.
 *
 *  Most of the important state of an object is contained its the data channels.
 *  Replicating selected channels from a source object to remote mirrors of
 *  that object provides a simple and efficient means of driving remote
 *  objects in a multiplayer environment.
 *
 *  An object can define any number of channel mirrors via its XML interface
 *  for the LocalController system (see Controller.cpp).  These definitions
 *  include the channel name, the level of detail threshold for the channel,
 *  quantization paramaters for encoding the channel value, and other details.
 *  These channel mirrors in turn are used to create master and slave instances
 *  that bind to the data buses on the local and remote machines, respectively.
 *  The masters read the channel values, quantize them appropriately, and add
 *  them to the message that is sent to update remote peers.  Slaves at each
 *  remote peer decode the channel data from the message and adjust the local
 *  channel value accordingly (e.g., via a low pass filter to smooth the jitter
 *  associated with the discrete updates).
 *
 *  This header declares only the base interfaces for mirroring channels.
 *  Subclasses for specific channel types and behaviors are defined in the
 *  corresponding .cpp file.
 *
 *  Note that for efficiency of communication,  proper channel mirroring
 *  requires that both sender and receiver agree on the parameters used to
 *  encode the channel data.  At the moment, changing these parameters can
 *  break multiplayer compatability.  As a future enhancement, the Controller
 *  subclasses could be modified to establish consistent parameters through a
 *  handshake on first connection.
 */

#ifndef __CSPSIM_CHANNEL_MIRROR_H__
#define __CSPSIM_CHANNEL_MIRROR_H__

#include <csp/csplib/data/Object.h>
#include <string>

CSP_NAMESPACE

class Bus;


/** Base class for replicating data channels between remote clients.  Specialized
 *  subclasses provide send and receive functionality for different channel types.
 *  ChannelLink subclasses are used by the Controller classes to mirror the motion
 *  and animation of an object from a local simulation to one or more remote peers.
 */
class ChannelLink: public Referenced {
	std::string m_ChannelName;
	int m_Lod;
	bool m_Master;
public:
	typedef std::vector<uint8> ValueSet;
	ChannelLink(std::string const &channel_name, int lod, bool master): m_ChannelName(channel_name), m_Lod(lod), m_Master(master) { }
	virtual ~ChannelLink() {}
	virtual void bind(Bus*) = 0;
	std::string const &getChannelName() const { return m_ChannelName; }
	int getLod() const { return m_Lod; }
	bool isMaster() const { return m_Master; }
};


/** A ChannelLink specialization that serializes a local data channel for transmission
 *  to remote peers.
 */
class ChannelMaster: public ChannelLink {
public:
	ChannelMaster(std::string const &channel_name, int lod): ChannelLink(channel_name, lod, true) { }
	virtual bool send(int lod, ValueSet &values, bool force=false) = 0;
};


/** A ChannelLink specialization that deserializes a remote data channel to drive a
 *  local object that mirrors the remote object.
 */
class ChannelSlave: public ChannelLink {
public:
	ChannelSlave(std::string const &channel_name, int lod): ChannelLink(channel_name, lod, false) { }
	virtual bool receive(ValueSet const &values, unsigned &index) = 0;
	virtual void update(double /*dt*/) { };
};


/** An base class for defining and creating remote links between data channels.
 *  Subclasses of ChannelMirror define the properities of the link (channel name,
 *  lod, limits, etc) via external XML definitions, and are specialized for
 *  different channel types (double, enumlink, bool, etc).
 */
class ChannelMirror: public Object {
public:
	virtual ChannelMaster *createMaster() const = 0;
	virtual ChannelSlave *createSlave() const = 0;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_CHANNEL_MIRROR_H__

