// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file Platform.h
 * 
 * Basic definitions and classes for platform independence.
 *
 * The routines here are extracted from GLib and modified to handle
 * just a subset of typical cross-platform needs.
 *
 * Glib is Copyright (C) 1995-1998 Peter Mattis, Spencer Kimball and 
 * Josh MacDonald
 *
 */


#include "Platform.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


#ifdef NATIVE_WIN32
#  define STRICT			/* Strict typing, please */
#  include <windows.h>
#  include <direct.h>
#  include <errno.h>
#  include <ctype.h>
#  ifdef _MSC_VER
#    include <io.h>
#  endif /* _MSC_VER */
#endif /* NATIVE_WIN32 */


std::string OSPath::basename(const string &path)
{
	std::string::size_type base;
	base = path.rfind(DIR_SEPARATOR);
	if (base != std::string::npos) {
		return path.substr(base+1);
	}
#ifdef NATIVE_WIN32
	if (path.length() > 1 && isalpha(path[0]) && path[1] == ':') {
		return path.substr(2);
	}
#endif /* NATIVE_WIN32 */
	return path;
}

bool OSPath::isabs(const std::string &path)
{
	int n = path.length();
	if (n==0) {
		return false;
	}
	if (path[0] == DIR_SEPARATOR) {
		return true;
	}
	#ifdef NATIVE_WIN32
	if (n > 2 && isalpha(path[0]) && path[1] == ':' && path[2] == DIR_SEPARATOR) {
		return TRUE;
	}
	#endif /* NATIVE_WIN32 */
	return false;
}

std::string OSPath::skiproot(const std::string &path) {
	int n = path.length();
	if (n > 0 && path[0] == DIR_SEPARATOR) {
		return path.substr(1);
	}
#ifdef NATIVE_WIN32
	if (n > 2 && isalpha(path[0]) && path[1] == ':' && path[2] == DIR_SEPARATOR) {
		return path.substr(3);
	}
#endif
	return path;
}

std::string OSPath::dirname(const std::string &path) {
	std::string::size_type base;
	base = path.rfind(DIR_SEPARATOR);
	if (base == std::string::npos) {
		return ".";
	}
	return path.substr(0, base-1);
}

std::string OSPath::currentdir() {
	std::string dir;
	dir = DIR_SEPARATOR;
	char *buffer = getcwd(0, 0);
	if (buffer) {
		dir = buffer;
		free(buffer);
	}
	return dir;
}

std::string OSPath::ext(const std::string &path) {
	std::string::size_type ext;
	ext = path.find_last_of("."+DIR_SEPARATOR);
	if (ext != std::string::npos && path[ext]=='.') {
		return path.substr(ext);
	}
	return "";
}

std::string OSPath::join(const std::string &a, const std::string &b) {
	std::string result;
	std::string::size_type idx;
	idx = a.length();
	if (idx > 0 && a[idx-1] == DIR_SEPARATOR) idx--;
	result = a.substr(0, idx);
	return result + DIR_SEPARATOR + skiproot(b);
}

std::string OSPath::stringreplace(const std::string &path, char search, char replace) {
	if (search == replace) return path;
	std::string result;
	char *str = strdup(path.c_str());
	char *idx = str;
	for (; *idx; idx++) {
		if (*idx == '\\') *idx = '/';
	}
	result = str;
	free(str);
	return result;
}
	
std::string OSPath::normalize(const std::string &path) {
	return stringreplace(path, '\\', '/');
}

std::string OSPath::denormalize(const std::string &path) {
	return stringreplace(path, '/', DIR_SEPARATOR);
}

std::string OSPath::filter(const std::string &path) {
	return denormalize(normalize(path));
}

