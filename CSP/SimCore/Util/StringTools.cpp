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
 * @file StringTools.cpp
 *
 **/


#include <SimCore/Util/StringTools.h>
#include <ctype.h>
#include <algorithm>


void ConvertStringToUpper(std::string &str) {
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void ConvertStringToLower(std::string &str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

StringTokenizer::StringTokenizer(const std::string &str, const std::string &delimiters) {
	std::string::size_type lastPos(str.find_first_not_of(delimiters, 0));
	std::string::size_type pos(str.find_first_of(delimiters, lastPos));
	while (std::string::npos != pos || std::string::npos != lastPos) {
		push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

std::string TrimString(std::string const &str, std::string const &chars) {
	const std::string::size_type begin = str.find_first_not_of(chars);
	const std::string::size_type end = str.find_last_not_of(chars);
	return begin == std::string::npos ? "" : str.substr(begin, (1 + end) - begin);
}

std::string LeftTrimString(std::string const &str, std::string const &chars) {
	const std::string::size_type begin = str.find_first_not_of(chars);
	return begin == std::string::npos ? "" : str.substr(begin);
}

std::string RightTrimString(std::string const &str, std::string const &chars) {
	const std::string::size_type end = str.find_last_not_of(chars);
	return end == std::string::npos ? "" : str.substr(0, end + 1);
}
