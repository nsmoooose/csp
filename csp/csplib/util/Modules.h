// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file Modules.h
 *
 **/

#ifndef __CSPLIB_UTIL_MODULES_H__
#define __CSPLIB_UTIL_MODULES_H__

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Properties.h>
#include <string>

CSP_NAMESPACE

/** Interface for loading shared libraries at runtime.  Works only on systems
 *  that support dynamic loading.
 */
class CSPLIB_EXPORT ModuleLoader: public NonConstructable {
public:

	/** Load a new module from the specified path.  Returns true on success.
	 *  This function is idempotent.
	 */
	static bool load(std::string const &path);

	/** Unload a module, returning true if the module was previously loaded.
	 */
	static bool unload(std::string const &path);

	/** Unload all modules.
	 */
	static void unloadAll();

	/** Test if a module is loaded.
	 */
	static bool isLoaded(std::string const &path);
};

CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_MODULES_H__

