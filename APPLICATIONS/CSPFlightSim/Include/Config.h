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
 * @file Config.h
 *
 **/

///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of class CConfig
//   reads and provides configuration data
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	9/26/2001	created file and implemented functionality- zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(CONFIG_H)
#define __CONFIG_H__

#pragma warning( disable : 4786 )	// disable warning about truncated characters in debug-info

#include <map>
#include <string>

typedef std::map<std::string, std::string>	ConfigData;



/**
 * class CConfig - Describe me!
 *
 * @author unknown
 */
class CConfig
{
public:
	CConfig();
	void ReadConfigFile(const char* cFilename);
	void GetString(char* cValue, const char* cParam);
	int GetInteger(const char* cParam);
	float GetFloat(const char* cParam);

private:
	ConfigData	m_oConfigData;

};

extern CConfig Config;

#endif // __CONFIG_H__

