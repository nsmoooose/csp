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

#ifndef __SIMCORE_UTIL_STRINGTOOLS_H__
#define __SIMCORE_UTIL_STRINGTOOLS_H__


#include <string>
#include <deque>


/** Convert a string to uppercase (in place).
 */
void ConvertStringToUpper(std::string &str);

/** Convert a string to lowercase (in place).
 */
void ConvertStringToLower(std::string &str);


/** Tokenize a string, placing the tokens into a deque.
 */
class StringTokenizer: public std::deque<std::string> {
public:
	StringTokenizer(const std::string &str, const std::string &delimiters = " ,\n");
};


#endif // __SIMCORE_UTIL_STRINGTOOLS_H__
