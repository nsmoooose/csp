/* SimData: Data Infrastructure for Simulations
 * Copyright 2002-2005 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file LogConstants.h
 * @brief Constants writing to the SimData log.
 */


#ifndef __SIMDATA_LOGCONSTANTS_H__
#define __SIMDATA_LOGCONSTANTS_H__

#include <SimData/Namespace.h>

NAMESPACE_SIMDATA


/** Log message priorities
 *
 *  Arranged in order of increasing importance.  Each log entry has
 *  a priority.  Low priority messages can be suppressed by setting
 *  the threshold priority of the log instance (see LogStream for
 *  details).
 */
enum { LOG_BULK, LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ALERT, LOG_ERROR };

/** Log message categories used internally by SimData
 *
 *  An application using the SimData logging facilities should define
 *  its own specialized set of categories.
 */
enum {
	LOG_NONE      = 0x00000000,
	LOG_TYPE      = 0x00000001,
	LOG_ARCHIVE   = 0x00000002,
	LOG_REGISTRY  = 0x00000004,
	LOG_THREAD    = 0x00000008,
	LOG_TIME      = 0x00000010,
	LOG_ALL       = 0xffffffff
};


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_LOGCONSTANTS_H__

