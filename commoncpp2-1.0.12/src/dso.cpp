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
#include <cc++/file.h>
#include "private.h"

using std::string;

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

#ifdef	HAVE_MODULES
DSO	*DSO::first = NULL;
DSO	*DSO::last = NULL;
Mutex	DSO::mutex;

#if defined(HAVE_DLFCN_H)

extern "C" {
#include <dlfcn.h>
}

#ifndef	RTLD_GLOBAL
#define	RTLD_GLOBAL	0
#endif

#endif // HAVE_DLFN_H

#ifdef	HAVE_SHL_LOAD
#include <dl.h> 
#endif

void DSO::dynunload(void)
{
	while(DSO::last)
		delete DSO::last;
}


DSO::~DSO()
{
#if defined(HAVE_MACH_DYLD)
	NSSymbol sym;
	void (*fini)(void);
#endif
	MutexLock lock(mutex);
#if defined(WIN32)
	if(hImage)
		FreeLibrary(hImage);
#elif defined(HAVE_MACH_DYLD)
	if(oModule == NULL)
		return;

	sym = NSLookupSymbolInModule(oModule, "__fini");
	if(sym != NULL)
	{
		fini = (void (*)(void))NSAddressOfSymbol(sym);
		fini();
	}

	NSUnLinkModule(oModule, NSUNLINKMODULE_OPTION_NONE);		
#elif defined(HAVE_SHL_LOAD)
	if(image)
		shl_unload((shl_t)image);
#else
	if(image)
		dlclose(image);
#endif

	if(first == this && last == this)
		first = last = NULL;

	if(!next && !prev)
		return;

	if(prev)
		prev->next = next;

	if(next)
		next->prev = prev;
	
	if(first == this)
		first = next;
	if(last == this)
		last = prev;
}

void DSO::loader(const char *filename, bool flag)
{
#if defined(HAVE_MACH_DYLD)
	NSObjectFileImage oImage;
	NSSymbol sym = NULL;
	void (*init)(void);
#endif

	next = prev = NULL;

#if defined(WIN32)
	hImage = LoadLibrary(filename);
	err = "none";
#elif defined(HAVE_MACH_DYLD)
	err = "none";
	oModule = NULL;

	switch(NSCreateObjectFileImageFromFile(filename, &oImage)) 
	{
	case NSObjectFileImageSuccess:
		break;
	default:
		err = "unknown error";
		return;
	}
	if(flag)
		oModule = NSLinkModule(oImage, filename, NSLINKMODULE_OPTION_BINDNOW | NSLINKMODULE_OPTION_RETURN_ON_ERROR);
	else
		oModule = NSLinkModule(oImage, filename, NSLINKMODULE_OPTION_RETURN_ON_ERROR); 
	NSDestroyObjectFileImage(oImage);
	if(oModule != NULL)
		sym = NSLookupSymbolInModule(oModule, "__init");
	if(sym)
	{
		init = (void (*)(void))NSAddressOfSymbol(sym);
		init();
	}

#elif defined(HAVE_SHL_LOAD)
	err = "none";
	if(flag)
		image = (void *)shl_load(filename, BIND_IMMEDIATE, 0L);
	else
		image = (void *)shl_load(filename, BIND_DEFERRED, 0L); 
#else
	if(flag)
		image = dlopen(filename, RTLD_NOW | RTLD_GLOBAL);
	else
		image = dlopen(filename, RTLD_LAZY | RTLD_GLOBAL);
#endif

#if defined(WIN32)
	if(!hImage)
	{
		err = "load failed";
#elif defined(HAVE_MACH_DYLD)
	if(oModule == NULL)
	{
		err = "load failed";
#elif defined(HAVE_SHL_LOAD)
	if(!image)
	{
		err = "load failed";
#else
	if(!image)
	{
#endif

		if(getException() == Thread::throwObject)
			throw(this);
#ifdef	COMMON_STD_EXCEPTION
		else if(getException() == Thread::throwException)
			throw(DSOException(string(filename) + 
#if defined(WIN32) || defined(HAVE_MACH_DYLD) || defined(HAVE_SHL_LOAD)
					   err));
#else
					   dlerror()));
#endif //WIN32

#endif
		return;
	}

	if(!last)
	{
		last = first = this;
		return;
	}

	mutex.enterMutex();
	last->next = this;
	prev = last;
	last = this;
	mutex.leaveMutex();
}	


bool DSO::isValid(void)
{
#if	defined(WIN32)
	if(!hImage)
#elif	defined(HAVE_MACH_DYLD)
	if(oModule == NULL)
#else
	if(!image)
#endif
		return false;

	return true;
}

void *DSO::operator[](const char *sym)
{
#if   defined(HAVE_SHL_LOAD)
	int value;
	shl_t handle = (shl_t)image;

	if(shl_findsym(&handle, sym, 0, &value) == 0)
		return (void *)value;
	else
		return NULL;
#elif defined(HAVE_MACH_DYLD)
	NSSymbol oSymbol = NSLookupSymbolInModule(oModule, sym);
	if(oSymbol)
		return NSAddressOfSymbol(oSymbol);
	else
		return NULL;
#elif defined(WIN32)
	void *addr = (void *)GetProcAddress(hImage, sym);
	if(!addr)
		err = "symbol missing";

	return addr;
#else
	return dlsym(image, (char *)sym);
#endif
}

char *DSO::getError(void)
{
#if	defined(HAVE_SHL_LOAD) || defined(HAVE_MACH_DYLD) || defined(WIN32)
	return err;
#else
	return (char *)dlerror();
#endif
}

#endif

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
