// Combat Simulator Project - FlightSim Demo
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
 * @file NetworkInterface.h
 *
 */

#ifndef __SIMNET_NETWORKINTERFACE_H__
#define __SIMNET_NETWORKINTERFACE_H__

#include <SimData/ScopedPointer.h>
#include <SimData/Ref.h>

#include <SimNet/NetBase.h>
#include <SimNet/HandlerSet.h>
#include <SimNet/PacketHandler.h>
#include <SimNet/PacketSource.h>
#include <SimNet/PacketQueue.h>


namespace simnet {

// forward declarations
class NetworkNode;
class DatagramReceiveSocket;
class DatagramTransmitSocket;
class ReliablePacket;
class PeerInfo;
class ActivePeerList;


class NetworkInterface: public simdata::Referenced {

	struct BufferInfo {
		DatagramTransmitSocket *socket;
	};

	// reserved message ids
	static const simdata::uint16 PingID = 65535;

	static const simdata::uint32 PeerIndexSize = 4096;
	static const simdata::uint32 MaxPayloadLength = 500;
	static const simdata::uint32 InfoSize = sizeof(BufferInfo);
	static const simdata::uint32 HeaderSize = sizeof(PacketHeader);
	static const simdata::uint32 ReceiptHeaderSize = sizeof(PacketReceiptHeader);

	PacketQueue *m_RxQueues[4];
	PacketQueue *m_TxQueues[4];

	PeerId m_ServerId;
	PeerInfo *m_PeerIndex;

	simdata::ScopedPointer<ActivePeerList> m_ActivePeers;

	bool m_Initialized;

	simdata::ScopedPointer<DatagramReceiveSocket> m_Socket;

	// temporary hack
	simdata::ScopedPointer<NetworkNode> m_RemoteNode;
	simdata::ScopedPointer<NetworkNode> m_LocalNode;

	HandlerSet<PacketHandler> m_PacketHandlers;
	// may want more than one eventually
	PacketSource::Ref m_PacketSource;

	simdata::uint32 m_OutputStalls;
	simdata::uint32 m_SentPackets;
	simdata::uint32 m_ReceivedPackets;
	simdata::uint32 m_BadPackets;
	simdata::uint32 m_DroppedPackets;
	simdata::uint32 m_ThrottledPackets;

	double m_LastUpdate;

	// internal methods for shuttling data to and from the sockets
	void sendPackets(double timeout);
	int receivePackets(double timeout);

	// bandwidths are in bytes per second
	void addPeer(PeerId id, NetworkNode const &remote_node, double incoming_bw, double outgoing_bw);
	void removePeer(PeerId id);

public:

	typedef simdata::Ref<NetworkInterface> Ref;

	NetworkInterface();
	~NetworkInterface();

	void initialize(NetworkNode const &local_node);
	void resetStats();

	// TODO internalize
	void setServerId(PeerId id);
	void hackPeerIndex(PeerId id, NetworkNode const &remote_node, double incoming_bw, double outgoing_bw);
	void setPacketSource(PacketSource::Ref source) { m_PacketSource = source; }
	bool pingPeer(PeerInfo *peer);
	void resend(simdata::Ref<ReliablePacket> &packet);

	void processIncoming(double timeout);
	void processOutgoing(double timeout);

	void addPacketHandler(PacketHandler::Ref handler);
	void removePacketHandler(PacketHandler::Ref handler);

};

} // namespace simnet

#endif // __SIMNET_NETWORKINTERFACE_H__
