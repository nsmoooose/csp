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

#ifndef __SIMDATA_DATAARCHIVE_H__
#define __SIMDATA_DATAARCHIVE_H__

# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning(disable : 4786)
# endif

#include <string>
#include <cstdio>
#include <cstdlib>

#include <SimData/Link.h>
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


class DataManager;
class Object;

//For SWIG (not currently used):
//struct FP { FILE* f; std::string name; std::string mode; };


/**
 * C++ interface to data archive files.
 * 
 * The DataArchive class provides read and write access to data archive
 * files.  Write mode is primarily intended for use by the data complier.
 * In read mode, objects can be created dynamically from their path
 * strings, with all external data loaded and linked objects created
 * transparently.  Static objects are cached internally.  All created
 * objects are returned via smart-pointers.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT DataArchive {

	friend class LinkBase;
	friend class DataManager;

private:

	struct TableEntry {
		hasht pathhash;
		hasht classhash;
		gint32 offset;
		gint32 length;
	};

	/**
	 * This provides an optional means to share a data archive
	 * globally in an application.  In principle this is not a
	 * very safe thing to do, but is provided for the convenience
	 * of hanging yourself.  You've been warned. 
	 *
	 * Use getDefault() and setDefault() to access this variable 
	 * (which may be null).
	 */
	static DataArchive* defaultArchive;

	static const int AS;
	static const int BUFFERSIZE;
	static const int BUFFERS; 

	
	TableEntry* table;
	int n_objects, allocation;

	char* object_buffer[10]; // vector<string> here?
	int n_buffer;

	/////////// 
	
	/* TODO: explore using C++ constructs for data archive storage
	 
	enum { 
		DEFAULT_BUFFERSIZE = 4096,
		DEFAULT_BUFFERS = 10,
	};

	struct Buffer {
		char *cbuf;
		Buffer(int size=DEFAULT_BUFFERSIZE) { cbuf = new char[size]; }
		~Buffer() { delete[] cbuf; }
	};

	std::vector<TableEntry> _table;
	std::vector<Buffer> _buffers;
	*/

	/////////// 
	
	typedef HASH_MAPS<hasht, std::vector<hasht>, hasht_hash, hasht_eq>::Type child_map;
	child_map _children;
	typedef HASH_MAPS<hasht, std::string, hasht_hash, hasht_eq>::Type path_map;
	path_map _pathmap;
	std::vector<std::string> _paths;
	
	FILE *f;
	int is_read;
	long table_offset;
	int bytes;
	int finalized;
	hasht_map table_map;
	std::string _fn;
	bool chain;
	DataManager *manager;

	typedef HASH_MAPS<hasht, LinkBase, hasht_hash, hasht_eq>::Type cache_map;
	cache_map static_map;

	/**
	 * Write a "magic" string to the start of the file to
	 * identify it as a data archive.
	 */
	void writeMagic();

	/**
	 * Read a string from the start of the file and make
	 * sure the "magic" matches.
	 */
	void readMagic();

	/**
	 * Read the object table from the end of the file.
	 *
	 * This table contains a list of all objects stored in
	 * the archive.  The list include the object id hash
	 * which is used for fast access, and the class id hash
	 * which allows a new instance to be created from the
	 * correct entry in the interface registry.
	 */
	void readTable();

	/**
	 * Write the completed object table to the end of the file.
	 */
	void writeTable();

	/**
	 * Read the path table of contents from the archive.
	 *
	 * The table of contents associates each path node with
	 * its parent directory to enable searching for all
	 * objects below a given path.  It also includes all
	 * path strings so that path hash values can be translated
	 * back to human readible form for debugging.
	 */
	void _readPaths();

	/**
	 * Write the path table of contents to the archive.
	 */
	void _writePaths() const;

