// Copyright (C) 1999-2002 Open Source Telecom Corporation.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// As a special exception to the GNU General Public License, permission is
// granted for additional uses of the text contained in its release
// of Common C++.
//
// The exception is that, if you link the Common C++ library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the Common C++ library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
//
// This exception applies only to the code released under the
// name Common C++.  If you copy code from other releases into a copy of
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
//
// If you write modifications of your own for Common C++, it is your
// choice whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

/**
 * @file file.h
 * @short Files, fifos, directories, pipes and dynamic loader services.
 **/

#ifndef	CCXX_FILE_H_
#define	CCXX_FILE_H_

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#ifndef	CCXX_THREAD_H_
#include <cc++/thread.h>
#endif

#include <iostream>
#include <fstream>

#ifndef WIN32
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mman.h>
#else
#include <direct.h>
#endif

#ifdef	HAVE_MACH_DYLD
#include <mach-o/dyld.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

typedef	unsigned long pos_t;
#ifndef WIN32
// use a define so that if the sys/types.h header already defines caddr_t
// as it may on BSD systems, we do not break it by redefining again.
#undef  caddr_t
#define	caddr_t	char *
typedef size_t ccxx_size_t;
#else
typedef LONG off_t;
typedef void* caddr_t;
typedef DWORD ccxx_size_t;
#endif

class File
{
public:
	enum Error
	{
		errSuccess = 0,
		errNotOpened,
		errMmapFailed,
		errInitFailed,
		errOpenDenied,
		errOpenFailed,
		errOpenInUse,
		errReadInterrupted,
		errReadIncomplete,
		errReadFailure,
		errWriteInterrupted,
		errWriteIncomplete,
		errWriteFailure,
		errExtended
	};
	typedef enum Error Error;

	enum Access
	{
#ifndef WIN32
		accessReadOnly = O_RDONLY,
		accessWriteOnly= O_WRONLY,
		accessReadWrite = O_RDWR
#else
		accessReadOnly = GENERIC_READ,
		accessWriteOnly = GENERIC_WRITE,
		accessReadWrite = GENERIC_READ | GENERIC_WRITE
#endif
	};
	typedef enum Access Access;

protected:
	typedef struct _fcb
	{
		struct _fcb *next;
		caddr_t address;
		ccxx_size_t len;
		off_t pos;
	} fcb_t;

#ifdef	WIN32
	enum Open
	{
		openReadOnly, // = FILE_OPEN_READONLY,
		openWriteOnly, // = FILE_OPEN_WRITEONLY,
		openReadWrite, // = FILE_OPEN_READWRITE,
		openAppend, // = FILE_OPEN_APPEND,
		openTruncate // = FILE_OPEN_TRUNCATE
	};
	#else
	enum Open
	{
		openReadOnly = O_RDONLY,
		openWriteOnly = O_WRONLY,
		openReadWrite = O_RDWR,
		openAppend = O_WRONLY | O_APPEND,
#ifdef	O_SYNC
		openSync = O_RDWR | O_SYNC,
#else
		openSync = O_RDWR,
#endif
		openTruncate = O_RDWR | O_TRUNC
	};
	typedef enum Open Open;

/* to be used in future */

#ifndef	S_IRUSR
#define	S_IRUSR	0400
#define	S_IWUSR 0200
#define	S_IRGRP 0040
#define	S_IWGRP 0020
#define	S_IROTH 0004
#define	S_IWOTH	0002
#endif

#endif // !WIN32

#ifndef WIN32
	enum Attr
	{
		attrInvalid = 0,
		attrPrivate = S_IRUSR | S_IWUSR,
		attrGroup = attrPrivate | S_IRGRP | S_IWGRP,
		attrPublic = attrGroup | S_IROTH | S_IWOTH
	};
	#else // defined WIN32
	enum Attr {
		attrInvalid=0,
		attrPrivate,
		attrGroup,
		attrPublic
	};
#endif // !WIN32
	typedef enum Attr Attr;

#ifdef	WIN32
	enum Complete
	{
		completionImmediate, // = FILE_COMPLETION_IMMEDIATE,
		completionDelayed, // = FILE_COMPLETION_DELAYED,
		completionDeferred // = FILE_COMPLETION_DEFERRED
	};

