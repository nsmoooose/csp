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


%module Config
%{
#include <csp/cspsim/Config.h>
#include <csp/cspsim/config/Configuration.h>
#include <csp/cspsim/config/Display.h>
#include <csp/cspsim/config/UserInterface.h>
%}

%include "std_string.i"
%include "std_vector.i"
%template(cspsim_vector_string) std::vector<std::string>;


namespace csp {
namespace config {

%feature("ref")   Configuration "$this->_incref();"
%feature("unref") Configuration "$this->_decref();"
%feature("ref")   Display "$this->_incref();"
%feature("unref") Display "$this->_decref();"
%feature("ref")   UserInterface "$this->_incref();"
%feature("unref") UserInterface "$this->_decref();"

%newobject Display::clone();

typedef std::vector<std::string> StringVector;	

class Display {
public:
	Display(int width, int height, bool fullscreen);
	virtual ~Display();
	
	virtual int getWidth();
	virtual void setWidth(int width);
	
	virtual int getHeight();
	virtual void setHeight(int height);
	
	virtual bool getFullscreen();
	virtual void setFullscreen(bool fullscreen);
	
	virtual StringVector enumerateDisplayModes();

	virtual Display* clone();
};

%newobject UserInterface::clone();

class UserInterface {
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
};

%newobject Configuration::getDisplay();
%newobject Configuration::getUserInterface();
%newobject Configuration::clone();

class Configuration {
public:
	Configuration(Display* display, UserInterface* userInterface);
	virtual ~Configuration();
	
	virtual Display* getDisplay();
	virtual UserInterface* getUserInterface();
	
	virtual Configuration* clone();
};

} // namespace config

extern bool openConfig(std::string path, bool report_error);
extern std::string getCachePath();
extern std::string getConfigPath();
extern std::string getConfigPath(std::string const &);
extern std::string getDataPath();
extern std::string getDataPath(std::string const &);
} // namespace csp
