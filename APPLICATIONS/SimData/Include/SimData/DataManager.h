/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
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

# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning(disable : 4786)
# endif

#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <SimData/Path.h>
#include <SimData/HashUtility.h>
#include <SimData/ns-simdata.h>


NAMESPACE_SIMDATA


class DataArchive;


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
	const PointerBase getObject(const char* path_str);

	/**
	 * Create a new object from a Path instance.
	 *
	 * @path the Path instance.
	 * @path_str the path identifier string (if available).  This is
	 *           only used for error logging.
	 */
	const PointerBase getObject(Path const& path, const char* path_str=0);

	/**
	 * Add a new data archive to the manager.
	 * 
	 * All objects in the archive will subsequently be available from the
	 * manager.  The manager "owns" the pointer and will delete it when the
	 * manager is destroyed.
	 */
	void addArchive(DataArchive *);

private:
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
	const PointerBase getObject(Path const& path, const char* path_str, DataArchive* d);
	
	std::vector<DataArchive*> archives;
	hasht_map archive_map;
};


NAMESPACE_END // namespace simdata

#endif //__SIMDATA_DATAMANAGER_H__

