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
#include <cc++/groups.h>
#include <ctime>
#include <cstdio>

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

TimedGroup *TimedGroup::firstMember = NULL;
ThreadGroup *ThreadGroup::firstMember = NULL;
PluginGroup *PluginGroup::firstMember = NULL;

PluginGroup::PluginGroup(PluginGroup **group, const char *id, bool flag) :
DSO(id, flag)
{
	nextMember = *group;
	*group = this;
}

PluginGroup::PluginGroup(const char *id, bool flag) :
DSO(id, flag)
{
	nextMember = firstMember;
	firstMember = this;
}

void loadPlugins(PluginGroup *group, const char *path, const char *ext, bool resolve)
{
	Dir dir(path);
	char buffer[256];
	const char *name;
	const char *tail;

	if(*ext == '.')
		++ext;

	while(NULL != (name = dir.getName()))
	{
		tail = strrchr(name, '.');
		if(!tail)
			continue;

		// FIXME: define another function name as filenamecmp
#ifdef WIN32
		// win32 use insensitive filename
		if(stricmp(++tail, ext))
#else
		if(strcmp(++tail, ext))
#endif
			continue;

		// FIXME: bad fix, best define new function
		// FIXME: not correct if buffer exceed
		// FIXME: win32 use \\ instead of /
#ifdef _MSC_VER
		_snprintf(buffer, 256, "%s\\%s", path, name);
#else
# ifdef HAVE_SNPRINTF
		snprintf(buffer, 256, "%s/%s", path, name);
# else
		strncpy(buffer,path,256);
		strncat(buffer,"/",256);
		strncat(buffer,name,256);
		buffer[255] = 0;
# endif
#endif
		buffer[255] = 0; // assert to terminate (not in all implementation)
		new PluginGroup(&group, buffer, resolve);
	}	
}

void removePlugins(PluginGroup *group)
{
	PluginGroup *next;

	while(group)
	{
		next = group->nextMember;
		delete group;
		group = next;
	}
}
	
TimedGroup::TimedGroup(TimedGroup **group)
{
	if(group)
		group = &firstMember;

	nextMember = *group;
	*group = this;
	nextRun = 0;
}

bool TimedGroup::isRunnable(void)
{
	time_t now;

	if(nextRun == (time_t)-1)
		return false;

	if(!nextRun)
	{
		initial();
		time(&nextRun);
	}

	time(&now);
	if(now < nextRun)
		return false;

	return true;
}

void TimedGroup::stop(void)
{
	if(!nextRun)
		initial();

	nextRun = (time_t)-1;
}

void TimedGroup::start(void)
{
	if(!nextRun)
		initial();

	time(&nextRun);
}

void runTimed(TimedGroup *group)
{
	if(!group)
		group = TimedGroup::firstMember;

	while(group)
	{
		if(group->isRunnable())
			group->run();
		group = group->nextMember;
	}
}

ThreadGroup::ThreadGroup(int pri, unsigned stack) :
Thread(pri, stack)
{
	nextMember = firstMember;
	firstMember = this;
}

ThreadGroup::ThreadGroup(ThreadGroup **grp, int pri, unsigned stack) :
Thread(pri, stack)
{
	nextMember = *grp;
	*grp = this;
}

void startThreads(ThreadGroup *base)
{
	if(!base)
		base = ThreadGroup::firstMember;

	while(base)
	{
		base->start();
		base = base->nextMember;
	}
}

void stopThreads(ThreadGroup *base)
{
	if(!base)
		base = ThreadGroup::firstMember;

	while(base)
	{
		base->stop();
		base = base->nextMember;
	}
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
