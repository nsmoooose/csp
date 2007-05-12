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


#ifndef __CSPSIM_CONFIG_USERINTERFACE_H__
#define __CSPSIM_CONFIG_USERINTERFACE_H__

#include <string>
#include <vector>
#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Export.h>

CSP_NAMESPACE

namespace config {

typedef std::vector<std::string> StringVector;

class CSPSIM_EXPORT UserInterface: public Referenced {
public:
	UserInterface(const std::string& language, const std::string& theme);
	virtual ~UserInterface();

	virtual std::string getLanguage();
	virtual void setLanguage(const std::string& language);

	virtual std::string getTheme();
	virtual void setTheme(const std::string& themeName);

	virtual UserInterface* clone();

	virtual StringVector enumerateThemes();
	virtual StringVector enumerateLanguages();

private:
	std::string m_Language;
	std::string m_Theme;
};

}

CSP_NAMESPACE_END

#endif // __CSPSIM_CONFIG_USERINTERFACE_H__

