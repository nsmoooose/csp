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
 * @file Config.cpp
 *
 **/

#include "Config.h"
#include <csp/csplib/util/FileUtility.h>

/**
 * Global configuration file
 */
SimpleConfig g_Config;


bool openConfig(std::string ini_path, bool report_error) {
        ini_path = simdata::ospath::filter(ini_path);
        bool found_config = false;
        try {
                found_config = g_Config.open(ini_path);
        } catch (ConfigError &) {
                return false;
        }
        if (!found_config && report_error) {
                std::cerr << "Unable to open configuration file " << ini_path << std::endl;
        }
        return found_config;
}

std::string getCachePath() {
	return g_Config.getPath("Paths", "CachePath", ".", true);
}

std::string getConfigPath() {
	std::string base = g_Config.getConfigurationDirectory();
	return g_Config.getPath("Paths", "ConfigPath", base, true);
}

std::string getConfigPath(std::string const &pathname) {
	std::string path = g_Config.getPath("Paths", pathname, ".", true);
	if (!simdata::ospath::isabs(path)) {
		path = simdata::ospath::join(getConfigPath(), path);
	}
	return path;
}

std::string getDataPath() {
	return g_Config.getPath("Paths", "DataPath", ".", true);
}

std::string getDataPath(std::string const &pathname) {
	std::string path = g_Config.getPath("Paths", pathname, ".", true);
	if (!simdata::ospath::isabs(path)) {
		path = simdata::ospath::join(getDataPath(), path);
	}
	return path;
}

