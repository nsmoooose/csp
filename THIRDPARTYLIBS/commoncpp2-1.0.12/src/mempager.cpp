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
#include <cc++/misc.h>
#include <cc++/slog.h>
#include "private.h"

#ifdef	CCXX_NAMESPACES
namespace ost {
using std::endl;
#endif

MemPager::MemPager(int pg)
{
	pagesize = pg;
	pages = 1;
	page = (struct _page *)::new char[pagesize];
	page->next = NULL;
	page->used = sizeof(struct _page);
#ifdef	COMMON_MEMORY_AUDIT
	slog(Slog::levelDebug) << "MemPager: creating pool, id=" << this << endl;
	slog(Slog::levelDebug) << "MemPager: alloc, id="
		<< this << " page=" << page << endl;
#endif
}

MemPager::~MemPager()
{
	struct _page *root = page;

	while(root)
	{
		page = page->next;
#ifdef	COMMON_MEMORY_AUDIT
		slog(Slog::levelDebug) << "MemPager: delete, id="
			<< this << " page=" << root << endl;
#endif
		::delete[] root;
		root = page;
	}
#ifdef	COMMON_MEMORY_AUDIT
	slog(Slog::levelDebug) << "Mempager: destroy pool, id=" << this << endl;
#endif
}

void MemPager::purge(void)
{
	struct _page *root = page;

	while(root->next)
	{
		page = root->next;
#ifdef	COMMON_MEMORY_AUDIT
		slog(Slog::levelDebug) << "Mempager: delete, id="
			<< this << " page=" << root << endl;
#endif
		::delete[] root;
		--pages;
		root = page;
	}
	page->used = sizeof(struct _page);
}
	

void *MemPager::alloc(size_t size)
{
	char *ptr;
	struct _page *npage;
	if(page->used + size > pagesize)
	{
		npage = (struct _page *) ::new char[pagesize];
#ifdef	COMMON_MEMORY_AUDIT
		slog(Slog::levelDebug) << "MemPager: alloc, id="
			<< this << " page=" << npage << endl;
#endif
		npage->next = page;
		npage->used = sizeof(struct _page);
		page = npage;
		++pages;
	}
	ptr = (char *)page;
	ptr += page->used;
	page->used += size;
	return (void *)ptr; 
}

void *MemPager::first(size_t size)
{
	struct _page *npage = page;
	char *ptr;

	while(npage)
	{
		if(npage->used + size <= pagesize)
			break;

		npage = npage->next;
	}
	if(!npage)
		return alloc(size);

	ptr = (char *)npage;
	ptr += npage->used;
	npage->used += size;
	return (void *)ptr;
}

char *MemPager::alloc(char *str)
{
	char *cp = (char *)alloc(strlen(str) + 1);
	strcpy(cp, str);
	return cp;
}

char *MemPager::first(char *str)
{
	char *cp = (char *)first(strlen(str) + 1);
	strcpy(cp, str);
	return cp;
}

SharedMemPager::SharedMemPager(int pg) :
MemPager(pg), Mutex()
{};

void SharedMemPager::purge(void)
{
	enterMutex();
	MemPager::purge();
	leaveMutex();
}

void *SharedMemPager::first(size_t size)
{
	void *mem;

	enterMutex();
	mem = MemPager::first(size);
	leaveMutex();
	return mem;
}

void *SharedMemPager::alloc(size_t size)
{
	void *mem;

	enterMutex();
	mem = MemPager::alloc(size);
	leaveMutex();
	return mem;
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
