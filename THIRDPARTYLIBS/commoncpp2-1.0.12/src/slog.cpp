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
#include <cc++/slog.h>
#include "../src/private.h"

#ifdef	HAVE_SYSLOG_H
#include <syslog.h>
#endif

using std::streambuf;
using std::ofstream;
using std::ostream;
using std::clog;
using std::endl;
using std::ios;

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

Slog slog;

Slog::Slog(void) :
streambuf()
#ifdef	HAVE_OLD_IOSTREAM
,ostream()
#else
,ostream((streambuf *)this)
#endif
{
#ifdef	HAVE_OLD_IOSTREAM
	init((streambuf *)this);
#endif
	_enable = true;
	_level = levelDebug;
        _clogEnable = true;
}

Slog::~Slog(void)
{
#ifdef	HAVE_SYSLOG_H
	closelog();
#else
	syslog.close();
#endif
}

void Slog::close(void)
{
#ifdef	HAVE_SYSLOG_H
	closelog();
#else
	lock.enterMutex();
	syslog.close();
	lock.leaveMutex();
#endif
}

void Slog::open(const char *ident, Class grp)
{
	const char *cp;

#ifdef	HAVE_SYSLOG_H
	cp = strrchr(ident, '/');
	if(cp)
		ident = ++cp;

	int fac;

	switch(grp)
	{
	case classUser:
		fac = LOG_USER;
		break;

	case classDaemon:
		fac = LOG_DAEMON;
		break;

	case classAudit:
#ifdef	LOG_AUTHPRIV
		fac = LOG_AUTHPRIV;
		break;
#endif
	case classSecurity:
		fac = LOG_AUTH;
		break;

	case classLocal0:
		fac = LOG_LOCAL0;
		break;

	case classLocal1:
		fac = LOG_LOCAL1;
		break;

	case classLocal2:
		fac = LOG_LOCAL2;
		break;

	case classLocal3:
		fac = LOG_LOCAL3;
		break;

	case classLocal4:
		fac = LOG_LOCAL4;
		break;

	case classLocal5:
		fac = LOG_LOCAL5;
		break;

	case classLocal6:
		fac = LOG_LOCAL6;
		break;

	case classLocal7:
		fac = LOG_LOCAL7;
		break;

	default:
		fac = LOG_USER;
		break;
	}
	openlog(ident, 0, fac);
#else
	char *buf;

	lock.enterMutex();
	syslog.close();
	buf = new char[strlen(ident) + 1];
	strcpy(buf, ident);
	cp = (const char *)buf;
	buf = strrchr(buf, '.');
	if(buf)
	{
                if(!stricmp(buf, ".exe"))
                        strcpy(buf, ".log");
	}
	syslog.open(cp,ios::app);
	delete[] (char *)cp;
	lock.leaveMutex();
#endif
}

int Slog::overflow(int c)
{
	ThreadImpl *thread = getThread()->priv;
	if(!thread)
		return c;

	if(c == '\n' || !c)
	{
		if(!thread->_msgpos)
			return c;

		thread->_msgbuf[thread->_msgpos] = 0;
		if (_enable)
#ifdef	HAVE_SYSLOG_H
			syslog(priority, "%s", thread->_msgbuf);
#else
                {
			lock.enterMutex();
			syslog << "[" << priority << "] " << thread->_msgbuf << endl;
			lock.leaveMutex();
                }
#endif
		thread->_msgpos = 0;

		if ( _enable && _clogEnable
#ifndef	WIN32
                     && (getppid() > 1)
#endif
                     )
			clog << thread->_msgbuf << endl;
		_enable = true;
		return c;
	}

	if (thread->_msgpos < (int)(sizeof(thread->_msgbuf) - 1))
		thread->_msgbuf[thread->_msgpos++] = c;

	return c;
}

Slog &Slog::operator()(const char *ident, Class grp, Level level)
{
	ThreadImpl *thread = getThread()->priv;

	if(!thread)
		return *this;

	thread->_msgpos = 0;
	_enable = true;
	open(ident, grp);
	return this->operator()(level, grp);
}

Slog &Slog::operator()(Level level, Class grp)
{
	ThreadImpl *thread = getThread()->priv;

	if(!thread)
		return *this;

	thread->_msgpos = 0;
	if(_level >= level)
		_enable = true;
	else
		_enable = false;

#ifdef	HAVE_SYSLOG_H
	switch(level)
	{
	case levelEmergency:
		priority = LOG_EMERG;
		break;
	case levelAlert:
		priority = LOG_ALERT;
		break;
	case levelCritical:
		priority = LOG_CRIT;
		break;
	case levelError:
		priority = LOG_ERR;
		break;
	case levelWarning:
		priority = LOG_WARNING;
		break;
	case levelNotice:
		priority = LOG_NOTICE;
		break;
	case levelInfo:
		priority = LOG_INFO;
		break;
	case levelDebug:
		priority = LOG_DEBUG;
		break;
	}
	switch(grp)
	{
	case classAudit:
#ifdef	LOG_AUTHPRIV
		priority |= LOG_AUTHPRIV;
		break;
#endif
	case classSecurity:
		priority |= LOG_AUTH;
		break;
	case classUser:
		priority |= LOG_USER;
		break;
	case classDaemon:
		priority |= LOG_DAEMON;
		break;
	case classDefault:
		priority |= LOG_USER;
		break;
	case classLocal0:
		priority |= LOG_LOCAL0;
		break;
	case classLocal1:
		priority |= LOG_LOCAL1;
		break;
	case classLocal2:
		priority |= LOG_LOCAL2;
		break;
	case classLocal3:
		priority |= LOG_LOCAL3;
		break;
	case classLocal4:
		priority |= LOG_LOCAL4;
		break;
	case classLocal5:
		priority |= LOG_LOCAL5;
		break;
	case classLocal6:
		priority |= LOG_LOCAL6;
		break;
	case classLocal7:
		priority |= LOG_LOCAL7;
		break;
	}
#else
	priority = level;
#endif
	return *this;
}

Slog &Slog::operator()(void)
{
	return *this;
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
