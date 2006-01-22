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
 * @file GlobalBattlefield.h
 *
 **/

#ifndef __CSPSIM_BATTLEFIELD_GLOBALBATTLEFIELD_H__
#define __CSPSIM_BATTLEFIELD_GLOBALBATTLEFIELD_H__

#include <csp/cspsim/battlefield/SimObject.h>
#include <csp/cspsim/battlefield/Battlefield.h>
#include <csp/cspsim/battlefield/BattlefieldMessages.h>

#include <csp/csplib/net/NetBase.h>
#include <csp/csplib/net/ClientServer.h>
#include <csp/csplib/net/PeerInfo.h>

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/util/Timing.h>

#include <set>

CSP_NAMESPACE

class Server;

// TODO when a peer is dropped, remove its objects from the battlefield.
// (eventually they should be reassigned to an agent server.)


class UnitContact: public SimObject {
	Path m_Path;
public:
	UnitContact(SimObject::TypeId type, Path const &path, SimObject::ObjectId id): SimObject(type), m_Path(path) {
		setId(id);
		setContact();
	}
	virtual Vector3 getGlobalPosition() const { assert(0); return Vector3::ZERO; }
	virtual Path getObjectPath() const { return m_Path; }
};


struct ClientData {
	std::string user_name;
	PeerId id;
	std::set<SimObject::ObjectId> units;
	uint32 internal_ip_addr;
	uint32 external_ip_addr;
};


class GlobalBattlefield: public Battlefield {
	unsigned int m_NextId;
	typedef std::map<PeerId, ClientData> ClientDataMap;

	template <class MSG>
	class ClientResponse {
		Ref<MSG> m_Response;
	public:
		ClientResponse(Ref<NetworkMessage> const &request): m_Response(new MSG()) {
			m_Response->setReplyTo(request);
			m_Response->setReliable();
			m_Response->setRoutingType(ROUTE_COMMAND);
		}
		inline Ref<MSG> const &operator->() { return m_Response; }
		inline void send(Ref<MessageQueue> const &queue) {
			queue->queueMessage(m_Response);
		}
	};


	class ContactWrapper: public UnitWrapper {
		// map from peer id to unit count
		typedef std::map<int, int> UpdateCount;
		// the number of units per peer that this unit updates
		UpdateCount m_UpdateCount;

	public:
		ContactWrapper(Unit const &u, PeerId owner): UnitWrapper(u, owner) { }
		ContactWrapper(ObjectId id, Path const &path, PeerId owner): UnitWrapper(id, path, owner) { }
		Ref<UnitContact> contact() { return static_cast<UnitContact*>(object().get()); }
		inline bool isUpdating(int peer) {
			return m_UpdateCount.find(peer) != m_UpdateCount.end();
		}
		inline bool incrementCount(int peer) {
			UpdateCount::iterator iter = m_UpdateCount.find(peer);
			if (iter == m_UpdateCount.end()) {
				m_UpdateCount[peer] = 1;
				return true;
			}
			iter->second += 1;
			return false;
		}
		inline bool decrementCount(int peer) {
			UpdateCount::iterator iter = m_UpdateCount.find(peer);
			if (iter == m_UpdateCount.end()) {
				// TODO log an error
				return true;
			}
			if (iter->second == 1) {
				m_UpdateCount.erase(iter);
				return true;
			}
			iter->second -= 1;
			return false;
		}
	};


public:

	GlobalBattlefield(): m_NextId(2000) { }
	~GlobalBattlefield() { }

	void update(double dt) {
		while (m_NetworkServer->hasDisconnectedPeers()) {
			PeerId id = m_NetworkServer->nextDisconnectedPeerId();
			removeClient(id);
		}
		Battlefield::update(dt);
	}

	void setNetworkServer(Ref<Server> const &server) {
		assert(m_NetworkServer.isNull());
		assert(server.valid());
		m_NetworkServer = server;
		Ref<DispatchHandler> dispatch = new DispatchHandler(server->queue());
		bindCommandDispatch(dispatch);
		server->routeToHandler(ROUTE_COMMAND, dispatch);
	}


private:

	virtual void _updateUnitPositions() {
		// NOOP: prevents Battlefield from wasting time trying to update positions
	}

