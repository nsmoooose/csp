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


#include <SimNet/NetLog.h>
#include <SimNet/RecordCodec.h>
#include <SimData/Verify.h>
#include <SimData/TaggedRecordRegistry.h>

#include <iostream>


namespace simnet {

RecordCodec::RecordCodec(): m_TagWriter(m_Writer), m_TagReader(m_Reader) {
	simdata::TaggedRecordRegistry const &registry = simdata::TaggedRecordRegistry::getInstance();
	simdata::TaggedRecordRegistry::FactoryList factories = registry.getFactories();
	for (int i = 0; i < MAX_MESSAGE_IDS; ++i) {
		m_Factories[i] = 0;
	}
	for (int i = 0; i < static_cast<int>(factories.size()); ++i) {
		int id = factories[i]->getCustomId();
		if (id != 0) {
			SIMNET_LOG(MESSAGE, DEBUG, "registering static message id " << id);
			m_Factories[id] = factories[i];
		}
	}
}

size_t RecordCodec::encode(TaggedRecord::Ref record, simdata::uint8 *buffer, size_t buffer_length) {
	assert(record.valid());
	assert(buffer != 0);
	m_Writer.bind(buffer, buffer_length);
	// XXX need to catch overflows
	record->serialize(m_TagWriter);
	return m_Writer.length();
}

TaggedRecord::Ref RecordCodec::decode(int local_id, simdata::uint8 const *buffer, const size_t buffer_length) {
	if (buffer == 0 || buffer_length == 0) {
		SIMNET_LOG(MESSAGE, ERROR, "no buffer");
		return 0;
	}
	SIMDATA_VERIFY_GE(local_id, 0);
	SIMDATA_VERIFY_LT(local_id, MAX_MESSAGE_IDS);
	simdata::TaggedRecordFactoryBase const *factory = m_Factories[local_id];
	if (!factory) {
		SIMNET_LOG(MESSAGE, ERROR, "unknown message id: " << local_id);
		return 0;
	}
	TaggedRecord::Ref record = factory->create();
	m_Reader.bind(buffer, buffer_length);
	// TODO need to catch errors
	record->serialize(m_TagReader);
	SIMDATA_VERIFY(!m_Reader.underflow());
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
	SIMDATA_VERIFY_LT(local_id, MAX_MESSAGE_IDS);
	SIMDATA_VERIFY_GE(local_id, 64);  // can't reassign reserved messages
	simdata::TaggedRecordRegistry const &registry = simdata::TaggedRecordRegistry::getInstance();
	simdata::TaggedRecordFactoryBase const *factory = registry.getFactory(id);
	if (!factory) return false;
	simdata::uint16 current_id = static_cast<simdata::uint16>(factory->getCustomId());
	if (current_id == local_id) {
		SIMDATA_VERIFY(m_Factories[current_id] == factory);
		return true;
	}
	if (current_id != 0) {
		if (m_Factories[current_id] != 0) {
			SIMDATA_VERIFY(m_Factories[current_id] == factory);
			m_Factories[current_id] = 0;
		}
	}
	SIMDATA_VERIFY(m_Factories[local_id] == 0);
	m_Factories[local_id] = factory;
	factory->setCustomId(local_id);
	return true;
}

} // namespace simnet

