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


#ifndef __SIMDATA_EXCEPTION_H__
#define __SIMDATA_EXCEPTION_H__

#include <string>

#include <SimData/Export.h>
#include <SimData/ns-simdata.h>


NAMESPACE_SIMDATA


/**
 * General expection base class with error reporting.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT ExceptionBase { //: public std::runtime_error {
	std::string _msg;
	std::string _type;
	mutable bool dump;
public:
	/**
	 * Create a new exception.
	 *
	 * @param type a string representing the type of error.
	 * @param msg a string providing additional information about the error.
	 */
	ExceptionBase(std::string const &type="Exception", std::string const &msg="");

	/**
	 * Copy constructor.
	 */
	ExceptionBase(ExceptionBase const &e);

	/**
	 * Destructor.
	 *
	 * If the exception has not cleared, it will display its
	 * details to stderr on destruction.
	 */
	virtual ~ExceptionBase();

	/**
	 * Get the string describing the error.
	 */
	std::string getMessage();

	/**
	 * Get the string representing the type of error.
	 */
	std::string getType();

	/**
	 * Add additional information to the error description.
	 */
	void appendMessage(std::string const &msg);

	/**
	 * Reset the exception so that it will not print to stderr on 
	 * destruction.
	 */
	void clear();

	/**
	 * Dump information about the exception to stderr.
	 */
	void details();
};

/**
 * Base class for all SimData specific exceptions.
 */
class Exception: public ExceptionBase {
public:
	Exception(std::string const &type="Exception", std::string const &msg=""): 
		ExceptionBase(type, msg) { }
};

#define SIMDATA_EXCEPTION(a)	\
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
SIMDATA_EXCEPTION(PythonException);


NAMESPACE_END // namespace simdata


#endif // __SIMDATA_EXCEPTION_H__
