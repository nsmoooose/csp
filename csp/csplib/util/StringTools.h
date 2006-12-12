// Combat Simulator Project
// Copyright (C) 2002, 2004 The Combat Simulator Project
// http://csp.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file StringTools.h
 *
 **/

#ifndef __CSPLIB_UTIL_STRINGTOOLS_H__
#define __CSPLIB_UTIL_STRINGTOOLS_H__

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Uniform.h>
#include <string>
#include <deque>

CSP_NAMESPACE


/** Convert a string to uppercase (in place).
 */
void CSPLIB_EXPORT ConvertStringToUpper(std::string &str);

/** Convert a string to lowercase (in place).
 */
void CSPLIB_EXPORT ConvertStringToLower(std::string &str);

/** Tokenize a string, appending the tokens into a container.
 */
template <class T_BackInsertionSequence>
void Tokenize(std::string const &str, T_BackInsertionSequence &output, const char *delimiters = " ,\n") {
	if (!delimiters) return;
	std::string::size_type lastPos(str.find_first_not_of(delimiters, 0));
	std::string::size_type pos(str.find_first_of(delimiters, lastPos));
	while (std::string::npos != pos || std::string::npos != lastPos) {
		output.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

/** Tokenize a string, placing the tokens into a deque.
 */
class CSPLIB_EXPORT TokenQueue: public std::deque<std::string> {
public:
	typedef std::deque<std::string>::iterator iterator;
	typedef std::deque<std::string>::const_iterator const_iterator;
	TokenQueue(const std::string &str, const std::string &delimiters = " ,\n");
};

/** @DEPRECATED For backwards compatibility only; use TokenQueue instead.
 */
typedef TokenQueue StringTokenizer;

/** Remove leading and trailing whitespace, or other characters if specified. */
std::string CSPLIB_EXPORT TrimString(std::string const &str, std::string const &chars = " \n\r\t");

/** Remove leading whitespace, or other characters if specified. */
std::string CSPLIB_EXPORT LeftTrimString(std::string const &str, std::string const &chars = " \n\r\t");

/** Remove trailing whitespace, or other characters if specified. */
std::string CSPLIB_EXPORT RightTrimString(std::string const &str, std::string const &chars = " \n\n\t");

/** Find the first non-whitespace character in a c-string. */
inline const char *skipWhitespace(const char *str) {
	for (char c = *str; c == ' ' || c == '\t' || c == '\n' || c == '\r'; c = *(++str));
	return str;
}

/** Parse an integer value from a c-string.  Accepts numerical values in
 *  standard C octal, decimal, and hex notation.  Returns true if the input
 *  is a valid numeric string and can be represented by the specified type.
 *  Leading and trailing whitespace is ignored.  Trailing non-numeric data,
 *  even after whitespace, is not allowed.
 */
bool CSPLIB_EXPORT parseInt(const char *s, int64 &x);
bool CSPLIB_EXPORT parseInt(const char *s, uint64 &x);
bool CSPLIB_EXPORT parseInt(const char *s, int32 &x);
bool CSPLIB_EXPORT parseInt(const char *s, uint32 &x);
bool CSPLIB_EXPORT parseInt(const char *s, int16 &x);
bool CSPLIB_EXPORT parseInt(const char *s, uint16 &x);
bool CSPLIB_EXPORT parseInt(const char *s, int8 &x);
bool CSPLIB_EXPORT parseInt(const char *s, uint8 &x);

/** A wrapper for basic types used implicitly by stringprintf.  You should
 *  not need to use this class directly.
 */
class CSPLIB_EXPORT FormatArg {
public:
	class stringbuf;
	struct formatspec;

	FormatArg(int x): x_type(TYPE_INT) { x_val.i = x; }
	FormatArg(unsigned x): x_type(TYPE_UINT) { x_val.ui = x; }
	FormatArg(int64 x): x_type(TYPE_INT64) { x_val.i64 = x; }
	FormatArg(uint64 x): x_type(TYPE_UINT64) { x_val.ui64 = x; }
	FormatArg(char x): x_type(TYPE_CHAR) { x_val.c = x; }
	FormatArg(double x): x_type(TYPE_DOUBLE) { x_val.d = x; }
	FormatArg(const char *x): x_type(TYPE_STRING), x_len(-1) { x_val.s = x; }
	FormatArg(std::string const &x): x_type(TYPE_STRING), x_len(x.size()) { x_val.s = x.c_str(); }
	FormatArg(const void *x): x_type(TYPE_PTR) { x_val.p = x; }

	bool getWidth(formatspec &) const;
	bool format(formatspec &spec, stringbuf &out) const;
	static const FormatArg nil;

private:
	bool formatInt(stringbuf &out, formatspec const &spec, int base, bool lower=true) const;
	bool formatChar(stringbuf &out, formatspec const &spec, char c) const;
	bool formatString(stringbuf &out, formatspec const &spec, const char *s, int len) const;
	bool formatFloat(stringbuf &out, formatspec const &spec, double value, char style) const;
	enum {TYPE_INT, TYPE_UINT, TYPE_INT64, TYPE_UINT64, TYPE_CHAR, TYPE_DOUBLE, TYPE_CHARSTAR, TYPE_STRING, TYPE_PTR} x_type;
	union { int i; unsigned ui; int64 i64; uint64 ui64; char c; double d; const char *s; const void *p; } x_val;
	int x_len;
};

/** A safe substitute for sprintf with up to six arguments.  Between 25% faster
 *  and 50% slower than using snprintf to write to a preallocated buffer and
 *  initialize a std::string from that buffer, depending on the number and types
 *  of the arguments (fewer is faster, floats are slower).  Supports most C99
 *  format strings except wide-character strings, %n, and the '#' for floating
 *  point types.
 */
std::string CSPLIB_EXPORT stringprintf(const char *fmt,
	FormatArg const &a0=FormatArg::nil, FormatArg const &a1=FormatArg::nil, FormatArg const &a2=FormatArg::nil,
	FormatArg const &a3=FormatArg::nil, FormatArg const &a4=FormatArg::nil, FormatArg const &a5=FormatArg::nil);

CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_STRINGTOOLS_H__

