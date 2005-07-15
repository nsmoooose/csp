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

#include <SimData/DataArchive.h>
#include <SimData/Archive.h>
#include <SimData/DataManager.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/InterfaceProxy.h>
#include <SimData/Log.h>
#include <SimData/Object.h>
#include <SimData/Verify.h>

#include <iomanip>
#include <iostream>
#include <sstream>


NAMESPACE_SIMDATA


/*
long DataArchive::_getOffset() { return ftell(_f); } // for Python use only
FP DataArchive::_filePointer() {
	FP x;
	x.f = _f;
	x.name = _fn;
	x.mode = isWrite() ? "wb" : "rb";
	return x;
}
*/

std::string base_path(std::string const &path) {
	int x = path.find_last_of('.');
	if (x <= 0) return "";
	return std::string(path, 0, x);
}


void DataArchive::_addEntry(int offset, int length, ObjectID hash, std::string const &path) {
	TableEntry t;
	t.offset = offset;
	t.length = length;
	t.classhash = hash;
	t.pathhash = hash_string(path);
	_table_map[t.pathhash] = _table.size();
	_table.push_back(t);
	ObjectID parent = hash_string(base_path(path));
	_children[parent].push_back(t.pathhash);
	_paths.push_back(path);
	_pathmap[t.pathhash] = path;
}

void DataArchive::writeMagic() {
	fprintf(_f, "RAWDAT-L");
}

void DataArchive::readMagic() {
	char magic[9];
	fread(magic, 8, 1, _f);
	magic[8] = 0;
	if (strncmp(magic, "RAWDAT-", 7)) {
		std::string msg;
		msg = msg + "incorrect magic '" + magic + "' in archive '" + _fn + "'";
		throw BadMagic(msg.c_str());
	}
	const bool data_little = (magic[7] == 'L');
	SIMDATA_VERIFY(data_little);
}

void DataArchive::readTable() {
	size_t n;
	n = fread(&_table_offset, sizeof(_table_offset), 1, _f);
	if (n != 1) {
		throw CorruptArchive("Lookup table offset.");
	}
	fseek(_f, _table_offset, SEEK_SET);
	uint32 n_objects;
	n = fread(&n_objects, sizeof(n_objects), 1, _f);
	if (n != 1 || n_objects > 100000) {
		throw CorruptArchive("Number of objects.");
	}
	_table.resize(n_objects);
	n = fread(&(_table[0]), sizeof(TableEntry), n_objects, _f);
	if (static_cast<uint32>(n) != n_objects) {
		throw CorruptArchive("Lookup table truncated.");
	}
	for (std::size_t i = 0; i < static_cast<std::size_t>(n_objects); i++) {
		_table_map[_table[i].pathhash] = i;
	}
	_readPaths();
}

void DataArchive::_readPaths() {
	uint32 path_toc_size;
	size_t n;
	n = fread(&path_toc_size, sizeof(path_toc_size), 1, _f);
	if (n != 1 || path_toc_size < static_cast<uint32>(sizeof(uint32)*2) || path_toc_size > 1000000) {
		throw CorruptArchive("Path table of contents.");
	}
	std::vector<char> toc_buffer(path_toc_size);
	n = fread(&(toc_buffer[0]), 1, path_toc_size, _f);
	if (static_cast<uint32>(n) != path_toc_size || toc_buffer[path_toc_size-1] != 0) {
		throw CorruptArchive("Path table of contents truncated.");
	}
	uint32 *iptr = reinterpret_cast<uint32*>(&toc_buffer[0]);
	uint32 n_paths = *iptr++;
	uint32 n_directories = *iptr++;
	_paths.reserve(n_paths);
	ObjectID *hptr = reinterpret_cast<ObjectID *>(iptr);
	while (n_directories-- > 0) {
		ObjectID node = *hptr++;
		uint32 n_children = hptr->a;
		++hptr;
		if (n_children > n_paths) {
			throw CorruptArchive("Path table of contents.");
		}
		std::vector<ObjectID> &childlist = _children[node];
		childlist.reserve(n_children);
		while (n_children-- > 0) {
			childlist.push_back(*hptr++);
		}
	}
	char *cptr = reinterpret_cast<char*>(hptr);
	char *toc_end = &(toc_buffer[0]) + path_toc_size;
	while (n_paths-- > 0) {
		std::string path = cptr;
		cptr += path.size() + 1;
		_paths.push_back(path);
		_pathmap[hash_string(path)] = path;
		if (cptr >= toc_end && n_paths != 0) {
			throw CorruptArchive("Path table of contents truncated.");
		}
	}
	if (cptr != toc_end) {
		throw CorruptArchive("Path table of contents truncated.");
	}
}

