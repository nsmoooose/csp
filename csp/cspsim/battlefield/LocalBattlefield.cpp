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
 * @file LocalBattlefield.cpp
 *
 **/

#include <csp/cspsim/battlefield/LocalBattlefield.h>
#include <csp/cspsim/battlefield/Battlefield.h>
#include <csp/cspsim/battlefield/SceneManager.h>

#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/DataArchive.h>
#include <csp/csplib/data/DataManager.h>
#include <csp/csplib/net/ClientServer.h>
#include <csp/csplib/net/DispatchHandler.h>
#include <csp/csplib/util/Callback.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/SynchronousUpdate.h>
#include <csp/csplib/util/Timing.h>
#include <csp/csplib/util/Verify.h>

#include <cmath>

namespace csp {

/** Storage class for information about remote players.
 */
class LocalBattlefield::PlayerInfo: public Referenced {
	PeerId m_PeerId;
	std::string m_Name;
public:
	PlayerInfo(PeerId id, std::string const &name): m_PeerId(id), m_Name(name) { }
	std::string const &GetName() const { return m_Name; }
	PeerId GetPeerId() const { return m_PeerId; }
};


/** A helper class for sending unit updates to remote hosts.  A single UpdateProxyConnection
 *  is shared by all UnitUpdateProxy instances.  Provides access to the battlefield's outbound
 *  message queue, as well as a counter that is managed by the battlefield and used by the
 *  unit update proxies to timestamp update messages.
 */
class LocalBattlefield::UpdateProxyConnection: public Referenced {
	Ref<MessageQueue> m_MessageQueue;
	TimeStamp m_TimeStamp;
public:
	/** Construct a UpdateProxyConnection.  Binds the outbound message queue, and initializes
	 *  the timestamp to zero.
	 */
	UpdateProxyConnection(Ref<MessageQueue> queue): m_MessageQueue(queue), m_TimeStamp(0) { }

	/** Set the current timestamp.  This is done by the battlefield prior to updating the
	 *  UnitUpdateProxies.
	 */
	void setTimeStamp(TimeStamp const &time_stamp) { m_TimeStamp = time_stamp; }

	/** Get the current timestamp, which is used to mark outbound unit update messages.
	 */
	inline TimeStamp getTimeStamp() const { return m_TimeStamp; }

	/** Send a message to the specified peer.
	 */
	void send(Ref<NetworkMessage> const &msg, PeerId destination) {
		if (m_MessageQueue.valid()) m_MessageQueue->queueMessage(msg, destination);
	}
};


/** Wrapper for inserting dynamic objects into a quadtree index.
 *
 *  @TODO Keep track of the aggregation bubble count for an object, which
 *  is the number aggregation bubbles projected by human-controlled vehicles
 *  that overlap the object's position.  If this count is greater than zero,
 *  the object should be deaggregated.  This functionality was temporarily
 *  removed for the initial implementation of the distributed battlefield.
 */
class LocalBattlefield::LocalUnitWrapper: public UnitWrapper {
	ScopedPointer<UnitUpdateProxy> m_UpdateProxy;
public:
	LocalUnitWrapper(Unit const &u, PeerId owner): UnitWrapper(u, owner) { }
	LocalUnitWrapper(ObjectId id, Path const &path, PeerId owner): UnitWrapper(id, path, owner) { }
	void setUnit(Unit const &object);
	inline bool hasUpdateProxy() const { return m_UpdateProxy.valid(); }
	void setUpdateProxy(UpdateMaster &master, Ref<UpdateProxyConnection> connection);
	void addPeerUpdate(PeerId id);
	void removePeerUpdate(PeerId id);
	void setUpdateDistance(PeerId id, double distance);
};


/** A helper class for sending updates from a local unit to remote hosts.
 *  An instance of this class is lazily bound to a LocalUnitWrapper when
 *  the wrapped unit is first directed to send remote updates.  The UnitUpdateProxy
 *  instance is registered with an UpdateMaster, which drives the updates.
 */
class LocalBattlefield::UnitUpdateProxy: public UpdateTarget {

	struct PeerUpdateRecord {
		inline PeerUpdateRecord() {}
		PeerUpdateRecord(PeerId id_, uint16_t interval_, uint16_t detail_): id(id_), interval(interval_), next_update(0), detail(detail_) { }
		PeerId id;  // host id to update
		uint16_t interval;  // time between updates, ms
		uint32_t next_update; // ms resolution, 46 day limit
		uint16_t detail; // detail level (not yet used, but important for reducing bandwidth)
		// order of priority
		bool operator < (PeerUpdateRecord const &other) const { return next_update > other.next_update; }
	};

	struct PeerUpdateCache {
		Ref<NetworkMessage> msg;
		uint32_t last_refresh;
	};

	static const int DETAIL_LEVELS = 10;
	PeerUpdateCache m_DetailCache[DETAIL_LEVELS];

	/** the unit (wrapper) sending updates to remote peers */
	LocalUnitWrapper *m_Wrapper;

	/** connection used for sending updates */
	Ref<UpdateProxyConnection> m_Connection;

