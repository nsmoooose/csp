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
 * @file LocalBattlefield.h
 *
 **/

#ifndef __CSPSIM_BATTLEFIELD_LOCALBATTLEFIELD_H__
#define __CSPSIM_BATTLEFIELD_LOCALBATTLEFIELD_H__

#include <csp/cspsim/battlefield/Battlefield.h>
#include <csp/cspsim/battlefield/BattlefieldMessages.h>

#include <csp/csplib/util/Callback.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/ScopedPointer.h>

#include <sigc++/signal.h>

CSP_NAMESPACE

class Client;
class DataManager;
class DispatchHandler;
class MessageQueue;
class NetworkMessage;
class Path;
class SceneManager;
class UpdateMaster;

class CSPSIM_EXPORT LocalBattlefield: public Battlefield {
public:

	LocalBattlefield(Ref<DataManager> const &data_manager);
	~LocalBattlefield();

	void update(double dt);

	/** Specify the network client used to communicate with the global battlefield
	 *  and other local battlefields.  The client's external node, if set, will be
	 *  used by the global battlefield to negotiate the initial connection between
	 *  remote peers.  If the peers are on the same LAN, the local ip address will
	 *  be used.  NB: in either case, only the port number specified in the local
	 *  node will used---the port number in the external node is currently ignored.
	 */
	void setNetworkClient(Ref<Client> const &client);

	void connectToServer(std::string const &name);

	/** Attach a scene manager to the battlefield.  Unless a scene manager is
	 *  set, camera and object visibility updates will not be computed.
	 */
	void setSceneManager(Ref<SceneManager> const &manager);

	/** Update the camera position.
	 *
	 *  Adjusts the camera position and object visibility.  Does nothing unless
	 *  a SceneManager has been specified with setSceneManager().  Should be
	 *  called everytime the camera moves (typically every frame).
	 *
	 *  @param eye_point the position of the camera in global coordinates.
	 *  @param look_pos the aim point of the camera in global coordinates.
	 *  @param up_vec a vector fixing the camera orientation around the viewing direction.
	 */
	void setCamera(Vector3 const &eye_point, const Vector3& look_pos, const Vector3& up_vec);

	// testing interface
	void __test__addLocalHumanUnit(Unit const &unit, bool human);

	inline bool isConnectionActive() const {
		return m_ConnectionState == CONNECTION_ACTIVE;
	}

	void registerPlayerJoinCallback(callback<void, int, const std::string&> &callback);
	void registerPlayerQuitCallback(callback<void, int, const std::string&> &callback);

private:

	struct ObjectIdPool;

	enum {
		CONNECTION_DETACHED,
		CONNECTION_JOIN,
		CONNECTION_ACTIVE,
		CONNECTION_ENDED
	} m_ConnectionState;

	// local-global messages
	void onIdAllocationResponse(Ref<IdAllocationResponse> const &msg, Ref<MessageQueue> const &queue);
	void onPlayerJoin(Ref<PlayerJoin> const &msg, Ref<MessageQueue> const &queue);
	void onPlayerQuit(Ref<PlayerQuit> const &msg, Ref<MessageQueue> const &queue);
	void onCommandUpdatePeer(Ref<CommandUpdatePeer> const &msg, Ref<MessageQueue> const &queue);
	void onCommandAddUnit(Ref<CommandAddUnit> const &msg, Ref<MessageQueue> const &queue);
	void onCommandRemoveUnit(Ref<CommandRemoveUnit> const &msg, Ref<MessageQueue> const &queue);
	void onJoinResponse(Ref<JoinResponse> const &msg, Ref<MessageQueue> const &queue);
	void bindCommandDispatch(Ref<DispatchHandler> const &dispatch);

	// peer-peer messages from one object to another
	void onUnitMessage(Ref<NetworkMessage> const &msg);
	// peer-peer messages from a source object to its mirror on this host
	void onUnitUpdate(Ref<NetworkMessage> const &msg);

	// allocate an id from the pool
	void assignNewId(Object const &object);

	/** Add and remove objects from the scene as the camera moves.
	 *
	 *  Called by setCamera() to update the scene when the camera grid position has
	 *  changed.  Although setCamera() may be called once for every frame, the grid
	 *  position of the camera should change much more slowly (with hysteresis), so
	 *  this method should not incur much overhead.
	 *
	 *  @param old_camera_position the grid coordinates of the previous camera position.
	 *  @param new_camera_position the grid coordinates of the new camera position.
	 */
	void updateVisibility(GridPoint old_camera_position, GridPoint new_camera_position);

	virtual void moveUnit(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position);

	bool updateUnitVisibility(UnitWrapper *wrapper, GridPoint const &old_position, GridPoint const &new_position);

	void sendServerCommand(Ref<NetworkMessage> const &msg);

	// Scene manager; used to add and remove units and static features from
	// the scene (if present).
	Ref<SceneManager> m_SceneManager;

	Ref<DataManager> m_DataManager;

	// The current position of the camera, in grid coordinates.
	GridPoint m_CameraGridPosition;

	Ref<Client> m_NetworkClient;

	ScopedPointer<UpdateMaster> m_UnitUpdateMaster;

	ScopedPointer<ObjectIdPool> m_LocalIdPool;

	double m_ServerTimeOffset;
	SimTime m_CurrentTime;
	TimeStamp m_CurrentTimeStamp;

	/// A helper class for sending unit updates to remote hosts.
	class UpdateProxyConnection;

	/// Wrapper for inserting dynamic objects into a quadtree index.
	class LocalUnitWrapper;

	/// A helper class for sending updates from a local unit to remote hosts.
	class UnitUpdateProxy;

	/// Information about remote players.
	class PlayerInfo;
	typedef Ref<PlayerInfo> PlayerInfoRef;
	typedef std::map<PeerId, PlayerInfoRef> PlayerInfoMap;
	PlayerInfoMap m_PlayerInfoMap;

	Ref<UpdateProxyConnection> m_UpdateProxyConnection;
	ScopedPointer<UpdateMaster> m_UnitRemoteUpdateMaster;

	LocalUnitWrapper* findLocalUnitWrapper(ObjectId id);

	// Data used for the slow iteration through local units, which performs
	// a spatial query for nearby units (local and remote).  The results of
	// this query are used to construct contact lists and to adjust the
	// the rate at which updates are sent to remote hosts.
	double m_ScanElapsedTime;
	double m_ScanRate;
	unsigned m_ScanIndex;
	int m_ScanSignature;
	std::vector<Unit> m_ScanUnits;

	// Perform a spatial query for objects near the specified unit.  Updates
	// the unit's contact list and the rate at which position messages are
	// sent to peers.  Called by continueUnitScan.
	void scanUnit(LocalUnitWrapper *wrapper);

	// Continue a slow iteration through all units in the battlefield, calling
	// scanUnit on each.  This method should be called once per time step.
	void continueUnitScan(double dt);

	ScopedPointer<sigc::signal<void, int, const std::string&> > m_PlayerJoinSignal;
	ScopedPointer<sigc::signal<void, int, const std::string&> > m_PlayerQuitSignal;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_BATTLEFIELD_LOCALBATTLEFIELD_H__
