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

#include <cstdlib>

#ifndef WIN32

#include <sys/time.h>
#ifdef	SIGTSTP
#include <sys/file.h>
#include <sys/ioctl.h>
#endif

#ifndef	_PATH_TTY
#define	_PATH_TTY "/dev/tty"
#endif

#endif // !WIN32

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

CCXX_EXPORT(void) setException(Thread::Throw mode)
{
	Thread *thread = getThread();
	thread->priv->_throw = mode;
}

CCXX_EXPORT(Thread::Throw) getException(void)
{
	Thread *thread = getThread();
	return thread->priv->_throw;
}

#ifndef WIN32

timespec *getTimeout(struct timespec *spec, timeout_t timer)
{
	static	struct timespec myspec;

	if(spec == NULL)
		spec = &myspec;

#ifdef	PTHREAD_GET_EXPIRATION_NP
	struct timespec offset;

	offset.tv_sec = timer / 1000;
	offset.tv_nsec = (timer % 1000) * 1000000;
	pthread_get_expiration_np(&offset, sec);
#else
	struct timeval current;

	gettimeofday(&current, NULL);
	spec->tv_sec = current.tv_sec + timer / 1000;
	spec->tv_nsec = (current.tv_usec + (timer % 1000) * 1000) * 1000;
#endif
	return spec;
}	

#if !defined(__CYGWIN32__) && !defined(__MINGW32__)
void	wait(signo_t signo)
{
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, signo);
#ifdef	HAVE_SIGWAIT2
	sigwait(&mask, &signo);
#else
	sigwait(&mask);
#endif
}
#endif

/*
void	Thread::yield(void)
{
#ifdef HAVE_PTHREAD_YIELD
	pthread_yield();
#endif
}
*/

#ifdef HAVE_PTHREAD_DELAY

void	Thread::sleep(timeout_t timeout)
{
	struct timespec timer;
	timer.tv_sec = timeout / 1000;
	timer.tv_nsec = (timeout % 1000) * 1000000;
	pthread_delay(&timer);
}

#else

void	Thread::sleep(timeout_t timeout)
{
	usleep(timeout * 1000);
}
	
#endif	

#endif // !WIN32

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
