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


#include <SimCore/Battlefield/LocalBattlefield.h>
#include <SimCore/Battlefield/Battlefield.h>
#include <SimCore/Battlefield/SceneManager.h>
#include <SimCore/Util/SynchronousUpdate.h>

#include <SimNet/ClientServer.h>
#include <SimNet/DispatchHandler.h>
#include <SimData/Verify.h>
#include <SimData/Link.h>
#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>


/** Storage class for information about remote players.
 */
class LocalBattlefield::PlayerInfo: public simdata::Referenced {
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
class LocalBattlefield::UpdateProxyConnection: public simdata::Referenced {
	simnet::MessageQueue::Ref m_MessageQueue;
	simcore::TimeStamp m_TimeStamp;
public:
	typedef simdata::Ref<UpdateProxyConnection> Ref;

	/** Construct a UpdateProxyConnection.  Binds the outbound message queue, and initializes
	 *  the timestamp to zero.
	 */
	UpdateProxyConnection(simnet::MessageQueue::Ref queue): m_MessageQueue(queue), m_TimeStamp(0) { }

	/** Set the current timestamp.  This is done by the battlefield prior to updating the
	 *  UnitUpdateProxies.
	 */
	void setTimeStamp(simcore::TimeStamp const &time_stamp) { m_TimeStamp = time_stamp; }

	/** Get the current timestamp, which is used to mark outbound unit update messages.
	 */
	inline simcore::TimeStamp getTimeStamp() const { return m_TimeStamp; }

	/** Send a message to the specified peer.
	 */
	void send(simnet::NetworkMessage::Ref const &msg, PeerId destination) {
		if (m_MessageQueue.valid()) m_MessageQueue->queueMessage(msg, destination);
	}
};


/** Wrapper for inserting dynamic objects into a quadtree index.
 *
 *  TODO
 *  Keep track of the aggregation bubble count for an object, which
 *  is the number aggregation bubbles projected by human-controlled vehicles
 *  that overlap the object's position.  If this count is greater than zero,
 *  the object should be deaggregated.  This functionality was temporarily
 *  removed for the initial implementation of the distributed battlefield.
 */
class LocalBattlefield::LocalUnitWrapper: public UnitWrapper {
	simdata::ScopedPointer<UnitUpdateProxy> m_UpdateProxy;
public:
	LocalUnitWrapper(Unit const &u, PeerId owner): UnitWrapper(u, owner) { }
	LocalUnitWrapper(ObjectId id, simdata::Path const &path, PeerId owner): UnitWrapper(id, path, owner) { }
	void setUnit(Unit const &object);
	inline bool hasUpdateProxy() const { return m_UpdateProxy.valid(); }
	void setUpdateProxy(UpdateMaster &master, UpdateProxyConnection::Ref connection);
	void addPeerUpdate(PeerId id);
	void removePeerUpdate(PeerId id);
	void setUpdateInterval(PeerId id, double interval);
};


/** A helper class for sending updates from a local unit to remote hosts.
 *  An instance of this class is lazily bound to a LocalUnitWrapper when
 *  the wrapped unit is first directed to send remote updates.  The UnitUpdateProxy
 *  instance is registered with an UpdateMaster, which drives the updates.
 */
class LocalBattlefield::UnitUpdateProxy: public UpdateTarget {

	struct PeerUpdateRecord {
		inline PeerUpdateRecord() {}
		PeerUpdateRecord(PeerId id_, simdata::uint16 interval_, simdata::uint16 detail_): id(id_), interval(interval_), next_update(0), detail(detail_) { }
		PeerId id;  // host id to update
		simdata::uint16 interval;  // time between updates, ms
		simdata::uint32 next_update; // ms resolution, 46 day limit
		simdata::uint16 detail; // detail level (not yet used, but important for reducing bandwidth)
		// order of priority
		bool operator < (PeerUpdateRecord const &other) const { return next_update > other.next_update; }
	};

	// the unit (wrapper) sending updates to remote peers
	LocalUnitWrapper *m_Wrapper;

	// connection used for sending updates
	UpdateProxyConnection::Ref m_Connection;

