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


#include <csp/csplib/net/RecordCodec.h>
#include <csp/csplib/net/TaggedRecordRegistry.h>
#include <csp/csplib/util/Verify.h>

#include <iostream>


namespace csp {

CSP_STATIC_CONST_DEF(int RecordCodec::MAX_MESSAGE_IDS);

RecordCodec::RecordCodec(): m_TagWriter(m_Writer), m_TagReader(m_Reader) {
	TaggedRecordRegistry const &registry = TaggedRecordRegistry::getTaggedRecordRegistry();
	TaggedRecordRegistry::FactoryList factories = registry.getFactories();
	for (int i = 0; i < MAX_MESSAGE_IDS; ++i) {
		m_Factories[i] = 0;
	}
	for (int i = 0; i < static_cast<int>(factories.size()); ++i) {
		int id = factories[i]->getCustomId();
		if (id != 0) {
			CSPLOG(DEBUG, MESSAGE) << "REGISTER: <" << factories[i]->getName() << "> " << id;
			m_Factories[id] = factories[i];
		}
	}
}

size_t RecordCodec::encode(Ref<TaggedRecord> record, uint8_t *buffer, size_t buffer_length) {
	assert(record.valid());
	assert(buffer != 0);
	m_Writer.bind(buffer, buffer_length);
	// XXX need to catch overflows
	record->serialize(m_TagWriter);
	return m_Writer.length();
}

Ref<TaggedRecord> RecordCodec::decode(int local_id, uint8_t const *buffer, const size_t buffer_length) {
	if (buffer == 0 || buffer_length == 0) {
		CSPLOG(ERROR, MESSAGE) << "no buffer";
		return 0;
	}
	CSP_VERIFY_GE(local_id, 0);
	CSP_VERIFY_LT(local_id, MAX_MESSAGE_IDS);
	TaggedRecordFactoryBase const *factory = m_Factories[local_id];
	if (!factory) {
		CSPLOG(ERROR, MESSAGE) << "unknown message id: " << local_id;
		return 0;
	}
	Ref<TaggedRecord> record = factory->create();
	m_Reader.bind(buffer, buffer_length);
	try {
		record->serialize(m_TagReader);
	} catch (DataUnderflow const &/*err*/) {
		CSPLOG(ERROR, MESSAGE) << "buffer underflow decoding message";
		// TODO need to modify tagged record serialization so that compound blocks
		// are prefixed with their length (instead of start/end tags).  this will
		// allow the deserializer to skip unknown tags at the end of each compound
		// block so that new fields remain backwards compatible.
		return 0;
	}
	return record;
}

void RecordCodec::clearMessageIds() {
	for (int i = 64; i < MAX_MESSAGE_IDS; ++i) {
		if (m_Factories[i] == 0) continue;
		m_Factories[i]->setCustomId(0);
		m_Factories[i] = 0;
	}
}

bool RecordCodec::registerMessageId(TaggedRecord::Id id, int local_id) {
	CSP_VERIFY_LT(local_id, MAX_MESSAGE_IDS);
	CSP_VERIFY_GE(local_id, 64);  // can't reassign reserved messages
	TaggedRecordRegistry const &registry = TaggedRecordRegistry::getTaggedRecordRegistry();
	TaggedRecordFactoryBase const *factory = registry.getFactory(id);
	if (!factory) return false;
	uint16_t current_id = static_cast<uint16_t>(factory->getCustomId());
	if (current_id == local_id) {
		CSP_VERIFY(m_Factories[current_id] == factory);
		return true;
	}
	if (current_id != 0) {
		if (m_Factories[current_id] != 0) {
			CSP_VERIFY(m_Factories[current_id] == factory);
			m_Factories[current_id] = 0;
		}
	}
	CSP_VERIFY(m_Factories[local_id] == 0);
	m_Factories[local_id] = factory;
	factory->setCustomId(local_id);
	return true;
}

} // namespace csp

