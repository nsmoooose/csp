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
#include "private.h"

#ifdef WIN32
#include <malloc.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

Dir::Dir(const char *fname)
#ifdef WIN32
	: 	hDir(INVALID_HANDLE_VALUE), name(NULL)
#endif
{
#ifdef WIN32
	DWORD attr = GetFileAttributes(fname);
	if( (attr == (DWORD)~0l) || !(attr & FILE_ATTRIBUTE_DIRECTORY) )
	{
		if(getException() == Thread::throwObject)
			throw(this);
#ifdef	COMMON_STD_EXCEPTION
		else if(getException() == Thread::throwException)
			throw(DirException(std::string(fname) + ": failed"));
#endif
	}

	char* path = (char*)alloca(strlen(fname)+4);
	if (!path && getException() == Thread::throwObject)
		throw(this);
#ifdef	COMMON_STD_EXCEPTION
	else if(!path && getException() == Thread::throwException)
		throw(DirException(std::string(fname) + ": failed"));
#endif

	strcpy(path, fname);
	strcat(path, "\\*");
	hDir = FindFirstFile(path, &data);
	if(hDir != INVALID_HANDLE_VALUE)
		name = data.cFileName;

#else // WIN32
	dir = opendir(fname);
	if(!dir && getException() == Thread::throwObject)
		throw(this);
#ifdef	COMMON_STD_EXCEPTION
	else if(!dir && getException() == Thread::throwException)
		throw(DirException(std::string(fname) + ": failed"));
#endif
#endif // WIN32
}
	
Dir::~Dir()
{
#ifdef WIN32
	if(hDir != INVALID_HANDLE_VALUE)
		FindClose(hDir);
#else
	if(dir)
		closedir(dir);
#endif
}

bool Dir::isValid(void)
{
#ifdef WIN32
	if(hDir == INVALID_HANDLE_VALUE)
#else
	if(!dir)
#endif
		return false;

	return true;
}

char *Dir::getName(void)
{
#ifdef WIN32
	char *retname = name;
	
	if(retname)
	{
		name = NULL;
		if(FindNextFile(hDir, &data))
			name = data.cFileName;
	}
	return retname;

#else
	struct dirent *entry;

	if(!dir)
		return NULL;

	entry = readdir(dir);
	if(!entry)
		return NULL;

	return entry->d_name;
#endif // WIN32
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
