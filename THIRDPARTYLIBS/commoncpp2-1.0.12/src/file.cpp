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
// The exception is that, if you link the Common C++ library with other files
// to produce an executable, this does not by itself cause the
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
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.  

// needed for GNU/LINUX glibc otherwise pread/pwrite wont work

#ifdef	__linux__
#ifndef	_XOPEN_SOURCE
#define	_XOPEN_SOURCE 500
#endif
/*
 * on old glibc's, this has to be
 * defined explicitly
 */
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif
#endif

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/file.h>
#include "private.h"

#include <cstdio>
#include <sys/stat.h>
#include <cerrno>

using std::string;

#ifndef WIN32

#ifdef	HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#ifdef	HAVE_SYS_LOCKF_H
#include <sys/lockf.h>
#endif

#ifdef	COMMON_AIX_FIXES
#undef	LOCK_EX
#undef	LOCK_SH
#endif

#ifdef	MACOSX
#define	MISSING_LOCKF
#endif

#ifndef	F_LOCK
#define	MISSING_LOCKF

enum
{
	F_ULOCK = 1,
	F_LOCK,
	F_TLOCK,
	F_TEST
};

#endif

#ifdef	MISSING_LOCKF

static	int lockf(int fd, int cmd, long len)
{
	struct	flock lck;

	lck.l_start = 0l;
	lck.l_whence = SEEK_CUR;
	lck.l_len = len;

	switch(cmd)
	{
	case F_ULOCK:
		lck.l_type = F_UNLCK;
		return fcntl(fd, F_SETLK, &lck);
	case F_LOCK:
		lck.l_type = F_WRLCK;
		return fcntl(fd, F_SETLKW, &lck);
	case F_TLOCK:
		lck.l_type = F_WRLCK;
		return fcntl(fd, F_SETLK, &lck);
	case F_TEST:
		lck.l_type = F_WRLCK;
		fcntl(fd, F_GETLK, &lck);
		if(lck.l_type == F_UNLCK)
			return 0;
		return -1;
	}
}
#endif // ndef F_LOCK

#endif // ndef WIN32
		

#if 0
/*
 * not used anymore ? (hen)
 */
static const char *clearfile(const char *pathname)
{
	remove(pathname);
	return pathname;
}

static const char *clearfifo(const char *pathname, int mode)
{
	remove(pathname);
	mkfifo(pathname, mode);
	return pathname;
}
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

RandomFile::RandomFile() {
#ifdef WIN32
	fd = INVALID_HANDLE_VALUE;
	// immediate is not defined on Win32 
#else
	fd = -1;
	flags.immediate = false;
#endif
	flags.thrown = flags.initial = flags.temp = false;
	flags.count = 0;
}

