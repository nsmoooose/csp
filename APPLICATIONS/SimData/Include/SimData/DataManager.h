/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
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
 * @file DataManager.h
 * @author Mark Rose <mrose@stm.lbl.gov>
 */

#ifndef __SIMDATA_DATAMANAGER_H__
#define __SIMDATA_DATAMANAGER_H__

#include <SimData/Export.h>
#include <SimData/Path.h>
#include <SimData/HashUtility.h>
#include <SimData/Namespace.h>

#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>



NAMESPACE_SIMDATA


class DataArchive;
class LinkBase;
class InterfaceProxy;


/**
 * Class for managing read access to multiple data archives.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT DataManager {
	friend class DataArchive;

public:
	/**
	 * Construct a new (empty) data manager.
	 */
	DataManager();

	/**
	 * Destroy the data manager and all the archives it contains.
	 */
	virtual ~DataManager();
	
	/**
	 * Create a new object from a path identifier string.
	 *
	 * @path_str the path identifier string.
	 * @returns a smart-pointer to the new object.
	 */
	const LinkBase getObject(std::string const &path_str);

	/**
	 * Create a new object from a Path instance.
	 *
	 * @path the Path instance.
	 * @path_str the path identifier string (if available).  This is
	 *           only used for error logging.
	 */
	const LinkBase getObject(Path const& path, std::string const &path_str="");

	/**
	 * Add a new data archive to the manager.
	 * 
	 * All objects in the archive will subsequently be available from the
	 * manager.  The manager "owns" the pointer and will delete it when the
	 * manager is destroyed.
	 */
	void addArchive(DataArchive *);

	/**
	 * Get all children of a given path.
	 *
	 * For path "A:X.Y", returns all object id's "A:X.Y.*".  The id's
	 * can be converted to human-readable form by getPathString().
	 *
	 * @param path the path to search for children
	 * @returns a list of object id's immediately below the given path.
	 */
	std::vector<ObjectID> getChildren(ObjectID const &id) const;

	/**
	 * Get all children of a given path.
	 *
	 * For path "A:X.Y", returns all object id's "A:X.Y.*".  The id's
	 * can be converted to human-readable form by getPathString().
	 *
	 * @param path the path to search for children
	 * @returns a list of object id's immediately below the given path.
	 */
	std::vector<ObjectID> getChildren(std::string const & path) const;

	/**
	 * Check for the existance of an object in the archives.
	 *
	 * @returns true if the object id exists.
	 */
	bool hasObject(ObjectID const &id) const;

	/**
	 * Check for the existance of an object in the archives.
	 *
	 * @returns true if the object id exists.
	 */
	bool hasObject(std::string const & path) const;

	/** 
	 * Get the path string corresponding to a give object id.
	 *
	 * This provides a human-readable path string that is useful
	 * for error and debugging messages.
	 *
	 * @returns the path string if found, otherwise an empty string.
	 */
	std::string getPathString(ObjectID const &id) const; 

	/**
	 * Remove unused static objects from the cache.
	 *
	 * Call this method after many objects have been deleted
	 * to free any unused cache entries.
	 */
	void cleanStatic();

	/**
	 * Return the interface proxy corresponding to the specified
	 * object in the archive.
	 */
	InterfaceProxy *getObjectInterface(ObjectID const &id) const;

	/**
	 * Return the interface proxy corresponding to the specified
	 * object in the archive.
	 */
	InterfaceProxy *getObjectInterface(std::string const &path) const;


private:
	/**
	 * Find the archive that holds the specified object.
	 *
	 * Throws an exception if the object isn't found.
	 */
	DataArchive *findArchive(ObjectID const &id, std::string const &path_str, DataArchive const *d) const;

	/**
	 * Create a new object from a Path instance.
	 *
	 * For internal use by the DataArchive class.  When a particular
	 * DataArchive class fails to find an object, it asks the associated
	 * Manager to create the object.  The last parameter is used to
	 * prevent unwanted recursion if an object isn't found.
	 *
	 * @path the Path instance.
	 * @path_str the path identifier string (if available).  This is
	 *           only used for error logging.
	 * @d the data archive that is requesting the object.
	 */
	const LinkBase getObject(Path const& path, std::string const &path_str, DataArchive const *d) const;

	/**
	 * Return the interface proxy corresponding to the specified
	 * object in the archive.
	 *
	 * For internal use by the DataArchive class.  When a particular
	 * DataArchive class fails to find an object, it asks the associated
	 * Manager to create the object.  The last parameter is used to
	 * prevent unwanted recursion if an object isn't found.
	 */
	InterfaceProxy *getObjectInterface(ObjectID const &id, std::string const &path_str, DataArchive const *d) const;
	
	std::vector<DataArchive*> _archives;
	hasht_map _archive_map;

	typedef HASH_MAPS<hasht, std::vector<hasht>, hasht_hash, hasht_eq>::Type child_map;
	child_map _children;
};


NAMESPACE_SIMDATA_END

#endif //__SIMDATA_DATAMANAGER_H__