void DataArchive::writeTable() {
	assert(!_is_read);
	if (_finalized) return;
	_table_offset = static_cast<uint32>(ftell(_f));
	uint32 n_objects = static_cast<uint32>(_table.size());
	fwrite(&n_objects, sizeof(n_objects), 1, _f);
	fwrite(&(_table[0]), sizeof(TableEntry), _table.size(), _f);
	_writePaths();
	fseek(_f, 8, SEEK_SET);
	fwrite(&_table_offset, sizeof(_table_offset), 1, _f);
	_finalized = true;
}	

void DataArchive::_writePaths() const {
	ChildMap::const_iterator iter;
	long start = ftell(_f);
	uint32 size = 0;
	fwrite(&size, sizeof(size), 1, _f);
	size = _paths.size();
	fwrite(&size, sizeof(size), 1, _f);
	size = _children.size();
	fwrite(&size, sizeof(size), 1, _f);
	for (iter = _children.begin(); iter != _children.end(); iter++) {
		ObjectID n_children = iter->second.size();
		fwrite(&(iter->first), sizeof(ObjectID), 1, _f);
		fwrite(&n_children, sizeof(n_children), 1, _f);
		std::vector<ObjectID>::const_iterator child = iter->second.begin();
		std::vector<ObjectID>::const_iterator last_child = iter->second.end();
		while (child != last_child) {
			fwrite(&(*child++), sizeof(ObjectID), 1, _f);
		}
	}
	std::vector<std::string>::const_iterator path = _paths.begin();
	std::vector<std::string>::const_iterator last_path = _paths.end();
	for (; path != last_path; path++) {
		fwrite(path->c_str(), path->size()+1, 1, _f);
	}
	long end = ftell(_f);
	fseek(_f, start, SEEK_SET);
	size = static_cast<uint32>(end - start - sizeof(int));
	fwrite(&size, sizeof(size), 1, _f);
	fseek(_f, end, SEEK_SET);
}

DataArchive::DataArchive(std::string const &fn, bool read, bool chain) {
	_chain = chain;
	_finalized = false;
	_manager = 0;
	_buffers.resize(BUFFERS);
	for (int b = 0; b < BUFFERS; b++) {
		_buffers[b].resize(BUFFERSIZE);
	}
	_buffer = 0;
	_fn = fn;
	_f = (FILE*) fopen(fn.c_str(), read ? "rb" : "wb");
	if (_f == NULL) {
		std::string msg;
		msg = msg + "Unable to open DataArchive '" + fn + "'";
		if (read)
			msg += " for reading.";
		else
			msg += " for writing.";
		throw IOError(msg.c_str());
	}
	_is_read = read;
	if (_is_read) {
		readMagic();
		readTable();
	} else {
		_table.reserve(AS);
		writeMagic();
		fwrite(&_table_offset, sizeof(_table_offset), 1, _f);
	}
}

DataArchive::~DataArchive() {
	assert(_manager == 0);
	if (!_is_read && !_finalized) {
		writeTable();
	}
	if (_f != 0) fclose(_f);
}

void DataArchive::addObject(Object& a, std::string const &path) {
	if (!_is_read && !_finalized) {
		int offset = ftell(_f);
		ArchiveWriter writer(_f);
		SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "DataArchive: adding " << path << " [" << hash_string(path) << "]");
		a.serialize(writer);
		int length = writer.getCount();
		SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "DataArchive: stored " << length << " bytes (" << path << ")");
		_addEntry(offset, length, a.getClassHash(), path);
	}
}

const LinkBase DataArchive::getObject(std::string const &path) {
	return getObject(Path(path), path);
}