	/**
	 * a counter used for scheduling update messages to remote hosts.  the count is
	 * milliseconds of elapsed time.  it is currently assumed that this will not
	 * rollover during the simulation time (limit ~50 days)
	 */
	uint32_t m_UpdateTime;

	/**
	 * a heap, ordered by next update time (first element is the earliest update)
	 */
	std::vector<PeerUpdateRecord> m_PeerUpdates;

public:
	/** Construct a new UnitUpdateProxy.
	 *
	 *  @param wrapper The wrapper of the unit that is sending updates.
	 *  @param connection The connection used to send updates to remote hosts.
	 */
	UnitUpdateProxy(LocalUnitWrapper *wrapper, Ref<UpdateProxyConnection> connection);

	/**
	 * Add a peer to the update list.
	 */
	void addPeerUpdate(PeerId id);

	/** 
	 * Remove a peer from the update list.
	 */
	void removePeerUpdate(PeerId id);

	/** 
	 * UpdateMaster callback, which sends updates about the unit to remote hosts.
	 */
	double onUpdate(double dt);

	/** 
	 * Adjust the update interval for the specified peer.  The change will
	 * take effect after the next update.
	 *
	 *  @param id The id of the peer.
	 *  @param interval The interval between updates, in seconds.
	 *  @param detail The level of detail for updates sent to this peer (0-9).
	 */
	void setUpdateParameters(PeerId id, double interval, uint16_t detail);
};


struct LocalBattlefield::ObjectIdPool {
	ObjectIdPool(): next(0), limit(0), reserve(0), reserve_limit(0) { }
	ObjectId next;
	ObjectId limit;
	ObjectId reserve;
	ObjectId reserve_limit;
};


LocalBattlefield::LocalBattlefield(Ref<DataManager> const &data_manager):
	m_ConnectionState(CONNECTION_DETACHED),
	m_DataManager(data_manager),
	m_CameraGridPosition(0,0),
	m_UnitUpdateMaster(new UpdateMaster()),
	m_LocalIdPool(new ObjectIdPool()),
	m_ServerTimeOffset(0),
	m_ScanElapsedTime(0),
	m_ScanRate(0),
	m_ScanIndex(0),
	m_PlayerJoinSignal(new sigc::signal<void(int, const std::string&)>()),
	m_PlayerQuitSignal(new sigc::signal<void(int, const std::string&)>())
{
	/**
	 * if no network connection, ids will be assigned sequentially
	 * starting at 1024.  otherwise, the id pool is initialized when
	 * we connect to the server.
	 */
	m_LocalIdPool->next = 1024;
	m_LocalIdPool->limit = 1000000000;
	// assert(data_manager.valid());
	if (!m_DataManager) {
		CSPLOG(ERROR, BATTLEFIELD) << "No data manager, cannot create objects.";
	}
}

LocalBattlefield::~LocalBattlefield() {
}


typedef std::pair<PeerId, float> PeerContact;
struct PeerContactSorter {
	bool operator()(PeerContact const &a, PeerContact const &b) {
		return (a.first < b.first) || (a.first == b.first && a.second < b.second);
	}
};

void LocalBattlefield::scanUnit(LocalUnitWrapper *wrapper) {
	if (wrapper == NULL) {
		CSPLOG(DEBUG, BATTLEFIELD) << "scan update, skipping removed unit";
		return;
	}
	GridPoint point = wrapper->point();
	if (!isNullPoint(point)) {
		Unit unit = wrapper->unit();
		CSPLOG(INFO, BATTLEFIELD) << "scan update for " << *unit;
		GridRegion region = makeGridRegionEnclosingCircle(point, 60000);
		std::vector<QuadTreeChild*> contacts;
		dynamicIndex()->query(region, contacts);
		const unsigned n_contacts = contacts.size();
		std::vector<PeerContact> peer_contacts;
		unit->m_ContactList.resize(0);
		peer_contacts.reserve(n_contacts);
		Vector3 unit_position = unit->getGlobalPosition();
		uint32_t signature = 0;
		for (unsigned i=0; i < n_contacts; ++i) {
			LocalUnitWrapper *contact = static_cast<LocalUnitWrapper*>(contacts[i]);
			if (contact->id() == wrapper->id()) continue;
			if (!contact->unit()) continue;
			float distance = static_cast<float>((contact->unit()->getGlobalPosition() - unit_position).length2());
			peer_contacts.push_back(PeerContact(contact->owner(), distance));
			unit->m_ContactList.push_back(contact->id());
			signature = make_unordered_fingerprint(signature, hash_uint32(static_cast<uint32>(contact->id())));
		}
		unit->m_ContactSignature = signature;
		CSPLOG(INFO, BATTLEFIELD) << "found " << peer_contacts.size() << " nearby units";
		if (!peer_contacts.empty()) {
			std::sort(peer_contacts.begin(), peer_contacts.end(), PeerContactSorter());
			PeerId current_id = 0;
			for (std::vector<PeerContact>::const_iterator iter = peer_contacts.begin(); iter != peer_contacts.end(); ++iter) {
				if (iter->first != current_id) {
					current_id = iter->first;
					float distance = sqrt(iter->second);
					CSPLOG(INFO, BATTLEFIELD) << "nearest unit owned by " << current_id << " is " << distance << " meters";
					wrapper->setUpdateDistance(current_id, distance);
				}
			}
		}
	} else {
		CSPLOG(DEBUG, BATTLEFIELD) << "scan update, skipping unit outside battlefield";
	}
}

void LocalBattlefield::continueUnitScan(double dt) {
	static const double loop_time = 3.0;  // seconds
	m_ScanElapsedTime += dt;
	//XXX
	unsigned target_index = static_cast<unsigned>(floor(m_ScanElapsedTime * m_ScanRate + 0.5));
	if (m_ScanIndex > target_index) return;
	if (m_ScanIndex == m_ScanUnits.size()) {
		if (m_ScanElapsedTime < loop_time) return;
		if (m_ScanSignature != getLocalUnitsSignature()) {
			m_ScanSignature = getLocalUnitsSignature();
			getLocalUnits(m_ScanUnits);
			m_ScanRate = m_ScanUnits.size() / loop_time;
		}
		m_ScanIndex = 0;
		m_ScanElapsedTime = 0;
	} else {
		for ( ; m_ScanIndex < target_index && m_ScanIndex < m_ScanUnits.size(); ++m_ScanIndex) {
			Unit unit = m_ScanUnits[m_ScanIndex];
			LocalUnitWrapper *wrapper = findLocalUnitWrapper(unit->id());
			scanUnit(wrapper);
		}
	}
}

void LocalBattlefield::update(double dt) {
	double offset = m_NetworkClient.valid() ? m_NetworkClient->getServerTimeOffset() : 0.0;
	double filter = std::min(1.0, dt);
	/**
	 * server time offset changes discontinuously when pings are received (with especially
	 * large jumps right after the connection is established), so we filter the offset
	 * to spread the jumps out over a few seconds.
	 */
	m_ServerTimeOffset = m_ServerTimeOffset * (1.0 - filter) + filter * offset;
	if (m_NetworkClient.valid()) {
		static int XXX = 0;
		if ((++XXX % 1000) == 0) std::cout << "LocalBattlefield.cpp: server time offset = " << m_ServerTimeOffset << "s\n";
	}
	m_CurrentTime = getCalibratedRealTime() + m_ServerTimeOffset;
	m_CurrentTimeStamp = getTimeStamp(m_CurrentTime);
	if (m_NetworkClient.valid()) {
		m_NetworkClient->processIncoming(0.01);
	}
	m_UnitUpdateMaster->update(dt);
	Battlefield::update(dt);
	continueUnitScan(dt);
	if (m_UnitRemoteUpdateMaster.valid()) {
		m_UpdateProxyConnection->setTimeStamp(m_CurrentTimeStamp);
		m_UnitRemoteUpdateMaster->update(dt);
	}
	if (m_NetworkClient.valid()) {
		m_NetworkClient->processOutgoing(0.01);
	}
}

void LocalBattlefield::setSceneManager(Ref<SceneManager> const &manager) {
	assert(m_SceneManager.isNull());
	m_SceneManager = manager;
}

void LocalBattlefield::setNetworkClient(Ref<Client> const &client) {
	assert(unitCount() == 0);
	if (client.isNull()) {
		assert(m_NetworkClient.valid());
		assert(!isConnectionActive());
		m_NetworkClient = 0;
		m_ConnectionState = CONNECTION_DETACHED;
		m_UpdateProxyConnection = 0;
		m_UnitRemoteUpdateMaster.reset(0);
		return;
	}
	assert(m_NetworkClient.isNull());
	assert(client.valid() && client->isConnected());
	assert(m_ConnectionState == CONNECTION_DETACHED);
	m_NetworkClient = client;
	Ref<DispatchHandler> dispatch = new DispatchHandler(client->queue());
	bindCommandDispatch(dispatch);
	client->routeToHandler(ROUTE_COMMAND, dispatch);
	client->routeToCallback(ROUTE_UNIT_UPDATE, this, &LocalBattlefield::onUnitUpdate);
	m_UpdateProxyConnection = new UpdateProxyConnection(client->queue());
	m_UnitRemoteUpdateMaster.reset(new UpdateMaster());
}

/** @TODO add to .h and uncomment
 * 
@code void LocalBattlefield::resetConnectionState() {
	assert(m_NetworkClient.valid());
	assert(!isConnectionActive());
	m_ConnectionState = CONNECTION_DETACHED;
	@endcode
}
*/

void LocalBattlefield::connectToServer(std::string const &name) {
	assert(m_NetworkClient.valid());
	assert(m_ConnectionState == CONNECTION_DETACHED);
	Ref<JoinRequest> msg = new JoinRequest();
	msg->set_user_name(name);
	msg->set_internal_ip_addr(m_NetworkClient->getLocalNode().getIp());
	msg->set_external_ip_addr(m_NetworkClient->getExternalNode().getIp());
	msg->set_local_time(static_cast<uint32>(getSecondsSinceUnixEpoch()));
	sendServerCommand(msg);
	m_ConnectionState = CONNECTION_JOIN;
}

void LocalBattlefield::bindCommandDispatch(Ref<DispatchHandler> const &dispatch) {
	dispatch->registerHandler(this, &LocalBattlefield::onPlayerJoin);
	dispatch->registerHandler(this, &LocalBattlefield::onPlayerQuit);
	dispatch->registerHandler(this, &LocalBattlefield::onCommandUpdatePeer);
	dispatch->registerHandler(this, &LocalBattlefield::onCommandAddUnit);
	dispatch->registerHandler(this, &LocalBattlefield::onCommandRemoveUnit);
	dispatch->registerHandler(this, &LocalBattlefield::onJoinResponse);
	dispatch->registerHandler(this, &LocalBattlefield::onIdAllocationResponse);
}

void LocalBattlefield::onPlayerQuit(Ref<PlayerQuit> const &msg, Ref<MessageQueue> const &) {
	CSPLOG(INFO, MESSAGE) << *msg;
	const PeerId id = msg->peer_id();
	PlayerInfoMap::iterator iter = m_PlayerInfoMap.find(id);
	if (iter != m_PlayerInfoMap.end()) {
		CSPLOG(INFO, BATTLEFIELD) << iter->second->GetName() << " just quit the game! (id " << id << ")";
		m_PlayerQuitSignal->emit(id, iter->second->GetName());
		m_PlayerInfoMap.erase(iter);
	} else {
		CSPLOG(WARNING, BATTLEFIELD) << "received quit message for unmapped peer " << id;
	}
	if (m_NetworkClient->getPeer(id)) {
		m_NetworkClient->disconnectPeer(id);
		/**
		 * the global battlefield should send separate messages to remove
		 * all objects owned by this peer
		 */
	} else {
		CSPLOG(WARNING, BATTLEFIELD) << "received quit message for unknown peer " << id;
	}
}

void LocalBattlefield::registerPlayerJoinCallback(callback<void(int, const std::string&)> &callback) {
	callback.bind(*m_PlayerJoinSignal);
}

void LocalBattlefield::registerPlayerQuitCallback(callback<void(int, const std::string&)> &callback) {
	callback.bind(*m_PlayerQuitSignal);
}

void LocalBattlefield::onPlayerJoin(Ref<PlayerJoin> const &msg, Ref<MessageQueue> const &) {
	CSPLOG(INFO, MESSAGE) << *msg;
	const PeerId id = msg->peer_id();
	if (!m_NetworkClient->getPeer(id)) {
		const NetworkNode remote_node(msg->ip_addr(), msg->port());
		CSPLOG(INFO, BATTLEFIELD) << msg->user_name() << " just joined the game! (id " << id << ", ip " << remote_node << ")";
		const int incoming_bw = msg->incoming_bw();
		const int outgoing_bw = msg->outgoing_bw();
		m_PlayerInfoMap[id] = new PlayerInfo(id, msg->user_name());
		m_NetworkClient->addPeer(id, remote_node, incoming_bw, outgoing_bw);
		m_PlayerJoinSignal->emit(id, msg->user_name());
	}
}

void LocalBattlefield::onCommandUpdatePeer(Ref<CommandUpdatePeer> const &msg, Ref<MessageQueue> const &) {
	CSPLOG(INFO, MESSAGE) << *msg;
	LocalUnitWrapper *wrapper = findLocalUnitWrapper(msg->unit_id());
	if (wrapper) {
		assert(wrapper->unit().valid() && wrapper->unit()->isLocal());
		if (msg->has_stop() && msg->stop()) {
			wrapper->removePeerUpdate(msg->peer_id());
		} else {
			if (!wrapper->hasUpdateProxy()) {
				CSPLOG(ERROR, BATTLEFIELD) << "creating update proxy for unit " << msg->unit_id();
				wrapper->setUpdateProxy(*m_UnitRemoteUpdateMaster, m_UpdateProxyConnection);
			}
			wrapper->addPeerUpdate(msg->peer_id());
		}
	} else {
		CSPLOG(ERROR, BATTLEFIELD) << "received update peer request for unknown unit " << msg->unit_id();
	}
}

void LocalBattlefield::onCommandAddUnit(Ref<CommandAddUnit> const &msg, Ref<MessageQueue> const &) {
	CSPLOG(INFO, MESSAGE) << *msg;
	LocalUnitWrapper *wrapper = findLocalUnitWrapper(msg->unit_id());
	if (wrapper) {
		/** @todo already added.  for testing, double check that the message is consistent (FIXME) */
		CSP_VERIFY_EQ(msg->owner_id(), wrapper->owner());
		CSPLOG(INFO, BATTLEFIELD) << "unit already exists, disregarding duplicate message";
		return;
	}
	wrapper = new LocalUnitWrapper(msg->unit_id(), msg->unit_class(), msg->owner_id());
	addUnit(wrapper);
	updatePosition(wrapper, GridPoint(msg->grid_x(), msg->grid_y()));
}

void LocalBattlefield::onCommandRemoveUnit(Ref<CommandRemoveUnit> const &msg, Ref<MessageQueue> const &) {
	CSPLOG(INFO, MESSAGE) << *msg;
	removeUnit(msg->unit_id());
}

void LocalBattlefield::sendServerCommand(Ref<NetworkMessage> const &msg) {
	msg->setRoutingType(ROUTE_COMMAND);
	m_NetworkClient->sendToServer(msg);
}

void LocalBattlefield::onJoinResponse(Ref<JoinResponse> const &msg, Ref<MessageQueue> const &) {
	CSPLOG(INFO, MESSAGE) << *msg;
	if (m_ConnectionState != CONNECTION_JOIN) {
		CSPLOG(ERROR, BATTLEFIELD) << "received unrequested join response";
		return;
	}
	if (!msg->success()) {
		CSPLOG(ERROR, BATTLEFIELD) << "battlefield failed to connect to server: " << msg->details();
		assert(0);
	}
	CSPLOG(INFO, BATTLEFIELD) << "battlefield connected to server: " << msg->details();
	assert(msg->first_id() > 0);
	assert(msg->id_count() > 63);
	m_LocalIdPool->next = msg->first_id();
	m_LocalIdPool->limit = msg->first_id() + msg->id_count();
	/** allocate reserve ids */
	sendServerCommand(new IdAllocationRequest());
	m_ConnectionState = CONNECTION_ACTIVE;
}

void LocalBattlefield::onIdAllocationResponse(Ref<IdAllocationResponse> const &msg, Ref<MessageQueue> const &) {
	CSPLOG(INFO, MESSAGE) << *msg;
	if (static_cast<ObjectId>(msg->first_id()) == m_LocalIdPool->reserve) return;  // duplicate
	assert(m_LocalIdPool->reserve_limit == 0);
	m_LocalIdPool->reserve = msg->first_id();
	m_LocalIdPool->reserve_limit = msg->first_id() + msg->id_count();
}

void LocalBattlefield::onUnitMessage(Ref<NetworkMessage> const &msg) {
	/**
	 * @todo no need to lookup the object if we have a cache hit (see DispatchManager::dispatch),
	 * so we should be trying the cache first!
	 */
	int unit_id = msg->getRoutingData();
	LocalUnitWrapper *wrapper = findLocalUnitWrapper(unit_id);
	if (wrapper) {
		bool handled = m_NetworkClient->dispatch(wrapper->unit().get(), msg);
		if (!handled) {
			CSPLOG(ERROR, BATTLEFIELD) << "unhandled message for unit " << unit_id;
		}
	} else {
		CSPLOG(ERROR, BATTLEFIELD) << "received message for unknown unit id " << unit_id;
	}
}

void LocalBattlefield::onUnitUpdate(Ref<NetworkMessage> const &msg) {
	int unit_id = msg->getRoutingData();
	LocalUnitWrapper *wrapper = findLocalUnitWrapper(unit_id);
	if (wrapper) {
		CSPLOG(INFO, BATTLEFIELD) << "received update for unit id " << unit_id;
		if (!wrapper->unit()) {
			CSPLOG(WARNING, BATTLEFIELD) << "creating object for unit " << unit_id << " (" << wrapper->path() << ")";
			if (!m_DataManager.valid()) {
				CSPLOG(ERROR, BATTLEFIELD) << "data manager not set, unable to create object " << wrapper->path();
				return;
			}
			Unit unit;
			try {
				unit = m_DataManager->getObject(wrapper->path());
			} catch (IndexError const &) {
				// pass (error handled below)
			}
			if (!unit) {
				CSPLOG(ERROR, BATTLEFIELD) << "unable to create object " << wrapper->path();
				return;
			}
			/** 
			 * @TODO other setup?
			 */
			unit->setId(unit_id);
			unit->setRemote();
			unit->registerUpdate(m_UnitUpdateMaster.get());
			GridPoint old_point = wrapper->point();
			/**
			 * hack: move the unit to 0,0 so we will reevaluate bubble overlaps on
			 * the next move
			 */
			moveUnit(wrapper, old_point, GridPoint(0, 0));
			wrapper->setUnit(unit);
			moveUnit(wrapper, GridPoint(0, 0), old_point);
		}
		wrapper->unit()->setState(msg, m_CurrentTimeStamp);
	} else {
		CSPLOG(ERROR, BATTLEFIELD) << "received update for unknown unit id " << unit_id;
	}
}

void LocalBattlefield::assignNewId(Object const &object) {
	CSP_VERIFY_LT(m_LocalIdPool->next, m_LocalIdPool->limit);
	ObjectId id = m_LocalIdPool->next;
	if (++m_LocalIdPool->next >= m_LocalIdPool->limit) {
		m_LocalIdPool->next = m_LocalIdPool->reserve;
		m_LocalIdPool->limit = m_LocalIdPool->reserve_limit;
		m_LocalIdPool->reserve_limit = 0;
		if (isConnectionActive()) {
			sendServerCommand(new IdAllocationRequest());
		}
	}
	CSPLOG(INFO, BATTLEFIELD) << "allocating unit id " << id;
	_assignObjectId(object, id);
}

void LocalBattlefield::__test__addLocalHumanUnit(Unit const &unit, bool human) {
	assert(!unit->isStatic());
	assert(!unit->isHuman());
	assert(unit->id() == 0);

	/**
	 * @warning the order of the following operations is important!
	 */
	initializeLocalUnit(unit, true);
	assignNewId(unit);
	LocalUnitWrapper *wrapper = new LocalUnitWrapper(unit, 0);
	addUnit(wrapper);

	if (human) {
		setHumanUnit(wrapper, human);
	}

	unit->registerUpdate(m_UnitUpdateMaster.get());
	if (isConnectionActive()) {
		Ref<RegisterUnit> msg = new RegisterUnit();
		msg->set_unit_id(unit->id());
		msg->set_unit_class(unit->getObjectPath());
		msg->set_unit_type(static_cast<uint8>(unit->type()));
		msg->set_grid_x(wrapper->point().x());
		msg->set_grid_y(wrapper->point().y());
		sendServerCommand(msg);
	}
}

void LocalBattlefield::moveUnit(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position) {
	Battlefield::moveUnit(wrapper, old_position, new_position);
	if (!wrapper->unit()) return;
	updateUnitVisibility(wrapper, old_position, new_position);
	if (wrapper->unit()->isLocal() && isConnectionActive()) {
		Ref<NotifyUnitMotion> msg = new NotifyUnitMotion();
		msg->set_unit_id(wrapper->unit()->id());
		msg->set_grid_x(wrapper->point().x());
		msg->set_grid_y(wrapper->point().y());
		sendServerCommand(msg);
	}
}

bool LocalBattlefield::updateUnitVisibility(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position) {
	assert(wrapper->unit().valid());
	if (!m_SceneManager.valid()) return false;
	if (!wrapper->unit().valid()) return false;
	const double vis_bubble = m_SceneManager->getVisibleRange();
	const double vis_r2 = vis_bubble * vis_bubble;
	const bool in_old_bubble = (globalDistance2(m_CameraGridPosition, old_position) <= vis_r2) && !isNullPoint(old_position);
	const bool in_new_bubble = (globalDistance2(m_CameraGridPosition, new_position) <= vis_r2) && !isNullPoint(new_position);
	if (in_old_bubble == in_new_bubble) return false;
	if (in_new_bubble) {
		m_SceneManager->scheduleShow(wrapper->unit());
	} else {
		m_SceneManager->scheduleHide(wrapper->unit());
	}
	return true;
}

void LocalBattlefield::updateVisibility(GridPoint old_camera_position, GridPoint new_camera_position) {
	assert(m_SceneManager.valid());

	const double vis_bubble = m_SceneManager->getVisibleRange();
	const double vis_r2 = vis_bubble * vis_bubble;

	GridRegion old_region = makeGridRegionEnclosingCircle(old_camera_position, vis_bubble);
	GridRegion new_region = makeGridRegionEnclosingCircle(new_camera_position, vis_bubble);

	std::vector<QuadTreeChild*> show;
	std::vector<QuadTreeChild*> hide;

	const bool old_position_is_null = isNullPoint(old_camera_position);
	const bool new_position_is_null = isNullPoint(new_camera_position);

	if (old_region.overlaps(new_region)) {
		/**
		 * usual case, overlapping old and new visibility regions.  do one query
		 * and filter out the objects that enter and leave the visibility bubble.
		 */
		CSPLOG(DEBUG, BATTLEFIELD) << "updateVisibility(): small camera move";

		/**
		 * construct a query region that includes both the old and new bubbles
		 * and find all objects in that region.
		 */
		old_region.expand(new_region);
		std::vector<QuadTreeChild*> mixed;
		dynamicIndex()->query(old_region, mixed);
		staticIndex()->query(old_region, mixed);

		/** now filter the mixed list into the show and hide lists */
		for (unsigned i = 0; i < mixed.size(); ++i) {
			bool in_old_bubble = (globalDistance2(old_camera_position, mixed[i]->point()) <= vis_r2) && !old_position_is_null;
			bool in_new_bubble = (globalDistance2(new_camera_position, mixed[i]->point()) <= vis_r2) && !new_position_is_null;
			if (in_old_bubble && !in_new_bubble) {
				hide.push_back(mixed[i]);
			} else if (in_new_bubble && !in_old_bubble) {
				show.push_back(mixed[i]);
			}
		}
	} else {
		/**
		 * less common case, the camera has jumped so far that the old and new
		 * visibility regions don't overlap.  this is easier, since no sorting
		 * is required.
		 */
		CSPLOG(DEBUG, BATTLEFIELD) << "updateVisibility(): large camera move";
		if (!old_position_is_null) {
			dynamicIndex()->query(old_region, hide);
			staticIndex()->query(old_region, hide);
		}
		if (!new_position_is_null) {
			dynamicIndex()->query(new_region, show);
			staticIndex()->query(new_region, show);
		}
	}

	CSPLOG(DEBUG, BATTLEFIELD) << "updateVisibility(): hiding " << hide.size() << " objects";
	for (unsigned i = 0; i < hide.size(); ++i) {
		Object object = static_cast<ObjectWrapper*>(hide[i])->object();
		/**
		 * object will be null if we have not received any peer updates yet.  in this case
		 * the object isn't really visible, and there is nothing to do. this situation can
		 * arise transiently when new objects are added to the global battlefield, but in
		 * general we should have received peer updates before an object enters or leaves
		 * visible range.
		 */
		if (object.valid()) {
			m_SceneManager->scheduleHide(object);
		}
	}

	CSPLOG(DEBUG, BATTLEFIELD) << "updateVisibility(): showing " << show.size() << " objects";
	for (unsigned i = 0; i < show.size(); ++i) {
		Object object = static_cast<ObjectWrapper*>(show[i])->object();
		/**
		 * object will be null if we have not received any peer updates yet.  in this case
		 * we can't show the object, but the visibility will be reevaluated when the first
		 * update arives and the object is created.  this situation can arise transiently
		 * when new objects are added to the global battlefield, but in general we should
		 * receive peer updates before an object enters visible range.
		 */
		if (object.valid()) {
			m_SceneManager->scheduleShow(object);
		}
	}
}

void LocalBattlefield::setCamera(Vector3 const &eye_point, const Vector3& look_pos, const Vector3& up_vec) {
	/**
	 * if there is no scene manager, then we ignore camera updates (which probably
	 * shouldn't be occur anyway).
	 */
	if (!m_SceneManager) return;

	/**
	 * first update the camera, which will force the local terrain to be loaded
	 * before features are added to the scene (the terrain elevation must be
	 * available for features to be position correctly). this works with demeter
	 * since it blocks during the terrain load, but cspchunklod uses a threaded
	 * loader.
	 * 
	 * @TODO ElevationCorrection needs to be able to query the terrain object
	 * to deterimine if elevation data is available, and block (in the feature
	 * construction thread) if it isn't.
	 */
	CSPLOG(DEBUG, BATTLEFIELD) << "setCamera(): update scene camera";
	m_SceneManager->setCamera(eye_point, look_pos, up_vec);

	/**
	 * if the camera has move sufficiently far, add and remove features and dynamic
	 * objects from the scene based on the current camera position.
	 */
	GridPoint new_grid_position = globalToGrid(eye_point);
	if (hasMoved(m_CameraGridPosition, new_grid_position)) {
		CSPLOG(DEBUG, BATTLEFIELD) << "setCamera(): updating visibility " << new_grid_position.x() << ", " << new_grid_position.y();
		updateVisibility(m_CameraGridPosition, new_grid_position);
		m_CameraGridPosition = new_grid_position;
	}
}

LocalBattlefield::UnitUpdateProxy::UnitUpdateProxy(LocalUnitWrapper *wrapper, Ref<UpdateProxyConnection> connection): m_Wrapper(wrapper), m_Connection(connection), m_UpdateTime(0) {
}

void LocalBattlefield::UnitUpdateProxy::addPeerUpdate(PeerId id) {
	for (unsigned i = 0; i < m_PeerUpdates.size(); ++i) {
		if (m_PeerUpdates[i].id == id) return;
	}
	m_PeerUpdates.push_back(PeerUpdateRecord(id, 50, 0));
	std::push_heap(m_PeerUpdates.begin(), m_PeerUpdates.end());
}

void LocalBattlefield::UnitUpdateProxy::removePeerUpdate(PeerId id) {
	for (unsigned i = 0; i < m_PeerUpdates.size(); ++i) {
		if (m_PeerUpdates[i].id == id) {
			m_PeerUpdates.erase(m_PeerUpdates.begin() + i);
			std::make_heap(m_PeerUpdates.begin(), m_PeerUpdates.end());
			return;
		}
	}
}

double LocalBattlefield::UnitUpdateProxy::onUpdate(double dt) {
	CSPLOG(INFO, BATTLEFIELD) << "update proxy called";

	const unsigned n_updates = m_PeerUpdates.size();
	if (n_updates == 0) {
		CSPLOG(WARNING, BATTLEFIELD) << "no peers to update";
		return 1.0;  /** wait 1 sec before checking again */
	}

	uint32_t dt_ms = static_cast<uint32>(dt * 1000.0);
	m_UpdateTime += dt_ms;
	/** @bug this can get choppy when the frame rate and update rates are comparable */
	if (m_UpdateTime < m_PeerUpdates[0].next_update) {
		CSPLOG(DEBUG, BATTLEFIELD) << "too soon, " << (m_UpdateTime - m_PeerUpdates[0].next_update) << " ms";
		return (m_UpdateTime - m_PeerUpdates[0].next_update) * 1e-3;
	}

	TimeStamp timestamp = m_Connection->getTimeStamp();

	/** targets stores (detail, peer id) in an int to facilite sorting
	 */
	int targets[128];
	int target_count = 0;

	/**
	 * find all peers that need updates now, and generate/cache the update messages
	 * for each detail level.  the updates are kept in a heap, with the next update
	 * at the top.
	 */
	while ((m_PeerUpdates[0].next_update <= m_UpdateTime) && (target_count < 128)) {
		std::pop_heap(m_PeerUpdates.begin(), m_PeerUpdates.end());
		const uint32_t interval = m_PeerUpdates[n_updates - 1].interval;
		m_PeerUpdates[n_updates - 1].next_update = m_UpdateTime + interval;

		const int detail = m_PeerUpdates[n_updates - 1].detail;
		assert(detail >=0 && detail < DETAIL_LEVELS);

		/** the object may choose to skip an update due to low estimated error */
		bool skipped = false;

		/** 
		 * ok to use slightly stale messages.  note that if we refresh the cache for any
		 * peer, all other peers at that detail level will get the new message.
		 */
		if (m_UpdateTime - m_DetailCache[detail].last_refresh > interval / 10) {
			/** @bug the state message will be used for multiple peers that are updated at different intervals, so the content should not depend on interval. */
			Ref<NetworkMessage> msg = m_Wrapper->unit()->getState(timestamp, 0 /*interval*/, detail);
			if (msg.valid()) {
				msg->setRoutingType(ROUTE_UNIT_UPDATE);
				msg->setRoutingData(m_Wrapper->id());
				msg->setPriority(2);  /** XXX msg/detail dependent? */
				m_DetailCache[detail].msg = msg;
				m_DetailCache[detail].last_refresh = m_UpdateTime;
			} else {
				skipped = true;
			}
		}

		if (!skipped) {
			/** hack for sorting by detail level, see below */
			targets[target_count++] = (detail << 24) | m_PeerUpdates[n_updates - 1].id;
		}

		std::push_heap(m_PeerUpdates.begin(), m_PeerUpdates.end());
	}

	/** order by detail level to take advantage of outbound message caching */
	std::sort(targets, targets + target_count);

	/** finally, send all pending updates */
	for (int i = 0; i < target_count; ++i) {
		const int detail = targets[i] >> 24;
		const PeerId id = static_cast<PeerId>(targets[i] & 0xffffff);
		Ref<NetworkMessage> msg = m_DetailCache[detail].msg;
		if (msg.valid()) {
			m_Connection->send(msg, static_cast<PeerId>(id));
		} else {
			CSPLOG(ERROR, BATTLEFIELD) << "no state message";
		}
	}

	/**
	 * we don't need to be called again until the next update time is reached
	 */
	return (m_PeerUpdates[0].next_update - m_UpdateTime) * 1e-3;
}

void LocalBattlefield::UnitUpdateProxy::setUpdateParameters(PeerId id, double interval, uint16_t detail) {
	assert(detail < DETAIL_LEVELS);
	uint16_t interval_ms = static_cast<uint16>(interval * 1000.0);
	const unsigned n = m_PeerUpdates.size();
	for (unsigned i = 0; i < n; ++i) {
		if (m_PeerUpdates[i].id == id) {
			m_PeerUpdates[i].interval = interval_ms;
			m_PeerUpdates[i].detail = static_cast<uint16>(detail);
			CSPLOG(INFO, BATTLEFIELD) << "set update interval for peer " << id << " to " << interval_ms << " ms";
			return;
		}
	}
	CSPLOG(WARNING, BATTLEFIELD) << "set update interval, peer " << id << " not found";
}

void LocalBattlefield::LocalUnitWrapper::setUnit(Unit const &object) {
	if (object.valid()) {
		CSP_VERIFY(!(object->isStatic()));
		CSP_VERIFY_EQ(object->isLocal(), owner() == 0);
	}
	setObject(object);
}

void LocalBattlefield::LocalUnitWrapper::setUpdateProxy(UpdateMaster &master, Ref<UpdateProxyConnection> connection) {
	assert(m_UpdateProxy.isNull());
	m_UpdateProxy.reset(new UnitUpdateProxy(this, connection));
	master.registerUpdate(m_UpdateProxy.get());
}

void LocalBattlefield::LocalUnitWrapper::addPeerUpdate(PeerId id) {
	assert(m_UpdateProxy.valid());
	m_UpdateProxy->addPeerUpdate(id);
}

void LocalBattlefield::LocalUnitWrapper::removePeerUpdate(PeerId id) {
	assert(m_UpdateProxy.valid());
	m_UpdateProxy->removePeerUpdate(id);
}

void LocalBattlefield::LocalUnitWrapper::setUpdateDistance(PeerId id, double distance) {
	assert(m_UpdateProxy.valid());
	double interval = clampTo(distance / 10000.0, 0.05, 5.0);
	/**
	 * detail levels:
	 * 9 = 0-50 m
	 * 8 = 50-100 m
	 * 7 = 100-200 m
	 * 6 = 200-400 m
	 * 5 = 400-800 m
	 * 4 = 800-1600 m
	 * 3 = 1.6-3.2 km
	 * 2 = 3.2-6.4 km
	 * 1 = 6.4-12.8 km
	 * 0 = >12.8 km
	 */
	uint16_t detail = static_cast<uint16>(std::max(0, 9 - static_cast<int>(1.4427 * ::log(std::max(1.0, distance / 25.0)))));
	m_UpdateProxy->setUpdateParameters(id, interval, detail);
}

LocalBattlefield::LocalUnitWrapper* LocalBattlefield::findLocalUnitWrapper(ObjectId id) {
	return static_cast<LocalUnitWrapper*>(findUnitWrapper(id));
}

} // namespace csp

