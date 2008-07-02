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
#include <csp/cspwf/ResourceLocator.h>
#include <csp/cspwf/Window.h>

csp::Ref<csp::wf::ResourceLocator> defaultResourceLocator;

namespace csp {
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
	
CSPWF_EXPORT Ref<ResourceLocator> getDefaultResourceLocator() {
	return defaultResourceLocator;
}

CSPWF_EXPORT void setDefaultResourceLocator(ResourceLocator* locator) {
	defaultResourceLocator = locator;
}

} // namespace wf
} // namespace csp
