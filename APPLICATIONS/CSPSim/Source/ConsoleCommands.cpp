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


#include "ConsoleCommands.h"
#include "Console.h"
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
	m_ModelViewAbs->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);
	m_ModelViewAbs->setMatrix(osg::Matrix::identity());
	addChild(m_ModelViewAbs.get());

	/* Without the following code, the bitmap font used by the console
	 * is garbled.  Don't ask me why.  This is the smallest chunk of
	 * code that I could find that prevents the problem (or at least
	 * evidence of the problem).  As far as I can tell this is a bug
	 * in the font libraries used by OSG.  I'd recommend commenting
	 * out this code when a new version of OSG is tested (>0.9.3) to
	 * see if the bug has been fixed.  BTW, I can't get TextureFonts
	 * to work at all in the Console, and PixmapFonts appear upside
	 * down!
	 */
	osgText::Text *text_ = new osgText::Text(new osgText::TextureFont("arial.ttf", 20));
	osg::Geode *geode = new osg::Geode;
	geode->addDrawable(text_);
	m_ModelViewAbs->addChild(geode);
	geode->setNodeMask(0);
	/* End of really weird font hack. */
	
	m_Console = new osgConsole::Console(int(ScreenWidth*0.1), int(ScreenHeight*0.1), int(ScreenWidth*0.8), int(ScreenHeight*0.8));
	m_Geode = new osg::Geode;
	m_Geode->addDrawable(m_Console.get());
	m_ModelViewAbs->addChild(m_Geode.get());
	m_Out = new std::ostream(m_Console.get());
	m_Out->rdbuf(m_Console.get());
	(*m_Out) << "Welcome to the CSP (Python) interactive shell!  Enter 'help' for details.\n\n";
	osg::Depth* depth = new osg::Depth;
	depth->setRange(0,0);
	osg::StateSet* ss = m_ModelViewAbs->getOrCreateStateSet();
	ss->setAttribute(depth);
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
	m_Console->setline(m_Prompt + m_Command);
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
		if (m_Shell) {
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


#if 0

//#include "ConsoleCommands.h"
//#include "GlobalCommands.h"

void KillProgram(ConsoleInformation *console, char *String)
{
    
}



/* lets the user change the alpha level */
void AlphaChange(ConsoleInformation *console, char *alpha)
{
	CON_Alpha(console, atoi(alpha));
	CON_Out(console, "Alpha set to %s.", alpha);
}


/* Move the console, takes and x and a y */
void Move(ConsoleInformation *console, char *string)
{
	int x, y;


	if(2 != sscanf(string, "%d %d", &x, &y))
	{
		x = 0;
		y = 0;
	}

	CON_Position(console, x, y);
}

/* resizes the console window, takes and x and y, and a width and height */
void Resize(ConsoleInformation *console, char *string)
{
	int x, y, w, h;

	SDL_Rect rect;
	if(4 != sscanf(string, "%d %d %d %d", &x, &y, &w, &h))
	{
		CON_Out(console, "Usage: X Y Width Height");
		return;
	}
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	CON_Resize(console, rect);

}

/* Lists all the commands. */
void ListCommands(ConsoleInformation *console, char *string)
{
	CON_ListCommands(console);
}



void DefaultCommand(ConsoleInformation *console, char *str)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "DefaultConsoleCommand - string: " << str);

	std::string sstr = std::string(str);
	//std::string rtnStr = ProcessCommandString( sstr );
	//CON_Out(console, "%s", rtnStr.c_str());
}
#endif