	enum Mapping
	{
		mappedRead, // = FILE_MAPPED_READ
		mappedWrite, // = FILE_MAPPED_WRITE,
		mappedReadWrite // = FILE_MAPPED_RDWR
	};
#else
	enum Mapping
	{
		mappedRead = accessReadOnly,
		mappedWrite = accessWriteOnly,
		mappedReadWrite = accessReadWrite
	};
	enum Complete
	{
		completionImmediate,
		completionDelayed,
		completionDeferred
	};
#endif
	typedef enum Complete Complete;
	typedef enum Mapping Mapping;
};

#ifndef WIN32
/**
 * This class provides a few alterations to the standard fstream class
 * for dealing with fifo devices.  In particular, a fifo is assumed to
 * be created via mkfifo and is destroyed when closed.  The fifo is a
 * r/w streamable object.  fifo streams are presumed unique to posix
 * systems and are generally not portable classes.
 *
 * @todo implement in win32
 */

class fifostream : public std::fstream, public File
{
private:
	char *pathname;

public:
	/**
	 * Create fifo "object", but do not create or open a fifo
	 * stream.
	 */
	fifostream();

	/**
	 * Create and open a fifo stream object.
	 *
	 * @param fname name of fifo.
	 * @param access access flags.
	 */
	fifostream(const char *fname, long access = (long)attrGroup);

	/**
	 * close any open fifo and remove.
	 */
	virtual ~fifostream();

	/**
	 * open a new fifo stream.  Kill the currently open one if
	 * present.
	 *
	 * @param fname name of fifo.
	 * @param access access flags.
	 */
	void open(const char *fname, long access = (long)attrGroup);

	/**
	 * close the fifo stream and remove it.
	 */
	void close(void);
};
#endif // !WIN32

#ifndef WIN32
/**
 * The FIFOSession produces a seperate thread which can manage a fifo
 * stream session.  This is somewhat similar to TTYSession and TCPSession
 * in purpose.
 * @todo implement in win32
 */
class FIFOSession : public Thread, public std::fstream, public File
{
private:
	char *pathname;

public:
	FIFOSession(const char *session, long access = (long)attrGroup, int pri = 0, int stack = 0);
	virtual ~FIFOSession();
};
#endif // !WIN32

/**
 * A low level portable directory class.  Used to support ccstd Directory
 * container.  This provides a basic mechanism for allocating and
 * accessing file entries.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short low level directory access class.
 */
class CCXX_CLASS_EXPORT Dir
{
private:
#ifndef WIN32
	DIR *dir;
#else
	HANDLE hDir;
	WIN32_FIND_DATA data;
	char *name;
#endif

public:
	Dir(const char *name);
	virtual ~Dir();

	char *getName(void);

	bool operator!()
#ifndef WIN32
		{return !dir;};
#else
		{return hDir != INVALID_HANDLE_VALUE;};
#endif

	bool isValid(void);
};

/**
 * The purpose of this class is to define a base class for low level
 * random file access that is portable between Win32 and Posix systems.
 * This class is a foundation both for optimized thread shared and
 * traditional locked file access that is commonly used to build
 * database services, rather than the standard C++ streaming file classes.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Portable random disk file access.
 */
class CCXX_CLASS_EXPORT RandomFile : protected Mutex, public File
{
private:
	Error errid;
	char *errstr;

protected:
#ifndef WIN32
	int fd;
	// FIXME: WIN32 as no access member
	Access access;
#else
	HANDLE fd;
#endif
	char *pathname;

