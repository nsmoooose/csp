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
#include <cc++/misc.h>
#include <cc++/file.h>
#include "private.h"
#include <cstdlib>
#include <cstdio>

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

int Keydata::count = 0;
int Keydata::sequence = 1;
ifstream *Keydata::cfgFile = NULL;
char Keydata::lastpath[KEYDATA_PATH_SIZE + 1];

void endKeydata();

Keydata::Keydata() : 
MemPager(KEYDATA_PAGER_SIZE)
{
	link = 0;
	memset(&keys, 0, sizeof(keys)); 
}

Keydata::Keydata(const char *path, const char *environment) :
MemPager(KEYDATA_PAGER_SIZE)
{
	link = 0;
	memset(&keys, 0, sizeof(keys));
	load(path, environment);
}

Keydata::~Keydata()
{
	unlink();
	if(count < 1)
		endKeydata();
}

Keydata::Keysym *Keydata::getSymbol(const char *sym, bool create)
{
	unsigned path = getIndex(sym);
	Keysym *key = keys[path];

	while(key)
	{
		if(!stricmp(sym, key->sym))
			return key;
		key = key->next;
	}
	if(!create)
		return NULL;

	key = (Keysym *)alloc(sizeof(Keysym) + strlen(sym));
	strcpy(key->sym, sym);
	key->count = 0;
	key->next = keys[path];
	key->data = NULL;
	key->list = NULL;
	keys[path] = key;
	return key;
}

unsigned Keydata::getIndex(const char *str)
{
	unsigned key = 0;

	while(*str)
		key = (key << 1) ^ *(str++);

	return key % KEYDATA_INDEX_SIZE;
}

int Keydata::getCount(const char *sym)
{
	Keysym *key = getSymbol(sym, false);
	if(!key)
		return 0;

	return key->count;
}

const char *Keydata::getFirst(const char *sym)
{
	Keysym *key = getSymbol(sym, false);
	Keyval *val;

	if(!key)
		return NULL;

	val = key->data;
	if(!val)
		return NULL;

	while(val->next)
		val = val->next;

	return val->val;
}

const char *Keydata::getLast(const char *sym)
{
	Keysym *key = getSymbol(sym, false);
	if(!key)
		return NULL;

	if(!key->data)
		return NULL;

	return key->data->val;
}

unsigned Keydata::getCount(void)
{
	unsigned icount = 0;
	Keysym *key;
	int idx;

	for(idx = 0; idx < KEYDATA_INDEX_SIZE; ++idx)
	{
		key = keys[idx];
		while(key)
		{
			++icount;
			key = key->next;
		}
	}
	return icount;
}

unsigned Keydata::getIndex(char **data, unsigned max)
{
	int idx;
	Keysym *key;
	unsigned icount = 0;

	for(idx = 0; idx < KEYDATA_INDEX_SIZE; ++idx)
	{
		if(icount >= max)
			break;
		key = keys[idx];
		while(key && icount < max)
		{
			*(data++) = key->sym;
			++icount;
			key = key->next;
		}
	}
	*data = NULL;
	return icount;
}

void Keydata::setValue(const char *sym, const char *data)
{
	Keysym *key = getSymbol(sym, true);
	Keyval *val;

	if(!data)
		data = "";

	val = (Keyval *)alloc(sizeof(Keyval) + strlen(data));
	++key->count;
	key->list = NULL;
	val->next = key->data;
	key->data = val;
	strcpy(val->val, data);
}

const char *const *Keydata::getList(const char *sym)
{
	int icount;
	Keysym *key = getSymbol(sym, false);
	Keyval *data;

	if(!key)
		return NULL;

	icount = key->count;
	if(!icount)
		return NULL;

	++icount;
	if(!key->list)
	{
		key->list =(const char **)first(sizeof(const char**) * icount);
		key->list[--icount] = NULL;
		data = key->data;
		while(icount && data)
		{
			key->list[--icount] = data->val;
			data = data->next;
		}
		while(icount)
			key->list[--icount] = "";
	}
	return key->list;
}

