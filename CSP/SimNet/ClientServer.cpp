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
 * @file ClientServer.cpp
 *
 */


#include <SimNet/ClientServer.h>
#include <SimNet/NetBase.h>
#include <SimNet/NetworkNode.h>
#include <SimNet/NetworkMessage.h>
#include <SimNet/MessageHandler.h>
#include <SimNet/ClientServerMessages.h>
#include <SimNet/NetworkInterface.h>
#include <SimNet/PacketDecoder.h>
#include <SimNet/PeerInfo.h>
#include <SimNet/MessageQueue.h>
#include <SimNet/RoutingHandler.h>
#include <SimNet/DispatchHandler.h>

#include <SimData/Ref.h>
#include <SimData/Timing.h>


namespace simnet {


ClientServerBase::ClientServerBase(NetworkNode const &bind, bool isServer, int inbound_bw, int outbound_bw) {
	m_NetworkInterface = new NetworkInterface();
	m_NetworkInterface->initialize(bind, isServer, inbound_bw, outbound_bw);
	m_MessageQueue = new MessageQueue();
	m_PacketDecoder = new PacketDecoder();
	m_NetworkInterface->addPacketHandler(m_PacketDecoder);
	m_NetworkInterface->setPacketSource(m_MessageQueue);
	m_RoutingHandler = new RoutingHandler();
	m_PacketDecoder->addMessageHandler(m_RoutingHandler);
	m_HandshakeDispatch = new DispatchHandler(m_MessageQueue);
	m_RoutingHandler->setMessageHandler(0, m_HandshakeDispatch);
	m_HandshakeDispatch->registerHandler(this, &ClientServerBase::onConnectionRequest);
	m_HandshakeDispatch->registerHandler(this, &ClientServerBase::onConnectionResponse);
	m_HandshakeDispatch->registerHandler(this, &ClientServerBase::onAcknowledge);
	m_HandshakeDispatch->registerHandler(this, &ClientServerBase::onDisconnect);
	m_HandshakeDispatch->setDefaultHandler(this, &ClientServerBase::onUnknownMessage);
}

PeerInfo const *ClientServerBase::getPeer(PeerId id) const {
	const NetworkInterface * const ni = m_NetworkInterface.get();
	const PeerInfo *info = ni->getPeer(id);
	return info->isActive() ? info : 0;
}

void ClientServerBase::addPeer(PeerId id, NetworkNode const &remote_node, double incoming_bw, double outgoing_bw) {
	m_NetworkInterface->addPeer(id, remote_node, incoming_bw, outgoing_bw);
}

void ClientServerBase::processIncoming(double timeout) {
	m_NetworkInterface->processIncoming(timeout);
}

void ClientServerBase::processOutgoing(double timeout) {
	m_NetworkInterface->processOutgoing(timeout);
}

void ClientServerBase::processTraffic(double read_timeout, double write_timeout) {
	m_NetworkInterface->processIncoming(read_timeout);
	m_NetworkInterface->processOutgoing(write_timeout);
}

void ClientServerBase::processAndWait(double read_timeout, double write_timeout, double timeout) {
	m_NetworkInterface->waitPending(timeout);
	processTraffic(read_timeout, write_timeout);
}

void ClientServerBase::routeToHandler(RoutingType routing_type, simdata::Ref<MessageHandler> const &handler) {
	m_RoutingHandler->setMessageHandler(routing_type, handler);
}

void ClientServerBase::addDispatchManager(int cache_size) {
	assert(!m_DispatchManager);
	m_DispatchManager = new DispatchManager(m_MessageQueue, cache_size);
}

bool ClientServerBase::dispatch(DispatchTarget *target, NetworkMessage::Ref const &msg) {
	assert(m_DispatchManager.valid());
	return m_DispatchManager->dispatch(target, msg);
}

void ClientServerBase::queueMessage(NetworkMessage::Ref const &msg) {
	m_MessageQueue->queueMessage(msg);
}


Server::Server(NetworkNode const &bind, int inbound_bw, int outbound_bw):
	ClientServerBase(bind, true /*isServer*/, inbound_bw, outbound_bw)
{
}

void Server::onConnectionRequest(simdata::Ref<ConnectionRequest> const &msg, simdata::Ref<MessageQueue> const &queue) {
	SIMNET_LOG(HANDSHAKE, INFO, "connection request " << *msg);
	const PeerId client_id = msg->getSource();
	ConnectionData &connection_data = m_PendingConnections[client_id];
	if (msg->incoming_bw() < 1000 || msg->outgoing_bw() < 1000) {
		SIMNET_LOG(HANDSHAKE, ERROR, "client reports very low bandwidth " << *msg);
	}
	connection_data.incoming_bw = msg->incoming_bw();
	connection_data.outgoing_bw = msg->outgoing_bw();
	ConnectionResponse::Ref response = new ConnectionResponse();
	response->setReliable();
	response->setReplyTo(msg);
	response->setRoutingType(0);
	response->set_success(true);
	response->set_client_id(client_id);
	queue->queueMessage(response);
}

void Server::onAcknowledge(simdata::Ref<Acknowledge> const &msg, simdata::Ref<MessageQueue> const &queue) {
	SIMNET_LOG(HANDSHAKE, DEBUG, "received acknowledgement " << *msg);
	PendingConnectionMap::iterator iter = m_PendingConnections.find(msg->getSource());
	if (iter == m_PendingConnections.end()) {
		SIMNET_LOG(HANDSHAKE, WARNING, "received unsolicited connection acknowledgement from client id " << msg->getSource());
		return;
	}
	SIMNET_LOG(HANDSHAKE, INFO, "adding client " << msg->getSource()
		<< " (bw " << (iter->second.incoming_bw) << "/" << (iter->second.outgoing_bw) << ")");
	m_NetworkInterface->establishConnection(msg->getSource(), iter->second.incoming_bw, iter->second.outgoing_bw);
	m_PendingConnections.erase(iter);
}

void Server::onDisconnect(simdata::Ref<Disconnect> const &msg, simdata::Ref<MessageQueue> const &queue) {
	SIMNET_LOG(HANDSHAKE, DEBUG, "received disconnect notice " << *msg);
	m_NetworkInterface->disconnectPeer(msg->getSource());
}

void Server::setExternalNode(NetworkNode const &external_node) {
	m_NetworkInterface->setExternalNode(external_node);
}

Client::Client(NetworkNode const &bind, int inbound_bw, int outbound_bw):
	ClientServerBase(bind, false /*isServer*/, inbound_bw, outbound_bw),
	m_Connected(false)
{
}

Client::~Client() {
	disconnectFromServer(true);
}

bool Client::connectToServer(NetworkNode const &server, double timeout) {
	assert(!m_Connected);
	// FIXME actual server bandwidth needs to be passed and set in onConnectionResponse
	m_NetworkInterface->setServer(server, 10000, 10000);

	// prepare and queue request
	ConnectionRequest::Ref request = new ConnectionRequest();
	request->setDestination(1);
	request->setReliable();
	request->setRoutingType(0);
	request->setRoutingData(getLocalNode().getPort());
	request->set_incoming_bw(m_NetworkInterface->getNominalIncomingBandwidth());
	request->set_outgoing_bw(m_NetworkInterface->getNominalOutgoingBandwidth());
	m_MessageQueue->queueMessage(request);

	// wait for a response
	simdata::Timer timer;
	timer.start();
	while (!m_Connected && timer.elapsed() < timeout) {
		processAndWait(0.01, 0.01, 0.1);
	}

	return m_Connected;
}

void Client::onConnectionResponse(simdata::Ref<ConnectionResponse> const &msg, simdata::Ref<MessageQueue> const &queue) {
	SIMNET_LOG(HANDSHAKE, DEBUG, "received connection response " << *msg);
	if (m_Connected) return;
	if (!msg->has_success() || !msg->success() || !msg->has_client_id()) {
		if (msg->has_response()) {
			SIMNET_LOG(HANDSHAKE, ERROR, "connection failed: " << msg->response());
		} else {
			SIMNET_LOG(HANDSHAKE, ERROR, "connection failed: " << *msg);
		}
		// TODO set m_Connected to indicate failure
		return;
	}
	if (msg->client_id() < 2) {
		SIMNET_LOG(HANDSHAKE, ERROR, "connection failed: bad id assignment from server");
		// TODO set m_Connected to indicate failure
		return;
	}
	m_NetworkInterface->setClientId(msg->client_id());
	Acknowledge::Ref ack = new Acknowledge();
	ack->setReliable();
	ack->setReplyTo(msg);
	ack->setRoutingType(0);
	queue->queueMessage(ack);
	m_Connected = true;
}

void Client::disconnectFromServer(bool immediate) {
	if (m_Connected) {
		Disconnect::Ref disconnect = new Disconnect();
		disconnect->setDestination(1);
		disconnect->setRoutingType(0);
		if (immediate) {
			disconnect->setPriority(2);
			m_MessageQueue->queueMessage(disconnect);
			m_MessageQueue->queueMessage(disconnect);
			m_NetworkInterface->processOutgoing(0.1);  // TODO flushOutgoing?
		} else {
			disconnect->setReliable();
		}
		m_MessageQueue->queueMessage(disconnect);
		m_Connected = false;
		// TODO need to call disconnectPeer, but only after the outbound messages
		// have been flushed!
	}
}

void Client::sendToServer(NetworkMessage::Ref const &msg) {
	assert(m_Connected);
	msg->setDestination(NetworkInterface::ServerId);
	msg->setReliable();
	queueMessage(msg);
}

void Client::disconnectPeer(PeerId id) {
	m_NetworkInterface->disconnectPeer(id);
}

double Client::getServerTimeOffset() const {
	if (!m_Connected) return 0.0;
	const PeerInfo *info = getPeer(NetworkInterface::ServerId);
	return (info == 0) ? 0.0 : info->getTimeSkew() * 0.001;
}

void Client::setExternalNode(NetworkNode const &external_node) {
	m_NetworkInterface->setExternalNode(external_node);
}

} // namespace simnet
