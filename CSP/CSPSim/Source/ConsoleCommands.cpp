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
 * @file ConsoleCommands.cpp
 *
 **/


// python ends up defining _POSIX_C_SOURCE, without testing if it has
// previously been defined.  ConsoleCommands.h ultimately includes stl
// headers than bring in features.h, which also define this constant,
// but are more careful about overriding previous definitions.  So
// although we generally put the header file associated with a cpp
// file first in the includes list, moving Python.h to the front of
// the list serves to suppress this warning (and the dozen or so lines
// of diagnostic output that g++ emits).
#include <Python.h>

#include "ConsoleCommands.h"
#include "Console.h"
#include "Shell.h"

#include <osg/Group>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/Depth>

#include <osg/BlendFunc>
#include <osgText/Text>
#include <osgText/Font>


PyConsole::PyConsole(int ScreenWidth, int ScreenHeight) {
	m_Cursor = 0;
	m_HistoryIndex = 0;
	m_History.push_back("");
	m_Prompt = ">>> ";

	setMatrix(osg::Matrix::ortho2D(0, ScreenWidth, 0, ScreenHeight));
	m_ModelViewAbs = new osg::MatrixTransform;
#ifdef OSG096
	m_ModelViewAbs->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);
#else
	m_ModelViewAbs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
#endif // OSG096
	m_ModelViewAbs->setMatrix(osg::Matrix::identity());
	addChild(m_ModelViewAbs.get());

	m_Console = new osgConsole::Console(int(ScreenWidth*0.1), int(ScreenHeight*0.1), int(ScreenWidth*0.8), int(ScreenHeight*0.8));
	m_ModelViewAbs->addChild(m_Console.get());
	m_Out = new std::ostream(m_Console.get());
	m_Out->rdbuf(m_Console.get());
	(*m_Out) << "Welcome to the CSP (Python) interactive shell!  Enter 'help' for details.\n\n";
	osg::StateSet* ss = m_ModelViewAbs->getOrCreateStateSet();
	ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	setNodeMask(0);
}

PyConsole::~PyConsole() {
	if (m_Out) delete m_Out;
}

void PyConsole::enable() {
	setNodeMask(1);
	SDL_EnableKeyRepeat(300, 30);
	update();
	setCursor(0);
}

void PyConsole::disable() {
	setNodeMask(0);
	SDL_EnableKeyRepeat(0, 0);
}


void PyConsole::setPrompt(std::string prompt) {
	m_Prompt = prompt;
}

void PyConsole::bind(simdata::Ref<PyShell> const &shell) { 
	m_Shell = shell; 
}

bool PyConsole::onArrow(SDL_keysym const &key) {
	if (key.sym == SDLK_UP) {
		int idx = m_History.size() - m_HistoryIndex - 1;
		if (idx > 0) {
			m_History[idx] = m_Command;
			m_HistoryIndex++;
			m_Command = m_History[idx-1];
			update();
			setCursor(m_Command.size());
		}
	} else 
	if (key.sym == SDLK_DOWN) {
		int idx = m_History.size() - m_HistoryIndex - 1;
		if (m_HistoryIndex > 0) {
			m_History[idx] = m_Command;
			m_HistoryIndex--;
			m_Command = m_History[idx+1];
			update();
			setCursor(m_Command.size());
		}
	} else 
	if (key.sym == SDLK_LEFT) {
		if (m_Cursor > 0) {
			setCursor(m_Cursor-1);
		}
	} else 
	if (key.sym == SDLK_RIGHT) {
		if (m_Cursor < (int)m_Command.size()) {
			setCursor(m_Cursor+1);
		}
	} else {
		return false;
	}
	return true;
}

void PyConsole::setCursor(int pos) {
	m_Cursor = pos;
	m_Console->setCursor(m_Prompt.size() + pos);
}

void PyConsole::update() {
	m_Console->setLine(m_Prompt + m_Command);
}

bool PyConsole::onBackspace(SDL_keysym const &key) {
	if (key.sym == SDLK_BACKSPACE) {
		if (m_Cursor > 0) {
			m_Command.erase(m_Cursor-1, 1);
			update();
			setCursor(m_Cursor-1);
		}
		return true;
	}
	return false;
}

bool PyConsole::onCharacter(SDL_keysym const &key) {
	if (key.sym >= SDLK_SPACE && key.sym <= SDLK_z) {
		char character = (char) key.unicode;
		m_Command.insert(m_Cursor, 1, character);
		update();
		setCursor(m_Cursor+1);
		return true;
	}
	return false;
}

bool PyConsole::onEnter(SDL_keysym const &key) {
	if (key.sym == SDLK_RETURN) {
		m_Console->print("\n");
		m_History.back() = m_Command;
		m_History.push_back("");
		m_HistoryIndex = 0;
		if (m_Shell.valid()) {
			std::string result = m_Shell->run(m_Command);
			if (result.size() > 0) {
				result = result + "\n";
			}
			m_Console->print(result);
		}
		m_Command = "";
		update();
		setCursor(0);
		return true;
	}
	return false;
}

bool PyConsole::onKey(SDL_keysym const &key) {
	return (onBackspace(key) || onArrow(key) || onCharacter(key) || onEnter(key));
}


