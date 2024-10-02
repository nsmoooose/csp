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
 * @file UserInterface.h
 *
 **/

#include <string>
#include <vector>
#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Export.h>

namespace csp {
namespace config {

typedef std::vector<std::string> StringVector;

/** Use to retreive and change user interface settings. Use CSPSim singleton in order
 *  to make changes permanent. See Configuration class for more information. */
class CSPSIM_EXPORT UserInterface: public Referenced {
public:
	UserInterface(const std::string& language, const std::string& theme);
	virtual ~UserInterface();

	/** Retreive name of the current language settings. */
	virtual std::string getLanguage();
	/** Changes the language. Call enumerateLanguages() for information about
		known languages. */
	virtual void setLanguage(const std::string& language);

	/** Returns the name of the current graphical theme used. */
	virtual std::string getTheme();
	/** Set a new theme. Call enumerateThemes() to get existing themes
		to choose from. */
	virtual void setTheme(const std::string& themeName);

	virtual UserInterface* clone();

	/** Enumerates the known themes for csp. Each theme has its own directory so
		this method simply enumerates known directories. */
	virtual StringVector enumerateThemes();
	/** Enumerates the known user interface languages for csp. Each language has
		its own directory so this method simply enumerates known directories. */
	virtual StringVector enumerateLanguages();

private:
	std::string m_Language;
	std::string m_Theme;
};

} // end namespace config
} // end namespace csp
