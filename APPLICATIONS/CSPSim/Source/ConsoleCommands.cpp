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


#include "stdinc.h"

#include "ConsoleCommands.h"
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

