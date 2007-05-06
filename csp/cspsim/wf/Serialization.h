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
 * @file Serialization.h
 *
 **/

#ifndef __CSPSIM_WF_SERIALIZATION_H__
#define __CSPSIM_WF_SERIALIZATION_H__

#include <csp/cspsim/Export.h>
#include <csp/csplib/util/Referenced.h>

CSP_NAMESPACE

namespace wf {

class StringResourceManager;
class Window;

/** Is responsible for parsing xml-content into control objects.
 */
class CSPSIM_EXPORT Serialization : public Referenced {
public:
	Serialization();
	Serialization(const std::string& userInterfaceDirectory);
	virtual ~Serialization();
	
	virtual void load(Window* window, const std::string& theme, const std::string& file);
	virtual void load(StringResourceManager* resourceManager, const std::string& filePath);
	
protected:

private:
	std::string m_UserInterfaceDirectory;
};

template<class T>
std::pair<std::string, T*> make_nvp(const std::string& name, T& value) {
	return std::pair<std::string, T*>(name, (T*)&value);
}

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_SERIALIZATION_H__
