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

#include "SDL.h"
#include "CON_console.h"
#include "CON_consolecommands.h"


void ListCommands(ConsoleInformation *console, char *string);
void Resize(ConsoleInformation *console, char *string);
void Move(ConsoleInformation *console, char *string);
void AlphaChange(ConsoleInformation *console, char *alpha);
void KillProgram(ConsoleInformation *console, char *String);
void DefaultCommand(ConsoleInformation *console, char *string);


#endif // __CONSOLECOMMANDS_H__

