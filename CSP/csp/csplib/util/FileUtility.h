/* Combat Simulator Project
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
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
 * @file FileUtility.h
 * @brief Platform independent file utilities.
 */


#ifndef __CSPLIB_UTIL_FILEUTILITY_H__
#define __CSPLIB_UTIL_FILEUTILITY_H__


#include <string>
#include <vector>

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>


CSP_NAMESPACE

/**
 * @defgroup FileUtils File Utilities
 */

//@{

/** Platform-neutral file and directory utilities.
 */
namespace ospath {

	/** The local, platform-specific directory separator.
	 *
	 *  @note The value listed in the documentation will depend
	 *        on the platform under which the documentation was
	 *        built.
	 */
	extern CSP_EXPORT const char DIR_SEPARATOR;

	/** The local, platform-specific search-path separator.
	 *
	 *  @note The value listed in the documentation will depend
	 *        on the platform under which the documentation was
	 *        built.
	 */
	extern CSP_EXPORT const char PATH_SEPARATOR;
	
	/** Return the basename of pathname path.
	 *
	 *  Example:
	 *    @li <tt>basename("/usr/local/csp") == "csp"</tt>
	 */
	extern CSP_EXPORT std::string basename(const std::string &path);
	
	/** Returns true if path is an absolute pathname.
	 */
	extern CSP_EXPORT bool isabs(const std::string &path);
	
	/** Strips leading drive specifier and root directory marker.
	 *
	 *  Examples:
	 *    @li <tt>skiproot("c:\\windows") == "windows"</tt>
	 *    @li <tt>skiproot("/linux/csp") == "linux/csp"</tt>
	 */
	extern CSP_EXPORT std::string skiproot(const std::string &path);
	
	/** Returns the directory name of pathname path.
	 *
	 *  Example:
	 *    @li <tt>dirname("/usr/local/csp") == "/usr/local"</tt>.
	 */
	extern CSP_EXPORT std::string dirname(const std::string &path);
	
	/** Returns the current working directory.
	 */
	extern CSP_EXPORT std::string currentdir();
	
	/** Returns the (last) extension of pathname path.
	 *
	 *  Example:
	 *    @li <tt>ext("/usr/local/script.csp") == ".csp"</tt>
	 */
	extern CSP_EXPORT std::string ext(const std::string &path);
	
	/** Intelligently joins two path components into a pathname.
	 *
	 *  Examples:
	 *    @li <tt>join("/usr/local", "lib") == "/usr/local/lib"</tt>
	 *    @li <tt>join("c:\\windows\\", "system")
	 *               == "c:\\windows\\system"</tt>
	 */
	extern CSP_EXPORT std::string join(const std::string &a, const std::string &b);
	
	/** Substitute characters in a string.
	 *
	 *  @todo This routine is slightly out of place.  Can we find a
	 *  substitute or better home for it?
	 *
	 *  @param path The path to modify.
	 *  @param search The character to replace.
	 *  @param replace The replacement character.
	 */
	extern CSP_EXPORT std::string stringreplace(const std::string &path, char search, char replace);
	
	/** Convert a native path to a standard format (uses '/' instead of '\')
	 *
	 *  @note does <b>not</b> handle drive specifiers under windows.
	 */
	extern CSP_EXPORT std::string normalize(const std::string &path);
	
	/** Convert a normalized path to the native format.
	 */
	extern CSP_EXPORT std::string denormalize(const std::string &path);
	
	/** Filter an input path to use the native directory separation character.
	 */
	extern CSP_EXPORT std::string filter(const std::string &path);

	/** Add a path to a list of paths using the platform path separator.
	 */
	extern CSP_EXPORT std::string const &addpath(std::string &pathlist, const std::string &path);
	
	/** Removes the last extension from a filepath.  Returns the extension
	 *  (excluding the '.'), or an empty string if no extensions were found.
	 */
 	extern CSP_EXPORT std::string stripFileExtension(std::string &path);

	/** Get the last extension of a filename (excluding the '.'), or an empty
	 *  string if no extensions were found.
	 */
	extern CSP_EXPORT std::string getFileExtension(const std::string &path);

	/** Test if a file exists.
	 */
	extern CSP_EXPORT bool exists(const std::string &path);

	typedef std::vector<std::string> DirectoryContents;

	/** Retrieve a list of entries (files and subdirectories) from the given
	 *  path (non-recursive).
	 */
	extern CSP_EXPORT DirectoryContents getDirectoryContents(std::string const &path);

} // namespace ospath

//@}


CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_FILEUTILITY_H__