	void removeClient(PeerId id) {
		ClientDataMap::iterator data = m_ClientData.find(id);
		if (data != m_ClientData.end()) {
			std::set<SimObject::ObjectId> const &units = data->second.units;
			CSPLOG(INFO, BATTLEFIELD) << "Removing client " << id << " with " << units.size() << " units";
			std::set<SimObject::ObjectId>::const_iterator uiter = units.begin();
			std::set<SimObject::ObjectId>::const_iterator uend = units.end();
			for (; uiter != uend; ++uiter) {
				removeUnit(*uiter);
			}
			m_ClientData.erase(data);
		} else {
			CSPLOG(ERROR, BATTLEFIELD) << "Removing client " << id << " but client data not found!";
		}
		announceExit(id);
	}

	void sendClientCommand(Ref<NetworkMessage> const &command, PeerId client_id) {
		command->setDestination(client_id);
		command->setRoutingType(ROUTE_COMMAND);
		command->setReliable();
		m_NetworkServer->queue()->queueMessage(command, client_id);
	}

	void bindCommandDispatch(Ref<DispatchHandler> const &dispatch) {
		dispatch->registerHandler(this, &GlobalBattlefield::onJoinRequest);
		dispatch->registerHandler(this, &GlobalBattlefield::onIdAllocationRequest);
		dispatch->registerHandler(this, &GlobalBattlefield::onNotifyUnitMotion);
		dispatch->registerHandler(this, &GlobalBattlefield::onRegisterUnit);
	}

	Ref<PlayerJoin> makeJoin(const PeerId id, bool local) {
		Ref<PlayerJoin> join = new PlayerJoin();
		join->setRoutingType(ROUTE_COMMAND);
		join->setReliable();
		ClientData &data = m_ClientData[id];
		PeerInfo const *peer_info = m_NetworkServer->getPeer(id);
		join->set_peer_id(id);
		join->set_user_name(data.user_name);
		join->set_outgoing_bw(static_cast<int>(peer_info->outgoingBandwidth()));
		join->set_incoming_bw(static_cast<int>(peer_info->incomingBandwidth()));
		join->set_port(peer_info->getNode().getPort());
		join->set_ip_addr(local ? data.internal_ip_addr : data.external_ip_addr);
		return join;
	}

	void announceJoin(const PeerId id) {
		ClientData &new_client_data = m_ClientData[id];
		Ref<PlayerJoin> local_join = makeJoin(id, true);
		Ref<PlayerJoin> remote_join = makeJoin(id, false);
		for (ClientDataMap::const_iterator iter = m_ClientData.begin(); iter != m_ClientData.end(); ++iter) {
			// don't announce to ourself
			if (iter->first == id) continue;

			// if the external ips match, these hosts are probably on a LAN and should communicate via their
			// internal interfaces; otherwise use the external ip.
			Ref<PlayerJoin> join;
			bool local = (iter->second.external_ip_addr == new_client_data.external_ip_addr);
			m_NetworkServer->queue()->queueMessage(local ? local_join : remote_join, iter->first);
		}
	}

	void announceExit(const PeerId id) {
		Ref<PlayerQuit> msg = new PlayerQuit();
		msg->setRoutingType(ROUTE_COMMAND);
		msg->set_peer_id(id);
		for (ClientDataMap::const_iterator iter = m_ClientData.begin(); iter != m_ClientData.end(); ++iter) {
			if (iter->first == id) continue;
			m_NetworkServer->queue()->queueMessage(msg, iter->first);
		}
	}


	// FIXME sends one message for each existing player (should be batched into one, or a few, messages).
	void announceExistingPlayers(const PeerId new_id) {
		ClientData &new_client_data = m_ClientData[new_id];
		for (ClientDataMap::const_iterator iter = m_ClientData.begin(); iter != m_ClientData.end(); ++iter) {
			const PeerId id = iter->first;
			if (id == new_id) continue;
			bool local = (iter->second.external_ip_addr == new_client_data.external_ip_addr);
			Ref<PlayerJoin> join = makeJoin(id, local);
			m_NetworkServer->queue()->queueMessage(join, new_id);
		}
	}

