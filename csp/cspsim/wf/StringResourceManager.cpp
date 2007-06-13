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
 * @file StringResourceManager.cpp
 *
 **/

#include <algorithm>
#include <csp/cspsim/wf/StringResourceManager.h>

CSP_NAMESPACE

namespace wf {

StringResourceManager::StringResourceManager() {
}

StringResourceManager::~StringResourceManager() {
}

std::string StringResourceManager::getString(const std::string& key) const {
	StringMap::const_iterator foundString = m_Values.find(key);
	if(foundString == m_Values.end()) {
		return key;
	}
	return foundString->second;
}

void StringResourceManager::merge(StringResourceManager* stringsToMerge) {
	StringMap::const_iterator newString = stringsToMerge->m_Values.begin();
	for(;newString != stringsToMerge->m_Values.end();++newString) {
		m_Values[newString->first] = newString->second;
	}
}

std::string StringResourceManager::parseAndReplace(const std::string& original) const {
	std::string copy = original;
	
	std::string::size_type pos = copy.find("${");
	while(pos != std::string::npos) {
		std::string::size_type pos2 = copy.find("}", pos);
		if(pos2 == std::string::npos) {
			return copy; 
		}
		
		std::string resourceKey = copy.substr(pos + 2, pos2 - pos - 2);
		std::string resourceValue = getString(resourceKey);
		
		copy.replace(pos, pos2 - pos + 1, resourceValue); 
		
		pos = copy.find("${");
	}

	pos = copy.find("\r");
	while(pos != std::string::npos) {
		copy.replace(pos, 1, "");
		pos = copy.find("\r");
	}

	return copy;
}

} // namespace wf

CSP_NAMESPACE_END
