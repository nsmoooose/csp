#pragma once
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

#include <csp/csplib/util/Export.h>

namespace csp {

/** Log message priorities
 *
 *  Arranged in order of increasing importance.  Each log entry has
 *  a priority.  Low priority messages can be suppressed by setting
 *  the threshold priority of the log instance (see LogStream for
 *  details).
 */
enum {
	Prio_DEBUG,
	Prio_INFO,
	Prio_WARNING,
	Prio_ERROR,
	Prio_FATAL
};

/** Log message categories.
 */
enum {
	// csplib
	Cat_DATA         = 0x00000001,
	Cat_ARCHIVE      = 0x00000002,
	Cat_REGISTRY     = 0x00000004,
	Cat_THREAD       = 0x00000008,
	Cat_TIME         = 0x00000010,
	Cat_NETWORK      = 0x00000020,
	Cat_AUDIO        = 0x00000040,
	Cat_OPENGL       = 0x00000080,
	Cat_INPUT        = 0x00000100,
	Cat_NUMERIC      = 0x00000200,
	Cat_PHYSICS      = 0x00000200,  // same as NUMERIC
	Cat_TESTING      = 0x00000400,
	Cat_GENERIC      = 0x00000800,

	// net
	Cat_TIMING       = 0x00001000,
	Cat_HANDSHAKE    = 0x00002000,
	Cat_BALANCING    = 0x00004000,
	Cat_PACKET       = 0x00008000,
	Cat_PEER         = 0x00010000,
	Cat_MESSAGE      = 0x00020000,

	// cspsim
	Cat_TERRAIN      = 0x00100000,
	Cat_SCENE        = 0x00200000,
	Cat_BATTLEFIELD  = 0x00400000,
	Cat_OBJECT       = 0x00800000,
	Cat_APP          = 0x01000000,
	Cat_VIEW         = 0x02000000,
	Cat_THEATER      = 0x04000000,

	// groups
	Cat_NONE         = 0x00000000,
	Cat_ALL          = 0x7fffffff
};

CSPLIB_EXPORT const char *getLogCategoryName(int category);

} // namespace csp