void Keydata::clrValue(const char *sym)
{
	Keysym *key = getSymbol(sym, false);
	if(!key)
		return;

	key->count = 0;
	key->list = NULL;
	key->data = NULL;
}

void Keydata::load(Define *defs)
{
	Keysym *key;

	while(defs->keyword)
	{
		key = getSymbol(defs->keyword, true);
		if(!key->data)
			setValue(defs->keyword, defs->value);
		++defs;
	}
}

void Keydata::load(const char *keypath, const char *environment)
{
	loadPrefix(NULL, keypath, environment);
}

void Keydata::loadPrefix(const char *pre, const char *keypath, const char *environment)
{
	// FIXME: use string of dinamic allocation
	char path[512];
	char buffer[256];
	char seek[33];
	char find[33];
	const char *prefix = NULL;
	char *cp, *ep;
	int fpos;

	// FIXME: win32 do not use HOME environment
	// FIXME: win32 use \\ instead of / for directory separator
	if(*keypath == '~')
	{
		prefix = getenv("HOME");
		strncpy(path, prefix, 507); // 512 - "/." - "rc" - '\0'
		strcat(path, "/.");
		++keypath;
	}

	if(!prefix)
	{
		prefix = getenv(environment);
		if(!prefix)
			prefix = ETC_PREFIX;
		strncpy(path, prefix, 506); // 512 - ".conf" - '\0'
		prefix = NULL;
	}

	if(*keypath == '/')
		++keypath;

	strcat(path, keypath);
	cp = strrchr(path, '/');

	strncpy(seek, cp + 1, 32);
	seek[32] = 0;
	*cp = 0;

	if(!prefix)
		strcat(path, ".conf");
	else
		strcat(path, "rc");

	if(strcmp(path, lastpath))
	{
		endKeydata();
		if(canAccess(path))
			cfgFile->open(path, ios::in);
		else if(canAccess(path + 5) && !prefix)
			cfgFile->open(path + 5, ios::in);
		else
			return;
		if(!cfgFile->is_open())
			return;
		strcpy(lastpath, path);
	}

	if(link != sequence)
	{
		link = sequence;
		++count;
	}

	find[0] = 0;
	cfgFile->seekg(0);
	while(stricmp(seek, find))
	{
		cfgFile->getline(path, sizeof(path) - 1);
		if(cfgFile->eof())
		{
			lastpath[0] = 0;
			cfgFile->close();
			cfgFile->clear();
			return;
		}

		cp = path;
		while(*cp == ' ' || *cp == '\n' || *cp == '\t')
			++cp;

		if(*cp != '[')
			continue;

		ep = strchr(cp, ']');
		if(ep)
			*ep = 0;
		else
			continue;

		strncpy(find, ++cp, 32);
		find[32] = 0;
	}

	for(;;)
	{
		if(cfgFile->eof())
		{
			lastpath[0] = 0;
			cfgFile->close();
			cfgFile->clear();
			return;
		}

		cfgFile->getline(path, sizeof(path) - 1);

		cp = path;
		while(*cp == ' ' || *cp == '\t' || *cp == '\n')
			++cp;

		if(!*cp || *cp == '#' || *cp == ';' || *cp == '!')
			continue;

		if(*cp == '[')
			return;

		fpos = 0;
		while(*cp && *cp != '=')
		{
			if(*cp == ' ' || *cp == '\t')
			{
				++cp;
				continue;
			}
			find[fpos] = *(cp++);
			if(fpos < 32)
				++fpos;
		}
		find[fpos] = 0;
		if(*cp != '=')
			continue;

		++cp;
		while(*cp == ' ' || *cp == '\t' || *cp == '\n')
			++cp;

		ep = cp + strlen(cp);
		while((--ep) > cp)
		{
			if(*ep == ' ' || *ep == '\t' || *ep == '\n')
				*ep = 0;
			else
				break;
		}

		if(*cp == *ep && (*cp == '\'' || *cp == '\"'))
		{
			++cp;
			*ep = 0;
		}

		if(pre)
		{
#ifdef HAVE_SNPRINTF
			snprintf(buffer, 256, "%s.%s", pre, find);
#else
			strncpy(buffer,pre,256);
			strncat(buffer,".",256);
			strncat(buffer,find,256);
			buffer[255] = 0;
#endif
			setValue(buffer, cp);
		}
		else
			setValue(find, cp);
	}
}