	struct
	{
		unsigned count : 16;
		bool thrown : 1;
		bool initial : 1;
#ifndef WIN32
		bool immediate : 1;
#endif
		bool temp : 1;
	} flags;

	/**
	 * Create an unopened random access file.
	 */
	RandomFile();

	/**
	 * Default copy constructor.
	 */
	RandomFile(const RandomFile &rf);

	/**
	 * Post an error event.
	 *
	 * @return error code.
	 * @param errid error code.
	 * @param errstr error message string.
	 */
	Error error(Error errid, char *errstr = NULL);

	/**
	 * Post an extended string error message.
	 *
	 * @return errExtended.
	 * @param errstr error string.
	 */
	inline Error error(char *errstr)
		{return error(errExtended, errstr);};

	/**
	 * Used to enable or disable throwing of exceptions on
	 * errors.
	 *
	 * @param enable true if errors will be thrown.
	 */
	inline void setError(bool enable)
		{flags.thrown = !enable;};

#ifndef WIN32
	/**
	 * Used to set file completion modes.
	 *
	 * @return errSuccess if okay.
	 * @param mode completion mode.
	 * @todo implement in win32
	 */
	Error setCompletion(Complete mode);
#endif

	/**
	 * Used to set the temporary attribute for the file.  Temporary
	 * files are automatically deleted when closed.
	 *
	 * @param enable true for marking as temporary.
	 */
	inline void setTemporary(bool enable)
		{flags.temp = enable;};

	/**
	 * This method is used to initialize a newly created file as
	 * indicated by the "initial" flag.  This method also returns
	 * the file access permissions that should be associated with
	 * the file.  This method should never be called directly, but
	 * is instead used to impliment the "Initial" method.  Typically
	 * one would use this to build an empty database shell when a
	 * previously empty database file is created.
	 *
	 * @return access, or attrInvalid if should be removed.
	 */
	virtual Attr initialize(void)
		{return attrPublic;};

	/**
	 * Close the file.
	 */
	void final(void);

public:
	/**
	 * Destroy a random access file or it's derived class.
	 */
	virtual ~RandomFile()
		{final();};

	/**
	 * This method should be called right after a RandomFile derived
	 * object has been created.  This method will invoke initialize
	 * if the object is newly created, and set file access permissions
	 * appropriately.
	 *
	 * @return true if file had to be initialized.
	 */
	bool initial(void);

	/**
	 * Get current file capacity.
	 *
	 * @return total file size.
	 */
	off_t getCapacity(void);

	/**
	 * This method is commonly used to close and re-open an existing
	 * database.  This may be used when the database has been unlinked
	 * and an external process provides a new one to use.
	 */
	virtual Error restart(void)
		{return errOpenFailed;};

	/**
	 * Return current error id.
	 *
	 * @return last error identifier set.
	 */
	inline Error getErrorNumber(void)
		{return errid;};

	/**
	 * Return current error string.
	 *
	 * @return last error string set.
	 */
	inline char *getErrorString(void)
		{return errstr;};

	bool operator!(void);
};

/**
 * This class defines a database I/O file service that can be shared
 * by multiple threads.  All threads access a global copy of the database
 * object, and mutex locks can be used to preserve transaction
 * integrety.  pread/pwrite calls can be used for optimized I/O when
 * supported.
 * 
 * ThreadFile is meant for use by a threaded database server where multiple
 * threads may each perform semi-independent operations on a given database
 * table stored on disk.  A special "fcb" structure is used to hold file
 * "state", and pread/pwrite is used whenever possible for optimized I/O.  On
 * systems that do not offer pwread/pwrite, a Mutex lock is used to protect
 * concurrent lseek and read/write operations.  ThreadFile managed databases
 * are assumed to be used only by the local server and through a single file
 * descriptor.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short This class defines a database I/O file service that can be shared by multiple threads.
 */
