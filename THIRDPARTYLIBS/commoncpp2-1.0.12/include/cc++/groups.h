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

/**
 * @file groups.h
 * @short Groups of plugins, scheduled entities and thread.
 **/

#ifndef	CCXX_GROUPS_H_
#define	CCXX_GROUPS_H_

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#ifndef CCXX_THREAD_H_
#include <cc++/thread.h>
#endif

#ifndef	CCXX_FILE_H_
#include <cc++/file.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

/**
 * A plugin group is used to manage a chain of associated plugins.
 *
 * @short Plugin grouping class.
 * @author David Sugar <dyfet@ostel.com>
 */
class PluginGroup : public DSO
{
private:
	static PluginGroup *firstMember;
	PluginGroup *nextMember;

	friend CCXX_EXPORT(void) removePlugins(PluginGroup *group = NULL);
	friend CCXX_EXPORT(void) loadPlugins(PluginGroup *group, const char *path, const char *ext, bool flag);
	friend inline void loadPlugins(const char *path, const char *ext, bool flag)
		{loadPlugins(PluginGroup::firstMember, path, ext, flag);};
		

public:
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) PluginGroup(const char *path, bool resolve = true);
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) PluginGroup(PluginGroup **grp, const char *path, bool resolve = true);
};

/**
 * A timed group is a scheduled entity that involves things that must
 * be processed on some kind of interval.
 *
 * @short Timed grouping class for server processes.
 * @author David Sugar <dyfet@ostel.com>
 */
class TimedGroup
{
private:
	static TimedGroup *firstMember;
	TimedGroup *nextMember;
#ifdef __BORLANDC__
	std::time_t nextRun;
#else
	time_t nextRun;
#endif

	friend CCXX_EXPORT(void) runTimed(TimedGroup *grp = NULL);

protected:
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) TimedGroup(TimedGroup **group = NULL);

	virtual ~TimedGroup() {};

	CCXX_MEMBER_EXPORT(bool) isRunnable(void);

	virtual void initial(void)
		{return;};

	virtual void run(void) = 0;

public:
	CCXX_MEMBER_EXPORT(void) stop(void);
	CCXX_MEMBER_EXPORT(void) start(void);
};

/**
 * A thread grouping class for a set of server classes that may be started
 * and stopped together as a common group.
 *
 * @short Thread grouping class for server processes.
 * @author David Sugar <dyfet@ostel.com>
 */
class ThreadGroup : public Thread
{
private:
	static ThreadGroup *firstMember;
	ThreadGroup *nextMember;

	/**
	 * Start a group of threads together.
	 */
	friend CCXX_EXPORT(void) startThreads(ThreadGroup *group = NULL);
	friend CCXX_EXPORT(void) stopThreads(ThreadGroup *group = NULL);

protected:
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) ThreadGroup(int pri, unsigned stack);
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) ThreadGroup(ThreadGroup **group, int pri, unsigned stack);

	virtual ~ThreadGroup()
		{terminate();};

	virtual void stop(void)
		{terminate();};

	virtual void start(void)
		{start();};
};

#ifdef	CCXX_NAMESPACES
};
#endif

#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
