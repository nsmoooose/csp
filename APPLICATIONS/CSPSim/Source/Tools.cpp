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
 * @file Tools.cpp
 *
 **/

#include <algorithm>

#include "LogStream.h"
#include "Tools.h"


// catches non-existent files to open, displays an error and exits the program
FILE* fileopen(const char *cFilename, const char *cMode)
{

	//CSP_LOG(CSP_TERRAIN, CSP_TRACE, "fileopen() - " << cFilename);

	FILE *f = fopen(cFilename, cMode);
	
	if (f==0)
	{ 
		printf("\n\n\nerror while opening file '%s'! Exiting program ....\n\n", cFilename);
		exit(-1);
	}

	return f;
}


void ConvertStringToUpper(string & str)
{
    transform(str.begin(), str.end(), str.begin(), ::toupper);
}


StringTokenizer::StringTokenizer(const string &rStr, const string &rDelimiters)
{
	string::size_type lastPos(rStr.find_first_not_of(rDelimiters, 0));
	string::size_type pos(rStr.find_first_of(rDelimiters, lastPos));
	while (string::npos != pos || string::npos != lastPos)
	{
		push_back(rStr.substr(lastPos, pos - lastPos));
		lastPos = rStr.find_first_not_of(rDelimiters, pos);
		pos = rStr.find_first_of(rDelimiters, lastPos);
	}
}