class CCXX_CLASS_EXPORT ThreadFile : public RandomFile
{
private:
	ThreadKey state;
	fcb_t *first;
	fcb_t *getFCB(void);
	Error open(const char *path);

public:
	/**
	 * Open or create a new database file.  You should also use
	 * Initial.
	 *
	 * @param path pathname of database to open.
	 */
	ThreadFile(const char *path);

	/**
	 * Close and finish a database file.
	 */
	virtual ~ThreadFile();

	/**
	 * Restart an existing database; close and re-open.
	 *
	 * @return errSuccess if successful.
	 */
	Error restart(void)
		{return open(pathname);};

	/**
	 * Fetch a portion of the file into physical memory.  This can use
	 * state information to fetch the current record multiple times.
	 *
	 * @return errSuccess on success.
	 * @param address  address to use, or NULL if same as last I/O.
	 * @param length   length to use, or 0 if same as last I/O.
	 * @param position file position to use -1 if same as last I/O.
	 */
	Error fetch(caddr_t address = NULL, ccxx_size_t length = 0, off_t position = -1);

	/**
	 * Update a portion of a file from physical memory.  This can use
	 * state information to commit the last read record.
	 *
	 * @return errSuccess on success.
	 * @param address  address to use, or NULL if same as last I/O.
	 * @param length   length to use, or 0 if same as last I/O.
	 * @param position file position to use or -1 if same as last I/O.
	 */
	Error update(caddr_t address = NULL, ccxx_size_t length = 0, off_t position = -1);

	/**
	 * Add new data to the end of the file.
	 * @param address address to use, or NULL if same as last I/O.
	 * @param length  length to use, or 0 if same as last I/O.
	 */
	Error append(caddr_t address = NULL, ccxx_size_t length = 0);

	/**
	 * Fetch the current file position marker for this thread.
	 *
	 * @return file position offset.
	 */
	off_t getPosition(void);

	bool operator++(void);
	bool operator--(void);
};

/**
 * This class defines a database I/O file service that can be shared
 * by multiple processes.  Each thread should access a dup of the database
 * object, and mutex locks can be used to preserve transaction
 * integrety if multiple threads are used.
 *
 * SharedFile is used when a database may be shared between multiple
 * processes.  SharedFile automatically applies low level byte-range "file
 * locks", and provides an interface to fetch and release byte-range locked
 * portions of a file.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short This class defines a database I/O file service that can be shared by multiple processes.
 */
class CCXX_CLASS_EXPORT SharedFile : public RandomFile
{
private:
	fcb_t fcb;
	Error open(const char *path);

public:
	/**
	 * Open or create a new database file.  You should also use
	 * Initial.
	 *
	 * @param path pathname of database to open.
	 */
	SharedFile(const char *path);

	/**
	 * Create a shared file as a duplicate of an existing shared
	 * file.
	 *
	 * @param file original file.
	 */
	SharedFile(const SharedFile &file);

	/**
	 * Close and finish a database file.
	 */
	virtual ~SharedFile();

	/**
	 * Restart an existing database; close and re-open.
	 *
	 * @return errSuccess if successful.
	 */
	Error restart(void)
		{return open(pathname);};

	/**
	 * Lock and Fetch a portion of the file into physical memory.
         * This can use state information to fetch the current record
         * multiple times.
	 *
	 * @return errSuccess on success.
	 * @param address  address to use, or NULL if same as last I/O.
	 * @param length   length to use, or 0 if same as last I/O.
	 * @param position file position to use -1 if same as last I/O.
	 */
	Error fetch(caddr_t address = NULL, ccxx_size_t length = 0, off_t position = -1);

	/**
	 * Update a portion of a file from physical memory.  This can use
	 * state information to commit the last read record.  The current
	 * lock is also cleared.
	 *
	 * @return errSuccess on success.
	 * @param address  address to use, or NULL if same as last I/O.
	 * @param length   length to use, or 0 if same as last I/O.
	 * @param position file position to use or -1 if same as last I/O.
	 */
	Error update(caddr_t address = NULL, ccxx_size_t length = 0, off_t position = -1);

