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
 * @file ConsoleCommands.h
 *
 **/

#ifndef __CONSOLECOMMANDS_H__
#define __CONSOLECOMMANDS_H__

#include <osg/Projection>

#include <SimData/Ref.h>

#include <SDL/SDL_keyboard.h>

class PyShell;

namespace osg {
	class Group;
	class MatrixTransform;
}
namespace osgConsole {
	class Console;
}


class PyConsole: public osg::Projection {
public:
	PyConsole(int ScreenWidth, int ScreenHeight);
	void enable();
	void disable();
	bool onKey(SDL_keysym const &key);
	void setPrompt(std::string prompt);
	void update();
	void bind(simdata::Ref<PyShell> const &shell);
protected:
	virtual ~PyConsole();
	osg::ref_ptr<osgConsole::Console> m_Console;
	osg::ref_ptr<osg::Group> m_Parent;
	osg::ref_ptr<osg::MatrixTransform> m_ModelViewAbs;
	std::ostream *m_Out;
	std::string m_Command;
	std::string m_Prompt;
	std::vector<std::string> m_History;
	int m_Cursor, m_HistoryIndex;
	bool onBackspace(SDL_keysym const &key);
	bool onArrow(SDL_keysym const &key);
	bool onCharacter(SDL_keysym const &key);
	bool onEnter(SDL_keysym const &key);
	void setCursor(int pos);
	simdata::Ref<PyShell> m_Shell;
};


#endif // __CONSOLECOMMANDS_H__

