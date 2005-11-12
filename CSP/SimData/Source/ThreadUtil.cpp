/* SimData: Data Infrastructure for Simulations
 * Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This file is part of SimData.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


/**
 * @file ThreadUtil.cpp
 * @brief PosixThreads utility classes.
 **/

#ifndef SIMDATA_NOTHREADS

#include <SimData/ThreadUtil.h>
#include <SimData/Log.h>

#include <string>
#include <sstream>

NAMESPACE_SIMDATA

ThreadException::ThreadException(const int error): Exception("ThreadException"), m_error(error) {
	translateError();
}

void ThreadException::checkThrow(const int result) {
	if (result != 0) {
		throw ThreadException(result);
	}
}

void ThreadException::checkLog(const int result) {
	if (result != 0) {
		ThreadException e(result);
		SIMDATA_LOG(ERROR, THREAD) << e.getError();
		e.clear();
	}
}

void ThreadException::translateError() {
	std::ostringstream msg;
	switch (m_error) {
		case EDEADLK: msg << "deadlock (EDEADLK)"; break;
		case EAGAIN: msg << "unable to allocate resource (EAGAIN)"; break;
		case EBUSY: msg << "busy: resource already in use (EBUSY)"; break;
		case EINVAL: msg << "invalid parameters (EINVAL)"; break;
		case ENOMEM: msg << "out of memory (ENOMEM)"; break;
		case ETIMEDOUT: msg << "timed out (ETIMEDOUT)"; break;
		case EPERM: msg << "permission denied (EPERM)"; break;
		case ESRCH: msg << "not found (ESRCH)"; break;
		default: msg << "ISO C99 error code: " << m_error;
	}
	addMessage(msg.str());
}


NAMESPACE_SIMDATA_END

#endif // SIMDATA_NOTHREADS

