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


#include <SimData/DataManager.h>
#include <SimData/DataArchive.h>
#include <SimData/Log.h>
#include <SimData/Exception.h>


NAMESPACE_SIMDATA



DataManager::DataManager() {
}

DataManager::~DataManager() {
	std::vector<DataArchive*>::iterator i;
	for (i = _archives.begin(); i != _archives.end(); i++) {
		if (*i) {
			(*i)->setManager(0);
			delete *i;
		}
	}
}

void DataManager::addArchive(DataArchive *d) {
	if (d) {
		int n = _archives.size();
		_archives.push_back(d);
		std::vector<ObjectID> ids = d->getAllObjects();
		std::vector<ObjectID>::iterator i;
		for (i = ids.begin(); i != ids.end(); i++) {
			if (_archive_map.find(*i) != _archive_map.end()) {
				SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "Duplicate object ID [" << (*i) << "] adding data archive '" << d->getFileName() << "' to data manager.");
			}
			_archive_map[*i] = n;
		}
		d->setManager(this);
		DataArchive::child_map const &map = d->getChildMap();
		DataArchive::child_map::const_iterator imap = map.begin();
		for (; imap != map.end(); imap++) {
			std::vector<ObjectID> &childlist = _children[imap->first];
			childlist.insert(childlist.begin(), imap->second.begin(), imap->second.end());
		}
	}
}

std::vector<ObjectID> DataManager::getChildren(ObjectID const &id) const {
	child_map::const_iterator idx = _children.find(id);
	if (idx == _children.end()) { return std::vector<ObjectID>(); }
	return idx->second;
}

std::vector<ObjectID> DataManager::getChildren(std::string const & path) const {
	return getChildren(hash_string(path.c_str()));
}

bool DataManager::hasObject(ObjectID const &id) const {
	hasht_map::const_iterator idx = _archive_map.find(id);
	return (idx != _archive_map.end());
}

bool DataManager::hasObject(std::string const & path) const {
	return hasObject(hash_string(path.c_str()));
}

std::string DataManager::getPathString(ObjectID const &id) const {
	hasht_map::const_iterator idx = _archive_map.find(id);
	if (idx == _archive_map.end()) return "";
	return _archives[idx->second]->getPathString(id);
}

const LinkBase DataManager::getObject(const char* path) {
	return getObject(Path(path), path, 0);
}

const LinkBase DataManager::getObject(Path const& path, const char* path_str) {
	return getObject(path, path_str, 0);
}

const LinkBase DataManager::getObject(Path const& path, const char* path_str, DataArchive *d) {
	hasht key = (hasht) path.getPath();
	hasht_map::iterator idx = _archive_map.find(key);
	DataArchive *archive = 0;
	if (idx != _archive_map.end()) {
		assert(idx->second >= 0 && idx->second < _archives.size());
		archive = _archives[idx->second];
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

void DataManager::cleanStatic() {
	std::vector<DataArchive*>::iterator i;
	for (i = _archives.begin(); i != _archives.end(); i++) {
		if (*i) {
			(*i)->cleanStatic();
		}
	}
}

NAMESPACE_END // namespace simdata