RandomFile::RandomFile(const RandomFile &rf) : Mutex()
{
	// first, `dup'-licate the file descriptor/handle
#ifdef WIN32
	HANDLE pidHandle = GetCurrentProcess();
	HANDLE dupHandle;

	if(rf.fd != INVALID_HANDLE_VALUE)
	{
		if(!DuplicateHandle(pidHandle, rf.fd, pidHandle, &dupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
			fd = INVALID_HANDLE_VALUE;
		else
			fd = dupHandle;
	}
	else
		fd = INVALID_HANDLE_VALUE;

#else
	if(rf.fd > -1)
		fd = dup(rf.fd);
	else
		fd = -1;

#endif	

	flags = rf.flags;
	flags.count = 0;

	if(rf.pathname)
	{
		pathname = new char[strlen(rf.pathname) + 1];
		strcpy(pathname, rf.pathname);
	}
	else
		pathname = NULL;
}

void RandomFile::final(void)
{
#ifdef WIN32
	if(fd != INVALID_HANDLE_VALUE)
	{
		CloseHandle(fd);
		if(flags.temp)
			DeleteFile(pathname);
	}

#else
	if(fd > -1)
	{
		close(fd);
		if(flags.temp)
			remove(pathname);
	}
#endif

	if(pathname)
	{
		delete[] pathname;
		pathname = NULL;
	}

#ifdef WIN32
	fd = INVALID_HANDLE_VALUE;
#else
	fd = -1;
#endif
	flags.count = 0;
	flags.initial = false;
}

RandomFile::Error RandomFile::error(Error id, char *str)
{
	errstr = str;
	errid = id;
	if(!flags.thrown)
	{
		flags.thrown = true;
		if(getException() == Thread::throwObject)
			throw(this);
#ifdef	COMMON_STD_EXCEPTION
		else if(getException() == Thread::throwException)
		{
			if(!str)
				str = "";
			throw FileException(str);
		}
#endif
	}
	return id;
}

bool RandomFile::initial(void)
{
	bool init;

#ifdef WIN32
	if(fd == INVALID_HANDLE_VALUE)
#else
	if(fd < 0)
#endif		
		return false;

	enterMutex();
	init = flags.initial;
	flags.initial = false;

	if(!init)
	{
		leaveMutex();
		return false;
	}

#ifdef WIN32
	Attr access = initialize();
	if(access == attrInvalid)
	{
		CloseHandle(fd);
		if(pathname)
			DeleteFile(pathname);
		fd = INVALID_HANDLE_VALUE;
		error(errInitFailed);
	}

#else
	int mode = (int)initialize();
	if(!mode)
	{
		close(fd);
		fd = -1;
		if(pathname)
			remove(pathname);
		// FIXME: exception safe, mutex ??
		error(errInitFailed);
	}
	else
		fchmod(fd, mode);
#endif
	
	leaveMutex();
	return init;
}

#ifndef WIN32
RandomFile::Error RandomFile::setCompletion(Complete mode)
{
	long flag = fcntl(fd, F_GETFL);

	if(fd < 0)
		return errNotOpened;

	flags.immediate = false;
#ifdef O_SYNC
	flag &= ~(O_SYNC | O_NDELAY);
#else
	flag &= ~O_NDELAY;
#endif
	switch(mode)
	{
	case completionImmediate:
#ifdef O_SYNC
		flag |= O_SYNC;
#endif
		flags.immediate = true;
		break;

	case completionDelayed:
		flag |= O_NDELAY;

	case completionDeferred:
		break;
	}
	fcntl(fd, F_SETFL, flag);
	return errSuccess;
}
#endif

off_t RandomFile::getCapacity(void)
{
	off_t eof, pos = 0;
#ifdef WIN32
	if(!fd)
#else
	if(fd < 0)
#endif
		return 0;

	enterMutex();
#ifdef WIN32
	pos = SetFilePointer(fd, 0l, NULL, FILE_CURRENT);
	eof = SetFilePointer(fd, 0l, NULL, FILE_END);
	SetFilePointer(fd, pos, NULL, FILE_BEGIN);
#else
	lseek(fd, pos, SEEK_SET);
	pos = lseek(fd, 0l, SEEK_CUR);
	eof = lseek(fd, 0l, SEEK_END);
#endif
	leaveMutex();
	return eof;
}

bool RandomFile::operator!(void)
{
#ifdef WIN32
	return fd == INVALID_HANDLE_VALUE;
#else
	if(fd < 0)
		return true;

	return false;
#endif
}

ThreadFile::ThreadFile(const char *path) : RandomFile()
{
	first = NULL;
	open(path);
}

ThreadFile::~ThreadFile()
{
	final();
	fcb_t *next;
	while(first)
	{
		next = first->next;
		delete first;
		first = next;
	}
}

ThreadFile::Error ThreadFile::open(const char *path)
{
#ifdef WIN32
	if(fd != INVALID_HANDLE_VALUE)
#else
	if(fd > -1)
#endif
		final();


	if(path != pathname)
	{
		pathname = new char[strlen(path) + 1];
		strcpy(pathname, path);
	}

	flags.initial = false;

#ifdef WIN32
	fd = CreateFile(pathname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if(fd == INVALID_HANDLE_VALUE)
	{
		flags.initial = true;
		fd = CreateFile(pathname, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	}
	if(fd == INVALID_HANDLE_VALUE)
		return errOpenFailed;

#else
	fd = ::open(pathname, O_RDWR);
	if(fd < 0)
	{
		flags.initial = true;
		fd = ::open(pathname, O_CREAT | O_RDWR | O_TRUNC,
			    (int)attrPrivate);
	}
	if(fd < 0)
		return error(errOpenFailed);

#ifdef	LOCK_EX
	if(flock(fd, LOCK_EX | LOCK_NB))
	{
		close(fd);
		fd = -1;
		return error(errOpenInUse);
	}
#endif
#endif // WIN32

	return errSuccess;
}

ThreadFile::fcb_t *ThreadFile::getFCB(void)
{
	fcb_t *fcb = (fcb_t *)state.getKey();

	if(!fcb)
	{
		fcb = new fcb_t;
		fcb->next = first;
		first = fcb;
		fcb->address = NULL;
		fcb->len = 0;
		fcb->pos = 0;
		state.setKey(fcb);
	}
	return fcb;
}

ThreadFile::Error ThreadFile::fetch(caddr_t address, ccxx_size_t len, off_t pos)
{
	fcb_t *fcb = getFCB();
#ifdef WIN32
	if(fd == INVALID_HANDLE_VALUE)
#else
	if(fd < 0)
#endif
		return errNotOpened;

	if(address)
		fcb->address = address;

	if(len)
		fcb->len = len;

	if(pos != -1)
		fcb->pos = pos;


#ifdef WIN32
	enterMutex();
	SetFilePointer(fd, fcb->pos, NULL, FILE_BEGIN);
	DWORD count;
	if(!ReadFile(fd, fcb->address, fcb->len, &count, NULL))
	{
		leaveMutex();
		return errReadFailure;
	}
	leaveMutex();
	if(count < fcb->len)
		return errReadIncomplete;

	return errSuccess;

#else
#ifdef	HAVE_PREAD_PWRITE
	int io = ::pread(fd, fcb->address, fcb->len, fcb->pos);
#else
	enterMutex();
	lseek(fd, fcb->pos, SEEK_SET);
	int io = ::read(fd, fcb->address, fcb->len);
	leaveMutex();
#endif

	if((size_t) io == fcb->len)
		return errSuccess;

	if(io > -1)
		return errReadIncomplete;

	switch(errno)
	{
	case EINTR:
		return errReadInterrupted;
	default:
		return errReadFailure;
	}
#endif // WIN32
}

ThreadFile::Error ThreadFile::update(caddr_t address, ccxx_size_t len, off_t pos)
{
	fcb_t *fcb = getFCB();
#ifdef WIN32
	if(fd == INVALID_HANDLE_VALUE)
#else
	if(fd < 0)
#endif
		return errNotOpened;


	if(address)
		fcb->address = address;

	if(len)
		fcb->len = len;

	if(pos != -1)
		fcb->pos = pos;

#ifdef WIN32
	enterMutex();
	SetFilePointer(fd, fcb->pos, NULL, FILE_BEGIN);
	DWORD count;
	if(!WriteFile(fd, fcb->address, fcb->len, &count, NULL))
	{
		leaveMutex();
		return errWriteFailure;
	}
	leaveMutex();
	if(count < fcb->len)
		return errWriteIncomplete;

	return errSuccess;

#else
#ifdef	HAVE_PREAD_PWRITE
	int io = ::pwrite(fd, fcb->address, fcb->len, fcb->pos);
#else
	enterMutex();
	lseek(fd, fcb->pos, SEEK_SET);
	int io = ::write(fd, fcb->address, fcb->len);
	leaveMutex();
#endif

	if((size_t) io == fcb->len)
		return errSuccess;

	if(io > -1)
		return errWriteIncomplete;

	switch(errno)
	{
	case EINTR:
		return errWriteInterrupted;
	default:
		return errWriteFailure;
	}
#endif //WIN32
}

ThreadFile::Error ThreadFile::append(caddr_t address, ccxx_size_t len)
{
	fcb_t *fcb = getFCB();
#ifdef WIN32
	if(fd == INVALID_HANDLE_VALUE)
#else
	if(fd < 0)
#endif
		return errNotOpened;

	if(address)
		fcb->address = address;

	if(len)
		fcb->len = len;

	enterMutex();

#ifdef WIN32
	fcb->pos = SetFilePointer(fd, 0l, NULL, FILE_END);
	DWORD count;
	if(!WriteFile(fd, fcb->address, fcb->len, &count, NULL))
	{
		leaveMutex();
		return errWriteFailure;
	}
	leaveMutex();
	if(count < fcb->len)
		return errWriteIncomplete;

	return errSuccess;

#else
	fcb->pos = lseek(fd, 0l, SEEK_END);
	int io = ::write(fd, fcb->address, fcb->len);
	leaveMutex();

	if((size_t) io == fcb->len)
		return errSuccess;

	if(io > -1)
		return errWriteIncomplete;

	switch(errno)
	{
	case EINTR:
		return errWriteInterrupted;
	default:
		return errWriteFailure;
	}
#endif // WIN32
}

off_t ThreadFile::getPosition(void)
{
	fcb_t *fcb = getFCB();

	return fcb->pos;
}

bool ThreadFile::operator++(void)
{
	off_t eof;
	fcb_t *fcb = getFCB();

	fcb->pos += fcb->len;
	enterMutex();
#ifdef WIN32
	eof = SetFilePointer(fd, 0l, NULL, FILE_END);
#else
	eof = lseek(fd, 0l, SEEK_END);
#endif
	leaveMutex();

	if(fcb->pos >= eof)
	{
		fcb->pos = eof;
		return true;
	}
	return false;
}

bool ThreadFile::operator--(void)
{
	fcb_t *fcb = getFCB();

	fcb->pos -= fcb->len;
	if(fcb->pos <= 0)
	{
		fcb->pos = 0;
		return true;
	}
	return false;
}

SharedFile::SharedFile(const char *path) :
RandomFile()
{
	fcb.address = NULL;
	fcb.len = 0;
	fcb.pos = 0;
	open(path);
}

SharedFile::SharedFile(const SharedFile &sh) :
RandomFile(sh)
{
}

SharedFile::~SharedFile()
{
	final();
}

SharedFile::Error SharedFile::open(const char *path)
{
#ifdef WIN32
	if(fd != INVALID_HANDLE_VALUE)
#else
	if(fd > -1)
#endif
		final();

	if(path != pathname)
	{
		pathname = new char[strlen(path) + 1];
		strcpy(pathname, path);
	}

	flags.initial = false;

#ifdef WIN32
	fd = CreateFile(pathname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if(fd == INVALID_HANDLE_VALUE)
	{
		flags.initial = true;
		fd = CreateFile(pathname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	}
	if(fd == INVALID_HANDLE_VALUE)
		return errOpenFailed;

	return errSuccess;

#else
	fd = ::open(pathname, O_RDWR);
	if(fd < 0)
	{
		flags.initial = true;
		fd = ::open(pathname, O_CREAT | O_RDWR | O_TRUNC,
			    (int)attrPrivate);
	}
	if(fd < 0)
		return error(errOpenFailed);

#ifdef	LOCK_SH
	if(flock(fd, LOCK_SH | LOCK_NB))
	{
		close(fd);
		fd = -1;
		return error(errOpenInUse);
	}
#endif
	return errSuccess;
#endif // WIN32
}

SharedFile::Error SharedFile::fetch(caddr_t address, ccxx_size_t len, off_t pos)
{
#ifdef WIN32
	if(fd == INVALID_HANDLE_VALUE)
#else
	if(fd < 0)
#endif
		return errNotOpened;

	enterMutex();
	if(address)
		fcb.address = address;

	if(len)
		fcb.len = len;

	if(pos != -1)
		fcb.pos = pos;

#ifdef WIN32
	OVERLAPPED over;
	SetFilePointer(fd, fcb.pos, NULL, FILE_BEGIN);
	over.hEvent = 0;
	over.Offset = fcb.pos;
	over.OffsetHigh = 0;
	LockFileEx(fd, LOCKFILE_EXCLUSIVE_LOCK, 0, fcb.len, 0, &over);
	DWORD count;
	if(!ReadFile(fd, fcb.address, fcb.len, &count, NULL))
	{
		leaveMutex();
		return errReadFailure;
	}
	leaveMutex();
	if(count < fcb.len)
		return errReadIncomplete;

	return errSuccess;

#else
	lseek(fd, fcb.pos, SEEK_SET);
	lockf(fd, F_LOCK, fcb.len);
	int io = ::read(fd, fcb.address, fcb.len);
	leaveMutex();

	if((size_t) io == fcb.len)
		return errSuccess;

	if(io > -1)
		return errReadIncomplete;

	switch(errno)
	{
	case EINTR:
		return errReadInterrupted;
	default:
		return errReadFailure;
	}
#endif
}

#ifndef WIN32
SharedFile::Error SharedFile::clear(ccxx_size_t len, off_t pos)
{
	if(fd < 0)
		return errNotOpened;

	enterMutex();
	if(len)
		fcb.len = len;

	if(pos != -1)
		fcb.pos = pos;

	lseek(fd, fcb.pos, SEEK_SET);
	lockf(fd, F_ULOCK, fcb.len);
	leaveMutex();
	return errSuccess;
}
#endif // ndef WIN32

SharedFile::Error SharedFile::update(caddr_t address, ccxx_size_t len, off_t pos)
{
#ifdef WIN32	
	if(fd == INVALID_HANDLE_VALUE)
#else
	if(fd < 0)
#endif
		return errNotOpened;

	enterMutex();
	if(address)
		fcb.address = address;

	if(len)
		fcb.len = len;

	if(pos != -1)
		fcb.pos = pos;

#ifdef WIN32
	OVERLAPPED over;
	SetFilePointer(fd, fcb.pos, NULL, FILE_BEGIN);
	over.hEvent = 0;
	over.Offset = pos;
	over.OffsetHigh = 0;
	DWORD count;
	if(!WriteFile(fd, fcb.address, fcb.len, &count, NULL))
	{
		SetFilePointer(fd, fcb.pos, NULL, FILE_CURRENT);
		UnlockFileEx(fd, 0, len, 0, &over);
		leaveMutex();
		return errWriteFailure;
	}
	SetFilePointer(fd, fcb.pos, NULL, FILE_CURRENT);
	UnlockFileEx(fd, 0, len, 0, &over);
	leaveMutex();
	if(count < fcb.len)
		return errWriteIncomplete;

	return errSuccess;

#else
	lseek(fd, fcb.pos, SEEK_SET);
	int io = ::write(fd, fcb.address, fcb.len);
	lockf(fd, F_ULOCK, fcb.len);
	leaveMutex();

	if((size_t) io == fcb.len)
		return errSuccess;

	if(io > -1)
		return errWriteIncomplete;

	switch(errno)
	{
	case EINTR:
		return errWriteInterrupted;
	default:
		return errWriteFailure;
	}
#endif // WIN32
}

SharedFile::Error SharedFile::append(caddr_t address, ccxx_size_t len)
{
#ifdef WIN32
	if(fd == INVALID_HANDLE_VALUE)
#else
	if(fd < 0)
#endif
		return errNotOpened;

	enterMutex();
	if(address)
		fcb.address = address;

	if(len)
		fcb.len = len;

#ifdef WIN32
	fcb.pos = SetFilePointer(fd, 0l, NULL, FILE_END);
	OVERLAPPED over;
	over.hEvent = 0;
	over.Offset = fcb.pos;
	over.OffsetHigh = 0;
	LONG eof = fcb.pos;
	LockFileEx(fd, LOCKFILE_EXCLUSIVE_LOCK, 0, 0x7fffffff, 0, &over);
	fcb.pos = SetFilePointer(fd, 0l, NULL, FILE_END);
	DWORD count;
	if(!WriteFile(fd, fcb.address, fcb.len, &count, NULL))
	{
		SetFilePointer(fd, eof, NULL, FILE_CURRENT);
		UnlockFileEx(fd, 0, 0x7fffffff, 0, &over);
		leaveMutex();
		return errWriteFailure;
	}
	SetFilePointer(fd, eof, NULL, FILE_CURRENT);
	UnlockFileEx(fd, 0, 0x7fffffff, 0, &over);
	leaveMutex();
	if(count < fcb.len)
		return errWriteIncomplete;

	return errSuccess;

#else
	fcb.pos = lseek(fd, 0l, SEEK_END);
	lockf(fd, F_LOCK, -1);
	fcb.pos = lseek(fd, 0l, SEEK_END);
	int io = ::write(fd, fcb.address, fcb.len);
	lseek(fd, fcb.pos, SEEK_SET);
	lockf(fd, F_ULOCK, -1);
	leaveMutex();

	if((size_t) io == fcb.len)
		return errSuccess;

	if(io > -1)
		return errWriteIncomplete;

	switch(errno)
	{
	case EINTR:
		return errWriteInterrupted;
	default:
		return errWriteFailure;
	}
#endif // WIN32
}

off_t SharedFile::getPosition(void)
{
	return fcb.pos;
}

bool SharedFile::operator++(void)
{
	off_t eof;

	enterMutex();
	fcb.pos += fcb.len;
#ifdef WIN32
	eof = SetFilePointer(fd, 0l, NULL, FILE_END);
#else
	eof = lseek(fd, 0l, SEEK_END);
#endif

	if(fcb.pos >= eof)
	{
		fcb.pos = eof;
		leaveMutex();
		return true;
	}
	leaveMutex();
	return false;
}

bool SharedFile::operator--(void)
{
	enterMutex();
	fcb.pos -= fcb.len;
	if(fcb.pos <= 0)
	{
		fcb.pos = 0;
		leaveMutex();
		return true;
	}
	leaveMutex();
	return false;
}

#ifndef WIN32
MappedFile::MappedFile(const char *fname, Access mode) :
RandomFile()
{
	fd = open(fname, (int)mode);
	if(fd < 0 && mode != accessReadOnly)
		fd = ::open(pathname, O_CREAT | O_RDWR | O_TRUNC,
			    (int)attrPrivate);

	if(fd < 0)
	{
		error(errOpenFailed);
		return;
	}

	switch(mode)
	{
	case O_RDONLY:
		prot = PROT_READ;
		break;
	case O_WRONLY:
		prot = PROT_WRITE;
		break;
	default:
		prot = PROT_READ | PROT_WRITE;
	}
}

MappedFile::MappedFile(const char *fname, pos_t pos, size_t len, Access mode) :
RandomFile()
{
	fd = open(fname, (int)mode);
	if(fd < 0)
	{
		error(errOpenFailed);
		return;
	}

	switch(mode)
	{
	case O_RDONLY:
		prot = PROT_READ;
		break;
	case O_WRONLY:
		prot = PROT_WRITE;
		break;
	default:
		prot = PROT_READ | PROT_WRITE;
	}

	enterMutex();
	lseek(fd, pos + len, SEEK_SET);
	fcb.address = (caddr_t)mmap(NULL, len, prot, MAP_SHARED, fd, pos);
	fcb.len = len;
	fcb.pos = pos;
	leaveMutex();
	if((caddr_t)(fcb.address) == (caddr_t)(MAP_FAILED))
	{
		close(fd);
		fd = -1;
		error(errMmapFailed);
	}
}

MappedFile::~MappedFile()
{
	final();
}

void MappedFile::release(caddr_t address, size_t len)
{
	enterMutex();
	if(address)
		fcb.address = address;

	if(len)
		fcb.len = len;

	munmap(fcb.address, fcb.len);
	leaveMutex();
}

caddr_t MappedFile::fetch(off_t pos, size_t len)
{
	enterMutex();
	fcb.len = len;
	fcb.pos = pos;
	lseek(fd, fcb.pos + len, SEEK_SET);
	fcb.address = (caddr_t)mmap(NULL, len, prot, MAP_SHARED, fd, pos);
	leaveMutex();
	return fcb.address;
}

void MappedFile::update(size_t offset, size_t len)
{
	int mode = MS_ASYNC;
	caddr_t address;

	if(flags.immediate)
		mode = MS_SYNC;

	enterMutex();
	address = fcb.address;
	address += offset;
	if(!len)
		len = fcb.len;
	leaveMutex();
	msync(address, len, mode);
}

void MappedFile::update(caddr_t address, size_t len)
{
	int mode = MS_ASYNC;
	if(flags.immediate)
		mode = MS_SYNC;

	msync(address, len, mode);
}
#endif // ndef WIN32

bool isDir(const char *path)
{
#ifdef WIN32
	DWORD attr = GetFileAttributes(path);
	if(attr == (DWORD)~0l)
		return false;

	if(attr & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;

#else
	struct stat ino;

	if(stat(path, &ino))
		return false;

	if(S_ISDIR(ino.st_mode))
		return true;

	return false;
#endif // WIN32
}

bool isFile(const char *path)
{
#ifdef WIN32
	DWORD attr = GetFileAttributes(path);
	if(attr == (DWORD)~0l)
		return false;

	if(attr & FILE_ATTRIBUTE_DIRECTORY)
		return false;

	return true;

#else
	struct stat ino;

	if(stat(path, &ino))
		return false;

	if(S_ISREG(ino.st_mode))
		return true;

	return false;
#endif // WIN32
}

#ifndef WIN32
// the Win32 version is given in line in the header
bool isDevice(const char *path)
{
	struct stat ino;

	if(stat(path, &ino))
		return false;

	if(S_ISCHR(ino.st_mode))
		return true;

	return false;
}
#endif

bool canAccess(const char *path)
{
#ifdef WIN32
	DWORD attr = GetFileAttributes(path);
	if(attr == (DWORD)~0l)
		return false;

	if(attr & FILE_ATTRIBUTE_SYSTEM)
		return false;

	if(attr & FILE_ATTRIBUTE_HIDDEN)
		return false;

	return true;

#else
	if(!access(path, R_OK))
		return true;

	return false;

#endif
}

bool canModify(const char *path)
{
#ifdef WIN32
	DWORD attr = GetFileAttributes(path);

	if(!canAccess(path))
		return false;

	if(attr & FILE_ATTRIBUTE_READONLY)
		return false;

	if(attr & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_NORMAL))
		return true;

	return false;

#else
	if(!access(path, W_OK | R_OK))
		return true;

	return false;
#endif
}

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
