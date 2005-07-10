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
 * @file TaggedRecordRegistry.h
 * @brief Classes for storing and accessing tagged record factories.
 */


#ifndef __SIMDATA_TAGGEDRECORDREGISTRY_H__
#define __SIMDATA_TAGGEDRECORDREGISTRY_H__

#include <string>
#include <vector>

#include <SimData/Export.h>
#include <SimData/HashUtility.h>
#include <SimData/TaggedRecord.h>
#include <SimData/Singleton.h>


NAMESPACE_SIMDATA


/** Abstract interface for TaggedRecord factories.
 *
 *  Automatically registers with the TaggedRecordRegistry when instantiated.
 */
class SIMDATA_EXPORT TaggedRecordFactoryBase {
public:
	TaggedRecordFactoryBase() { }
	virtual Ref<TaggedRecord> create() const=0;
	virtual std::string getName() const=0;
	virtual int getVersion() const=0;
	virtual TaggedRecord::Id getId() const=0;
	virtual void setCustomId(int) const=0;
	virtual int getCustomId() const=0;
	virtual ~TaggedRecordFactoryBase() { }
};


/** Singleton class to store and access all TaggedRecords in the application.
 *
 *  TaggedRecord classes automatically register themselves with the global instance
 *  of this class at startup.
 *
 *  @author Mark Rose <mkrose@users.sf.net>
 */
class SIMDATA_EXPORT TaggedRecordRegistry: public Singleton<TaggedRecordRegistry> {

friend class Singleton<TaggedRecordRegistry>;

template <class RECORD> friend class TaggedRecordFactory;

public:
	typedef std::vector<TaggedRecordFactoryBase *> FactoryList;

	/** Create a new tagged record by name.
	 *
	 *  @param The name of the tagged record class.
	 *  @returns 0 if the tagged record is not found.
	 */
	Ref<TaggedRecord> createRecord(std::string const &name) const;

	/** Create a new tagged record by id.
	 *
	 *  @returns 0 if the tagged record is not found.
	 */
	Ref<TaggedRecord> createRecord(TaggedRecord::Id id) const;

	/** Test if a tagged record is registered.
	 *
	 *  @param name The tagged record class name.
	 */
	bool hasFactory(std::string const &name) const;

	/** Test if a tagged record is registered.
	 *
	 *  @param id The tagged record id.
	 */
	bool hasFactory(TaggedRecord::Id id) const;

	/** Get the factory for a given tagged record id.
	 *  @param id The tagged record id.
	 *  @return The corresponding factory, or NULL.
	 */
	TaggedRecordFactoryBase const *getFactory(TaggedRecord::Id id) const;

	/** Get a list of all interfaces in the registry.
	 */
	FactoryList getFactories() const;

	/** Get the interface registry singleton.
	 */
	static inline TaggedRecordRegistry &getTaggedRecordRegistry() {
		return getInstance();
	}

private:

	~TaggedRecordRegistry() { }

	/** Register a factory with the registry.
	 */
	void registerFactory(TaggedRecordFactoryBase *factory);

	TaggedRecordRegistry() { }

	typedef HASH_MAPS<std::string, TaggedRecordFactoryBase*, hashstring, eqstring>::Type FactoryMap;
	typedef HASHT_MAP<TaggedRecordFactoryBase*>::Type FactoryIdMap;

	FactoryMap _map;
	FactoryIdMap _id_map;

};


/** Factory template for creating TaggedRecord subclasses.
 */
template <class RECORD>
class TaggedRecordFactory: public TaggedRecordFactoryBase {
public:
	typedef RECORD RecordType;
	TaggedRecordFactory(): TaggedRecordFactoryBase() {
		TaggedRecordRegistry &registry = TaggedRecordRegistry::getInstance();
		registry.registerFactory(this);
	}
	virtual Ref<TaggedRecord> create() const { return new RECORD(); }
	virtual std::string getName() const { return RECORD::_getName(); }
	virtual int getVersion() const { return RECORD::_getVersion(); }
	virtual TaggedRecord::Id getId() const { return RECORD::_getId(); };
	virtual void setCustomId(int id) const { RECORD::_setCustomId(id); }
	virtual int getCustomId() const { return RECORD::_getCustomId(); }
};


NAMESPACE_SIMDATA_END

#endif //  __SIMDATA_TAGGEDRECORDREGISTRY_H__