	void onJoinRequest(Ref<JoinRequest> const &msg, Ref<MessageQueue> const &queue) {
		CSPLOG(INFO, BATTLEFIELD) << "received join request";
		ClientResponse<JoinResponse> response(msg);
		response->set_success(false);

		if (!msg->has_user_name()) {
			CSPLOG(ERROR, BATTLEFIELD) << "join rejected: no user name";
			response->set_details("no user name");
			response.send(queue);
			return;
		}

		if (msg->has_local_time()) {
			int skew = static_cast<int>(static_cast<double>(msg->local_time()) - getSecondsSinceUnixEpoch());
			// 10 seconds is somewhat arbitrary, but reasonable
			if (std::abs(skew) > 10) {
				CSPLOG(ERROR, BATTLEFIELD) << "join rejected: large clock skew (" << skew << "s)";
				response->set_details("excessive clock skew");
				response.send(queue);
				return;
			}
		} else {
			CSPLOG(WARNING, BATTLEFIELD) << "join request missing local time";
		}

		PeerId id = msg->getSource();
		PeerInfo const *peer_info = m_NetworkServer->getPeer(id);

		uint32 inbound_ip_addr = peer_info->getNode().getIp();
		uint32 internal_ip_addr = msg->has_internal_ip_addr() ? msg->internal_ip_addr() : inbound_ip_addr;

		CSPLOG(INFO, BATTLEFIELD) << "join request from " << peer_info->getNode();
		CSPLOG(INFO, BATTLEFIELD) << "      internal ip " << NetworkNode::ipToString(internal_ip_addr);

		// basic sanity checking on ip addresses
		if (NetworkNode::isRoutable(internal_ip_addr) && (internal_ip_addr != inbound_ip_addr)) {
			CSPLOG(ERROR, BATTLEFIELD) << "join rejected: internal ip routable, but does not match external ip";
			response->set_details("internal ip routable, but does not match external ip");
			response.send(queue);
			return;
		}
		if (!NetworkNode::isRoutable(inbound_ip_addr) && (internal_ip_addr != inbound_ip_addr)) {
			CSPLOG(ERROR, BATTLEFIELD) << "join rejected: inbound ip unroutable, but does not match internal ip";
			response->set_details("inbound ip is unroutable, but does not match internal ip");
			response.send(queue);
			return;
		}
		if (NetworkNode::isRoutable(inbound_ip_addr) && (!m_NetworkServer->getExternalNode().isRoutable())) {
			CSPLOG(ERROR, BATTLEFIELD) << "join rejected: inbound ip routable, but server is not configured to accept remote connections (no external ip set).";
			response->set_details("server not configured to accept remote connections");
			response.send(queue);
			return;
		}

		// the correct "external" ip for remote clients is the ip the packets come from.  for clients
		// on the same lan, use the server's "external" ip address.  this simplifies the configuration,
		// since only the server needs to specify an external ip address; the clients can just bind to
		// their local interfaces and the server will decide which ip to use when introducing two peers.
		uint32 external_ip_addr = NetworkNode::isRoutable(inbound_ip_addr) ? inbound_ip_addr : m_NetworkServer->getExternalNode().getIp();

		ClientData &data = m_ClientData[id];
		response->set_success(true);
		response->set_details("welcome " + msg->user_name());
		response->set_first_id(m_NextId);
		response->set_id_count(100);
		m_NextId += 100;
		data.id = id;
		data.units.clear();
		data.user_name = msg->user_name();
		data.internal_ip_addr = internal_ip_addr;
		data.external_ip_addr = external_ip_addr;

		response.send(queue);
		announceJoin(id);
		announceExistingPlayers(id);
	}

	void onIdAllocationRequest(Ref<IdAllocationRequest> const &msg, Ref<MessageQueue> const &queue) {
		CSPLOG(INFO, BATTLEFIELD) << "allocating ids";
		ClientResponse<IdAllocationResponse> response(msg);
		response->set_first_id(m_NextId);
		response->set_id_count(100);
		m_NextId += 100;
		response.send(queue);
	}

	void onNotifyUnitMotion(Ref<NotifyUnitMotion> const &msg, Ref<MessageQueue> const &/*queue*/) {
		UnitWrapper *wrapper = findUnitWrapper(msg->unit_id());
		if (wrapper) {
			CSPLOG(INFO, BATTLEFIELD) << "unit moved " << *(wrapper->unit());
			GridPoint old_position = wrapper->point();
			GridPoint new_position(msg->grid_x(), msg->grid_y());
			updatePosition(wrapper, new_position);
		} else {
			CSPLOG(ERROR, BATTLEFIELD) << "unknown unit moved " << msg->unit_id();
		}
	}

	virtual void moveUnit(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position) {
		Battlefield::moveUnit(wrapper, old_position, new_position);
		recomputeUpdates(wrapper, old_position, new_position);
	}