	/**
	 * Clear a lock held from a previous fetch operation without
	 * updating.
	 *
	 * @return errSuccess on success.
	 * @param length length to use, or 0 if same as last I/O.
	 * @param pos    file position to use or -1 if same as last I/O.
	 */
	Error clear(ccxx_size_t length = 0, off_t pos = -1);

	/**
	 * Add new data to the end of the file.  Locks file during append.
	 *
	 * @param address address to use, or NULL if same as last I/O.
	 * @param length  length to use, or 0 if same as last I/O.
	 */
	Error append(caddr_t address = NULL, ccxx_size_t length = 0);

	/**
	 * Fetch the current file position marker for this thread.
	 *
	 * @return file position offset.
	 */
	off_t getPosition(void);

	bool operator++(void);
	bool operator--(void);
};

#ifndef WIN32
/**
 * Create and map a disk file into memory.  This portable class works
 * under both Posix via mmap and under the win32 API. A mapped file
 * can be referenced directly by it's memory segment. One can map 
 * and unmap portions of a file on demand, and update
 * changed memory pages mapped from files immediately through sync().
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Map a named disk file into memory.
 * @todo implement in win32
 */
class CCXX_CLASS_EXPORT MappedFile : public RandomFile
{
private:
	fcb_t fcb;
	int prot;

public:
	/**
	 * Open a file for mapping.  More than one segment of a file
	 * may be mapped into seperate regions of memory.
	 *
	 * @param fname file name to access for mapping.
	 * @param mode  access mode to map file.
	 */
	MappedFile(const char *fname, Access mode);

	/**
	 * Map a portion or all of a specified file in the specified
	 * shared memory access mode.  Valid mapping modes include
	 * mappedRead, mappedWrite, and mappedReadWrite.
	 * 
	 * @param fname pathname of file to map into memory.
	 * @param offset from start of file to begin mapping in bytes.
	 * @param size of mapped area in bytes.
	 * @param mode to map file.
	 */
	MappedFile(const char *fname, pos_t offset, size_t size, Access mode);	
	
	/**
	 * Release a mapped section of memory associated with a file.  The
	 * mapped area is updated back to disk.
	 */
	virtual ~MappedFile();

	// FIXME: not use library function in header ??
	/**
	 * Synchronize the contents of the mapped portion of memory with
	 * the disk file and wait for completion.  This assures the memory
	 * mapped from the file is written back.
	 */
	inline void sync(void)
		{msync(fcb.address, fcb.len, MS_SYNC);};

	/**
	 * Synchronize a segment of memory mapped from a segment fetch.
	 *
	 * @param address memory address to update.
	 * @param len size of segment.
	 */
	void sync(caddr_t address, size_t len);

	/**
	 * Map a portion of the memory mapped from the file back to the
	 * file and do not wait for completion.  This is useful when mapping
	 * a database file and updating a single record.
	 * 
	 * @param offset offset into the mapped region of memory.
	 * @param len length of partial region (example, record length).
	 */
	void update(size_t offset = 0, size_t len = 0);

	/**
	 * Update a mapped region back to disk as specified by address
	 * and length.
	 *
	 * @param address address of segment.
	 * @param len length of segment.
	 */
	void update(caddr_t address, size_t len);

	/**
	 * Release (unmap) a memory segment.
	 *
	 * @param address address of memory segment to release.
	 * @param len length of memory segment to release.
	 */
	void release(caddr_t address, size_t len);

	/**
	 * Fetch a pointer to an offset within the memory mapped portion
	 * of the disk file.  This really is used for convience of matching
	 * operations between Update and Fetch, as one could simply have
	 * accessed the base pointer where the file was mapped directly.
	 * 
	 * @param offset from start of mapped memory.
	 */
	inline caddr_t fetch(size_t offset = 0)
		{return ((char *)(fcb.address)) + offset;};

