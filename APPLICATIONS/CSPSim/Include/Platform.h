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
 **/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <string>

#ifdef _WIN32
	  #define NATIVE_WIN32
#endif

/**
 * Platform independent file path manipulation routines.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
namespace ospath {

	extern const char DIR_SEPARATOR;
	extern const char PATH_SEPARATOR;
	
	/**
	 * Return the basename of pathname path.
	 *
	 * Example: basename("/usr/local/csp") returns "csp"
	 */
	std::string basename(const std::string &path);
	
	/**
	 * Returns true if path is an absolute pathname.
	 */
	bool isabs(const std::string &path);
	
	/**
	 * Strips leading drive specifier and root directory marker.
	 *
	 * Examples: 
	 * 	skiproot("c:\\windows") == "windows"
	 * 	skiproot("/linux/csp") == "linux/csp"
	 */
	std::string skiproot(const std::string &path);
	
	/**
	 * Returns the directory name of pathname path.
	 *
	 * Example: dirname("/usr/local/csp") returns "/usr/local"
	 */
	std::string dirname(const std::string &path);
	
	/**
	 * Returns the current working directory.
	 */
	std::string currentdir();
	
	/**
	 * Returns the (last) extension of pathname path.
	 *
	 * Example: ext("/usr/local/script.csp") returns ".csp"
	 */
	std::string ext(const std::string &path);
	
	/**
	 * Intelligently joins two path components into a pathname.
	 *
	 * Examples: 
	 *     join("/usr/local", "lib") returns "/usr/local/lib"
	 *     join("c:\\windows\\", "system") returns "c:\\windows\\system"
	 */
	std::string join(const std::string &a, const std::string &b);
	
	/**
	 * Substitute characters in a string.
	 *
	 * This routine is slightly out of place.  Can we find a substitute or better 
	 * home for it?
	 *
	 * @param search The character to replace.
	 * @param replace The replacement character.
	 */
	std::string stringreplace(const std::string &path, char search, char replace);
	
	/**
	 * Convert a native path to a standard format (uses '/' instead of '\')
	 *
	 * Note: does *not* handle directory names under windows.
	 */
	std::string normalize(const std::string &path);
	
	/**
	 * Convert a normalized path to the native format.
	 */
	std::string denormalize(const std::string &path);
	
	/**
	 * Filter an input path to use the native directory separation character.
	 */
	std::string filter(const std::string &path);

	/**
	 * Add a path to a list of paths using the platform path separator.
	 */
	std::string const &addpath(std::string &pathlist, const std::string &path);
};


#endif // __PLATFORM_H__

