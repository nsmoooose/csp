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

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

Buffer::Buffer(size_t capacity) :
	lock_head(),
	lock_tail(),
	size_head(),
	size_tail(capacity)
{
	_size = capacity;
	_used = 0;
}

int Buffer::wait(void *buf)
{
	int	rc;

	size_head.wait();
	lock_head.enterMutex();
	rc = onWait(buf);
	--_used;
	lock_head.leaveMutex();
	size_tail.post();
	return rc;	
}

int Buffer::post(void *buf)
{
	int	rc;

	size_tail.wait();
	lock_tail.enterMutex();
	rc = onPost(buf);
	++_used;
	lock_tail.leaveMutex();
	size_head.post();
	return rc;
}

int Buffer::peek(void *buf)
{
	int	rc;

	MutexLock lock(lock_head);
	rc = onPeek(buf);
	return rc;
}

FixedBuffer::FixedBuffer(size_t capacity, size_t osize) :
Buffer(capacity)
{
	objsize = osize;
	buf = new char[capacity * objsize];
	if(!buf && getException() == Thread::throwObject)
		throw(this);
#ifdef	COMMON_STD_EXCEPTION
	else if(!buf && getException() == Thread::throwException)
		throw(SyncException("fixed buffer failure"));
#endif

	head = tail = buf;
}

FixedBuffer::~FixedBuffer()
{
	if(buf)
		delete[] buf;
}

bool FixedBuffer::isValid(void)
{
	if(head && tail)
		return true;

	return false;
}

#define	MAXBUF	(buf + (getSize() * objsize))

int FixedBuffer::onPeek(void *data)
{
	memcpy(data, head, objsize);
	return objsize;
}

int FixedBuffer::onWait(void *data)
{
	memcpy(data, head, objsize);
	if((head += objsize) >= MAXBUF)
		head = buf;
	return objsize;
}

int FixedBuffer::onPost(void *data)
{
	memcpy(tail, data, objsize);
	if((tail += objsize) >= MAXBUF)
		tail = buf;
	return objsize;
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
