/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002, 2003 Mark Rose <tm2@stm.lbl.gov>
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
 * @file DataArchive.h
 * @brief Classes for data archive manipulation.
 * @author Mark Rose <mrose@stm.lbl.gov>
 */

#ifndef __SIMDATA_DATAARCHIVE_H__
#define __SIMDATA_DATAARCHIVE_H__

#include <SimData/Export.h>
#include <SimData/Uniform.h>
#include <SimData/Link.h>
#include <SimData/Exception.h>
#include <SimData/HashUtility.h>
#include <SimData/Namespace.h>

#include <string>
#include <cstdio>
#include <cstdlib>


NAMESPACE_SIMDATA


/// Exception: Input file does not appear to be a valid data archive.
SIMDATA_EXCEPTION(BadMagic)

/// Exception: Input file byte-order does not match current architecture.
SIMDATA_EXCEPTION(BadByteOrder)

/// Exception: Input data archive appears to be corrupt.
SIMDATA_EXCEPTION(CorruptArchive)

/// Exception: Object path not found in the data archive.
SIMDATA_EXCEPTION(IndexError)

/// Exception: Object class does not match class stored in the archive.
SIMDATA_EXCEPTION(ObjectMismatch)

/// Exception: Error reading or writing a data archive.
SIMDATA_EXCEPTION(IOError)

/// Exception: The InterfaceRegistry does not have an interface for the requested object.
SIMDATA_EXCEPTION(MissingInterface);


class DataManager;
class Object;
class InterfaceProxy;


//For SWIG (not currently used):
//struct FP { FILE* f; std::string name; std::string mode; };


/** Interface for reading and writing data archive files.
 * 
 *  The DataArchive class provides read and write access to data archive
 *  files.  Write mode is primarily intended for use by the data complier.
 *  In read mode, objects can be created dynamically from their path
 *  strings, with all external data loaded and linked objects created
 *  transparently.  Static objects are cached internally.  All created
 *  objects are returned via smart-pointers.
 *
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT DataArchive {

	friend class LinkBase;
	friend class DataManager;

private:

	typedef std::vector<char> Buffer;

	/** An entry in the data archive lookup table.
	 *
	 *  The lookup table is an index of all objects in the
	 *  archive.  It is loaded as a hash map, providing O(1)
	 *  access to objects in the archive.
	 */
	struct TableEntry {
		/// the object path identifier hash
		ObjectID pathhash;
		/// the class identifier hash
		ObjectID classhash;
		/// the file offset of the serialized object
		uint32 offset;
		/// the size of the serialized object data (in bytes) 
		uint32 length;
	};

	/** Global default archive.
	 *
	 *  This provides an optional means to share a data archive
	 *  globally in an application.  In principle this is not a
	 *  very safe thing to do, but is provided for the convenience
	 *  of hanging yourself.  You've been warned. 
	 *
	 *  Use getDefault() and setDefault() to access this variable 
	 *  (which may be null).
	 *
	 *  @deprecated No longer necessary; do not use!
	 */
	static DataArchive* defaultArchive;

	enum {
		/// number of entries to allocate when growing the entry table
		AS = 1024,
		/// size (in bytes) of the default input buffers
		BUFFERSIZE = 4096,
		/// number of default input buffers
		BUFFERS = 10
	};

	/** A table that indexes all objects in the archive.
	 */
	std::vector<TableEntry> _table;

	/** Object read buffers.
	 *
	 *  An additional buffer is required for every subobject deserialized
	 *  by Link<>.  If more than BUFFER objects must be deserialized 
	 *  concurrently, or any object exceeds BUFFERSIZE, additional storage 
	 *  will be allocated dynamically.  The use of these fixed buffers
	 *  reduces the need for repeated buffer allocation and deallocation
	 *  when deserializing many small to medium sized objects.
	 */
	std::vector<Buffer> _buffers;
	/// The number of buffers currently in use.
	unsigned int _buffer;

	
	typedef HASH_MAPS<ObjectID, std::vector<ObjectID>, ObjectID_hash, ObjectID_eq>::Type ChildMap;
	/// A map of all parent-child relationships in the archive.
	ChildMap _children;

	typedef HASH_MAPS<ObjectID, std::string, ObjectID_hash, ObjectID_eq>::Type PathMap;
	/// A map for accessing the path string of any object id in the archive.
	PathMap _pathmap;

	typedef HASH_MAPS<ObjectID, std::size_t, ObjectID_hash, ObjectID_eq>::Type TableMap;
	/// A map for finding the table index of an object id in the archive.
	TableMap _table_map;

	typedef HASH_MAPS<ObjectID, LinkBase, ObjectID_hash, ObjectID_eq>::Type CacheMap;
	/// A map of all cached objects indexed by object id.
	CacheMap _static_map;

	std::vector<std::string> _paths;
	
	FILE *_f;
	bool _is_read;
	uint32 _table_offset;
	bool _finalized;
	std::string _fn;
	bool _chain;
	DataManager *_manager;

	/** Write a "magic" string to the start of the file to
	 *  identify it as a data archive.
	 */
	void writeMagic();

	/** Read a string from the start of the file and make
	 *  sure the "magic" matches.
	 */
	void readMagic();

	/** Read the object table from the end of the file.
	 *
	 *  This table contains a list of all objects stored in
	 *  the archive.  The list include the object id hash
	 *  which is used for fast access, and the class id hash
	 *  which allows a new instance to be created from the
	 *  correct entry in the interface registry.
	 */
	void readTable();

	/** Write the completed object table to the end of the file.
	 */
	void writeTable();

	/** Read the path table of contents from the archive.
	 *
	 *  The table of contents associates each path node with
	 *  its parent directory to enable searching for all
	 *  objects below a given path.  It also includes all
	 *  path strings so that path hash values can be translated
	 *  back to human readible form for debugging.
	 */
	void _readPaths();

	/** Write the path table of contents to the archive.
	 */
	void _writePaths() const;

