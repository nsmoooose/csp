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
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/thread.h>
#include "private.h"

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

#ifndef WIN32
#ifdef	CCXX_SYSV_SEMAPHORES

extern "C" {
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
};

#include <cerrno>

Semaphore::Semaphore(size_t resource)
{
#ifdef _AIX
        union semun
        {
                int val; 
                struct semid_ds *buf; 
                unsigned short *array; 
        } arg;
#else
	union semun arg;
#endif

	if((_semaphore = semget(IPC_PRIVATE, 1, 0644 | IPC_CREAT)) == -1)
	{
		if(getException() == Thread::throwObject)
			throw(this);
#ifdef	COMMON_STD_EXCEPTION
		else if(getException() == Thread::throwException)
			throw(SyncException("semaphone create failed"));
#endif
		return;
	}

	arg.val = resource;
	if(semctl(_semaphore, 0, SETVAL, arg) == -1)
	{
		if(getException() == Thread::throwObject)
			throw(this);
#ifdef	COMMON_STD_EXCEPTION
		else if(getException() == Thread::throwException)
			throw(SyncException("semaphore control failed"));
#endif
		return;
	}
}

Semaphore::~Semaphore()
{
	semctl(_semaphore, 0, IPC_RMID);
}

void Semaphore::wait(void)
{
	struct sembuf ops[] = {{0, -1, 0}};
	
	semop(_semaphore, ops, 1);
}

bool Semaphore::tryWait(void)
{
	struct sembuf ops[] = {{0, -1, IPC_NOWAIT}};
	
	return (semop(_semaphore, ops, 1) == EAGAIN) ? false : true;
}

void Semaphore::post(void)
{
	struct sembuf ops[] = {{0, 1, 0}};

	semop(_semaphore, ops, 1);
}

int Semaphore::getValue(void)
{
	return semctl(_semaphore,0,GETVAL);
}

#else // CCXX_SYSV_SEMAPHORES

Semaphore::Semaphore(size_t resource) 
{
#ifdef	CCXX_NAMED_SEMAPHORES
	static int semnum = 0;
	char name[33];

	snprintf(name, sizeof(name), "/tmp/.sem-%d-%4.4d", getpid(), semnum++);
	_semobject = sem_open(name, O_CREAT, 0600, resource);
	if(_semobject == (sem_t *)SEM_FAILED)
		_semobject = NULL;
	if(_semobject)
		sem_unlink(name);
	else
	{
#else
	_semobject = &_semaphore;
	if(sem_init(&_semaphore, 0, resource))
	{
		_semobject = NULL;
#endif
		if(getException() == Thread::throwObject)
			throw(this);
#ifdef	COMMON_STD_EXCEPTION
		else if(getException() == Thread::throwException)
			throw(SyncException("semaphore failed"));
#endif
	}
}

Semaphore::~Semaphore()
{
#ifdef	CCXX_NAMED_SEMAPHORES
	if(_semobject)
		sem_close(_semobject);
#else
	if(_semobject)
		sem_destroy(&_semaphore);
#endif
}

void Semaphore::wait(void)
{
	if(_semobject)
		sem_wait(_semobject);
}

void Semaphore::post(void)
{
	if(_semobject)
		sem_post(_semobject);
}

bool Semaphore::tryWait(void)
{
	if(!_semobject)
		return false;
	return (sem_trywait(_semobject) == 0) ? true : false;
}

#ifndef __CYGWIN32__
int Semaphore::getValue(void)
{
	int value;

	if(!_semobject)
		return 0;

	sem_getvalue(_semobject, &value);
	return value;
}
#endif

#endif //CCXX_SYSV_SEMAPHORES

#else

Semaphore::Semaphore(size_t resource)
{
	semObject = ::CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, resource, MAX_SEM_VALUE, (LPCTSTR)NULL);
}	

Semaphore::~Semaphore()
{
	::CloseHandle(semObject);
}

void Semaphore::wait(void)
{
	waitThread(semObject, INFINITE);
}

bool Semaphore::tryWait(void)
{
	return waitThread(semObject, 0) == WAIT_OBJECT_0;
}

void Semaphore::post(void)
{
	::ReleaseSemaphore(semObject, 1, (LPLONG)NULL);
}


#endif //WIN32

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
