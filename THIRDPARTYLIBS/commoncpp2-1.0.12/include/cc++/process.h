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
 * @file process.h
 * @short Process services.
 **/

#ifndef	CCXX_PROCESS_H_
#define	CCXX_PROCESS_H_

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#ifndef CCXX_THREAD_H_
#include <cc++/thread.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

/**
 * A class for containing portable process related functions
 * that help create portable code.  These are typically
 * referenced thru Process::xxx static member functions.
 * Many of these members are used both for win32 and posix
 * systems although some may be platform specific.
 *
 * @short Peocess wrapper class.
 * @author David Sugar <dyfet@ostel.com>
 */
class CCXX_CLASS_EXPORT Process
{
public:
#ifndef	WIN32
	typedef	RETSIGTYPE (*Trap)(int);
	/**
	 * Detach current process into a daemon, posix
	 * only.  Perhaps a similar method can be used
	 * for creating win32 "services"?
	 */
	static void detach(void);

	/**
	 * Set a posix compliant signal handler.
	 *
	 * @return previous handler.
	 * @param signo signal no.
	 * @param handler trap handler.
	 */
	static Trap setPosixSignal(int signo, Trap handler);

	/**
	 * Set system call interuptable signal handler.
	 *
	 * #return previous handler.
	 * @param signo   signal no.
	 * @param handler trap handler.
	 */
	static Trap setInterruptSignal(int signo, Trap handler);
#endif
	/**
	 * Spawn a process and wait for it's exit code.  In win32
	 * this is done with the spawn system call.  In posix,
	 * this is done with a fork, an execvp, and a waitpid.
	 *
	 * @warning The implementation differences between posix and
	 * win32 systems may cause side effects. For instance, if you
	 * use atexit() and this spawn method, on posix systems the
	 * function set up with atexit() will be called when the
	 * parent process of the fork exits, which will not happen on
	 * Win32 systems.
	 *
	 * @return error code from process.
	 * @param exec name of executable.
	 * @param argv list of command arguments.
	 */
	static int spawn(const char *exec, const char **argv);

	/**
	 * Get system environment.
	 *
	 * @return system environ symbol.
	 * @param name of symbol.
	 */
	static const char *getEnv(const char *name);

	/**
	 * Set system environment in a standard manner.
	 *
	 * @param name of environment symbol to set.
	 * @param value of environment symbol.
	 * @param overwrite true if replace existing symbol.
	 */
	static void setEnv(const char *name, const char *value, bool overwrite);
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
