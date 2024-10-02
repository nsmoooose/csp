#pragma once
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

#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Export.h>

namespace csp {

/** Data classes for configuring the simulator. Display settings, ui theme, language etc.  */
namespace config {

class Display;
class UserInterface;

/** A configuration object that contains data about changeable settings.
 * In order to make changes permanent you must call CSPSim singleton. It is wise
 * to use a clone while you are editing the configuration.
\code
  // clone the existing configuration.
  Ref<config::Configuration> newConfig = theSim->getConfiguration()->clone();
  // change something.
  newConfig->setXxxxYyyy("aaa");
  // apply the new configuration
  theSim->setConfiguration(newConfig.get());
\endcode
*/
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

} // end namespace config
} // end namespace csp
