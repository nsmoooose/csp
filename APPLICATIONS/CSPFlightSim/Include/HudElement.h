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
 * @file HudElement.h
 *
 **/

#ifndef HUD_ELEMENT_H
#define HUD_ELEMENT_H

#include <string>

#include <osg/Vec3>


// every "object" in the hud should inherite from this abstract class

/**
 * class HudElement - Describe me!
 *
 * @author unknown
 */
class HudElement
{
protected:
    std::string m_selementName;
	osg::Vec3 m_position;
public:
	 HudElement(){;};
	 HudElement(std::string const & p_selementName, osg::Vec3 const & p_position);
	 virtual ~HudElement();
	 virtual void OnUpdate() = 0;
	 std::string GetHudElementName() const;
	 //virtual void drawImmediateMode(osg::State& state) {};
	 //virtual const bool computeBound() const { return false;}
};

#endif // __HUDELEMENT_H__


