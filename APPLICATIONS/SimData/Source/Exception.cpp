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

#include <SimData/Exception.h>
#include <iostream>


NAMESPACE_SIMDATA


ExceptionBase::ExceptionBase(std::string const &type, std::string const &msg) {
	 //: std::runtime_error(type+": "+msg) {
	_type = type;
	_msg = msg;
	dump = true;
}

ExceptionBase::ExceptionBase(ExceptionBase const &e) {
	_type = e._type;
	_msg = e._msg;
	dump = e.dump;
	e.dump = false;
}

std::string ExceptionBase::getMessage() { 
	return _msg; 
}

std::string ExceptionBase::getType() { 
	return _type; 
}

void ExceptionBase::appendMessage(std::string const &msg) { 
	_msg += "\n" + msg;
}

void ExceptionBase::clear() { 
	dump = false; 
}

void ExceptionBase::details() {
	std::cerr << _type << ": " << _msg << std::endl;
}

ExceptionBase::~ExceptionBase() {
	if (dump) details();
}


NAMESPACE_END // namespace simdata