	void onRegisterUnit(Ref<RegisterUnit> const &msg, Ref<MessageQueue> const &/*queue*/) {
		const SimObject::ObjectId unit_id = msg->unit_id();
		const PeerId owner = msg->getSource();
		UnitWrapper *wrapper = findUnitWrapper(unit_id);
		if (wrapper) {
			CSPLOG(ERROR, BATTLEFIELD) << "request to register existing unit " << unit_id << " for " << owner << " (already owned by " << wrapper->owner() << ")";
			// wrapper != null could be a client side bug or a duplicate message that wasn't filtered.
			// either is bad enough that we want to fail (in debug mode).
			assert(0);
			return;
		}
		CSPLOG(INFO, BATTLEFIELD) << "registering unit " << unit_id << " " << msg->unit_class() << " owned by " << owner;
		Ref<UnitContact> unit = new UnitContact(static_cast<SimObject::TypeId>(msg->unit_type()), msg->unit_class(), unit_id);
		wrapper = new ContactWrapper(unit, owner);
		addUnit(wrapper);
		GridPoint old_position = wrapper->point();
		GridPoint new_position(msg->grid_x(), msg->grid_y());
		updatePosition(wrapper, new_position);
		m_ClientData[owner].units.insert(unit_id);
	}
	
	/** Recompute unit update messages sent between peers when a unit moves.
	 *
	 *  The basic approach is as follows.  First find all units within a certain
	 *  radius around the unit that has moved.  The choice of radius is somewhat
	 *  tricky, but in general it will be somewhat larger that a typical air-air
	 *  radar range.  (Units and fixed installations with very long range radar
	 *  will be handled separately.)  Call this set of units {B_j}, and call the
	 *  unit that moved A.  In addition, let PEER[U] represent the peer that
	 *  controls unit U.
	 *
	 *  Since A has moved, PEER[A] may need to begin or stop sending updates of
	 *  A's position to {PEER[B_j]}.  In addition, the rate at which PEER[A] sends
	 *  updates may need to be adjusted.  The same is true of updates about
	 *  {B_j} flowing to PEER[A].
	 *
	 *  When two peers are updating each other about a pair of units, they are
	 *  able to manage the update rates without help from the global battlefield.
	 *  Initially, however, one peer may be doing all the updates (e.g. a ground
	 *  unit updating an aircraft that has long range radar).  In this case, the
	 *  global battlefield provides hints about the aircraft position to the
	 *  client that controls the ground unit, allowing it to choose a reasonable
	 *  update rate (ie. infrequent at long range, rapid at close range).
	 *
	 *  In order to know which messages to send, the global battlefield keeps
	 *  track of which units are updating which peers.  The tracking is done
	 *  by maintaining a count for each (Unit, Client) pair.  The count is the
	 *  number of units controlled by the Client that the Unit is updating.  When
	 *  this count transitions from 0->1, the global battlefield sends a message
	 *  to the Client to expect peer updates for the Unit.  When the count
	 *  transitions from 1->0, the global battlefield informs the Client to drop
	 *  the Unit.
	 */
	void recomputeUpdates(UnitWrapper *wrapper, GridPoint const old_position, GridPoint const new_position) {
		CSPLOG(INFO, BATTLEFIELD) << "recomputing updates for " << *(wrapper->unit());

		// find all nearby objects.  the query range will be chosen to be somewhat larger than the most common
		// radar range (say 60 nm for A-A).
		// TODO longer range radars will be handled in a separate step.
		int search_radius = 60000; // XXX temporary hack
		std::vector<QuadTreeChild*> units;
		bool unit_exit = isNullPoint(new_position);
		if (unit_exit) {
			assert(!isNullPoint(old_position));
			GridRegion region = makeGridRegionEnclosingCircle(old_position, search_radius);
			dynamicIndex()->query(region, units);
		} else {
			GridRegion region = makeGridRegionEnclosingCircle(new_position, search_radius);
			dynamicIndex()->query(region, units);
		}

		CSPLOG(INFO, BATTLEFIELD) << "found " << units.size() << " nearby units";

		std::set<int> position_hints;  // set of peers that need hints about A's motion
		int radius = 40000;  // XXX temporary hack

		for (unsigned i = 0; i < units.size(); ++i) {
			UnitWrapper *other_wrapper = static_cast<UnitWrapper*>(units[i]);
			if (other_wrapper == wrapper) continue;
			int other_radius = 40000;  // XXX temporary hack
			int old_separation = static_cast<int>(sqrt(globalDistance2(other_wrapper->point(), old_position)));
			int new_separation = static_cast<int>(sqrt(globalDistance2(other_wrapper->point(), new_position)));
			CSPLOG(INFO, BATTLEFIELD) << "separations (old, new): " << old_separation << ", " << new_separation;

			// consider updates from other_wrapper to wrapper
			if (new_separation > radius) {
				if (old_separation <= radius) {
					decrementUpdateCount(other_wrapper, wrapper);
				}
			} else {
				if (old_separation > radius) {
					incrementUpdateCount(other_wrapper, wrapper);
				} else {  // B stayed within A's range, may need to tell B about A
					if (!isUpdatingPeer(wrapper, other_wrapper)) {
						position_hints.insert(other_wrapper->owner());
					}
				}
			}

			// consider updates from wrapper to other_wrapper
			if (new_separation > other_radius) {
				if (old_separation <= other_radius) {
					decrementUpdateCount(wrapper, other_wrapper);
				}
			} else {
				if (old_separation > other_radius) {
					incrementUpdateCount(wrapper, other_wrapper);
				}
			}
		}

		for (std::set<int>::const_iterator iter = position_hints.begin(); iter != position_hints.end(); ++iter) {
			// TODO send position update for wrapper to client *iter.
		}

	}

