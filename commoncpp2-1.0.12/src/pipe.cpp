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

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/file.h>

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

#undef IS_INVALID_FD
#undef INVALID_FD
#ifdef WIN32
# define IS_INVALID_FD(f) (!(f))
# define INVALID_FD 0
# undef close
# define close CloseHandle
#else
# define IS_INVALID_FD(f) ( (f) < 0)
# define INVALID_FD -1
#endif

Pipe::Pipe(int size, int count)
{
	objsize = size;
	objcount = count;

#ifdef WIN32
	if(!CreatePipe(&fd[0], &fd[1], NULL, 0))
		throw(this);
#else
	if(pipe(fd))
	{
		if(getException() == Thread::throwObject)
			throw this;
#ifdef	COMMON_STD_EXCEPTION
		else if(getException() == Thread::throwException)
			throw(PipeException("pipe failed"));
#endif
		fd[0] = -1;
		fd[1] = -1;
	}
#endif
}

Pipe::Pipe(const Pipe &p)
{
#ifndef WIN32
	fd[0] = dup(p.fd[0]);
	fd[1] = dup(p.fd[1]);
#else
	HANDLE pidHandle = GetCurrentProcess();

	if(!DuplicateHandle(pidHandle, p.fd[0], pidHandle, &fd[0], 0, FALSE, DUPLICATE_SAME_ACCESS))
		fd[0] = NULL;

	if(!DuplicateHandle(pidHandle, p.fd[1], pidHandle, &fd[1], 0, FALSE, DUPLICATE_SAME_ACCESS))
		fd[1] = NULL;
#endif

	objsize = p.objsize;
	objcount = p.objcount;

	if( IS_INVALID_FD(fd[0]) || IS_INVALID_FD(fd[1]) )
	{
#ifndef WIN32
		if(getException() == Thread::throwObject)
#endif
			throw this;
	}
}

Pipe::~Pipe()
{
	if(!isValid())
		return;
	close(fd[0]);
	close(fd[1]);
}

Pipe &Pipe::operator=(const Pipe &p)
{
	close(fd[0]);
	close(fd[1]);

#ifndef WIN32
	fd[0] = dup(p.fd[0]);
	fd[1] = dup(p.fd[1]);
#else
	HANDLE pidHandle = GetCurrentProcess();

	if(!DuplicateHandle(pidHandle, p.fd[0], pidHandle, &fd[0], 0, FALSE, DUPLICATE_SAME_ACCESS))
		fd[0] = NULL;

 	if(!DuplicateHandle(pidHandle, p.fd[1], pidHandle, &fd[1], 0, FALSE, DUPLICATE_SAME_ACCESS))
		fd[1] = NULL;
#endif

	objsize = p.objsize;
	objcount = p.objcount;

	if( IS_INVALID_FD(fd[0]) || IS_INVALID_FD(fd[1]) )
	{
#ifndef WIN32
		if(getException() == Thread::throwObject)
#endif
			throw this;
	}

	return *this;
}

#ifdef WIN32
int Pipe::send(void *addr)
{
	DWORD count;

	if(!WriteFile(fd[1], addr, (DWORD)objsize, &count, NULL))
		return -1;

	return (int)count;
}

int Pipe::receive(void *addr)
{
	DWORD count;

	if(!ReadFile(fd[0], addr, (DWORD)objsize, &count, NULL))
		return -1;

	return (int)count;
}
#endif

bool Pipe::operator!()
{
	if( IS_INVALID_FD(fd[0]) && IS_INVALID_FD(fd[1]) )
		return true;
	return false;
}

bool Pipe::isValid(void)
{
	if( IS_INVALID_FD(fd[0]) || IS_INVALID_FD(fd[1]) )
		return false;

	return true;
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
