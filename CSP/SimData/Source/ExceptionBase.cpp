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
 * @file ExceptionBase.cpp
 * @brief Exception base classes and macros for creating new exceptions.
 **/

#include <SimData/ExceptionBase.h>
#include <SimData/Trace.h>
#include <SimData/Log.h>

#include <iostream>
#include <sstream>

NAMESPACE_SIMDATA


ExceptionBase::ExceptionBase(std::string const &type, std::string const &msg, bool trace) {
	//: std::runtime_error(type+": "+msg) {
	_type = type;
	_msg = msg;
	_dump = true;
	if (trace) {
		std::stringstream trace_;
		// skip three stack frames:
		//   ExceptionBase()
		//   Exception()
		//   ActualExceptionClass().
		Trace::StackDump(trace_, 3);
		_trace = trace_.str();
	}
}

ExceptionBase::ExceptionBase(ExceptionBase const &e) {
	_type = e._type;
	_msg = e._msg;
	_trace = e._trace;
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
	return _trace;
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

void ExceptionBase::clear() {
	_dump = false;
}

void ExceptionBase::details() const {
	std::cerr << *this << std::flush;
}

void ExceptionBase::logAndClear(int category) const {
	SIMDATA_ERROR(category, *this);
}

bool ExceptionBase::hasTrace() const {
	return _trace.size() > 0;
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

NAMESPACE_SIMDATA_END
