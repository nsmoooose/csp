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

// The logging code originally used in SimData was based on code
// from SimGear (modified by Wolverine), which in turn was based
// on a stream library by Bernie Bright.  The vast majority of the
// current logging code has been reimplemented from scratch and
// bears little resemblance to the earlier approach.  This notice
// will be removed eventually.


#include <SimData/LogStream.h>
#include <SimData/Log.h>

#include <iostream>
#include <fstream>
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
	static std::ostream m_null(NULL);
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

void LogStream::_close() {
	if (m_fstream != NULL) {
		m_fstream->close();
		delete m_fstream;
		m_fstream = NULL;
	}
	m_stream = NULL;
}

void LogStream::setOutput(std::ofstream& out_) {
	_close();
	m_fstream = &out_;
	m_stream = &out_;
}

void LogStream::setOutput(std::string const &filename) {
	_close();
	m_fstream = new std::ofstream(filename.c_str());
	assert(m_fstream != NULL);
	m_stream = m_fstream;
}

void fatal(std::string const &msg) {
	std::cerr << "SIMDATA fatal error:" << std::endl;
	std::cerr << msg << std::endl;
	// use abort() instead of exit() to trigger the SIGABRT handler (if installed)
	// and generate a stack trace.
	::abort();
}

LogStream *_makeDefaultLog() {
	return new LogStream(std::cerr);
}

// not really the right place for this, but convenient.
void SIMDATA_EXPORT _log_reference_count_error(int count, int pointer) {
	SIMDATA_LOG(LOG_ALL, LOG_ERROR, "simdata::ReferencedBase(" << std::hex << pointer << ") deleted with non-zero reference count (" << count << "): memory corruption possible.");
}

// not really the right place for this, but convenient.
void SIMDATA_EXPORT _log_reference_conversion_error() {
	SIMDATA_LOG(LOG_ALL, LOG_ERROR, "simdata::Ref() assignment: incompatible types (dynamic cast failed).");
}

NAMESPACE_SIMDATA_END

