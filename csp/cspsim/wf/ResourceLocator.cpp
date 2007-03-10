// Combat Simulator Project
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
 * @file ResourceLocator.cpp
 *
 **/

#include <csp/csplib/util/FileUtility.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/wf/ResourceLocator.h>
#include <csp/cspsim/wf/Window.h>

CSP_NAMESPACE

namespace wf {
	
WindowResourceLocator::WindowResourceLocator(const Window* window) : m_Window(window) {
}

bool WindowResourceLocator::locateResource(std::string& file) const {
	// Build up the path to the file in order to be able to read it.
	std::string themesPath = ospath::join(getUIPath(), "themes");
	std::string themePath = ospath::join(themesPath, m_Window->getTheme());
	std::string filePath = ospath::join(themePath, file);
	
	// Test to see if the file exists at all?
	if(ospath::exists(filePath.c_str())) {
		file = filePath;
		return true;
	}
	else {
		// As a secondary solution we look for the file in the image data
		// directory. It may exist there.
		std::string dataPath = getDataPath();
		filePath = ospath::join(dataPath, file);
		if(ospath::exists(filePath.c_str())) {
			file = filePath;
			return true;
		}
	}
	
	// Didn't find any file that is matching the resource asked for.
	return false;
}

} // namespace wf

CSP_NAMESPACE_END