public:

	/** Open a new data archive.
	 *
	 *  @param fn the full path to the archive file
	 *  @param read true for read mode, false for write mode.
	 *  @param chain this is for internal use only.
	 */
	DataArchive(std::string const &fn, bool read, bool chain=true);

	/** Close the data archive and cleanup.
	 */
	~DataArchive();

	/** Add an object to the archive.
	 *
	 *  @param object the object to add.
	 *  @param path the path string of the object
	 */
	void addObject(Object &object, std::string const & path);

	/** Write the entry table and close the data archive.
	 *
	 *  This should only be called after writing objects
	 *  to a data archive.  Do not call this in read mode.
	 */
	void finalize();

	/** Test if the data archive has been finalized.
	 *
	 *  @returns true if the archive is finalized.
	 */
	bool isFinalized();

	/** Test if the archive is in read or write mode.
	 *
	 *  @returns true if in write mode.
	 */
	bool isWrite();

	/** Make this archive the "default" archive.  
	 *
	 *  It will remain so until another archive is assigned or it
	 *  is deleted (in which case the dafault reverts to NULL).  
	 *
	 *  @deprecated <em>Use of this default utility is not 
	 *              recommended.</em>
	 */
	void setDefault();

	/** Get the current default archive.
	 *
	 *  @returns the current default or NULL if there is none.
	 *
	 *  @deprecated <em>Use of this default utility is not 
	 *              recommended.</em>
	 */
	static DataArchive* getDefault();

	/** Create a new object from a path identifier string.
	 *
	 *  @param path_str The path identifier string.
	 *  @returns A smart-pointer to the new object.
	 */
	const LinkBase getObject(std::string const &path_str);

	/** Create a new object from a Path instance.
	 *
	 *  @param path the Path instance.
	 *  @param path_str The path identifier string (if available).  This 
	 *                  is only used for error logging.
	 */
	const LinkBase getObject(const Path& path, std::string const &path_str="");

	/** Get the full path of the archive file.
	 *
	 *  @returns the full path of the archive file.
	 */
	std::string getFileName() const { return _fn; }

	/** Get all children of a given object id.
	 *
	 *  For id "A:X.Y", returns all object id's "A:X.Y.*".  The id's
	 *  can be converted to human-readable form by getPathString().
	 *
	 *  @param id the object id to search for children
	 *  @returns a list of object id's immediately below the given path.
	 */
	std::vector<ObjectID> getChildren(ObjectID const &id) const;
	
	/** Get all children of a given path.
	 *
	 *  For path "A:X.Y", returns all object id's "A:X.Y.*".  The id's
	 *  can be converted to human-readable form by getPathString().
	 *
	 *  @param path the path to search for children
	 *  @returns a list of object id's immediately below the given path.
	 */
	std::vector<ObjectID> getChildren(std::string const & path) const;

	/** Check for the existance of an object in the archive.
	 *
	 *  @returns true if the object id exists.
	 */
	bool hasObject(ObjectID const &id) const;

	/** Check for the existance of an object in the archive.
	 *
	 *  @returns true if the object id exists.
	 */
	bool hasObject(std::string const & path) const;

	/** Get the path string corresponding to a give object id.
	 *
	 *  This provides a human-readable path string that is useful
	 *  for error and debugging messages.
	 *
	 *  @returns the path string if found, otherwise an empty string.
	 */
	std::string getPathString(ObjectID const &id) const; 

	/** Get a list of all objects in the archive.
	 */
	std::vector<ObjectID> getAllObjects() const;

	/** Get a list of all object paths in the archive.
	 */
	std::vector<std::string> getAllPathStrings() const;

	/** Remove unused static objects from the cache.
	 *
	 *  Call this method after many objects have been deleted
	 *  to free any unused cache entries.
	 */
	void cleanStatic();

	/** Return the interface proxy corresponding to the specified
	 *  object in the archive.
	 */
	InterfaceProxy *getObjectInterface(ObjectID const &id, std::string const &path="") const;

	/** Return the interface proxy corresponding to the specified
	 *  object in the archive.
	 */
	InterfaceProxy *getObjectInterface(std::string const &path) const;


	/** Dump arhcive information to stdout (for debugging).
	 */
	void dump() const;

	// protected methods made public for Python access, don't use!
	/*
	long _getOffset(); 
	FP _filePointer();
	*/

