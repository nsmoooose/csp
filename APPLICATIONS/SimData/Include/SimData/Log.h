/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
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
 * @file Log.h
 */


#ifndef __SIMDATA_LOG_H__
#define __SIMDATA_LOG_H__


#include <cstdio>
#include <string>

#include <SimData/LogStream.h>
#include <SimData/ns-simdata.h>
#include <SimData/Export.h>


NAMESPACE_SIMDATA

/**
 * Display a fatal error message to stderr and exit.
 */
inline void error(std::string const &msg) {
	std::cerr << "SIMDATA fatal error:" << std:: endl;
	std::cerr << msg << std::endl;
	::exit(1);
}

/**
 * Return the one and only logstream instance.
 * We use a function instead of a global object so we are assured that cerr
 * has been initialised.
 * @return current logstream
 */
inline SIMDATA_EXPORT logstream&
log()
{
	static logstream logstrm(std::cerr);
	return logstrm;
}

/** 
 * Log a message.
 * @param C debug class
 * @param P priority
 * @param M message
 */
#ifdef SIMDATA_NDEBUG
# define SIMDATA_LOG(C,P,M)
#else
# define SIMDATA_LOG(C,P,M) simdata::log() << simdata::loglevel(C,P) << M << std::endl
#endif

enum { LOG_BULK, LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ALERT, LOG_ERROR };
enum { 
	LOG_NONE      = 0x00000000, 
	LOG_TYPE      = 0x00000001, 
	LOG_ARCHIVE   = 0x00000002,
	LOG_REGISTRY  = 0x00000004,
	LOG_ALL       = 0xffffffff
};


NAMESPACE_END // namespace simdata


#endif // __SIMDATA_LOG_H__

