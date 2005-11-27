// Combat Simulator Project - CSPSim
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
 * @file DispatchCenter.h
 * @brief Singleton for decoding and dispatching network messsages.
 *
 * The dispatch center maintains an array of TaggedRecordFactories,
 * indexed by message type.  When a network message arrives, the
 * header is parsed to determine the message type.  The dispatch
 * center uses the message type to retrieve the appropriate tagged
 * record factory and create a tagged record instance.  The tagged
 * record instance then initializes itself from the network message
 * payload.
 *
 * The message header also include a destination id, which is used
 * to retrieve the object that should receive the message.  If the
 * object does not exist, it may be created and added to the
 * battlefield.  The tagged record instance then passed to the
 * object's dispatch method.  Handers defined in the object's
 * class hierarchy may receive the record and take appropriate
 * actions.  If no handler is found, the record is passsed to the
 * objects childDispatch method.  This virtual method can be
 * overridden in derived classes to selectively forward messages on
 * to internal objects (e.g. the vehicle subsystems).
 *
 * Caveats:
 *
 * The message type is just a 16-bit integer id for a given tagged
 * record class that all nodes agree on.  Eventually this assignment
 * will be managed by a centralized server, and transmitted to each
 * client that joins the game.  For now we simply assign these ids
 * sequentially to all tagged record classes in alphabetical order by
 * class name.  Thus all clients must have the exact same set of
 * tagged record classes to communicate properly.  No validation has
 * been implemented yet, so remote connections should only be initiated
 * between clients sync'd to the same repository revision.
 *
 * Eventually, tagged record factories will return instances from an
 * object pool in order to minimize allocation/deallocation overhead.
 * The dispatch center will be responsible for returning the tagged
 * records to the pool after dispatch.  This requires that the object
 * receiving the message not maintain a reference to the tagged
 * record.  This constraint can be enforced by testing the reference
 * count after dispatch is completed.  Alternatively, we can use
 * a separate smart-pointer class that automatically returns an
 * instance to the parent object pool when the reference count
 * reaches zero (instead of deleting the instance).  Such a pool +
 * smart pointer system has been implemented, but not yet integrated
 * with CSPSim/SimData.
 *
 **/


#ifndef __CSPSIM_DISPATCHCENTER_H__
#define __CSPSIM_DISPATCHCENTER_H__


#include <SimData/TaggedRecord.h>
#include <SimData/Singleton.h>
#include <vector>

// bring TaggedRecord into the global namespace
using simdata::TaggedRecord;

// forward declarations
class NetworkMessage;
class MessageDispatcher;

namespace simdata {
	class TaggedRecordFactoryBase;
}


/** Singleton class for decoding network messages to tagged records, and
 *  dispatching these records to object handlers.
 */
class DispatchCenter: public simdata::Singleton<DispatchCenter> {
	friend class simdata::Singleton<DispatchCenter>;

public:

	DispatchCenter();

	/** Decode a tagged record from a raw network message.
	 */
	TaggedRecord::Ref decode(NetworkMessage *message) const;

	/** Encode a tagged record to a raw network message.
	 */
	bool encode(TaggedRecord::Ref record, NetworkMessage *message) const;

	/** Dispatch a tagged record to an object handler.
	 */
	bool dispatch(TaggedRecord::Ref record, MessageDispatcher &object) const;

private:
	std::vector<simdata::TaggedRecordFactoryBase *> m_factories;

};


#endif // __CSPSIM_DISPATCHCENTER_H__

