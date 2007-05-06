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
 * @file Configuration.h
 *
 **/


#ifndef __CONFIG_CONFIGURATION_H__
#define __CONFIG_CONFIGURATION_H__

#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Export.h>

CSP_NAMESPACE

namespace config {
	
class Display;
class UserInterface;

class CSPSIM_EXPORT Configuration: public Referenced {
public:
	Configuration(Display* display, UserInterface* userInterface);
	virtual ~Configuration();
	
	virtual Display* getDisplay();
	virtual UserInterface* getUserInterface();
	
	virtual Configuration* clone();
	
private:
	Ref<Display> m_Display;
	Ref<UserInterface> m_UserInterface;
};

}

CSP_NAMESPACE_END

#endif // __CONFIG_CONFIGURATION_H__