	// a counter used for scheduling update messages to remote hosts.  the count is
	// milliseconds of elapsed time.  it is currently assumed that this will not
	// rollover during the simulation time (limit ~50 days)
	simdata::uint32 m_UpdateTime;

	// a heap, ordered by next update time (first element is the earliest update)
	std::vector<PeerUpdateRecord> m_PeerUpdates;

public:
	/** Construct a new UnitUpdateProxy.
	 *
	 *  @param wrapper The wrapper of the unit that is sending updates.
	 *  @param connection The connection used to send updates to remote hosts.
	 */
	UnitUpdateProxy(LocalUnitWrapper *wrapper, UpdateProxyConnection::Ref connection);

	/** Add a peer to the update list.
	 *  TODO allow the rate and detail level to be set and updated.  For now the update
	 *  rate and detail level are fixed at 300 ms and 0, respectively.
	 */
	void addPeerUpdate(PeerId id);

	/** Remove a peer from the update list.
	 */
	void removePeerUpdate(PeerId id);

	/** UpdateMaster callback, which sends updates about the unit to remote hosts.
	 */
	double onUpdate(double dt);

	/** Adjust the update interval for the specified peer.  The change will
	 *  take effect after the next update.
	 *
	 *  @param id The id of the peer.
	 *  @param interval The interval between updates, in seconds.
	 */
	void setUpdateInterval(PeerId id, double interval);
};


struct LocalBattlefield::ObjectIdPool {
	ObjectIdPool(): next(0), limit(0), reserve(0), reserve_limit(0) { }
	ObjectId next;
	ObjectId limit;
	ObjectId reserve;
	ObjectId reserve_limit;
};


LocalBattlefield::LocalBattlefield(simdata::Ref<simdata::DataManager> const &data_manager):
	m_ConnectionState(CONNECTION_DETACHED),
	m_DataManager(data_manager),
	m_CameraGridPosition(0,0),
	m_UnitUpdateMaster(new UpdateMaster()),
	m_LocalIdPool(new ObjectIdPool()),
	m_ScanElapsedTime(0),
	m_ScanRate(0),
	m_ScanIndex(0)
{
	// if no network connection, ids will be assigned sequentially
	// starting at 1024.  otherwise, the id pool is initialized when
	// we connect to the server.
	m_LocalIdPool->next = 1024;
	m_LocalIdPool->limit = 1000000000;
	//assert(data_manager.valid());
	if (!m_DataManager) {
		CSP_LOG(BATTLEFIELD, ERROR, "No data manager, cannot create objects.");
	}
}

LocalBattlefield::~LocalBattlefield() {
}


typedef std::pair<Battlefield::PeerId, float> PeerContact;
struct PeerContactSorter {
	bool operator()(PeerContact const &a, PeerContact const &b) {
		return (a.first < b.first) || (a.first == b.first && a.second < b.second);
	}
};

void LocalBattlefield::scanUnit(LocalUnitWrapper *wrapper) {
	if (wrapper == NULL) {
		CSP_LOG(BATTLEFIELD, DEBUG, "scan update, skipping removed unit");
		return;
	}
	GridPoint point = wrapper->point();
	if (!isNullPoint(point)) {
		Unit unit = wrapper->unit();
		CSP_LOG(BATTLEFIELD, INFO, "scan update for " << *unit);
		GridRegion region = makeGridRegionEnclosingCircle(point, 60000);
		std::vector<QuadTreeChild*> contacts;
		dynamicIndex()->query(region, contacts);
		const unsigned n_contacts = contacts.size();
		std::vector<PeerContact> peer_contacts;
		peer_contacts.reserve(n_contacts);
		simdata::Vector3 unit_position = unit->getGlobalPosition();
		for (unsigned i=0; i < n_contacts; ++i) {
			LocalUnitWrapper *contact = static_cast<LocalUnitWrapper*>(contacts[i]);
			if (contact->id() == wrapper->id()) continue;
			if (!contact->unit()) continue;
			float distance = static_cast<float>((contact->unit()->getGlobalPosition() - unit_position).length2());
			peer_contacts.push_back(PeerContact(contact->owner(), distance));
		}
		CSP_LOG(BATTLEFIELD, INFO, "found " << peer_contacts.size() << " nearby units");
		if (!peer_contacts.empty()) {
			std::sort(peer_contacts.begin(), peer_contacts.end(), PeerContactSorter());
			PeerId current_id = 0;
			for (std::vector<PeerContact>::const_iterator iter = peer_contacts.begin(); iter != peer_contacts.end(); ++iter) {
				if (iter->first != current_id) {
					current_id = iter->first;
					float distance = sqrt(iter->second);
					CSP_LOG(BATTLEFIELD, INFO, "nearest unit owned by " << current_id << " is " << distance << " meters");
					double interval = simdata::clampTo(distance / 10000.0, 0.05, 5.0);
					wrapper->setUpdateInterval(current_id, interval);
				}
			}
		}
	} else {
		CSP_LOG(BATTLEFIELD, DEBUG, "scan update, skipping unit outside battlefield");
	}
}

void LocalBattlefield::continueUnitScan(double dt) {
	static const double loop_time = 3.0;  // seconds
	m_ScanElapsedTime += dt;
	unsigned target_index = static_cast<unsigned>(round(m_ScanElapsedTime * m_ScanRate));
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
	m_CurrentTime = simdata::get_realtime();
	m_CurrentTimeStamp = simcore::getTimeStamp(m_CurrentTime);
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

void LocalBattlefield::setSceneManager(simdata::Ref<SceneManager> const &manager) {
	assert(m_SceneManager.isNull());
	m_SceneManager = manager;
}

void LocalBattlefield::setNetworkClient(simdata::Ref<simnet::Client> const &client) {
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
	simnet::DispatchHandler::Ref dispatch = new simnet::DispatchHandler(client->queue());
	bindCommandDispatch(dispatch);
	client->routeToHandler(ROUTE_COMMAND, dispatch);
	client->routeToCallback(ROUTE_UNIT_UPDATE, this, &LocalBattlefield::onUnitUpdate);
	m_UpdateProxyConnection = new UpdateProxyConnection(client->queue());
	m_UnitRemoteUpdateMaster.reset(new UpdateMaster());
}

/* TODO add to .h and uncomment
void LocalBattlefield::resetConnectionState() {
	assert(m_NetworkClient.valid());
	assert(!isConnectionActive());
	m_ConnectionState = CONNECTION_DETACHED;
}
*/

void LocalBattlefield::connectToServer(std::string const &name) {
	assert(m_NetworkClient.valid());
	assert(m_ConnectionState == CONNECTION_DETACHED);
	JoinRequest::Ref msg = new JoinRequest();
	msg->set_user_name(name);
	sendServerCommand(msg);
	m_ConnectionState = CONNECTION_JOIN;
}

void LocalBattlefield::bindCommandDispatch(simnet::DispatchHandler::Ref const &dispatch) {
	dispatch->registerHandler(this, &LocalBattlefield::onPlayerJoin);
	dispatch->registerHandler(this, &LocalBattlefield::onPlayerQuit);
	dispatch->registerHandler(this, &LocalBattlefield::onCommandUpdatePeer);
	dispatch->registerHandler(this, &LocalBattlefield::onCommandAddUnit);
	dispatch->registerHandler(this, &LocalBattlefield::onCommandRemoveUnit);
	dispatch->registerHandler(this, &LocalBattlefield::onJoinResponse);
	dispatch->registerHandler(this, &LocalBattlefield::onIdAllocationResponse);
}

void LocalBattlefield::onPlayerQuit(simdata::Ref<PlayerQuit> const &msg, simdata::Ref<simnet::MessageQueue> const &) {
	SIMNET_LOG(MESSAGE, INFO, *msg);
	const PeerId id = msg->peer_id();
	if (m_NetworkClient->getPeer(id)) {
		PlayerInfoMap::iterator iter = m_PlayerInfoMap.find(id);
		if (iter != m_PlayerInfoMap.end()) {
			CSP_LOG(BATTLEFIELD, INFO, iter->second->GetName() << " just quit the game!");
			m_PlayerInfoMap.erase(iter);
		}
		m_NetworkClient->disconnectPeer(id);
		// the global battlefield should send separate messages to remove
		// all objects owned by this peer
	}
}

void LocalBattlefield::onPlayerJoin(simdata::Ref<PlayerJoin> const &msg, simdata::Ref<simnet::MessageQueue> const &) {
	SIMNET_LOG(MESSAGE, INFO, *msg);
	const PeerId id = msg->peer_id();
	if (!m_NetworkClient->getPeer(id)) {
		CSP_LOG(BATTLEFIELD, INFO, msg->user_name() << " just joined the game!");
		const simnet::NetworkNode remote_node(msg->ip_addr(), msg->port());
		const int incoming_bw = msg->incoming_bw();
		const int outgoing_bw = msg->outgoing_bw();
		m_PlayerInfoMap[id] = new PlayerInfo(id, msg->user_name());
		m_NetworkClient->addPeer(id, remote_node, incoming_bw, outgoing_bw);
	}
}

void LocalBattlefield::onCommandUpdatePeer(simdata::Ref<CommandUpdatePeer> const &msg, simdata::Ref<simnet::MessageQueue> const &) {
	SIMNET_LOG(MESSAGE, INFO, *msg);
	LocalUnitWrapper *wrapper = findLocalUnitWrapper(msg->unit_id());
	if (wrapper) {
		assert(wrapper->unit().valid() && wrapper->unit()->isLocal());
		if (msg->has_stop() && msg->stop()) {
			wrapper->removePeerUpdate(msg->peer_id());
		} else {
			if (!wrapper->hasUpdateProxy()) {
				CSP_LOG(BATTLEFIELD, ERROR, "creating update proxy for unit " << msg->unit_id());
				wrapper->setUpdateProxy(*m_UnitRemoteUpdateMaster, m_UpdateProxyConnection);
			}
			wrapper->addPeerUpdate(msg->peer_id());
		}
	} else {
		CSP_LOG(BATTLEFIELD, ERROR, "received update peer request for unknown unit " << msg->unit_id());
	}
}

void LocalBattlefield::onCommandAddUnit(simdata::Ref<CommandAddUnit> const &msg, simdata::Ref<simnet::MessageQueue> const &) {
	SIMNET_LOG(MESSAGE, INFO, *msg);
	LocalUnitWrapper *wrapper = findLocalUnitWrapper(msg->unit_id());
	if (wrapper) {
		// already added.  for testing, double check that the message is consistent (FIXME)
		SIMDATA_VERIFY_EQ(msg->owner_id(), wrapper->owner());
		CSP_LOG(BATTLEFIELD, INFO, "unit already exists, disregarding duplicate message");
		return;
	}
	wrapper = new LocalUnitWrapper(msg->unit_id(), msg->unit_class(), msg->owner_id());
	addUnit(wrapper);
	updatePosition(wrapper, GridPoint(msg->grid_x(), msg->grid_y()));
}

void LocalBattlefield::onCommandRemoveUnit(simdata::Ref<CommandRemoveUnit> const &msg, simdata::Ref<simnet::MessageQueue> const &) {
	SIMNET_LOG(MESSAGE, INFO, *msg);
	removeUnit(msg->unit_id());
}

void LocalBattlefield::sendServerCommand(simdata::Ref<simnet::NetworkMessage> const &msg) {
	msg->setRoutingType(ROUTE_COMMAND);
	m_NetworkClient->sendToServer(msg);
}

void LocalBattlefield::onJoinResponse(simdata::Ref<JoinResponse> const &msg, simdata::Ref<simnet::MessageQueue> const &) {
	SIMNET_LOG(MESSAGE, INFO, *msg);
	if (m_ConnectionState != CONNECTION_JOIN) {
		CSP_LOG(BATTLEFIELD, ERROR, "received unrequested join response");
		return;
	}
	if (!msg->success()) {
		CSP_LOG(BATTLEFIELD, ERROR, "battlefield failed to connect to server: " << msg->message());
		assert(0);
	}
	CSP_LOG(BATTLEFIELD, INFO, "battlefield connected to server: " << msg->message());
	assert(msg->first_id() > 0);
	assert(msg->id_count() > 63);
	m_LocalIdPool->next = msg->first_id();
	m_LocalIdPool->limit = msg->first_id() + msg->id_count();
	// allocate reserve ids
	sendServerCommand(new IdAllocationRequest());
	m_ConnectionState = CONNECTION_ACTIVE;
}

void LocalBattlefield::onIdAllocationResponse(simdata::Ref<IdAllocationResponse> const &msg, simdata::Ref<simnet::MessageQueue> const &) {
	SIMNET_LOG(MESSAGE, INFO, *msg);
	assert(m_LocalIdPool->reserve_limit == 0);
	m_LocalIdPool->reserve = msg->first_id();
	m_LocalIdPool->reserve_limit = msg->first_id() + msg->id_count();
}

void LocalBattlefield::onUnitMessage(simdata::Ref<simnet::NetworkMessage> const &msg) {
	// FIXME no need to lookup the object if we have a cache hit (see DispatchManager::dispatch),
	// so we should be trying the cache first!
	int unit_id = msg->getRoutingData();
	LocalUnitWrapper *wrapper = findLocalUnitWrapper(unit_id);
	if (wrapper) {
		bool handled = m_NetworkClient->dispatch(wrapper->unit().get(), msg);
		if (!handled) {
			CSP_LOG(BATTLEFIELD, ERROR, "unhandled message for unit " << unit_id);
		}
	} else {
		CSP_LOG(BATTLEFIELD, ERROR, "received message for unknown unit id " << unit_id);
	}
}

void LocalBattlefield::onUnitUpdate(simdata::Ref<simnet::NetworkMessage> const &msg) {
	int unit_id = msg->getRoutingData();
	LocalUnitWrapper *wrapper = findLocalUnitWrapper(unit_id);
	if (wrapper) {
		CSP_LOG(BATTLEFIELD, INFO, "received update for unit id " << unit_id);
		if (!wrapper->unit()) {
			CSP_LOG(BATTLEFIELD, WARNING, "creating object for unit " << unit_id << " (" << wrapper->path() << ")");
			if (!m_DataManager.valid()) {
				CSP_LOG(BATTLEFIELD, ERROR, "data manager not set, unable to create object " << wrapper->path());
				return;
			}
			Unit unit;
			try {
				unit = m_DataManager->getObject(wrapper->path());
			} catch (simdata::IndexError const &err) {
				// pass (error handled below)
			}
			if (!unit) {
				CSP_LOG(BATTLEFIELD, ERROR, "unable to create object " << wrapper->path());
				return;
			}
			// TODO other setup?
			unit->setId(unit_id);
			unit->setRemote();
			// FIXME hack to ensure the correct systems model is loaded
			//unit->setHuman();
			//unit->setAgent();
			//unit->setLocal();
			//if (unit->isLocal()) unit->setRemote();
			// ^ end-of-hack
			unit->registerUpdate(m_UnitUpdateMaster.get());
			GridPoint old_point = wrapper->point();
			// hack: move the unit to 0,0 so we will reevaluate bubble overlaps on
			// the next move
			moveUnit(wrapper, old_point, GridPoint(0, 0));
			wrapper->setUnit(unit);
			moveUnit(wrapper, GridPoint(0, 0), old_point);
		}
		wrapper->unit()->setState(msg, m_CurrentTimeStamp);
	} else {
		CSP_LOG(BATTLEFIELD, ERROR, "received update for unknown unit id " << unit_id);
	}
}

void LocalBattlefield::assignNewId(Object const &object) {
	SIMDATA_VERIFY_LT(m_LocalIdPool->next, m_LocalIdPool->limit);
	ObjectId id = m_LocalIdPool->next;
	if (++m_LocalIdPool->next >= m_LocalIdPool->limit) {
		m_LocalIdPool->next = m_LocalIdPool->reserve;
		m_LocalIdPool->limit = m_LocalIdPool->reserve_limit;
		m_LocalIdPool->reserve_limit = 0;
		if (isConnectionActive()) {
			sendServerCommand(new IdAllocationRequest());
		}
	}
	CSP_LOG(BATTLEFIELD, INFO, "allocating unit id " << id);
	_assignObjectId(object, id);
}

void LocalBattlefield::__test__addLocalHumanUnit(Unit const &unit) {
	assert(!unit->isStatic());
	assert(!unit->isHuman());
	assert(unit->id() == 0);

	// the order of the following operations is important!
	initializeLocalUnit(unit, true);
	assignNewId(unit);
	LocalUnitWrapper *wrapper = new LocalUnitWrapper(unit, 0);
	addUnit(wrapper);
	setHumanUnit(wrapper, true);

	unit->registerUpdate(m_UnitUpdateMaster.get());
	if (isConnectionActive()) {
		RegisterUnit::Ref msg = new RegisterUnit();
		msg->set_unit_id(unit->id());
		msg->set_unit_class(unit->getObjectPath());
		msg->set_unit_type(unit->type());
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
		NotifyUnitMotion::Ref msg = new NotifyUnitMotion();
		msg->set_unit_id(wrapper->unit()->id());
		msg->set_grid_x(wrapper->point().x());
		msg->set_grid_y(wrapper->point().y());
		sendServerCommand(msg);
	}
}

bool LocalBattlefield::updateUnitVisibility(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position) {
	if (!m_SceneManager.valid()) return false;
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
		// usual case, overlapping old and new visibility regions.  do one query
		// and filter out the objects that enter and leave the visibility bubble.
		CSP_LOG(BATTLEFIELD, DEBUG, "updateVisibility(): small camera move");

		// construct a query region that includes both the old and new bubbles
		// and find all objects in that region.
		old_region.expand(new_region);
		std::vector<QuadTreeChild*> mixed;
		dynamicIndex()->query(old_region, mixed);
		staticIndex()->query(old_region, mixed);

		// now filter the mixed list into the show and hide lists
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
		// less common case, the camera has jumped so far that the old and new
		// visibility regions don't overlap.  this is easier, since no sorting
		// is required.
		CSP_LOG(BATTLEFIELD, DEBUG, "updateVisibility(): large camera move");
		if (!old_position_is_null) {
			dynamicIndex()->query(old_region, hide);
			staticIndex()->query(old_region, hide);
		}
		if (!new_position_is_null) {
			dynamicIndex()->query(new_region, show);
			staticIndex()->query(new_region, show);
		}
	}

	CSP_LOG(BATTLEFIELD, DEBUG, "updateVisibility(): hiding " << hide.size() << " objects");
	for (unsigned i = 0; i < hide.size(); ++i) {
		Object object = static_cast<ObjectWrapper*>(hide[i])->object();
		m_SceneManager->scheduleHide(object);
	}

	CSP_LOG(BATTLEFIELD, DEBUG, "updateVisibility(): showing " << show.size() << " objects");
	for (unsigned i = 0; i < show.size(); ++i) {
		Object object = static_cast<ObjectWrapper*>(show[i])->object();
		m_SceneManager->scheduleShow(object);
	}
}

void LocalBattlefield::setCamera(simdata::Vector3 const &eye_point, const simdata::Vector3& look_pos, const simdata::Vector3& up_vec) {
	// if there is no scene manager, then we ignore camera updates (which probably
	// shouldn't be occur anyway).
	if (!m_SceneManager) return;

	// first update the camera, which will force the local terrain to be loaded
	// before features are added to the scene (the terrain elevation must be
	// available for features to be position correctly). this works with demeter
	// since it blocks during the terrain load, but cspchunklod uses a threaded
	// loader.
	// TODO ElevationCorrection needs to be able to query the terrain object
	// to deterimine if elevation data is available, and block (in the feature
	// construction thread) if it isn't.
	CSP_LOG(BATTLEFIELD, DEBUG, "setCamera(): update scene camera");
	m_SceneManager->setCamera(eye_point, look_pos, up_vec);

	// if the camera has move sufficiently far, add and remove features and dynamic
	// objects from the scene based on the current camera position.
	GridPoint new_grid_position = globalToGrid(eye_point);
	if (hasMoved(m_CameraGridPosition, new_grid_position)) {
		CSP_LOG(BATTLEFIELD, DEBUG, "setCamera(): updating visibility " << new_grid_position.x() << ", " << new_grid_position.y());
		updateVisibility(m_CameraGridPosition, new_grid_position);
		m_CameraGridPosition = new_grid_position;
	}
}

LocalBattlefield::UnitUpdateProxy::UnitUpdateProxy(LocalUnitWrapper *wrapper, UpdateProxyConnection::Ref connection): m_Wrapper(wrapper), m_Connection(connection), m_UpdateTime(0) {
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
	CSP_LOG(BATTLEFIELD, INFO, "update proxy called");
	const unsigned n = m_PeerUpdates.size();
	if (n == 0) {
		CSP_LOG(BATTLEFIELD, WARNING, "no peers to update");
		return 1.0;
	}
	simdata::uint32 dt_ms = static_cast<simdata::uint32>(dt * 1000.0);
	m_UpdateTime += dt_ms;
	// FIXME this can get choppy when the frame rate and update rates are comparable
	if (m_UpdateTime < m_PeerUpdates[0].next_update) {
		CSP_LOG(BATTLEFIELD, DEBUG, "too soon, " << (m_UpdateTime - m_PeerUpdates[0].next_update) << " ms");
		return (m_UpdateTime - m_PeerUpdates[0].next_update) * 1e-3;
	}
	PeerUpdateRecord targets[128];
	int target_count = 0;
	while ((m_PeerUpdates[0].next_update <= m_UpdateTime) && (target_count < 128)) {
		std::pop_heap(m_PeerUpdates.begin(), m_PeerUpdates.end());
		m_PeerUpdates[n-1].next_update = m_UpdateTime + m_PeerUpdates[n-1].interval;
		targets[target_count++] = m_PeerUpdates[n-1];
		std::push_heap(m_PeerUpdates.begin(), m_PeerUpdates.end());
	}
	// TODO need getState arguments, detail sorting and caching
	simcore::TimeStamp timestamp = m_Connection->getTimeStamp();
	simnet::NetworkMessage::Ref msg = m_Wrapper->unit()->getState(timestamp, 0 /*interval*/, 0 /*detail*/);
	if (msg.valid()) {
		msg->setRoutingType(ROUTE_UNIT_UPDATE);
		msg->setRoutingData(m_Wrapper->id());
		msg->setPriority(2);  // XXX msg/detail dependent?
		for (int i = 0; i < target_count; ++i) {
			m_Connection->send(msg, targets[i].id);
		}
	} else {
		CSP_LOG(BATTLEFIELD, ERROR, "no state message");
	}
	return (m_PeerUpdates[0].next_update - m_UpdateTime) * 1e-3;
}

void LocalBattlefield::UnitUpdateProxy::setUpdateInterval(PeerId id, double interval) {
	simdata::uint16 interval_ms = static_cast<simdata::uint16>(interval * 1000.0);
	const unsigned n = m_PeerUpdates.size();
	for (unsigned i = 0; i < n; ++i) {
		if (m_PeerUpdates[i].id == id) {
			m_PeerUpdates[i].interval = interval_ms;
			CSP_LOG(BATTLEFIELD, INFO, "set update interval for peer " << id << " to " << interval_ms << " ms");
			return;
		}
	}
	CSP_LOG(BATTLEFIELD, WARNING, "set update interval, peer " << id << " not found");
}

void LocalBattlefield::LocalUnitWrapper::setUnit(Unit const &object) {
	if (object.valid()) {
		SIMDATA_VERIFY(!(object->isStatic()));
		SIMDATA_VERIFY_EQ(object->isLocal(), owner() == 0);
	}
	setObject(object);
}

void LocalBattlefield::LocalUnitWrapper::setUpdateProxy(UpdateMaster &master, UpdateProxyConnection::Ref connection) {
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

void LocalBattlefield::LocalUnitWrapper::setUpdateInterval(PeerId id, double interval) {
	assert(m_UpdateProxy.valid());
	m_UpdateProxy->setUpdateInterval(id, interval);
}

LocalBattlefield::LocalUnitWrapper* LocalBattlefield::findLocalUnitWrapper(ObjectId id) {
	return static_cast<LocalUnitWrapper*>(findUnitWrapper(id));
}