	/**
	 * Fetch and map a portion of a disk file to a logical memory
	 * block.
	 *
	 * @return pointer to memory segment.
	 * @param pos offset of file segment to map.
	 * @param len size of memory segment to map.
	 */
	caddr_t fetch(off_t pos, size_t len);
};
#endif // !WIN32

/**
 * The Pipe uses system kernel buffering to hold data being passed either
 * between two execution contexts within the same process, or between
 * different processes.  Unlike thread's "Buffer", Pipe uses system
 * descriptors and kernel memory.  Under Posix, the size of the pipe and
 * associated kernel memory is always a fixed constant as defined by
 * _PC_PIPE_BUF.  The Common C++ "pipe" class primarily deals with "atomic"
 * transfers of fixed sized objects through pipes.  Pipes may pass
 * data arbitrarily and can also be used through the "pipestream" class.
 *
 * The "Pipe" class is not meant to be a true "public" class, but as a
 * builder class for deriving other classes.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short kernel buffering between processes and/or threads.
 */
class CCXX_CLASS_EXPORT Pipe
{
protected:
#ifndef WIN32
	int fd[2];
#else
	HANDLE fd[2];
#endif
	int objcount;
	int objsize;

	/**
	 * Get the object size for atomic operations.
	 *
	 * @return size of atomic operations.
	 */
	inline int getSize(void)
		{return objsize;};

	/**
	 * Sender is often used for implementing a fork()'d message port
	 * between processes.  By stopping the sender, the current pipe
	 * can then only be used for receiving, and the sender is presumed
	 * to be in the other half of a fork()'d process.
	 * 
	 * @see #endReceiver
	 */
	inline void endSender(void)
#ifndef WIN32
		{close(fd[1]);};
#else
		{CloseHandle(fd[1]); fd[1] = NULL; };
#endif
	/**
	 * Receiver is often used for implementing a fork()'d message port
	 * between processes.  By stopping the receiver, the current pipe
	 * can then only be used for sending, and the receiver is presumed
	 * to be in the other half of a fork()'d process.
	 *
	 * @see #endSender
	 */
	inline void endReceiver(void)
#ifndef WIN32
		{close(fd[0]);};
#else
		{CloseHandle(fd[0]); fd[0] = NULL; };
#endif

	/**
	 * Create a kernel pipe descriptor set using pipe().  On systems
	 * which allow the pipe kernel buffer to be defined, a size for
	 * aligned atomic transfers can be set, as well as the number of
	 * atomic objects the kernel will buffer.  On Posix systems, these
	 * options are ignored.
	 *
	 * @param size objsize of atomic objects to send.
	 * @param count count of atomic objects to kernel buffer.
	 */
	Pipe(int size = 512, int count = 1);

	/**
	 * Destroy the pipe and kernel descriptor resources.
	 */
	virtual ~Pipe();

	/**
	 * Create a pipe as a duplicate of an existing pipe.
	 * 
	 * @param orig pipe to duplicate.
	 */
	Pipe(const Pipe &orig);

	Pipe &operator=(const Pipe &orig);

	/* depreciated methods */
	void sender(void)
		{endReceiver();};

	void receiver(void)
		{endSender();};

	int read(void *buf, int len)
#ifndef WIN32
		{return ::read(fd[0], (char *)buf, len);};
#else
		{DWORD dwLen; return ReadFile(fd[0],buf,len,&dwLen,NULL) ? dwLen : (DWORD)-1; };
#endif

	int write(void *buf, int len)
#ifndef WIN32
		{return ::write(fd[1], (char *)buf, len);};
#else
		{DWORD dwLen; return WriteFile(fd[1],buf,len,&dwLen,NULL) ? dwLen : (DWORD)-1; };
#endif

public:
	/**
	 * Used to see if the pipe has any open entities.
	 */
	bool operator!();

