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
#include <cc++/process.h>

#include <cstdlib>
#include <cstdio>

#ifndef	WIN32
#include <sys/time.h>
#include <sys/wait.h>

#ifdef	SIGTSTP
#include <sys/file.h>
#include <sys/ioctl.h>
#endif

#ifndef	_PATH_TTY
#define	_PATH_TTY "/dev/tty"
#endif
#endif

#ifdef	WIN32
#include <process.h>
#include <stdio.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

#ifdef	WIN32
int Process::spawn(const char *exename, const char **args)
{
	return ::spawnvp(P_WAIT, (char *)exename, (char **)args);
}
#else
int Process::spawn(const char *exename, const char **args)
{
	int status, pid;

	pid = vfork();
	if(pid == -1)
		return -1;

	if(!pid)
	{
		execvp((char *)exename, (char **)args);
		_exit(-1);
	}

#ifdef	__FreeBSD__
	wait4(pid, &status, WUNTRACED, NULL);
#else
	waitpid(pid, &status, WUNTRACED);
#endif

#ifdef	WEXITSTATAUS
	return WEXITSTATUS(status);
#else
	return status;
#endif
}

Process::Trap Process::setInterruptSignal(int signo, Trap func)
{
	struct	sigaction	sig_act, old_act;

	sig_act.sa_handler = func;
	sigemptyset(&sig_act.sa_mask);
	if(signo != SIGALRM)
		sigaddset(&sig_act.sa_mask, SIGALRM);

	sig_act.sa_flags = 0;
#ifdef	SA_INTERRUPT
	sig_act.sa_flags |= SA_INTERRUPT;
#endif
	if(sigaction(signo, &sig_act, &old_act) < 0)
		return SIG_ERR;

	return old_act.sa_handler;
}

Process::Trap Process::setPosixSignal(int signo, Trap func)
{
	struct	sigaction	sig_act, old_act;

	sig_act.sa_handler = func;
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	if(signo == SIGALRM)
	{
#ifdef	SA_INTERRUPT
		sig_act.sa_flags |= SA_INTERRUPT;
#endif
	}
	else
	{
		sigaddset(&sig_act.sa_mask, SIGALRM);
#ifdef	SA_RESTART
		sig_act.sa_flags |= SA_RESTART;
#endif
	}
	if(sigaction(signo, &sig_act, &old_act) < 0)
		return SIG_ERR;
	return old_act.sa_handler;
}

void    Process::detach(void)
{
        int pid;
        int fd;

        if(getppid() == 1)
                return;

#ifdef  SIGTTOU
        ::signal(SIGTTOU, SIG_IGN);
#endif

#ifdef  SIGTTIN
        ::signal(SIGTTIN, SIG_IGN);
#endif

#ifdef  SIGTSTP
        ::signal(SIGTSTP, SIG_IGN);
#endif

        if((pid = fork()) < 0)
                throw(pid);
        else if(pid > 0)
                exit(0);


#if	defined(SIGTSTP) && defined(TIOCNOTTY)
        if(setpgid(0, getpid()) == -1)
                throw(-1);

        if((fd = open(_PATH_TTY, O_RDWR)) >= 0)
        {
                ioctl(fd, TIOCNOTTY, NULL);
                close(fd);
        }
#else
        if(setpgrp() == -1)
                throw(-1);

        ::signal(SIGHUP, SIG_IGN);

        if((pid = fork()) < 0)
                throw(-1);
        else if(pid > 0)
                exit(0);

#endif
}
#endif

void Process::setEnv(const char *name, const char *value, bool overwrite)
{
#ifdef	HAVE_SETENV
	::setenv(name, value, (int)overwrite);
#else
	char strbuf[256];

	snprintf(strbuf, sizeof(strbuf), "%s=%s", name, value);
	if(!overwrite)
		if(getenv(strbuf))
			return;

	::putenv(strbuf);
#endif
}

const char *Process::getEnv(const char *name)
{
	return ::getenv(name);
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
