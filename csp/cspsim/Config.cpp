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
 * @file Config.cpp
 *
 **/

#include <csp/cspsim/Config.h>
#include <csp/csplib/util/FileUtility.h>

#include <iostream>

namespace csp {

/**
 * Global configuration file
 */
SimpleConfig g_Config;


bool openConfig(std::string ini_path, bool report_error) {
	ini_path = ospath::filter(ini_path);
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
	if (!ospath::isabs(path)) {
		path = ospath::join(getConfigPath(), path);
	}
	return path;
}

std::string getDataPath() {
	return g_Config.getPath("Paths", "DataPath", ".", true);
}

std::string getDataPath(std::string const &pathname) {
	std::string path = g_Config.getPath("Paths", pathname, ".", true);
	if (!ospath::isabs(path)) {
		path = ospath::join(getDataPath(), path);
	}
	return path;
}

ScreenSettings getScreenSettings() {
	ScreenSettings settings;
	settings.width = g_Config.getInt("Screen", "Width", 1024, true);
	settings.height = g_Config.getInt("Screen", "Height", 768, true);
	settings.fullScreen = g_Config.getInt("Screen", "FullScreen", 0, true) == 1 ? true : false;
	return settings;
}

void setScreenSettings(const ScreenSettings& screenSettings) {
	g_Config.setInt("Screen", "Width", screenSettings.width);
	g_Config.setInt("Screen", "Height", screenSettings.height);
	g_Config.setInt("Screen", "FullScreen", screenSettings.fullScreen);
}

extern CSPSIM_EXPORT std::string getUIPath() {
	return g_Config.getPath("Paths", "UIPath", ".", true);
}

extern CSPSIM_EXPORT std::string getUILanguage() {
	return g_Config.getPath("UI", "Language", "english", true);
}

extern CSPSIM_EXPORT void setUILanguage(const std::string& language) {
	g_Config.setPath("UI", "Language", language);
}

extern CSPSIM_EXPORT std::string getUITheme() {
	return g_Config.getPath("UI", "ThemeName", "default", true);
}

extern CSPSIM_EXPORT void setUITheme(const std::string& themeName) {
	g_Config.setPath("UI", "ThemeName", themeName);
}

} // namespace csp


