/* Combat Simulator Project
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
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
 * @file Exception.cpp
 * @brief Exception classes and macros for creating new exceptions.
 **/

#include <csp/lib/util/Exception.h>
#include <csp/lib/util/Trace.h>
#include <csp/lib/util/Log.h>

#include <iostream>
#include <sstream>

CSP_NAMESPACE


ExceptionBase::ExceptionBase(std::string const &type, std::string const &msg, bool trace): _trace(0) {
	//: std::runtime_error(type+": "+msg) {
	_type = type;
	_msg = msg;
	_dump = true;
	if (trace) {
		_trace = new StackTrace;
		// skip three stack frames: ExceptionBase(), Exception(), ActualExceptionClass().
		_trace->acquire(/*skip=*/3);
	}
}

ExceptionBase::ExceptionBase(ExceptionBase const &e): _trace(0) {
	_type = e._type;
	_msg = e._msg;
	if (e._trace) _trace = new StackTrace(*e._trace);
	_dump = e._dump;
	e._dump = false;
}

std::string ExceptionBase::getMessage() const {
	return _msg;
}

std::string ExceptionBase::getType() const {
	return _type;
}

std::string ExceptionBase::getTrace() const {
	if (!_trace) return "(TRACE NOT AVAILABLE)";
	std::stringstream os;
	os << *_trace;
	return os.str();
}

std::string ExceptionBase::getError() const {
	return _type + ": " + _msg;
}

void ExceptionBase::appendMessage(std::string const &msg) {
	if (_msg.size()) {
		_msg += '\n' + msg;
	} else {
		_msg = msg;
	}
}

void ExceptionBase::addMessage(std::string const &msg) {
	if (_msg.size()) {
		_msg = msg + '\n' + _msg;
	} else {
		_msg = msg;
	}
}

void ExceptionBase::clear() const {
	_dump = false;
}

void ExceptionBase::details(bool no_trace) const {
	if (no_trace || !hasTrace()) {
		std::cerr << getError() << std::flush;
	} else {
		std::cerr << getError() << "\n" << getTrace() << std::flush;
	}
}

void ExceptionBase::logAndClear() const {
	CSPLOG(ERROR, ALL) << *this;
	clear();
}

bool ExceptionBase::hasTrace() const {
	return _trace && _trace->valid();
}

ExceptionBase::~ExceptionBase() {
	if (_dump) {
		std::cerr << *this << std::flush;
	}
}

std::ostream &operator<<(std::ostream &out, const ExceptionBase& exc) {
	out << exc.getError();
	if (exc.hasTrace()) out << '\n' << exc.getTrace();
	return out;
}

CSP_NAMESPACE_END

