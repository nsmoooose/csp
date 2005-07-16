/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
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
 * @file DataManager.cpp
 * @brief Classes for managing multiple data archives
 */

#include <SimData/DataManager.h>
#include <SimData/DataArchive.h>
#include <SimData/Log.h>
#include <SimData/ExceptionBase.h>


NAMESPACE_SIMDATA



DataManager::DataManager() {
}

DataManager::~DataManager() {
	closeAll();
}

void DataManager::closeAll() {
	Archives::iterator i;
	for (i = _archives.begin(); i != _archives.end(); i++) {
		if (*i != 0) {
			(*i)->setManager(0);
			delete *i;
			*i = 0;
		}
	}
	_archives.clear();
}

void DataManager::addArchive(DataArchive *d) {
	if (d) {
		bool added = false;
		std::size_t idx = 0;
		for (idx = 0; idx < _archives.size(); idx++) {
			if (_archives[idx] == 0) {
				_archives[idx] = d;
				added = true;
			}
		}
		if (!added) {
			_archives.push_back(d);
		}
		std::vector<ObjectID> ids = d->getAllObjects();
		std::vector<ObjectID>::iterator id;
		for (id = ids.begin(); id != ids.end(); id++) {
			if (_archive_map.find(*id) != _archive_map.end()) {
				SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "Duplicate object ID [" << (*id) << "] adding data archive '" << d->getFileName() << "' to data manager.");
			}
			_archive_map[*id] = idx;
		}
		d->setManager(this);
		DataArchive::ChildMap const &map = d->getChildMap();
		DataArchive::ChildMap::const_iterator imap = map.begin();
		for (; imap != map.end(); imap++) {
			std::vector<ObjectID> &childlist = _children[imap->first];
			childlist.insert(childlist.begin(), imap->second.begin(), imap->second.end());
		}
	}
}

std::vector<ObjectID> DataManager::getChildren(ObjectID const &id) const {
	ChildMap::const_iterator idx = _children.find(id);
	if (idx == _children.end()) { return std::vector<ObjectID>(); }
	return idx->second;
}

std::vector<ObjectID> DataManager::getChildren(std::string const & path) const {
	return getChildren(hash_string(path.c_str()));
}

bool DataManager::hasObject(ObjectID const &id) const {
	ArchiveMap::const_iterator idx = _archive_map.find(id);
	return (idx != _archive_map.end());
}

bool DataManager::hasObject(std::string const & path) const {
	return hasObject(hash_string(path.c_str()));
}

std::string DataManager::getPathString(ObjectID const &id) const {
	ArchiveMap::const_iterator idx = _archive_map.find(id);
	if (idx == _archive_map.end()) return "";
	return _archives[idx->second]->getPathString(id);
}

const LinkBase DataManager::getObject(std::string const &path) {
	return getObject(Path(path), path, 0);
}

const LinkBase DataManager::getObject(Path const& path, std::string const &path_str) {
	return getObject(path, path_str, 0);
}

const LinkBase DataManager::getObject(Path const& path, std::string const &path_str, DataArchive const *d) const {
	hasht id = (hasht) path.getPath();
	DataArchive *archive = findArchive(id, path_str, d);
	return archive->getObject(path, path_str);
}

void DataManager::cleanStatic() {
	for (Archives::iterator i = _archives.begin(); i != _archives.end(); i++) {
		if (*i != 0) {
			(*i)->cleanStatic();
		}
	}
}

InterfaceProxy *DataManager::getObjectInterface(ObjectID const &id) const {
	return getObjectInterface(id, "", 0);
}

InterfaceProxy *DataManager::getObjectInterface(std::string const &path) const {
	return getObjectInterface(hash_string(path.c_str()), path, 0);
}

InterfaceProxy *DataManager::getObjectInterface(ObjectID const &id, std::string const &path_str, DataArchive const *d) const {
	DataArchive *archive = findArchive(id, path_str, d);
	return archive->getObjectInterface(id);
}

DataArchive *DataManager::findArchive(ObjectID const &id, std::string const &path_str, DataArchive const *d) const {
	ArchiveMap::const_iterator idx = _archive_map.find(id);
	DataArchive *archive = 0;
	if (idx != _archive_map.end()) {
		assert(idx->second < _archives.size());
		archive = _archives[idx->second];
	}
	if (archive == 0 || archive == d) {
		std::string msg = path_str;
		if (msg=="") {
			msg = "human-readable path unavailable";
		}
		msg = "path not found (" + msg + ")" + id.str() + "\n";
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "DataManager::findArchive() : " << msg);
		throw IndexError(msg.c_str());
	}
	return archive;
}

NAMESPACE_SIMDATA_END

