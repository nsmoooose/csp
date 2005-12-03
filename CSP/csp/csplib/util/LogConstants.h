/* Combat Simulator Project
 * Copyright 2002-2005 Mark Rose <mkrose@users.sourceforge.net>
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
 * @brief Constants writing to the csp log.
 */


#ifndef __CSPLIB_UTIL_LOGCONSTANTS_H__
#define __CSPLIB_UTIL_LOGCONSTANTS_H__

#include <csp/csplib/util/Namespace.h>

CSP_NAMESPACE

/** Log message priorities
 *
 *  Arranged in order of increasing importance.  Each log entry has
 *  a priority.  Low priority messages can be suppressed by setting
 *  the threshold priority of the log instance (see LogStream for
 *  details).
 */
enum {
	cLogPriority_DEBUG,
	cLogPriority_INFO,
	cLogPriority_WARNING,
	cLogPriority_ERROR,
	cLogPriority_FATAL
};

/** Log message categories.
 */
enum {
	// csplib
	cLogCategory_DATA         = 0x00000001,
	cLogCategory_ARCHIVE      = 0x00000002,
	cLogCategory_REGISTRY     = 0x00000004,
	cLogCategory_THREAD       = 0x00000008,
	cLogCategory_TIME         = 0x00000010,
	cLogCategory_FILE         = 0x00000020,
	cLogCategory_NETWORK      = 0x00000040,
	cLogCategory_AUDIO        = 0x00000080,
	cLogCategory_OPENGL       = 0x00000100,
	cLogCategory_INPUT        = 0x00000200,
	cLogCategory_PHYSICS      = 0x00000400,
	cLogCategory_TESTING      = 0x00000800,

	// net
	cLogCategory_TIMING       = 0x00001000,
	cLogCategory_HANDSHAKE    = 0x00002000,
	cLogCategory_BALANCING    = 0x00004000,
	cLogCategory_PACKET       = 0x00008000,
	cLogCategory_PEER         = 0x00010000,
	cLogCategory_MESSAGE      = 0x00020000,

	// cspsim
	cLogCategory_TERRAIN      = 0x00100000,
	cLogCategory_SCENE        = 0x00200000,
	cLogCategory_BATTLEFIELD  = 0x00400000,
	cLogCategory_OBJECT       = 0x00800000,
	cLogCategory_APP          = 0x01000000,
	cLogCategory_VIEW         = 0x02000000,
	cLogCategory_THEATER      = 0x04000000,

	// groups
	cLogCategory_NONE         = 0x00000000,
	cLogCategory_ALL          = 0xffffffff
};

CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_LOGCONSTANTS_H__

