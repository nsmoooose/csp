/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This file is part of SimData.
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


#include <SimData/TaggedRecordRegistry.h>
#include <SimData/Log.h>


NAMESPACE_SIMDATA


void TaggedRecordRegistry::registerFactory(TaggedRecordFactoryBase *factory) {
	assert(factory != 0);
	assert(!hasFactory(factory->getName()));
	SIMDATA_LOG(LOG_ALL, LOG_INFO, "Registering TaggedRecordFactory<" << factory->getName() << "> [" << factory->getId() << "]");
	TaggedRecord::Id id = factory->getId();
	HashT key(static_cast<uint32>(id), static_cast<uint32>(id>>32));
	_map[factory->getName()] = factory;
	_id_map[key] = factory;
}


NAMESPACE_SIMDATA_END

