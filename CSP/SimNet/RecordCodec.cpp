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


#include <SimNet/RecordCodec.h>
#include <SimData/TaggedRecordRegistry.h>

#include <iostream>


namespace simnet {

RecordCodec::RecordCodec(): m_TagWriter(m_Writer), m_TagReader(m_Reader) {
	simdata::TaggedRecordRegistry const &registry = simdata::TaggedRecordRegistry::getInstance();
	m_factories = registry.getFactories();
	std::cout << "RECORD CODE: size=" << m_factories.size() << "\n";
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
	assert(buffer != 0 && buffer_length > 0);
	assert(local_id < static_cast<int>(m_factories.size()));
	simdata::TaggedRecordFactoryBase *factory = m_factories[local_id];
	TaggedRecord::Ref record = factory->create();
	m_Reader.bind(buffer, buffer_length);
	// TODO need to catch errors
	record->serialize(m_TagReader);
	assert(!m_Reader.underflow());
	return record;
}

} // namespace simnet

