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
#include <SimNet/MessageQueue.h>
#include <SimNet/RoutingHandler.h>
#include <SimNet/DispatchHandler.h>

#include <SimData/Ref.h>
#include <SimData/Timing.h>


namespace simnet {


ClientServerBase::ClientServerBase(NetworkNode const &bind, bool isServer, int inbound_bw, int outbound_bw):
	m_LocalNode(bind)
{
	// TODO set bandwidth
	m_NetworkInterface = new NetworkInterface();
	m_NetworkInterface->initialize(bind, isServer);
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


Server::Server(NetworkNode const &bind, int inbound_bw, int outbound_bw): ClientServerBase(bind, true /*isServer*/, inbound_bw, outbound_bw) {
}

void Server::onConnectionRequest(simdata::Ref<ConnectionRequest> const &msg, simdata::Ref<MessageQueue> const &queue) {
	std::cout << "CONNECTION REQUEST " << *msg << "\n";
	PeerId client_id = msg->getSource();
	ConnectionResponse::Ref response = new ConnectionResponse();
	response->setReliable();
	response->setReplyTo(msg);
	response->setRoutingType(0);
	response->set_success(true);
	response->set_client_id(client_id);
	queue->queueMessage(response);
}

void Server::onAcknowledge(simdata::Ref<Acknowledge> const &msg, simdata::Ref<MessageQueue> const &queue) {
	std::cout << "ACKNOWLEDGED " << *msg << "\n";
	// hack (need to cache connection requests)
	m_NetworkInterface->establishConnection(msg->getSource(), 30000, 3000);
}

void Server::onDisconnect(simdata::Ref<Disconnect> const &msg, simdata::Ref<MessageQueue> const &queue) {
	std::cout << "DISCONNECT " << *msg << "\n";
	m_NetworkInterface->disconnectClient(msg->getSource());
}

Client::Client(NetworkNode const &bind, int inbound_bw, int outbound_bw):
	ClientServerBase(bind, false /*isServer*/, inbound_bw, outbound_bw),
	m_Connected(false)
{
}

Client::~Client() {
}

bool Client::connectToServer(NetworkNode const &server, double timeout) {
	assert(!m_Connected);
	m_NetworkInterface->setServer(server, 10000, 10000);

	// prepare and queue request
	ConnectionRequest::Ref request = new ConnectionRequest();
	request->setDestination(1);
	request->setReliable();
	request->setRoutingType(0);
	request->setRoutingData(getLocalNode().getPort());
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
	std::cout << "CONNECTION RESPONSE " << *msg << "\n";
	if (m_Connected) return;
	if (!msg->has_success() || !msg->success() || !msg->has_client_id()) {
		if (msg->has_response()) {
			SIMNET_LOG(HANDSHAKE, ERROR, "connection failed: " << msg->response());
		}
		// TODO set m_Connected to indicate failure
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
	}
}

} // namespace simnet