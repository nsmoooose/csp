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
#include <SimData/InterfaceRegistry.h>
#include <SimData/GlibCsp.h>


NAMESPACE_SIMDATA

const int DataArchive::AS = 1024;
const int DataArchive::BUFFERSIZE = 4096;
const int DataArchive::BUFFERS = 10;

DataArchive* g_defaultArchive;


long DataArchive::_getOffset() { return ftell(f); } // for Python use only
FP DataArchive::_filePointer() { 
	FP x;
	x.f = f;
	x.name = _fn;
	x.mode = isWrite() ? "wb" : "rb";
	return x; 
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
	n = fread(&table_offset, 4, 1, f);
	if (n != 1) {
		throw CorruptArchive("Lookup table offset.");
	}
	fseek(f, table_offset, SEEK_SET);
	n = fread(&n_objects, 4, 1, f);
	if (n != 1 || n_objects < 0 || n_objects > 100000) {
		throw CorruptArchive("Number of objects.");
	}
	allocation = n_objects;
	table = (TableEntry*) malloc(sizeof(TableEntry)*n_objects);
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
}

void DataArchive::writeTable() {
	assert(!is_read);
	if (closed) return;
	table_offset = ftell(f);
	fwrite(&n_objects, 4, 1, f);
	fwrite(table, sizeof(TableEntry), n_objects, f);
	fseek(f, 8, SEEK_SET);
	fwrite(&table_offset, 4, 1, f);
	closed = 1;
}	

DataArchive::DataArchive(const char*fn, int read) {
	closed = 0;
	for (int b = 0; b < BUFFERS; b++) {
		object_buffer[b] = (char*) malloc(BUFFERSIZE);
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
		allocation = AS;
		writeMagic();
		fwrite(&table_offset, 4, 1, f);
	}
}

DataArchive::~DataArchive() {
	if (!is_read && !closed) {
		writeTable();
	}
	if (f != 0) fclose(f);
	for (int b = 0; b < BUFFERS; b++) {
		if (object_buffer[b] != 0) free(object_buffer[b]);
	}
	if (g_defaultArchive == this) g_defaultArchive = 0;
}

void DataArchive::setDefault() {
	g_defaultArchive = this;
}


void DataArchive::addObject(Object& a, const char* path) {
	if (!is_read && !closed) {
		int offset = ftell(f);
		Packer p(f);
		a.pack(p);
		int length = p.getCount();
		std::cerr << path << " (" << length << " bytes) [" << hash_string(path) << "]" << std::endl;
		_addEntry(offset, length, a.getClassHash(), path);
	}
}

// caller creates a local object of the correct type and passes a pointer
// to it.  if the object is found in the static cache, a pointer to the
// cached object is returned.  otherwise, the object is loaded from the
// archive on disk.  if the object is marked as static, it is added to
// the static cache.  

/**
Object* DataArchive::getObject(const Object& a, const char* path) {
	hasht key = hash_string(path);
	return getObject(a, key, path);
}
	
Object* DataArchive::getObject(const Object& a, hasht key, const char* path) {
	// look among previously created static objects
	Object *cached = _getStatic(key);
	if (cached != 0) return cached;
	hasht_map::const_iterator i = table_map.find(key);
	if (i == table_map.end()) {
		std::string msg;
		msg = msg + "path '" + path + "' not found in archive '" + _fn + "'";
		throw IndexError(msg.c_str());
	}
	int idx = (*i).second;
	TableEntry &t = table[idx];
	printf("object registry @ %p\n", &g_ObjectRegistry);
	printf("object hashes: %llu %llu\n", t.classhash, a.getClassHash());
	printf("ok so far\n");
	if (t.classhash != a.getClassHash()) {
		std::string msg;
		printf("mismatch()\n");
		msg = msg + "object class '" + a.getClassName() + "' does not match saved object '" + path + "' in archive '" + _fn + "'";
		throw ObjectMismatch(msg.c_str());
	}
	printf("match\n");
	printf("registry length = %d\n", g_ObjectRegistry.size());
	assert(g_ObjectRegistry.isRegistered(t.classhash));
	int offset = t.offset;
	int length = t.length;
	char* buffer = object_buffer;
	if (length > BUFFERSIZE) {
		printf("BUFFERSIZE exceeded, allocating larger buffer\n");
		buffer = (char*) malloc(length);
	}
	//printf("%d", offset);
	fseek(f, offset, SEEK_SET);
	fread(buffer, length, 1, f);
	UnPacker p(buffer, length, this);
	Object* dup = a._new();
	printf("%s %s\n",dup->getClassName(),a.getClassName());
	dup->unpack(p);
	if (buffer != object_buffer) free(buffer);
	if (dup->isStatic()) {
		_addStatic(dup, path, key);
		dup->ref(); // we own a copy
	}
	return dup;
}

Object* DataArchive::getObject(hasht key) {
	// look among previously created static objects
	Object *cached = _getStatic(key);
	if (cached != 0) return cached;
	hasht_map::const_iterator i = table_map.find(key);
	if (i == table_map.end()) {
		std::string msg;
		msg = msg + "path not found in archive '" + _fn + "' (human-readable path unavailable)";
		throw IndexError(msg.c_str());
	}
	int idx = (*i).second;
	TableEntry &t = table[idx];
	printf("getObject using object registry @ %p\n", &g_ObjectRegistry);
	Object *dup = g_ObjectRegistry.createObject(t.classhash);
	int offset = t.offset;
	int length = t.length;
	char* buffer = object_buffer;
	if (length > BUFFERSIZE) {
		printf("BUFFERSIZE exceeded, allocating larger buffer\n");
		buffer = (char*) malloc(length);
	}
	fseek(f, offset, SEEK_SET);
	fread(buffer, length, 1, f);
	UnPacker p(buffer, length, this);
	printf("got object %s\n",dup->getClassName());
	dup->unpack(p);
	if (buffer != object_buffer) free(buffer);
	if (dup->isStatic()) {
		_addStatic(dup, 0, key);
		dup->ref(); // we own a copy
	}
	return dup;
}
*/

