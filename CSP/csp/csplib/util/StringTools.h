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
void CSPLIB_EXPORT Tokenize(std::string const &str, T_BackInsertionSequence &output, const char *delimiters = " ,\n") {
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


CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_STRINGTOOLS_H__