	/**
	 * Perform an object atomic transfer of data from a pipe.
	 * 
	 * @return number of bytes actually read if successful.
	 * @param addr pointer to store read data.
	 */
#ifndef WIN32
	inline int receive(void *addr)
		{return ::read(fd[0], (char *)addr, objsize);};
#else
	int receive(void *addr);
#endif

	/**
	 * Perform an object atomic transfer of data to a pipe.
	 * 
	 * @return number of bytes written if successful.
	 * @param addr pointer to write data from.
	 */
#ifndef WIN32
	inline int send(void *addr)
		{return ::write(fd[1], (char *)addr, objsize);};
#else
	int send(void *addr);
#endif

	/**
	 * Verify this object is "valid".
	 *
	 * @return true if valid.
	 */
	bool isValid(void);
};

/**
 * The DSO dynamic loader class is used to load object files.  On
 * elf based systems this is typically done with dlopen.  A dummy
 * stub class is generated for non-dl capable systems.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Dynamic class file loader.
 */
class DSO
{
private:
#ifdef	HAVE_MODULES
	static Mutex mutex;
	static DSO *first, *last;
	DSO *next, *prev;
#if	defined(HAVE_MACH_DYLD)
	NSModule oModule;
	char *err;	
#elif	defined(HAVE_SHL_LOAD)
	char *err;
	shl_t image;
#elif	defined(WIN32)
	char *err;
	HINSTANCE hImage;
#else
	void *image;
#endif
	CCXX_MEMBER_EXPORT(void) loader(const char *filename, bool resolve);
#endif

public:
	/**
	 * Construct and load a DSO object file.
	 *
	 * @param filename pathname of object file to load.
	 */ 
#ifdef	HAVE_MODULES
	DSO(const char *filename)
		{loader(filename, true);};

	DSO(const char *filename, bool resolve)
		{loader(filename, resolve);};
#else
	DSO(const char *filename)
		{throw this;};
	DSO(const char *filename, bool resolve)
		{throw this;};
#endif

	/**
	 * Retrieve error indicator associated with DSO failure.  This
	 * is often used in catch handlers.
	 */
	char *getError(void);

	/**
	 * Detach a DSO object from running memory.
	 */
#ifdef	HAVE_MODULES
	CCXX_MEMBER_EXPORT(virtual) ~DSO();
#endif

	/**
	 * Lookup a symbol in the loaded file.
	 */
#ifdef	HAVE_MODULES
	CCXX_MEMBER_EXPORT(void*) operator[](const char *sym);
#else
	void *operator[](const char *)
		{return NULL;};
#endif

#ifdef	HAVE_MODULES
	static CCXX_EXPORT(void) dynunload(void);
#else
	static void dynunload(void)
		{return;};
#endif

	/**
	 * See if DSO object is valid.
	 *
	 * @return true if valid.
	 */
	CCXX_MEMBER_EXPORT(bool) isValid(void);
};

/** @relates RandomFile */
bool isDir(const char *path);
/** @relates RandomFile */
bool isFile(const char *path);
#ifndef WIN32
/** @relates RandomFile */
bool isDevice(const char *path);
#else
/** @relates RandomFile */
inline bool isDevice(const char *path)
{ return false; }
#endif
/** @relates RandomFile */
bool canAccess(const char *path);
/** @relates RandomFile */
bool canModify(const char *path);

#ifdef	COMMON_STD_EXCEPTION

class DirException : public IOException
{
public:
	DirException(std::string str) : IOException(str) {};
};

class DSOException : public IOException
{
public:
	DSOException(std::string str) : IOException(str) {};
};

class FIFOException : public IOException
{
public:
	FIFOException(std::string str) : IOException(str) {};
};

class PipeException : public IOException
{
public:
	PipeException(std::string str) : IOException(str) {};
};

class FileException : public IOException
{
public:
	FileException(std::string str) : IOException(str) {};
};

#endif

#ifdef	CCXX_NAMESPACES
};
#endif

#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