	void incrementUpdateCount(UnitWrapper *from, UnitWrapper *to) {
		ContactWrapper *cfrom = static_cast<ContactWrapper*>(from);
		CSPLOG(DEBUG, BATTLEFIELD) << "INCREMENT: " << *(from->unit()) << " -> " << *(to->unit());
		if (cfrom->incrementCount(to->owner())) {
			// if the units are owned by the same client we still reference count
			// but we don't send an update.  both units will already be present in
			// the client's local battlefield.
			if (from->owner() != to->owner()) {
				CSPLOG(INFO, BATTLEFIELD) << "sending add unit " << from->unit()->id() << " to client " << to->owner();
				{  // tell OWNER[to] to expect updates from OWNER[from]
					Ref<CommandAddUnit> msg = new CommandAddUnit();
					msg->set_unit_id(from->id());
					msg->set_unit_class(Path(from->unit()->getObjectPath()));
					msg->set_unit_type(static_cast<const uint8>(from->unit()->type()));
					msg->set_owner_id(from->owner());
					msg->set_grid_x(from->point().x());
					msg->set_grid_y(from->point().y());
					sendClientCommand(msg, to->owner());
				}
				{ // tell OWNER[from] to update OWNER[to]
					Ref<CommandUpdatePeer> msg = new CommandUpdatePeer();
					msg->set_unit_id(from->id());
					msg->set_peer_id(to->owner());
					sendClientCommand(msg, from->owner());
				}
			}
		}
	}

	void decrementUpdateCount(UnitWrapper *from, UnitWrapper *to) {
		ContactWrapper *cfrom = static_cast<ContactWrapper*>(from);
		CSPLOG(DEBUG, BATTLEFIELD) << "DECREMENT: " << *(from->unit()) << " -> " << *(to->unit());
		if (cfrom->decrementCount(to->owner())) {
			// if the units are owned by the same client we still reference count
			// but we don't send an update.  removal and notification of local units
			// has already been done in the client's battlefield.
			if (from->owner() != to->owner()) {
				CSPLOG(INFO, BATTLEFIELD) << "sending remove unit " << from->unit()->id() << " to client " << to->owner();
				{
					Ref<CommandRemoveUnit> msg = new CommandRemoveUnit();
					msg->set_unit_id(from->id());
					sendClientCommand(msg, to->owner());
				}
				{
					Ref<CommandUpdatePeer> msg = new CommandUpdatePeer();
					msg->set_unit_id(from->id());
					msg->set_peer_id(to->owner());
					msg->set_stop(true);
					sendClientCommand(msg, from->owner());
				}
			}
		}
	}

	bool isUpdatingPeer(UnitWrapper* /*from*/, UnitWrapper* /*to*/) const {
		return false;
	}

private:
	Ref<Server> m_NetworkServer;
	ClientDataMap m_ClientData;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_BATTLEFIELD_GLOBALBATTLEFIELD_H__

