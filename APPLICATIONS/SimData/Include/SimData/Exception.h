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
 * @file Exception.h
 */


#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <string>
#include <SimData/ns-simdata.h>


NAMESPACE_SIMDATA


/**
 * Base class for all SimData expections.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class Exception { //: public std::runtime_error {
	std::string _msg;
	std::string _type;
	mutable bool dump;
public:
	Exception(std::string type="Exception", std::string msg="");
	Exception(Exception const &e);
	virtual ~Exception();
	std::string getMessage();
	std::string getType();
	void appendMessage(std::string const &msg);
	void clear();
	void details();
};

#define EXCEPTION(a)	\
class a: public Exception { \
public: \
	a(std::string msg=""): \
		Exception(#a, msg) { } \
};

/**
 * Exception for marshalling python exceptions through SWIG wrapers.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class PythonException {};


NAMESPACE_END // namespace simdata


#endif // __EXCEPTION_H__
