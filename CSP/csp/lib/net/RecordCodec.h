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
 * @file RecordCodec.h
 * @brief A class for decoding tagged records.
 *
 * The message decoder maintains an array of TaggedRecordFactories,
 * indexed by message type.  When a network message arrives, the
 * header is parsed to determine the message type.  The message
 * decoder uses the message type to retrieve the appropriate tagged
 * record factory and create a tagged record instance.  The tagged
 * record instance then initializes itself from the network message
 * payload.
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
 *
 **/


#ifndef __CSPLIB_NET_RECORDCODEC_H__
#define __CSPLIB_NET_RECORDCODEC_H__


#include <csp/lib/net/TaggedRecord.h>
#include <vector>

CSP_NAMESPACE


class TaggedRecordFactoryBase;


/** Singleton class for decoding network messages to tagged records, and
 *  dispatching these records to object handlers.
 */
class RecordCodec {

	static const int MAX_MESSAGE_IDS = 65536;

public:

	RecordCodec();

	/** Decode a tagged record from a raw network message.
	 */
	Ref<TaggedRecord> decode(int local_id, uint8 const *buffer, const size_t buffer_length);

	/** Encode a tagged record to a raw network message.
	 */
	size_t encode(Ref<TaggedRecord> record, uint8 *buffer, size_t buffer_length);

	/** Assign a message id to a tagged record.  The new message id must not be
	 *  in use, and must be above the reserved range (0-63).
	 *
	 *  @param id The tagged record id.
	 *  @param local_id The new (custom) message id.
	 *  @return True on success.
	 */
	bool registerMessageId(TaggedRecord::Id id, int local_id);

	/** Clear all non-reserved message ids.
	 */
	void clearMessageIds();

private:
	TaggedRecordFactoryBase const *m_Factories[MAX_MESSAGE_IDS];
	//std::vector<TaggedRecordFactoryBase *> m_factories;

	BufferWriter m_Writer;
	BufferReader m_Reader;
	TagWriter m_TagWriter;
	TagReader m_TagReader;

};

CSP_NAMESPACE_END

#endif // __CSPLIB_NET_RECORDCODEC_H__

