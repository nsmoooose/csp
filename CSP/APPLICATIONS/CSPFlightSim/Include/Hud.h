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
 * @file Hud.h
 *
 **/

#ifndef __HUD_H__
#define __HUD_H__

#include <string>

// testing various osg objects
#include <osg/PositionAttitudeTransform>

#include "HudTextElement.h"
#include "TypesVector3.h"


/**
 * class Hud - Describe me!
 *
 * @author unknown
 */
class Hud : public osg::PositionAttitudeTransform
{
	HudTextElement ReadHudTextElement(std::istream & p_istream) const;
public:
	Hud(StandardVector3 const & p_direction);
	virtual ~Hud();	
	virtual void BuildHud();
	virtual void BuildHud(std::string const & p_hudFileName);
	void OnUpdate();
};


#endif // __HUD_H__

