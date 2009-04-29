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


%{
#include <csp/cspsim/BaseScreen.h>
#include <csp/cspsim/GameScreen.h>
#include <csp/cspsim/LogoScreen.h>
#include <csp/cspsim/MenuScreen.h>
%}

namespace csp {

%newobject input::InputInterfaceWfAdapter::getActionSignal();
%newobject input::InputInterfaceWfAdapter::registerActionSignal();

namespace input {
class InputInterfaceWfAdapter : public Referenced {
private:
	InputInterfaceWfAdapter();
	~InputInterfaceWfAdapter();
	
public:
	virtual wf::Signal* getActionSignal(const std::string& id);
	virtual wf::Signal* registerActionSignal(const std::string& id);	
};
}

class BaseScreen {
	BaseScreen();	
};

class GameScreen : public ::csp::BaseScreen {
private:
	GameScreen();
public:
	virtual input::InputInterfaceWfAdapter* getInputInterfaceWfAdapter();
};

class LogoScreen : public ::csp::BaseScreen {
private:
	LogoScreen();
public:
};

class MenuScreen : public ::csp::BaseScreen {
public:
	virtual wf::Serialization* getSerializer();	
	
private:
	MenuScreen();	
};

%typemap(out) BaseScreen * = SWIGTYPE *DYNAMIC;
%{
  static swig_type_info *
  csp__BaseScreen_dynamic_cast(void **ptr) {
     csp::BaseScreen **nptr = (csp::BaseScreen **) ptr;
     csp::GameScreen *d = dynamic_cast<csp::GameScreen *>(*nptr);
     if (d) {
        *ptr = (void *) d;
        return SWIGTYPE_p_csp__GameScreen;
     }
     csp::LogoScreen *e = dynamic_cast<csp::LogoScreen *>(*nptr);
     if (e) {
        *ptr = (void *) e;
        return SWIGTYPE_p_csp__LogoScreen;
     }
     csp::MenuScreen *f = dynamic_cast<csp::MenuScreen *>(*nptr);
     if (f) {
        *ptr = (void *) f;
        return SWIGTYPE_p_csp__MenuScreen;
     }
     
     csp::BaseScreen* z = dynamic_cast<csp::BaseScreen*>(*nptr);
     if(z) {
     	*ptr = (void *) z;
     	return SWIGTYPE_p_csp__BaseScreen;
     }
     return 0;
  }
%}
}

DYNAMIC_CAST(SWIGTYPE_p_csp__BaseScreen, csp__BaseScreen_dynamic_cast);