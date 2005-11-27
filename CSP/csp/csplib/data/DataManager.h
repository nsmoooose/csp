/* Combat Simulator Project
 * Copyright (C) 2002, 2003 Mark Rose <mkrose@users.sf.net>
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
 * @brief Classes for managing multiple data archives.
 */

#ifndef __CSPLIB_DATA_DATAMANAGER_H__
#define __CSPLIB_DATA_DATAMANAGER_H__

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/HashUtility.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Path.h>

#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>


CSP_NAMESPACE


class DataArchive;
class LinkBase;
class InterfaceProxy;


/** Class for managing read access to multiple data archives.
 */
class CSP_EXPORT DataManager: public Referenced {
	friend class DataArchive;

public:
	/** Construct a new (empty) data manager.
	 */
	DataManager();

	/** Destroy the data manager and all the archives it contains.
	 */
	virtual ~DataManager();
	
	/** Create a new object from a path identifier string.
	 *
	 *  @param path_str the path identifier string.
	 *  @returns a smart-pointer to the new object.
	 */
	const LinkBase getObject(std::string const &path_str);

	/** Create a new object from a Path instance.
	 *
	 *  @param path the Path instance.
	 *  @param path_str the path identifier string (if available).  This is
	 *                  only used for error logging.
	 */
	const LinkBase getObject(Path const& path, std::string const &path_str="");

	/** Add a new data archive to the manager.
	 *
	 *  All objects in the archive will subsequently be available from the
	 *  manager.  The manager "owns" the pointer and will delete it when the
	 *  manager is destroyed.
	 */
	void addArchive(DataArchive *);

	/** Get all children of a given object id.
	 *
	 *  For id "A:X.Y", returns all object id's "A:X.Y.*".  The id's
	 *  can be converted to human-readable form by getPathString().
	 *
	 *  @param id the object id to search for children
	 *  @returns a list of object id's immediately below the given path.
	 */
	std::vector<ObjectID> getChildren(ObjectID const &id) const;

	/** Get all children of a given path.
	 *
	 *  For path "A:X.Y", returns all object id's "A:X.Y.*".  The id's
	 *  can be converted to human-readable form by getPathString().
	 *
	 *  @param path the path to search for children
	 *  @returns a list of object id's immediately below the given path.
	 */
	std::vector<ObjectID> getChildren(std::string const & path) const;

	/** Check for the existance of an object in the archives.
	 *
	 *  @returns true if the object id exists.
	 */
	bool hasObject(ObjectID const &id) const;

	/** Check for the existance of an object in the archives.
	 *
	 *  @returns true if the object id exists.
	 */
	bool hasObject(std::string const & path) const;

	/** Get the path string corresponding to a give object id.
	 *
	 *  This provides a human-readable path string that is useful
	 *  for error and debugging messages.
	 *
	 *  @returns the path string if found, otherwise an empty string.
	 */
	std::string getPathString(ObjectID const &id) const;

	/** Remove unused static objects from the cache.
	 *
	 *  Call this method after many objects have been deleted
	 *  to free any unused cache entries.
	 */
	void cleanStatic();

	/** Return the interface proxy corresponding to the specified
	 *  object in the archive.
	 */
	InterfaceProxy *getObjectInterface(ObjectID const &id) const;

	/** Return the interface proxy corresponding to the specified
	 *  object in the archive.
	 */
	InterfaceProxy *getObjectInterface(std::string const &path) const;

	/** Close all managed archives.
	 */
	void closeAll();

private:
	/** Find the archive that holds the specified object.
	 *
	 *  Throws an exception if the object isn't found.
	 */
	DataArchive *findArchive(ObjectID const &id, std::string const &path_str, DataArchive const *d) const;

	/** Create a new object from a Path instance.
	 *
	 *  For internal use by the DataArchive class.  When a particular
	 *  DataArchive class fails to find an object, it asks the associated
	 *  Manager to create the object.  The last parameter is used to
	 *  prevent unwanted recursion if an object isn't found.
	 *
	 *  @param path the Path instance.
	 *  @param path_str the path identifier string (if available).  This is
	 *         only used for error logging.
	 *  @param d the data archive that is requesting the object.
	 */
	const LinkBase getObject(Path const& path, std::string const &path_str, DataArchive const *d) const;

	/** Return the interface proxy corresponding to the specified
	 *  object in the archive.
	 *
	 *  For internal use by the DataArchive class.  When a particular
	 *  DataArchive class fails to find an object, it asks the associated
	 *  Manager to create the object.  The last parameter is used to
	 *  prevent unwanted recursion if an object isn't found.
	 */
	InterfaceProxy *getObjectInterface(ObjectID const &id, std::string const &path_str, DataArchive const *d) const;
	
	typedef std::vector<DataArchive *> Archives;
	/// The collection of managed archives.
	Archives _archives;

	typedef HashMap<ObjectID, std::size_t>::Type ArchiveMap;
	/// A map for finding the index of an archive.
	ArchiveMap _archive_map;

	typedef HashMap<ObjectID, std::vector<hasht> >::Type ChildMap;
	/// A map of all parent-child relationships in the managed archives.
	ChildMap _children;
};


CSP_NAMESPACE_END

#endif // __CSPLIB_DATA_DATAMANAGER_H__

