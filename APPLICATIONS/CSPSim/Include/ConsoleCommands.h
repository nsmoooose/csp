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
#include <SDL/SDL_keyboard.h>

#include "Shell.h"

namespace osg {
	class Group;
	class Geode;
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
	void bind(PyShell &shell) { m_Shell = &shell; }
protected:
	virtual ~PyConsole();
	osg::ref_ptr<osgConsole::Console> m_Console;
	osg::ref_ptr<osg::Group> m_Parent;
	osg::ref_ptr<osg::Geode> m_Geode;
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
	PyShell *m_Shell;
};



/*
#include <SDL/SDL.h>
#include <SDL/CON_console.h>
#include "CON_consolecommands.h"

void ListCommands(ConsoleInformation *console, char *string);
void Resize(ConsoleInformation *console, char *string);
void Move(ConsoleInformation *console, char *string);
void AlphaChange(ConsoleInformation *console, char *alpha);
void KillProgram(ConsoleInformation *console, char *String);
void DefaultCommand(ConsoleInformation *console, char *string);
*/


#endif // __CONSOLECOMMANDS_H__

