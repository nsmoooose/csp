#pragma once
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
 * @file Config.h
 *
 **/

#include <csp/cspsim/Export.h>
#include <csp/csplib/util/SimpleConfig.h>
#include <string>

namespace csp {

// TODO move to CSPSim?
/**
 * Global configuration file
 */
extern CSPSIM_EXPORT SimpleConfig g_Config;

/**
 * Open the global configuration file.
 */
extern CSPSIM_EXPORT bool openConfig(std::string path, bool report_error); //=true);

/**
 * Get the main cache path.
 */
extern CSPSIM_EXPORT std::string getCachePath();

/**
 * Get the main configuration path.
 */
extern CSPSIM_EXPORT std::string getConfigPath();

/**
 * Get the specific configuration path.
 */
extern CSPSIM_EXPORT std::string getConfigPath(std::string const &);

/**
 * Get the main data path.
 */
extern CSPSIM_EXPORT std::string getDataPath();

/**
 * Get the specific data path.
 */
extern CSPSIM_EXPORT std::string getDataPath(std::string const &);
extern CSPSIM_EXPORT void setDataPath(const std::string& key, const std::string& value);


struct ScreenSettings {
	ScreenSettings() : width(1024), height(768), fullScreen(false), clouds(false) {}
	int width;
	int height;
	bool fullScreen;
	bool clouds;
};

/**
 * Get screen size and fullscreen
 */
extern CSPSIM_EXPORT ScreenSettings getScreenSettings();
extern CSPSIM_EXPORT void setScreenSettings(const ScreenSettings&);

extern CSPSIM_EXPORT std::string getUIPath();

extern CSPSIM_EXPORT std::string getUILanguage();
extern CSPSIM_EXPORT void setUILanguage(const std::string& language);

extern CSPSIM_EXPORT std::string getUITheme();
extern CSPSIM_EXPORT void setUITheme(const std::string& themeName);

} // namespace csp
