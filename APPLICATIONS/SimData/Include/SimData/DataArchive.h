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

# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning(disable : 4786)
# endif

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
SIMDATA_EXCEPTION(BadMagic)
SIMDATA_EXCEPTION(BadByteOrder)
SIMDATA_EXCEPTION(CorruptArchive)
SIMDATA_EXCEPTION(IndexError)
SIMDATA_EXCEPTION(ObjectMismatch)
SIMDATA_EXCEPTION(IOError)
SIMDATA_EXCEPTION(MissingInterface);

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
class SIMDATA_EXPORT DataArchive {

	friend class PointerBase;

	//template <class T> friend class Pointer;
	//template <class T> friend Pointer<T> getObject(DataArchive&, const char*);
	
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
	bool chain;

	void writeMagic();
	void readMagic();
	void readTable();
	void writeTable();
public:
	DataArchive(const char*fn, int read, bool chain=true);
	~DataArchive();
	void _addEntry(int offset, int length, hasht hash, const char* path);
	void addObject(Object &a, const char* path);
	void close();
	bool isClosed();
	bool isWrite();
	void setDefault();
	const PointerBase getObject(const char*);
	const PointerBase getObject(const Path&, const char* =0);

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
	Object* _createObject(hasht classhash);
};


extern DataArchive* g_defaultArchive;


NAMESPACE_END // namespace simdata

#endif //__DATAARCHIVE_H__

