// Combat Simulator Project - FlightSim Demo
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
 * @file Config.h
 *
 **/

#ifndef __CONFIG_H__
#define __CONFIG_H__


#include <string>
#include "SimpleConfig.h"

/**
 * Global configuration file
 */
extern SimpleConfig g_Config;

/**
 * Open the global configuration file.
 */
extern bool openConfig(std::string path, bool report_error=true);


/**
 * Get the main data path.
 */
extern std::string getDataPath();

/**
 * Get the specific data path.
 */
extern std::string getDataPath(std::string const &);

#endif // __CONFIG_H__

