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

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/file.h>

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>

using std::ios;
using std::fstream;

#ifndef WIN32

fifostream::fifostream() :
fstream()
{
	pathname = NULL;
}

fifostream::fifostream(const char *fname, long access) :
fstream()
{
	pathname = NULL;
	open(fname, access);
}

fifostream::~fifostream()
{
	close();
}

void fifostream::close(void)
{
	fstream::close();
	if(pathname)
	{
		remove(pathname);
		delete[] pathname;
		pathname = NULL;
	}
}

void fifostream::open(const char *fname, long access)
{
	if(pathname)
		close();

	remove(fname);
	if(mkfifo(fname, access))
		return;

	fstream::open(fname, ios::in | ios::out);

	if(!*this)
		return;

	pathname = new char[strlen(fname) + 1];
	strcpy(pathname, fname);
}

FIFOSession::FIFOSession(const char *fname, long access, 
			 int pri, int stack) :
	Thread(pri, stack),
	fstream()
{
	pathname = NULL;
	remove(fname);

	if(mkfifo(fname, access))
	{
		if(getException() == Thread::throwObject)
			throw(this);
#ifdef	COMMON_STD_EXCEPTION
		else if(getException() == Thread::throwException)
			throw(FIFOException(std::string(fname) + ": create failed"));
#endif
		return;
	}

	open(fname, ios::in | ios::out);
	if(!*this)
	{
		if(getException() == throwObject)
			throw(this);
#ifdef	COMMON_STD_EXCEPTION
		else if(getException() == Thread::throwException)
			throw(FIFOException(std::string(fname) + ": open failed"));
#endif
		return;
	}

	pathname = new char[strlen(fname) + 1];
	strcpy(pathname, fname);
}

FIFOSession::~FIFOSession()
{
	terminate();
	fstream::close();
	if(pathname)
	{
		remove(pathname);	
		delete[] pathname;
	}
}

#endif // ndef WIN32

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
