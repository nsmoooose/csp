/* Combat Simulator Project
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


/**
 * @file TaggedRecordRegistry.cpp
 * @brief Classes for storing and accessing tagged record factories.
 */


#include <csp/csplib/net/TaggedRecordRegistry.h>
#include <csp/csplib/util/Log.h>

namespace csp {

TaggedRecordRegistry &TaggedRecordRegistry::getTaggedRecordRegistry() {
	return TaggedRecordRegistry::getInstance();
}

void TaggedRecordRegistry::registerFactory(TaggedRecordFactoryBase *factory) {
	assert(factory != 0);
	assert(!hasFactory(factory->getName()));
	CSPLOG(DEBUG, ALL) << "Registering TaggedRecordFactory<" << factory->getName() << "> [" << factory->getId() << "]";
	TaggedRecord::Id id = factory->getId();
	HashT key(static_cast<uint32_t>(id), static_cast<uint32_t>(id>>32));
	_map[factory->getName()] = factory;
	_id_map[key] = factory;
}

Ref<TaggedRecord> TaggedRecordRegistry::createRecord(std::string const &name) const {
	FactoryMap::const_iterator it = _map.find(name);
	if (it != _map.end()) return it->second->create();
	return 0;
}

Ref<TaggedRecord> TaggedRecordRegistry::createRecord(TaggedRecord::Id id) const {
	HashT key(static_cast<uint32_t>(id), static_cast<uint32_t>(id>>32));
	FactoryIdMap::const_iterator it = _id_map.find(key);
	if (it != _id_map.end()) return it->second->create();
	return 0;
}

bool TaggedRecordRegistry::hasFactory(std::string const &name) const {
	FactoryMap::const_iterator it = _map.find(name);
	return it != _map.end();
}

bool TaggedRecordRegistry::hasFactory(TaggedRecord::Id id) const {
	HashT key(static_cast<uint32_t>(id), static_cast<uint32_t>(id>>32));
	FactoryIdMap::const_iterator it = _id_map.find(key);
	return it != _id_map.end();
}

TaggedRecordFactoryBase const *TaggedRecordRegistry::getFactory(TaggedRecord::Id id) const {
	HashT key(static_cast<uint32_t>(id), static_cast<uint32_t>(id>>32));
	FactoryIdMap::const_iterator it = _id_map.find(key);
	if (it != _id_map.end()) return it->second;
	return 0;
}

TaggedRecordRegistry::FactoryList TaggedRecordRegistry::getFactories() const {
	FactoryList list;
	FactoryIdMap::const_iterator it = _id_map.begin();
	for (; it != _id_map.end(); ++it) {
		list.push_back(it->second);
	}
	return list;
}


} // namespace csp

