/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Version.cpp
 * @brief Functions and constants related to the version of SimData.
 */


#include <SimData/Version.h>
#include <SimData/Log.h>

#define SIMDATA_BUILD_DATE __TIME__ " " __DATE__

#ifndef SIMDATA_VERSION
	#define SIMDATA_VERSION  "[built " SIMDATA_BUILD_DATE "]"
#endif


NAMESPACE_SIMDATA

/** Get the SimData version number.
 */
const char *getVersion() {
	return SIMDATA_VERSION;
}

const char *buildDate() {
	return SIMDATA_BUILD_DATE;
}


#ifndef SIMDATA_NOLOADCHECK

/** Log a startup message to verify proper loading of SimData.
 *
 *  Only one copy of SimData should be active so that all object
 *  interface proxies will be stored in a single object registry.
 *  Unless you explicitly want distinct object registries, multiple
 *  "SimData XX loaded @ XX" messages are an indication of improper
 *  linking.
 *
 *  This message may be disabled by defining SIMDATA_NOLOADCHECK
 *  when building SimData.  The default log file can also be set
 *  by the SIMDATA_LOGFILE environment variable to redirect this
 *  message to a file rather than stderr.
 *
 *  This instance is created in BaseType, as opposed to some other
 *  object file, since BaseType must be loaded for all but the most
 *  basic SimData functionality.
 */
class load_check {
public:
	load_check() {
		log().entry(LOG_INFO, LOG_ALL)
		      << "SimData " << getVersion() << " loaded @ " << this
		      << std::endl;
	}
} check_basetype;

#endif // SIMDATA_NOLOADCHECK


NAMESPACE_SIMDATA_END

