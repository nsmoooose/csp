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
 * @file BaseSymbol.h
 *
 **/

#ifndef __BASESYMBOL_H__
#define __BASESYMBOL_H__


/**
 * class BaseSymbol - Describe me!
 *
 * @author unknown
 */
class BaseSymbol
{
public:
	BaseSymbol();
	virtual std::string SetCommand(std::deque<std::string> & args) = 0;
	virtual std::string GetCommand(std::deque<std::string> & args) = 0;

	friend std::ostream & operator << (std::ostream & os, const BaseSymbol& sym);

	void setIdentifier(std::string _Identifier);

protected:
	std::string m_Identifier;
};




#endif // __BASESYMBOL_H__

