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
 * @file ResourceLocator.h
 *
 **/

#ifndef __CSPSIM_WF_RESOURCELOCATOR_H__
#define __CSPSIM_WF_RESOURCELOCATOR_H__

#include <vector>
#include <string>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Referenced.h>
#include <csp/cspwf/Export.h>

namespace csp {
namespace wf {
	
class Window;

typedef std::vector<std::string> StringVector;

/** Class that is responsible for finding resources
 * in the filesystem. A resource can be placed in different
 * places in the data directory.
 */
class CSPWF_EXPORT ResourceLocator : public Referenced {
public:
	ResourceLocator(const StringVector& includeFolders);
	
	virtual bool locateResource(std::string& file) const;
	virtual const StringVector& getIncludeFolders() const;
	virtual void setIncludeFolders(const StringVector& includeFolders);
	
private:
	StringVector m_IncludeFolders;
};

/* Returns the default resource locator. */ 
CSPWF_EXPORT Ref<ResourceLocator> getDefaultResourceLocator();
CSPWF_EXPORT void setDefaultResourceLocator(ResourceLocator* locator);

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_RESOURCELOCATOR_H__