void Keydata::unlink(void)
{
	if(link != sequence)
	{
		link = 0;
		return;
	}
	
	link = 0;
	--count;
}
			
void endKeydata(void)
{
	Keydata::count = 0;
	++Keydata::sequence;
	if(!Keydata::sequence)
		++Keydata::sequence;

	Keydata::lastpath[0] = 0;
	if(!Keydata::cfgFile)
		Keydata::cfgFile = new std::ifstream();
	
	Keydata::cfgFile->close();
	Keydata::cfgFile->clear();
}

// sorted by the usual probability of occurence
// see also: manpage of isspace()
const char * const StringTokenizer::SPACE=" \t\n\r\f\v";

StringTokenizer::StringTokenizer (const char *_str,
				  const char *_delim,
				  bool _skipAll,
				  bool _trim) 
	: str(_str),delim(_delim),skipAll(_skipAll),trim(_trim) {
	if (str == 0)
		itEnd = iterator(*this, 0);
	else
		itEnd = iterator(*this,strchr(str, '\0')+1);
}

StringTokenizer::StringTokenizer (const char *s) 
	: str(s), delim(SPACE), skipAll(false),trim(true) {
	if (str == 0)
		itEnd = iterator(*this, 0);
	else
		itEnd = iterator(*this,strchr(str, '\0')+1);
}


StringTokenizer::iterator& StringTokenizer::iterator::operator ++ () 
	THROWS (StringTokenizer::NoSuchElementException) {

	// someone requested to read beyond the end .. tsts
	if (endp == myTok->itEnd.endp) 
		THROW (NoSuchElementException());

	if (token) { 
		// this is to help people find their bugs, if they
		// still maintain a pointer to this invalidated
		// area :-)
		*token = '\0'; 
		delete[] token; 
		token = 0;
	}

	start = ++endp;
	if (endp == myTok->itEnd.endp) return *this; // done

	// search for next delimiter
	while (*endp && strchr(myTok->delim, *endp)==NULL)
		++endp;

	tokEnd = endp;
	
	if (*endp && myTok->skipAll) { // skip all delimiters
		while (*(endp+1) && strchr(myTok->delim, *(endp+1)))
			++endp;
	}
	return *this;
}

/*
 * if no one requests the token, no time is spent skipping the whitespaces
 * or allocating memory.
 */
const char * StringTokenizer::iterator::operator * ()
	THROWS (StringTokenizer::NoSuchElementException) {
	// someone requested to read beyond the end .. tsts
	if (endp == myTok->itEnd.endp)
		THROW (NoSuchElementException());

	if (!token) {
		/*
		 * someone requests this token; return a copy to provide
		 * a NULL terminated string.
		 */

		/* don't clobber tokEnd, it is used in nextDelimiter() */
		const char *wsTokEnd = tokEnd;
		if (myTok->trim) {
			while (wsTokEnd > start && strchr(SPACE, *start))
				++start;
			while (wsTokEnd > start && strchr(SPACE,*(wsTokEnd-1)))
				--wsTokEnd;
		}
		size_t tokLen = wsTokEnd - start;
		if (start > wsTokEnd) {
			tokLen = 0;
		}
		token = new char [ tokLen + 1 ];
		strncpy (token, start, tokLen);
		token [ tokLen ] = '\0';
	}
	return token;
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
