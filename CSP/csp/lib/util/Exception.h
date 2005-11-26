/* Combat Simulator Project
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @brief Exception classes and macros for creating new exceptions.
 */


#ifndef __CSPLIB_UTIL_EXCEPTION_H__
#define __CSPLIB_UTIL_EXCEPTION_H__

#include <string>

#include <csp/lib/util/Export.h>
#include <csp/lib/util/Namespace.h>


CSP_NAMESPACE

class StackTrace;

/** General exception base class with error reporting.
 */
class CSP_EXPORT ExceptionBase { //: public std::runtime_error {
	std::string _msg;
	std::string _type;
	mutable bool _dump;
	StackTrace *_trace;
public:
	/** Create a new exception.
	 *
	 *  @param type a string representing the type of error.
	 *  @param msg a string providing additional information about the error.
	 *  @param trace collect a stack trace that can later be accessed using
	 *    getTrace().
	 */
	ExceptionBase(std::string const &type="Exception", std::string const &msg="", bool trace=true);

	/** Copy constructor (required for exceptions)
	 */
	ExceptionBase(ExceptionBase const &e);

	/** Destructor.
	 *
	 *  If the exception has not cleared, it will display its
	 *  details to stderr on destruction.
	 */
	virtual ~ExceptionBase();

	/** Get the string describing the error.
	 */
	std::string getMessage() const;

	/** Get the string representing the type of error.
	 */
	std::string getType() const;

	/** Get the string showing a stack trace at the time the
	 *  exception was generated.  The default behavior is to
	 *  save a stack trace, but this can be disabled in the
	 *  constructor.  If disabled, this method returns an
	 *  empty string.
	 */
	std::string getTrace() const;

	/** Returns true if a stack trace was collected.
	 */
	bool hasTrace() const;

	/** Get the full error message (type + message).
	 */
	std::string getError() const;

	/** Append additional information to the error description.
	 */
	void appendMessage(std::string const &msg);

	/** Add (prepend) additional information to the error description.
	 */
	void addMessage(std::string const &msg);

	/** Reset the exception so that it will not print to stderr on
	 *  destruction.
	 */
	void clear() const;

	/** Dump information about the exception to stderr.
	 */
	void details(bool no_trace=false) const;

	/** Log the exception and reset so that it will not print to stderr
	 *  on destruction.
	 */
	virtual void logAndClear() const;

#ifndef SWIG
	/** Dump details of an exception to an output stream
	 */
	friend CSP_EXPORT std::ostream& operator<< (std::ostream&, const ExceptionBase&);
#else
	%insert(shadow) %{
		def __str__(self): return self.getError()
		def __repr__(self): return self.getError()
	%}
#endif // SWIG

private:
	ExceptionBase const &operator=(ExceptionBase const &e);
};


/** Base class for all csp exceptions.
 */
class CSP_EXPORT Exception: public ExceptionBase {
public:
	Exception(std::string const &type="Exception", std::string const &msg="", bool trace=true):
		ExceptionBase(type, msg, trace) { }
};


#define CSP_SUBEXCEPTION(a, b) \
class a: public b { \
public: \
	a(std::string const &msg=""): \
		b(#a, msg) { } \
	a(std::string const &type, std::string const &msg): \
		b(type, msg) { } \
};

#define CSP_EXCEPTION(a)	CSP_SUBEXCEPTION(a, CSP(Exception))


/** Exception for marshalling python exceptions through SWIG wrapers.
 */
CSP_EXCEPTION(PythonException)

CSP_NAMESPACE_END


#endif // __CSPLIB_UTIL_EXCEPTION_H__

