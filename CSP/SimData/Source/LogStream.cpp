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


NAMESPACE_SIMDATA


LogBuffer::LogBuffer()
{
	m_enabled = true;
	m_category = 0xffffffff;
	m_priority = 0;
	m_buffer = NULL;
}

LogBuffer::~LogBuffer() {
}

void LogBuffer::set_sb(std::streambuf* sb) {
    m_buffer = sb;
}

void LogBuffer::setLogCategory(int c) {
    m_category = c;
}

int LogBuffer::getLogCategory() {
    return m_category;
}

void LogBuffer::setLogPriority(int p) {
    m_priority = p;
}

int LogBuffer::getLogPriority() {
    return m_priority;
}

void LogStream::setLogPriority(int p) {
    m_log_buffer.setLogPriority(p);
}

void LogStream::setLogCategory(int c) {
    m_log_buffer.setLogCategory(c);
}

void LogStream::initFromEnvironment() {
	char *env_logfile = getenv("SIMDATA_LOGFILE");
	if (env_logfile && *env_logfile) {
		setOutput(env_logfile);
	}
	char *env_priority = getenv("SIMDATA_LOGPRIORITY");
	if (env_priority && *env_priority) {
		int priority = atoi(env_priority);
		if (priority < 0) priority = 0;
		if (priority > LOG_ERROR) priority = LOG_ERROR;
		setLogPriority(priority);
	}
}

NAMESPACE_SIMDATA_END

