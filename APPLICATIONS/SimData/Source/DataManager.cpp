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


#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>
#include <SimData/Log.h>
#include <SimData/Exception.h>


NAMESPACE_SIMDATA



DataManager::DataManager() {
}

DataManager::~DataManager() {
	std::vector<DataArchive*>::iterator i;
	for (i = archives.begin(); i != archives.end(); i++) {
		if (*i) {
			(*i)->setManager(0);
			delete *i;
		}
	}
}

void DataManager::addArchive(DataArchive *d) {
	if (d) {
		int n = archives.size();
		archives.push_back(d);
		std::vector<ObjectID> ids = d->getAllObjects();
		std::vector<ObjectID>::iterator i;
		for (i = ids.begin(); i != ids.end(); i++) {
			if (archive_map.find(*i) != archive_map.end()) {
				SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "Duplicate object ID [" << (*i) << "] adding data archive '" << d->getFileName() << "' to data manager.");
			}
			archive_map[*i] = n;
		}
		d->setManager(this);
	}
}

const PointerBase DataManager::getObject(const char* path) {
	return getObject(Path(path), path, 0);
}


const PointerBase DataManager::getObject(Path const& path, const char* path_str) {
	return getObject(path, path_str, 0);
}

const PointerBase DataManager::getObject(Path const& path, const char* path_str, DataArchive *d) {
	hasht key = (hasht) path.getPath();
	hasht_map::iterator idx = archive_map.find(key);
	DataArchive *archive = 0;
	if (idx != archive_map.end()) {
		archive = archives[idx->second];
	}
	if (archive == 0 || archive == d) {
		std::string msg;
		if (path_str==0 || *path_str==0) {
			msg = "human-readable path unavailable";
		} else {
			msg = path_str;
		}
		msg = "path not found (" + msg + ")" + key.str() + "\n";
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "DataManager::getObject() : " << msg);
		throw IndexError(msg.c_str());
	}
	return archive->getObject(path, path_str);
}


NAMESPACE_END // namespace simdata

