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
 * @file TerrainSymbol.h
 *
 **/

#ifndef __TERRAINSYMBOL_H__
#define __TERRAINSYMBOL_H__


#include "BaseSymbol.h"
#include "TerrainObject.h"


/**
 * class TerrainSymbol
 *
 * @author unknown
 */
class TerrainSymbol : public BaseSymbol
{
public:
	TerrainSymbol();
	virtual ~TerrainSymbol();
	virtual std::string SetCommand(std::deque<std::string> & args);
	virtual std::string GetCommand(std::deque<std::string> & args);

	friend std::ostream & operator << (std::ostream & os, const TerrainSymbol& sym);


protected:
	TerrainObject * m_pTerrainObject;


};

#endif // __TERRAINSYMBOL_H__

