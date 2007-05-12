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
 * @file Configuration.cpp
 *
 **/


#include <csp/cspsim/Config.h>
#include <csp/cspsim/config/Configuration.h>
#include <csp/cspsim/config/Display.h>
#include <csp/cspsim/config/UserInterface.h>

CSP_NAMESPACE
namespace config {

Configuration::Configuration(Display* display, UserInterface* userInterface) : m_Display(display), m_UserInterface(userInterface) {
}

Configuration::~Configuration() {
}

Configuration* Configuration::clone() {
	return new Configuration(m_Display->clone(), m_UserInterface->clone());
}

Display* Configuration::getDisplay() {
	return m_Display.get();
}

UserInterface* Configuration::getUserInterface() {
	return m_UserInterface.get();
}

}  // namespace config
CSP_NAMESPACE_END