public:


	/**
	 * Open a new data archive.
	 *
	 * @param fn the full path to the archive file
	 * @param read nonzero for read mode.
	 * @param chain this is for internal use only.
	 */
	DataArchive(const char*fn, int read, bool chain=true);

	/**
	 * Close the data archive and cleanup.
	 */
	~DataArchive();

	/**
	 * Add an object to the archive.
	 *
	 * @param object the object to add.
	 * @param path the path string of the object
	 */
	void addObject(Object &object, const char* path);

	/**
	 * Write the entry table and close the data archive.
	 *
	 * This should only be called after writing objects
	 * to a data archive.  Do not call this in read mode.
	 */
	void finalize();

	/**
	 * Test if the data archive has been finalized.
	 *
	 * @returns true if the archive is finalized.
	 */
	bool isFinalized();

	/**
	 * Test if the archive is in read or write mode.
	 *
	 * @returns true if in write mode.
	 */
	bool isWrite();

	/**
	 * Make this archive the "default" archive.  
	 *
	 * It will remain so until another archive is assigned or it
	 * is deleted (in which case the dafault reverts to NULL).  
	 * Use of this default utility is not recommended.
	 */
	void setDefault();

	/**
	 * Get the current default archive.
	 *
	 * @returns the current default or NULL if there is none.
	 */
	static DataArchive* getDefault();

	/**
	 * Create a new object from a path identifier string.
	 *
	 * @path_str the path identifier string.
	 * @returns a smart-pointer to the new object.
	 */
	const LinkBase getObject(const char* path_str);

	/**
	 * Create a new object from a Path instance.
	 *
	 * @path the Path instance.
	 * @path_str the path identifier string (if available).  This is
	 *           only used for error logging.
	 */
	const LinkBase getObject(const Path& path, const char* path_str=0);

	/**
	 * Get the full path of the archive file.
	 *
	 * @returns the full path of the archive file.
	 */
	std::string getFileName() const { return _fn; }

	/**
	 * Get all children of a given path.
	 *
	 * For path "A:X.Y", returns all object id's "A:X.Y.*".  The id's
	 * can be converted to human-readable form by getPathString().
	 *
	 * @param path the path to search for children
	 * @returns a list of object id's immediately below the given path.
	 */
	std::vector<ObjectID> getChildren(ObjectID const &id) const;
	
	/**
	 * Get all children of a given path.
	 *
	 * For path "A:X.Y", returns all object id's "A:X.Y.*".  The id's
	 * can be converted to human-readable form by getPathString().
	 *
	 * @param path the path to search for children
	 * @returns a list of object id's immediately below the given path.
	 */
	std::vector<ObjectID> getChildren(std::string const & path) const;

	/**
	 * Check for the existance of an object in the archive.
	 *
	 * @returns true if the object id exists.
	 */
	bool hasObject(ObjectID const &id) const;

	/**
	 * Check for the existance of an object in the archive.
	 *
	 * @returns true if the object id exists.
	 */
	bool hasObject(std::string const & path) const;

	/** 
	 * Get the path string corresponding to a give object id.
	 *
	 * This provides a human-readable path string that is useful
	 * for error and debugging messages.
	 *
	 * @returns the path string if found, otherwise an empty string.
	 */
	std::string getPathString(ObjectID const &id) const; 

	/**
	 * Get a list of all objects in the archive.
	 */
	std::vector<ObjectID> getAllObjects() const;

	/**
	 * Get a list of all object paths in the archive.
	 */
	std::vector<std::string> getAllPathStrings() const;

	// protected methods made public for Python access, don't use!
	/*
	long _getOffset(); 
	FP _filePointer();
	*/

protected:
	/**
	 * Add a new entry to the object table.
	 *
	 * @param offset the byte offset of the start of the object data
	 * @param length the number of bytes in the serialized object
	 * @param hash the class hash identifier
	 * @param path the path string of the object
	 */
	void _addEntry(int offset, int length, hasht hash, const char* path);
	
	/**
	Object* getObject(const Object &a, const char* path);
	Object* getObject(const Object &a, hasht path, const char* path="");
	Object* getObject(hasht path);
	*/

	/**
	 * Add an object to the static object cache.
	 *
	 * @param ptr the object to cache
	 * @param path_str the path identifier string
	 * @param key the object identifier hash (path hash)
	 */
	void _addStatic(Object* ptr, const char* path_str, hasht key=0);

	/**
	 * Get an object from the static object cache.
	 *
	 * @returns the object if found, otherwise NULL.
	 */
	LinkBase const * _getStatic(hasht key);

	/**
	 * Create an a new instance using a class identifier hash.
	 *
	 * @param classhash the identifier hash of the class to create.
	 * @returs the newly created object.
	 */
	Object* _createObject(hasht classhash);

	/**
	 * Find the table entry corresponding to a give object path.
	 *
	 * @param path the path to lookup
	 * @param path_str the path string, if available (only used for
	 *                 error logging).
	 */
	const TableEntry* _lookupPath(Path const& path, const char* path_str=0) const;

private:
	void setManager(DataManager *m) { manager = m; }
	child_map const &getChildMap() const { return _children; }
};




NAMESPACE_END // namespace simdata

#endif //__SIMDATA_DATAARCHIVE_H__