std::vector<ObjectID> DataArchive::getChildren(ObjectID const &id) const {
	ChildMap::const_iterator idx = _children.find(id);
	if (idx == _children.end()) { return std::vector<ObjectID>(); }
	return idx->second;
}

std::vector<ObjectID> DataArchive::getChildren(std::string const & path) const {
	return getChildren(hash_string(path));
}

bool DataArchive::hasObject(ObjectID const &id) const {
	ChildMap::const_iterator idx = _children.find(id);
	return (idx != _children.end());
}

bool DataArchive::hasObject(std::string const & path) const {
	return hasObject(hash_string(path));
}

std::string DataArchive::getPathString(ObjectID const &id) const {
	PathMap::const_iterator idx = _pathmap.find(id);
	if (idx == _pathmap.end()) return "";
	return idx->second;
}

const DataArchive::TableEntry* DataArchive::_lookupPath(Path const &path, std::string const &path_str) const {
	ObjectID id = (ObjectID) path.getPath();
	return _lookupPath(id, path_str);
}

const DataArchive::TableEntry* DataArchive::_lookupPath(ObjectID const &id, std::string const &path_str) const {
	TableMap::const_iterator i = _table_map.find(id);
	if (i == _table_map.end()) {
		std::string msg = path_str;
		if (msg=="") {
			msg = getPathString(id);
			if (msg == "") {
				msg = "human-readable path unavailable";
			}
		}
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "DataArchive: path not found in '" << _fn << "' (" << msg << ") " + id.str());
		throw IndexError(msg.c_str());
	}
	int idx = (*i).second;
	return &(_table[idx]);
}

Object *DataArchive::_createObject(ObjectID classhash) {
	InterfaceProxy *proxy = InterfaceRegistry::getInterfaceRegistry().getInterface(classhash);
	if (!proxy) {
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "Interface proxy [" << classhash << "] not found.");
		throw MissingInterface("Missing interface for " + classhash.str());
	}
	Object *dup = proxy->createObject();
	dup->_setPath(0);
	return dup;
}


// if the object is found in the static cache, a pointer to the
// cached object is returned.  otherwise, the object is loaded from the
// archive on disk.  if the object is marked as static, it is added to
// the static cache.

const LinkBase DataArchive::getObject(const Path& path, std::string const &path_str) {
	ObjectID id = (ObjectID) path.getPath();
	// look among previously created static objects
	LinkBase const *cached = _getStatic(id);
	if (cached != 0) return *cached;
	TableEntry const *t;
	try {
		t = _lookupPath(path, path_str);
	}
	catch (IndexError) {
		if (_manager == 0) throw;
		return _manager->getObject(path, path_str, this);
	}
	SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "getObject using interface registry @ 0x" << std::hex << int(&(InterfaceRegistry::getInterfaceRegistry())));
	InterfaceProxy *proxy = InterfaceRegistry::getInterfaceRegistry().getInterface(t->classhash);
	std::string from = path_str;
	if (from == "") from = getPathString(path.getPath());
	if (!proxy) {
		std::string msg = "Missing interface for";
		if (from != "") {
			SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "getObject(" << from << "):");
			msg = msg + " '" + from + "'";
		}
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "Interface proxy [" << t->classhash << "] not found while loading " << path << " (" << from << ")");
		msg = msg + " " + t->classhash.str();
		throw MissingInterface(msg);
	}
	SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "Creating object using interface proxy [" << proxy->getClassName() << "]");
	Object *dup = proxy->createObject();
	uint32 offset = t->offset;
	uint32 length = t->length;
	char* buffer = 0;
	Buffer temp_buffer;
	if (_buffer < _buffers.size() && length <= _buffers[_buffer].size()) {
		buffer = &(_buffers[_buffer][0]);
		++_buffer;
	} else {
		SIMDATA_LOG(LOG_ARCHIVE, LOG_INFO, "BUFFERSIZE exceeded, allocating larger buffer");
		temp_buffer.resize(length);
		buffer = &(temp_buffer[0]);
	}
	assert(buffer);
	fseek(_f, offset, SEEK_SET);
	fread(buffer, length, 1, _f);
	ArchiveReader reader(buffer, length, this, _chain);
	SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "loading new object " << dup->getClassName() << " from " << from);
	dup->_setPath(id);
	try {
		dup->serialize(reader);
	} catch (DataUnderflow &e) {
		if (temp_buffer.size() == 0) --_buffer;
		e.clear();	
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "INTERNAL ERROR: Object extraction incomplete for class '" << dup->getClassName() << "': " << from << " (data underflow).");
		throw CorruptArchive("Object extraction incomplete for class '" + std::string(dup->getClassName()) + "'");
	}
	if (_chain) {
		dup->postCreate();
	}
	if (temp_buffer.size() == 0) --_buffer;
	if (!reader.isComplete()) {
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "INTERNAL ERROR: Object extraction incomplete for class '" << dup->getClassName() << "': " << from << " (data overflow).");
		throw CorruptArchive("Object extraction incomplete for class '" + std::string(dup->getClassName()) + "'");
	}
	if (proxy->isStatic()) {
		_addStatic(dup, "", id);
	}
	SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "finished loading " << dup->getClassName() << " from " << from);
	return LinkBase(path, dup);
}



