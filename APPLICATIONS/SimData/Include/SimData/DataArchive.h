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
 * @file DataArchive.h
 * @author Mark Rose <mrose@stm.lbl.gov>* 
 */

#ifndef __DATAARCHIVE_H__
#define __DATAARCHIVE_H__

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <SimData/Object.h>
#include <SimData/Path.h>
#include <SimData/Exception.h>
#include <SimData/HashUtility.h>
#include <SimData/ns-simdata.h>


NAMESPACE_SIMDATA


// exceptions:
class BadMagic: public Exception { 
	public: BadMagic(const char* msg): 
		Exception("BadMagic", msg) {}
};
class BadByteOrder: public Exception {
	public: BadByteOrder(const char* msg): 
		Exception("BadByteOrder", msg) {}
};
class CorruptArchive: public Exception {
	public: CorruptArchive(const char* msg): 
		Exception("CorruptArchive", msg) {}
};
class IndexError: public Exception {
	public: IndexError(const char* msg): 
		Exception("IndexError", msg) {}
};
class ObjectMismatch: public Exception {
	public: ObjectMismatch(const char* msg): 
		Exception("ObjectMismatch", msg) {}
};

EXCEPTION(IOError)

	// was nested in DataArchive
	// separated for SWIG
	class TableEntry {
		public:
		hasht pathhash;
		hasht classhash;
		gint32 offset;
		gint32 length;
	};

struct FP { FILE* f; std::string name; std::string mode; };

/**
 * C++ interface to data archive files.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class DataArchive {

	friend class PathPointerBase;

	//template <class T> friend class PathPointer;
	//template <class T> friend PathPointer<T> getObject(DataArchive&, const char*);
	
	static const int AS;
	static const int BUFFERSIZE;
	static const int BUFFERS; 
	TableEntry* table;
	FILE *f;
	int is_read;
	long table_offset;
	int n_objects, allocation;
	int bytes;
	int closed;
	char* object_buffer[10]; // vector<string> here?
	int n_buffer;
	hasht_map table_map;
	cache_map static_map;
	std::string _fn;

	void writeMagic();
	void readMagic();
	void readTable();
	void writeTable();
public:
	DataArchive(const char*fn, int read);
	~DataArchive();
	void _addEntry(int offset, int length, hasht hash, const char* path);
	void addObject(Object &a, const char* path);
	void close();
	bool isClosed();
	bool isWrite();
	void setDefault();
	const PathPointerBase getObject(const char*);
	const PathPointerBase getObject(const Path&, const char* =0);

	// protected methods made public for Python access, don't use!
	long _getOffset(); 
	FP _filePointer();
	const TableEntry& _lookupPath(const Path&, const char* =0);

protected:
	/**
	Object* getObject(const Object &a, const char* path);
	Object* getObject(const Object &a, hasht path, const char* path="");
	Object* getObject(hasht path);
	*/
	void _addStatic(Object* ptr, const char* path, hasht key=0);
	Object* _getStatic(hasht key);
};


extern DataArchive* g_defaultArchive;


NAMESPACE_END // namespace simdata

#endif //__DATAARCHIVE_H__

