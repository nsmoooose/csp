/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file ThreadUtil.h
 * @brief Thread utility classes.
 */


#ifndef __SIMDATA_THREADUTIL_H__
#define __SIMDATA_THREADUTIL_H__

#include <SimData/Namespace.h>
#include <SimData/Properties.h>
#include <SimData/ExceptionBase.h>

NAMESPACE_SIMDATA


/** Exception class for thread-related errors.  Wraps a small subset
 *  of cerrno constants.
 */
class SIMDATA_EXPORT ThreadException: public Exception {
public:
	ThreadException(const int error);

	static void checkThrow(const int result);

	static void checkLog(const int result);

	int getError() const { return m_error; }

private:
	void translateError();

	int m_error;
};


/** Dummy mutex for use with single-threaded classes.
 */
struct NoMutex {
	inline void lock() { }
	inline void unlock() { }
};


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_THREADUTIL_H__

