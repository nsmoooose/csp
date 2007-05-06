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
 * @file UserInterface.cpp
 *
 **/

#include <csp/csplib/util/FileUtility.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/config/UserInterface.h>

CSP_NAMESPACE
namespace config {

UserInterface::UserInterface(const std::string& language, const std::string& theme) : m_Language(language), m_Theme(theme) {
}

UserInterface::~UserInterface() {
}

std::string UserInterface::getLanguage() {
	return m_Language;
}

void UserInterface::setLanguage(const std::string& language) {
	m_Language = language;
}
	
std::string UserInterface::getTheme() {
	return m_Theme;
}

void UserInterface::setTheme(const std::string& theme) {
	m_Theme = theme;
}

UserInterface* UserInterface::clone() {
	return new UserInterface(m_Language, m_Theme);
}

StringVector UserInterface::enumerateThemes() {
	std::string themesPath = ospath::join(getUIPath(), "themes");
	ospath::DirectoryContents directoryContent = ospath::getDirectoryContents(themesPath);
	
	StringVector stringsToReturn;
	StringVector::const_iterator directoryEntry = directoryContent.begin();
	for(;directoryEntry != directoryContent.end();++directoryEntry) {
		if(directoryEntry->find(".") != std::string::npos) {
			continue;
		}
		stringsToReturn.push_back(*directoryEntry);
	}
	return stringsToReturn;
}

StringVector UserInterface::enumerateLanguages() {
	std::string localizationPath = ospath::join(getUIPath(), "localization");
	ospath::DirectoryContents directoryContent = ospath::getDirectoryContents(localizationPath);

	StringVector stringsToReturn;
	StringVector::const_iterator directoryEntry = directoryContent.begin();
	for(;directoryEntry != directoryContent.end();++directoryEntry) {
		if(directoryEntry->find(".") != std::string::npos) {
			continue;
		}
		stringsToReturn.push_back(*directoryEntry);
	}
	return stringsToReturn;
}

}
CSP_NAMESPACE_END
