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
 * @file HudGeometricElement.h
 *
 **/

#ifndef HUD_GEOMETRIC_ELEMENT_H
#define HUD_GEOMETRIC_ELEMENT_H

#include <osg/PositionAttitudeTransform>

#include "HudElement.h"

// hud geometric element


/**
 * class HudGeometricElement - Describe me!
 *
 * @author unknown
 */
class HudGeometricElement : public HudElement, public osg::PositionAttitudeTransform
{
	float m_fheadingAngle;
	float m_fpitchAngle;
public:
	 HudGeometricElement(){;};
	 HudGeometricElement(std::string const & p_selementName, osg::Vec3 const & p_position, 
		                 float const p_fheadingAngle = 0.0, float const p_fpitchAngle = 0.0);
	 virtual ~HudGeometricElement();
	 void MakeHsi();
	 void MakeFpm(float const p_radius = 10.0, float const p_segLengthH = 10.0, float const p_segLengthV = 5.0);
	 virtual void OnUpdate();
     void OnUpdate(osg::Vec3 const & p_position, float p_fangle = 0.0);
	 void OnUpdateHsi();
	 void OnUpdateFpm();
};



#endif // __HUDGEOMETRICELEMENT_H__

