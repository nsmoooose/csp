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

#ifndef __SIMCORE_BATTLEFIELD_LOCALBATTLEFIELD_H__
#define __SIMCORE_BATTLEFIELD_LOCALBATTLEFIELD_H__

#include <SimData/Ref.h>
#include <SimData/ScopedPointer.h>

#include <SimCore/Battlefield/Battlefield.h>
#include <SimCore/Battlefield/BattlefieldMessages.h>
#include <SimCore/Util/CallbackDecl.h>

class SceneManager;
class UpdateMaster;

namespace simnet { class Client; }
namespace simnet { class MessageQueue; }
namespace simnet { class NetworkMessage; }
namespace simnet { class DispatchHandler; }
namespace simdata { class Path; }
namespace simdata { class DataManager; }


class LocalBattlefield: public Battlefield {
public:

	LocalBattlefield(simdata::Ref<simdata::DataManager> const &data_manager);
	~LocalBattlefield();

	void update(double dt);

	/** Specify the network client used to communicate with the global battlefield
	 *  and other local battlefields.  The client's external node, if set, will be
	 *  used by the global battlefield to negotiate the initial connection between
	 *  remote peers.  If the peers are on the same LAN, the local ip address will
	 *  be used.  NB: in either case, only the port number specified in the local
	 *  node will used---the port number in the external node is currently ignored.
	 */
	void setNetworkClient(simdata::Ref<simnet::Client> const &client);

	void connectToServer(std::string const &name);

	/** Attach a scene manager to the battlefield.  Unless a scene manager is
	 *  set, camera and object visibility updates will not be computed.
	 */
	void setSceneManager(simdata::Ref<SceneManager> const &manager);

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
	void setCamera(simdata::Vector3 const &eye_point, const simdata::Vector3& look_pos, const simdata::Vector3& up_vec);

	// testing interface
	void __test__addLocalHumanUnit(Unit const &unit);

	inline bool isConnectionActive() const {
		return m_ConnectionState == CONNECTION_ACTIVE;
	}

	void registerPlayerJoinCallback(simcore::Callback2<int, const std::string&> &callback);
	void registerPlayerQuitCallback(simcore::Callback2<int, const std::string&> &callback);

private:

	struct ObjectIdPool;

	enum {
		CONNECTION_DETACHED,
		CONNECTION_JOIN,
		CONNECTION_ACTIVE,
		CONNECTION_ENDED
	} m_ConnectionState;

	// local-global messages
	void onIdAllocationResponse(simdata::Ref<IdAllocationResponse> const &msg, simdata::Ref<simnet::MessageQueue> const &queue);
	void onPlayerJoin(simdata::Ref<PlayerJoin> const &msg, simdata::Ref<simnet::MessageQueue> const &queue);
	void onPlayerQuit(simdata::Ref<PlayerQuit> const &msg, simdata::Ref<simnet::MessageQueue> const &queue);
	void onCommandUpdatePeer(simdata::Ref<CommandUpdatePeer> const &msg, simdata::Ref<simnet::MessageQueue> const &queue);
	void onCommandAddUnit(simdata::Ref<CommandAddUnit> const &msg, simdata::Ref<simnet::MessageQueue> const &queue);
	void onCommandRemoveUnit(simdata::Ref<CommandRemoveUnit> const &msg, simdata::Ref<simnet::MessageQueue> const &queue);
	void onJoinResponse(simdata::Ref<JoinResponse> const &msg, simdata::Ref<simnet::MessageQueue> const &queue);
	void bindCommandDispatch(simdata::Ref<simnet::DispatchHandler> const &dispatch);

	// peer-peer messages from one object to another
	void onUnitMessage(simdata::Ref<simnet::NetworkMessage> const &msg);
	// peer-peer messages from a source object to its mirror on this host
	void onUnitUpdate(simdata::Ref<simnet::NetworkMessage> const &msg);

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

	void sendServerCommand(simdata::Ref<simnet::NetworkMessage> const &msg);

	// Scene manager; used to add and remove units and static features from
	// the scene (if present).
	simdata::Ref<SceneManager> m_SceneManager;

	simdata::Ref<simdata::DataManager> m_DataManager;

	// The current position of the camera, in grid coordinates.
	GridPoint m_CameraGridPosition;

	simdata::Ref<simnet::Client> m_NetworkClient;

	simdata::ScopedPointer<UpdateMaster> m_UnitUpdateMaster;

	simdata::ScopedPointer<ObjectIdPool> m_LocalIdPool;

	double m_ServerTimeOffset;
	simdata::SimTime m_CurrentTime;
	simcore::TimeStamp m_CurrentTimeStamp;

	/// A helper class for sending unit updates to remote hosts.
	class UpdateProxyConnection;

	/// Wrapper for inserting dynamic objects into a quadtree index.
	class LocalUnitWrapper;

	/// A helper class for sending updates from a local unit to remote hosts.
	class UnitUpdateProxy;

	/// Information about remote players.
	class PlayerInfo;
	typedef simdata::Ref<PlayerInfo> PlayerInfoRef;
	typedef std::map<PeerId, PlayerInfoRef> PlayerInfoMap;
	PlayerInfoMap m_PlayerInfoMap;

	simdata::Ref<UpdateProxyConnection> m_UpdateProxyConnection;
	simdata::ScopedPointer<UpdateMaster> m_UnitRemoteUpdateMaster;

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

	void scanUnit(LocalUnitWrapper *wrapper);
	void continueUnitScan(double dt);

	simdata::ScopedPointer<simcore::Signal2<int, const std::string&> > m_PlayerJoinSignal;
	simdata::ScopedPointer<simcore::Signal2<int, const std::string&> > m_PlayerQuitSignal;
};


#endif // __SIMCORE_BATTLEFIELD_LOCALBATTLEFIELD_H__
