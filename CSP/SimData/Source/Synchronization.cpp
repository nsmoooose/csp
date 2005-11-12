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
 * @file Synchronization.cpp
 * @brief PosixThreads synchronization and utility classes.
 **/

#ifndef SIMDATA_NOTHREADS

#include <SimData/Synchronization.h>
#include <SimData/Timing.h>

NAMESPACE_SIMDATA


ThreadCondition::~ThreadCondition() {
	int result = pthread_cond_destroy(&m_cond);
	// not safe to throw from a dtor, so just log the error
	ThreadException::checkLog(result);
	if (&m_local_mutex == &m_mutex) {
		result = pthread_mutex_destroy(&m_local_mutex);
		// not safe to throw from a dtor, so just log the error
		ThreadException::checkLog(result);
	}
}

bool ThreadCondition::wait(double timeout) {
	if (timeout < 0) {
		const int result = pthread_cond_wait(&m_cond, &m_mutex);
		ThreadException::checkThrow(result);
	} else {
		double abstime_sec = get_realtime() + timeout;
		timespec abstime;
		double sec = floor(abstime_sec);
		abstime.tv_sec = static_cast<int>(sec);
		abstime.tv_nsec = static_cast<int>(1e+9 * (abstime_sec - sec));
		const int result = pthread_cond_timedwait(&m_cond, &m_mutex, &abstime);
		if (result == ETIMEDOUT) return false;
		ThreadException::checkThrow(result);
	}
	return true;
}

NAMESPACE_SIMDATA_END

#endif // SIMDATA_NOTHREADS

