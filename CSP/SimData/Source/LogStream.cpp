/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <tm2@stm.lbl.gov>
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

// This file is based on the logging implementation in FlightGear
// (SimGear), originally ported to CSP and modified by Wolverine.
//
// Stream based logging mechanism.
//
// Written by Bernie Bright, 1998
//
// Copyright (C) 1998  Bernie Bright - bbright@c031.aone.net.au
//
// Source code from Bernie Bright's LogStream library is used
// here under the turms of the GNU General Public License
// version 2 or later, as allowed by the GNU Library General
// Public License Version 2 (clause 3).


#include <SimData/LogStream.h>
#include <SimData/Log.h>

#include <cstdlib>
#include <map>


NAMESPACE_SIMDATA


void LogStream::initFromEnvironment(const char *log_file, const char *log_priority) {
	if (log_file) {
		char *env_logfile = getenv(log_file);
		if (env_logfile && *env_logfile) {
			setOutput(env_logfile);
		}
	}
	if (log_priority) {
		char *env_priority = getenv(log_priority);
		if (env_priority && *env_priority) {
			int priority = atoi(env_priority);
			if (priority < 0) priority = 0;
			if (priority > LOG_ERROR) priority = LOG_ERROR;
			setLogPriority(priority);
		}
	}
}

std::ostream & LogStream::entry(int priority, int category, const char *file, int line) {
	if (!isNoteworthy(priority, category)) return m_null;
	if (!m_stream) setOutput(std::cerr);
	std::ostream &out(*m_stream);
	out << priority << ' ';
	if (m_log_time) {
		char time_stamp[32];
		time_t now;
		time(&now);
		strftime(time_stamp, 32, "%Y%m%d %H%M%S", gmtime(&now));
		out << time_stamp << ' ';
	}
	if (file && m_log_point) {
		out << '(' << file << ':' << line << ") ";
	}
	return out;
}


// nothing very fancy.  the logstreams persist unless explicitly
// deleted by a caller, since they may be needed even during static
// destruction.
typedef std::map<std::string, LogStream *> LogStreamRegistry;
LogStreamRegistry NamedLogStreamRegistry;

LogStream *LogStream::getOrCreateNamedLog(const std::string &name) {
	LogStreamRegistry::iterator iter = NamedLogStreamRegistry.find(name);
	if (iter == NamedLogStreamRegistry.end()) {
		LogStream *logstream = new LogStream(std::cerr);
		NamedLogStreamRegistry[name] = logstream;
		return logstream;
	}
	return iter->second;
}


NAMESPACE_SIMDATA_END

