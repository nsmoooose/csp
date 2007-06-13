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

ResourceLocator::ResourceLocator(const StringVector& includeFolders) : m_IncludeFolders(includeFolders) {
}
	
bool ResourceLocator::locateResource(std::string& file) const {
	StringVector includeFolders = m_IncludeFolders;
	includeFolders.insert(includeFolders.begin(), ospath::dirname(file));
	
	StringVector::const_iterator folderIterator = includeFolders.begin();
	for(;folderIterator != includeFolders.end();++folderIterator) {
		std::string tempFilePath = ospath::join(*folderIterator, file);
		if(ospath::exists(tempFilePath)) {
			file = tempFilePath;
			return true;
		}
	}
	return false;
}
	
const StringVector& ResourceLocator::getIncludeFolders() const {
	return m_IncludeFolders;
}

void ResourceLocator::setIncludeFolders(const StringVector& includeFolders) {
	m_IncludeFolders = includeFolders;
}
	
ResourceLocator* createDefaultResourceLocator() {
	StringVector includeFolders;
	
	// Add the current theme folder.
	std::string themesPath = ospath::join(getUIPath(), "themes");
	std::string themePath = ospath::join(themesPath, getUITheme());
	includeFolders.push_back(themePath);
	
	// Add the current language folder
	std::string localizationPath = ospath::join(getUIPath(), "localization");
	std::string languagePath = ospath::join(localizationPath, getUILanguage());
	includeFolders.push_back(languagePath);
	
	// Add the data directory
	includeFolders.push_back(getDataPath());

	// Add the ui directory below the data directory.	
	includeFolders.push_back(getUIPath());

	return new ResourceLocator(includeFolders);
}


} // namespace wf

CSP_NAMESPACE_END
