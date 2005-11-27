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
 * @file HUDText.cpp
 *
 **/

#ifndef __GNUC__
#define snprintf _snprintf
#endif

#include <HUD/Text.h>
#include <osgText/Text>
#include <cstdio>
//#include <iomanip>
//#include <sstream>

using namespace std;

void StandardFormatter::format(char *buffer, int len, float value) {
	
	// make buffer a std::string&
	//stringstream line(buffer);
	//line.precision(0);
	//line.setf(ios::fixed);
	//line << setw(len) << value;
	//buffer = line.str(); 

	snprintf(buffer, len, m_Format.c_str(), value);
}

