/* SimData: Data Infrastructure for Simulations
 * Copyright 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Trace.cpp
 * @brief Automatic callstack traces reporting on error.
 *
 */

#include <SimData/Trace.h>
#include <iostream>

NAMESPACE_SIMDATA

std::ostream &TraceBase::log() {
	if (_log) {
		return _log->entry(LOG_ERROR, LOG_ALL);
	} else {
		return simdata::log().entry(LOG_ERROR, LOG_ALL);
	}
}

void TraceBase::error(int skip, bool segv) {
	if (_traced) return;
	_traced = true;
	if (segv) {
		log() << "FATAL ERROR: segmentation fault." << std::endl;
	}
	_preCallback();
	_backtrace(log(), skip);
	_postCallback();
}

#if defined(__GNUC__) && !defined(__MINGW32__)

void Trace::StackDump(std::ostream &out, int skip) {
	void *trace[64];
	char **messages = (char **)NULL;
	int i, trace_size = 0;

	trace_size = backtrace(trace, 64);
	out << "CALL STACK BACKTRACE:" << std::endl;
	messages = backtrace_symbols(trace, trace_size);
	if (messages) {
		for (i=skip; i<trace_size; ++i) {
			out << "  " << messages[i] << std::endl;
		}
		free(messages);
	} else {
		out << "  unavailable!" << std::endl;
	}
}

#else

void Trace::StackDump(std::ostream &, int) { }

#endif // __GNUC__


NAMESPACE_SIMDATA_END