void DataArchive::_addStatic(Object* ptr, std::string const &path, ObjectID id) {
	if (id == 0) id = hash_string(path);
	_static_map[id] = LinkBase(Path(path), ptr);
}

LinkBase const* DataArchive::_getStatic(ObjectID id=0) {
	CacheMap::const_iterator i = _static_map.find(id);
	if (i == _static_map.end()) return 0;
	return &(i->second);
}

void DataArchive::cleanStatic() {
	std::vector<ObjectID> unused;
	unused.reserve(64);
	if (!_static_map.empty()) {
		CacheMap::const_iterator i = _static_map.begin();
		CacheMap::const_iterator j = _static_map.end();
		for (; i != j; i++) {
			if (i->second.unique()) {
				unused.push_back(i->first);
			}
		}
	}
	if (!unused.empty()) {
		std::vector<ObjectID>::iterator i = unused.begin();
		std::vector<ObjectID>::iterator j = unused.end();
		for (; i != j; i++) {
			_static_map.erase(_static_map.find(*i));
		}
	}
}

void DataArchive::finalize() {
	writeTable();
	if (_f != 0) fclose(_f);
	_f = 0;
}

bool DataArchive::isFinalized() {
	return _finalized;
}

bool DataArchive::isWrite() {
	return !_is_read;
}

std::vector<ObjectID> DataArchive::getAllObjects() const {
	std::size_t n_objects = _table.size();
	std::vector<ObjectID> ids(n_objects);
	for (unsigned int i = 0; i < n_objects; i++) {
		ids[i] = _table[i].pathhash;
	}
	return ids;
}

std::vector<std::string> DataArchive::getAllPathStrings() const {
	return _paths;
}

InterfaceProxy *DataArchive::getObjectInterface(ObjectID const &id, std::string const &path) const {
	TableEntry const *t;
	try {
		t = _lookupPath(id, path);
	}
	catch (IndexError) {
		if (_manager == 0) throw;
		return _manager->getObjectInterface(id, path, this);
	}
	return InterfaceRegistry::getInterfaceRegistry().getInterface(t->classhash);
}

InterfaceProxy *DataArchive::getObjectInterface(std::string const &path) const {
	return getObjectInterface(hash_string(path), path);
}

void DataArchive::setManager(DataManager *m) {
	assert(_manager == 0 || m == 0);
	_manager = m;
}

void DataArchive::dump() const {
	std::cout << "OBJECT: (size, offset, id, class, path)\n";
	std::size_t n_objects = _table.size();
	for (unsigned int i = 0; i < n_objects; i++) {
		InterfaceProxy *proxy = InterfaceRegistry::getInterfaceRegistry().getInterface(_table[i].classhash);
		std::string classname = _table[i].classhash.str();
		if (proxy) classname = proxy->getClassName();
		std::cout << std::setw(6) << _table[i].length
		          << std::setw(9) << _table[i].offset << " "
		          << _table[i].pathhash << " "
		          << classname << " "
		          << getPathString(_table[i].pathhash) << "\n";
	}
}

NAMESPACE_SIMDATA_END