const PathPointerBase DataArchive::getObject(const char* path) {
	return getObject(Path(path), path);
}

const TableEntry& DataArchive::_lookupPath(const Path& path, const char* path_str) {
	hasht key = (hasht) path.getPath();
	hasht_map::const_iterator i = table_map.find(key);
	if (i == table_map.end()) {
		std::string msg;
		if (path_str==0 || *path_str==0) {
			msg = "human-readable path unavailable";
		} else {
			msg = path_str;
		}
		msg = "path not found in archive '" + _fn + "' (" + msg + ")" + key.str() + "\n";
		std::cerr << msg;
		throw IndexError(msg.c_str());
	}
	int idx = (*i).second;
	return table[idx];
}

const PathPointerBase DataArchive::getObject(const Path& path, const char* path_str) {
	hasht key = (hasht) path.getPath();
	// look among previously created static objects
	Object *cached = _getStatic(key);
	if (cached != 0) return PathPointerBase(path, cached);
	const TableEntry &t = _lookupPath(path, path_str);
	printf("getObject using interface registry @ %p\n", &(InterfaceRegistry::getInterfaceRegistry()));
	InterfaceProxy *proxy = InterfaceRegistry::getInterfaceRegistry().getInterface(t.classhash);
	if (!proxy) {
		if (path_str) {
			printf("getObject(\"%s\"):\n", path_str);
		}
		std::cout << "Interface proxy [" << t.classhash << "] not found." << std::endl;
		assert(0);
	}
	Object *dup = proxy->createObject();
	int offset = t.offset;
	int length = t.length;
	char* buffer = 0;
	bool free_buffer = false;
	if (n_buffer < BUFFERS && length <= BUFFERSIZE) {
		buffer = object_buffer[n_buffer];
		n_buffer++;
	} else {
		std::cout << "BUFFERSIZE exceeded, allocating larger buffer" << std::endl;
		buffer = (char*) malloc(length);
		free_buffer = true;
	}
	assert(buffer);
	fseek(f, offset, SEEK_SET);
	fread(buffer, length, 1, f);
	UnPacker p(buffer, length, this);
	std::cout << "got object " << dup->getClassName() << std::endl;
	dup->_setPath(key);
	dup->unpack(p);
	if (free_buffer) {
		free(buffer);
	} else {
		n_buffer--;
	}
	if (!p.isComplete()) {
		std::cout << "INTERNAL ERROR: Object extraction incomplete for class '" << dup->getClassName() << "'." << std::endl;
		assert(0);
	}
	if (dup->isStatic()) {
		_addStatic(dup, 0, key);
		dup->ref(); // we own a copy
	}
	return PathPointerBase(path, dup);
}



void DataArchive::_addStatic(Object* ptr, const char* path, hasht key) {
	if (key == 0) key = hash_string(path);
	static_map[key] = ptr;
}

Object* DataArchive::_getStatic(hasht key=0) {
	cache_map::const_iterator i = static_map.find(key);
	if (i == static_map.end()) return 0;
	return (*i).second;
}

void DataArchive::close() {
	writeTable();
	if (f != 0) fclose(f);
	f = 0;
}

bool DataArchive::isClosed() {
	return (closed != 0);
}

bool DataArchive::isWrite() {
	return !is_read;
}

NAMESPACE_END // namespace simdata

