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
 * @file DispatchCenter.cpp
 *
 **/


#include "DispatchCenter.h"
#include "Dispatch.h"
#include "Log.h"
#include "Config.h"
#include <SimNet/Networking.h>

#include <SimData/TaggedRecordRegistry.h>


DispatchCenter::DispatchCenter() {
	simdata::TaggedRecordRegistry const &registry = simdata::TaggedRecordRegistry::getInstance();
	// TODO will need to be sorted and validated eventually!
	m_factories = registry.getFactories();
}

TaggedRecord::Ref DispatchCenter::decode(NetworkMessage *message) const {
	assert(message);
	simdata::uint16 id = message->getType();
	assert(id < m_factories.size());
	simdata::TaggedRecordFactoryBase *factory = m_factories[id];
	TaggedRecord::Ref record = factory->create();
	// TODO too much buffer copying; need to optimize
	string payload(static_cast<char*>(message->getPayloadPtr()), message->getPayloadLen());
	simdata::StringReader reader(payload);
	simdata::TagReader tag_reader(reader);
	// TODO need to catch errors
	record->serialize(tag_reader);
	return record;
}

bool DispatchCenter::encode(TaggedRecord::Ref record, NetworkMessage *message) const {
	assert(record.valid());
	assert(message != 0);
	// TODO get the id from the record.  since ids are assigned dynamically, we
	// need to extend the tagged record interface to allow ids to be assigned to the
	// record class, and retrieved from any record instance (ie. need a class static
	// field --- uint16).
	simdata::uint16 id = 0; // XXX
	// TODO serialize without constructing a new writer/tagwriter each time, and
	// without reallocating the write buffer.
	simdata::StringWriter writer;
	simdata::TagWriter tag_writer(writer);
	record->serialize(tag_writer);
	std::string const &buffer = writer.str();
	// TODO message initialization from the buffer data needs to be implemented
	// (requires changes to NetworkMessage)
	// message->initialize(id, buffer.data(), buffer.size());
	return true;
}

bool DispatchCenter::dispatch(TaggedRecord::Ref record, MessageDispatchBase &object) const {
	return object.dispatchMessage(record);
}

