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
#include <SimData/InterfaceRegistry.h>
#include <SimData/GlibCsp.h>
#include <SimData/Object.h>
#include <SimData/Log.h>


NAMESPACE_SIMDATA

const int DataArchive::AS = 1024;
const int DataArchive::BUFFERSIZE = 4096;
const int DataArchive::BUFFERS = 10;


DataArchive* DataArchive::defaultArchive = 0;


/*
long DataArchive::_getOffset() { return ftell(f); } // for Python use only
FP DataArchive::_filePointer() { 
	FP x;
	x.f = f;
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


void DataArchive::_addEntry(int offset, int length, hasht hash, const char* path) {
	if (n_objects == allocation) {
		allocation += AS;
		table = (TableEntry*) realloc(table, sizeof(TableEntry)*allocation);
	}
	TableEntry &t = table[n_objects];
	t.offset = offset;
	t.length = length;
	t.classhash = hash;
	t.pathhash = hash_string(path);
	table_map[t.pathhash] = n_objects;
	n_objects++;
	hasht parent = hash_string(base_path(path).c_str());
	_children[parent].push_back(t.pathhash);
	_paths.push_back(path);
	_pathmap[t.pathhash] = path;
}

void DataArchive::writeMagic() {
	fprintf(f, "RAWDAT-%c", (G_BYTE_ORDER == G_LITTLE_ENDIAN) ? 'L' : 'B');
}

void DataArchive::readMagic() {
	char magic[9];
	fread(magic, 8, 1, f);
	magic[8] = 0;
	if (strncmp(magic, "RAWDAT-", 7)) {
		std::string msg;
		msg = msg + "incorrect magic '" + magic + "' in archive '" + _fn + "'";
		throw BadMagic(msg.c_str());
	}
	bool data_little = (magic[7] == 'L');
	bool machine_little = (G_BYTE_ORDER == G_LITTLE_ENDIAN);
	if (data_little != machine_little) {
		char msg[128];
		sprintf(msg, "le(machine, data) = (%d, %d)", machine_little, data_little);
		throw BadByteOrder(msg);
	}
}

void DataArchive::readTable() {
	size_t n;
	n = fread(&table_offset, sizeof(table_offset), 1, f);
	if (n != 1) {
		throw CorruptArchive("Lookup table offset.");
	}
	fseek(f, table_offset, SEEK_SET);
	n = fread(&n_objects, sizeof(n_objects), 1, f);
	if (n != 1 || n_objects < 0 || n_objects > 100000) {
		throw CorruptArchive("Number of objects.");
	}
	allocation = n_objects;
	table = (TableEntry*) malloc(sizeof(TableEntry)*n_objects);
	assert(table != 0);
	n = fread(table, sizeof(TableEntry), n_objects, f);
	if ((int) n != n_objects) {
		throw CorruptArchive("Lookup table truncated.");
	}
	int i;
	for (i = 0; i < n_objects; i++) {
		//printf("%6d %d\n", i, table[i].pathhash);
		table_map[table[i].pathhash] = i;
		//printf("%02d: ", i);
		//std::cout << table[i].pathhash << " " << table[i].classhash;
		//printf(" %08x %d\n", table[i].offset, table[i].length);
	}
	_readPaths();
}

void DataArchive::_readPaths() {
	int path_toc_size;
	char *toc_buffer = 0;
	size_t n;
	try {
		n = fread(&path_toc_size, sizeof(int), 1, f);
		if (n != 1 || path_toc_size < int(sizeof(int)*2) || path_toc_size > 1000000) {
			throw CorruptArchive("Path table of contents.");
		}
		toc_buffer = (char*) malloc(path_toc_size);
		assert(toc_buffer);
		n = fread(toc_buffer, 1, path_toc_size, f);
		if ((int) n != path_toc_size || toc_buffer[path_toc_size-1] != 0) {
			throw CorruptArchive("Path table of contents truncated.");
		}
		int *iptr;
		hasht *hptr;
		iptr = (int*) toc_buffer;
		int n_paths = *iptr++;
		int n_directories = *iptr++;
		_paths.reserve(n_paths);
		hptr = (hasht *) iptr;
		while (n_directories-- > 0) {
			hasht node = *hptr++;
			int n = hptr->a;
			hptr++;
			if (n < 0 || n > n_paths) {
				throw CorruptArchive("Path table of contents.");
			}
			std::vector<hasht> &childlist = _children[node];
			childlist.reserve(n);
			while (n-- > 0) {
				childlist.push_back(*hptr++);
			}
		}
		char *cptr = (char*) hptr;
		char *toc_end = toc_buffer + path_toc_size;
		while (n_paths-- > 0) {
			std::string path = cptr;
			cptr += path.size() + 1;
			_paths.push_back(path);
			_pathmap[hash_string(path.c_str())] = path;
			if (cptr >= toc_end && n_paths != 0) {
				throw CorruptArchive("Path table of contents truncated.");
			}
		}
		if (cptr != toc_end) {
			throw CorruptArchive("Path table of contents truncated.");
		}
	}
	catch (...) {
		if (toc_buffer != 0) free(toc_buffer);
		throw;
	}
	free(toc_buffer);
}

void DataArchive::writeTable() {
	assert(!is_read);
	if (finalized) return;
	table_offset = ftell(f);
	fwrite(&n_objects, 4, 1, f);
	fwrite(table, sizeof(TableEntry), n_objects, f);
	_writePaths();
	fseek(f, 8, SEEK_SET);
	fwrite(&table_offset, 4, 1, f);
	finalized = 1;
}	

void DataArchive::_writePaths() const {
	child_map::const_iterator iter;
	long start = ftell(f);
	int size = 0;
	fwrite(&size, sizeof(size), 1, f);
	size = _paths.size();
	fwrite(&size, sizeof(size), 1, f);
	size = _children.size();
	fwrite(&size, sizeof(size), 1, f);
	for (iter = _children.begin(); iter != _children.end(); iter++) {
		hasht size = iter->second.size();
		fwrite(&(iter->first), sizeof(hasht), 1, f);
		fwrite(&size, sizeof(size), 1, f);
		std::vector<hasht>::const_iterator child = iter->second.begin();
		std::vector<hasht>::const_iterator last_child = iter->second.end();
		while (child != last_child) {
			fwrite(&(*child++), sizeof(hasht), 1, f);
		}
	}
	std::vector<std::string>::const_iterator path = _paths.begin();
	std::vector<std::string>::const_iterator last_path = _paths.end();
	for (; path != last_path; path++) {
		fwrite(path->c_str(), path->size()+1, 1, f);
	}
	long end = ftell(f);
	fseek(f, start, SEEK_SET);
	size = end - start - sizeof(int);
	fwrite(&size, sizeof(size), 1, f);
	fseek(f, end, SEEK_SET);
}

DataArchive::DataArchive(const char*fn, int read, bool chain_) {
	chain = chain_;
	finalized = 0;
	manager = 0;
	for (int b = 0; b < BUFFERS; b++) {
		object_buffer[b] = (char*) malloc(BUFFERSIZE);
		assert(object_buffer[b] != 0);
	}
	n_buffer = 0;
	_fn = fn;
	f = (FILE*) fopen(fn, read ? "rb" : "wb");
	if (f == NULL) {
		std::string msg;
		msg = msg + "Unable to open DataArchive '" + fn + "'";
		if (read)
			msg += " for reading.";
		else 
			msg += " for writing.";
		throw IOError(msg.c_str());
	}
	is_read = read;
	if (is_read) {
		readMagic();
		readTable();
	} else {
		n_objects = 0;
		table = (TableEntry*) malloc(sizeof(TableEntry)*AS);
		assert(table != 0);
		allocation = AS;
		writeMagic();
		fwrite(&table_offset, 4, 1, f);
	}
}

DataArchive::~DataArchive() {
	assert(manager == 0);
	if (!is_read && !finalized) {
		writeTable();
	}
	if (f != 0) fclose(f);
	for (int b = 0; b < BUFFERS; b++) {
		if (object_buffer[b] != 0) free(object_buffer[b]);
	}
	if (table != 0) {
		free(table);
		table = 0;
	}
	if (defaultArchive == this) defaultArchive = 0;
}

void DataArchive::setDefault() {
	defaultArchive = this;
}


DataArchive *DataArchive::getDefault() {
	return defaultArchive;
}


void DataArchive::addObject(Object& a, const char* path) {
	if (!is_read && !finalized) {
		int offset = ftell(f);
		Packer p(f);
		a.pack(p);
		int length = p.getCount();
		SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "DataArchive: adding " << path << " (" << length << " bytes) [" << hash_string(path) << "]");
		_addEntry(offset, length, a.getClassHash(), path);
	}
}


const LinkBase DataArchive::getObject(const char* path) {
	return getObject(Path(path), path);
}

std::vector<ObjectID> DataArchive::getChildren(ObjectID const &id) const {
	child_map::const_iterator idx = _children.find(id);
	if (idx == _children.end()) { return std::vector<ObjectID>(); }
	return idx->second;
}

std::vector<ObjectID> DataArchive::getChildren(std::string const & path) const {
	return getChildren(hash_string(path.c_str()));
}

bool DataArchive::hasObject(ObjectID const &id) const {
	child_map::const_iterator idx = _children.find(id);
	return (idx != _children.end());
}

bool DataArchive::hasObject(std::string const & path) const {
	return hasObject(hash_string(path.c_str()));
}

std::string DataArchive::getPathString(ObjectID const &id) const {
	path_map::const_iterator idx = _pathmap.find(id);
	if (idx == _pathmap.end()) return "";
	return idx->second;
}

const DataArchive::TableEntry* DataArchive::_lookupPath(Path const& path, const char* path_str) const {
	hasht key = (hasht) path.getPath();
	hasht_map::const_iterator i = table_map.find(key);
	if (i == table_map.end()) {
		std::string msg;
		if (path_str==0 || *path_str==0) {
			msg = "human-readable path unavailable";
		} else {
			msg = path_str;
		}
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "DataArchive: path not found in '" << _fn << "' (" << msg << ") " + key.str());
		throw IndexError(msg.c_str());
	}
	int idx = (*i).second;
	return &(table[idx]);
}

Object *DataArchive::_createObject(hasht classhash) {
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

const LinkBase DataArchive::getObject(const Path& path, const char* path_str) {
	hasht key = (hasht) path.getPath();
	// look among previously created static objects
	LinkBase const *cached = _getStatic(key);
	if (cached != 0) return *cached;
	TableEntry const *t;
	try {
		t = _lookupPath(path, path_str);
	}
	catch (IndexError) {
		if (manager) {
			return manager->getObject(path, path_str, this);
		}
		throw;
	}
	SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "getObject using interface registry @ 0x" << std::hex << int(&(InterfaceRegistry::getInterfaceRegistry())));
	InterfaceProxy *proxy = InterfaceRegistry::getInterfaceRegistry().getInterface(t->classhash);
	if (!proxy) {
		std::string msg = "Missing interface for";
		if (path_str) {
			SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "getObject(" << path_str << "):");
			msg = msg + " '" + path_str + "'";
		} else {
			path_map::iterator i = _pathmap.find(key);
			if (i != _pathmap.end()) {
				msg = msg + " '" + i->second + "'";
			}
		}
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "Interface proxy [" << t->classhash << "] not found.");
		msg = msg + " " + t->classhash.str();
		throw MissingInterface(msg);
	}
	Object *dup = proxy->createObject();
	int offset = t->offset;
	int length = t->length;
	char* buffer = 0;
	bool free_buffer = false;
	if (n_buffer < BUFFERS && length <= BUFFERSIZE) {
		buffer = object_buffer[n_buffer];
		n_buffer++;
	} else {
		SIMDATA_LOG(LOG_ARCHIVE, LOG_INFO, "BUFFERSIZE exceeded, allocating larger buffer");
		buffer = (char*) malloc(length);
		assert(buffer != 0);
		free_buffer = true;
	}
	assert(buffer);
	fseek(f, offset, SEEK_SET);
	fread(buffer, length, 1, f);
	UnPacker p(buffer, length, this, chain);
	SIMDATA_LOG(LOG_ARCHIVE, LOG_DEBUG, "got object " << dup->getClassName());
	dup->_setPath(key);
	try {
		dup->unpack(p);
	} catch (DataUnderflow &e) {
		e.clear();	
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "INTERNAL ERROR: Object extraction incomplete for class '" << dup->getClassName() << "'.");
		throw CorruptArchive("Object extraction incomplete for class '" + std::string(dup->getClassName()) + "'");
	}
	if (chain) {
		dup->postCreate();
	}
	if (free_buffer) {
		free(buffer);
	} else {
		n_buffer--;
	}
	if (!p.isComplete()) {
		SIMDATA_LOG(LOG_ARCHIVE, LOG_ERROR, "INTERNAL ERROR: Object extraction incomplete for class '" << dup->getClassName() << "'.");
		throw CorruptArchive("Object extraction incomplete for class '" + std::string(dup->getClassName()) + "'");
	}
	if (dup->isStatic()) {
		_addStatic(dup, 0, key);
		// XXX dup->ref(); // we own a copy
	}
	return LinkBase(path, dup);
}



void DataArchive::_addStatic(Object* ptr, const char* path, hasht key) {
	if (key == 0) key = hash_string(path);
	static_map[key] = LinkBase(Path(path), ptr);
}

LinkBase const* DataArchive::_getStatic(hasht key=0) {
	cache_map::const_iterator i = static_map.find(key);
	if (i == static_map.end()) return 0;
	return &(i->second);
}

void DataArchive::cleanStatic() {
	std::vector<ObjectID> unused;
	unused.reserve(64);
	if (!static_map.empty()) {
		cache_map::const_iterator i = static_map.begin();
		cache_map::const_iterator j = static_map.end();
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
			static_map.erase(static_map.find(*i));
		}
	}
}

void DataArchive::finalize() {
	writeTable();
	if (f != 0) fclose(f);
	f = 0;
}

bool DataArchive::isFinalized() {
	return (finalized != 0);
}

bool DataArchive::isWrite() {
	return !is_read;
}

std::vector<ObjectID> DataArchive::getAllObjects() const {
	std::vector<ObjectID> ids(n_objects);
	int i;
	for (i = 0; i < n_objects; i++) {
		ids[i] = table[i].pathhash;
	}
	return ids;
}

std::vector<std::string> DataArchive::getAllPathStrings() const {
	return _paths;
}

NAMESPACE_END // namespace simdata