protected:
	/** Add a new entry to the object table.
	 *
	 *  @param offset the byte offset of the start of the object data
	 *  @param length the number of bytes in the serialized object
	 *  @param hash the class hash identifier
	 *  @param path the path string of the object
	 */
	void _addEntry(int offset, int length, ObjectID hash, std::string const &path);
	
	/*
	Object* getObject(const Object &a, const char* path);
	Object* getObject(const Object &a, ObjectID path, const char* path="");
	Object* getObject(ObjectID path);
	*/

	/** Add an object to the static object cache.
	 *
	 *  @param ptr the object to cache
	 *  @param path_str the path identifier string
	 *  @param key the object identifier hash (path hash)
	 */
	void _addStatic(Object* ptr, std::string const &path_str, ObjectID key=0);

	/** Get an object from the static object cache.
	 *
	 *  @returns the object if found, otherwise NULL.
	 */
	LinkBase const * _getStatic(ObjectID key);

	/** Create an a new instance using a class identifier hash.
	 *
	 *  @param classhash the identifier hash of the class to create.
	 *  @returns the newly created object.
	 */
	Object* _createObject(ObjectID classhash);

	/** Find the table entry corresponding to a given object path.
	 *
	 *  @param path the path to lookup
	 *  @param path_str the path string, if available (only used for
	 *                  error logging).
	 */
	const TableEntry* _lookupPath(Path const& path, std::string const &path_str="") const;

	/** Find the table entry corresponding to a given object.
	 *
	 *  @param id the object id to lookup
	 *  @param path_str the path string, if available (only used for
	 *                  error logging).
	 */
	const TableEntry* _lookupPath(ObjectID const &id, std::string const &path_str="") const;

	/** Get the associated data manager instance, if any.
	 */
	DataManager *getManager() const { return _manager; }

private:
	/** Called by DataManager to bind the archive.
	 */
	void setManager(DataManager *m);
	ChildMap const &getChildMap() const { return _children; }
};




NAMESPACE_SIMDATA_END

#endif //__SIMDATA_DATAARCHIVE_H__

