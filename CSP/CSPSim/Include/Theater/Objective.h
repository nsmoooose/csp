// Combat Simulator Project
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
 * @file Objective.h
 *
 **/


#ifndef __THEATER_OBJECTIVE_H__
#define __THEATER_OBJECTIVE_H__


#include "Theater/FeatureGroup.h"


/**
 * class Objective.
 *
 * An Objective is a special kind of FeatureGroup that has special
 * strategic importance in a campaign.  
 */
class Objective: public FeatureGroup { 
public:
	char m_StrategicValueModifier;

	SIMDATA_OBJECT(Objective, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(Objective, FeatureGroup)
		SIMDATA_XML("strategic_value_modifier", Objective::m_StrategicValueModifier, true)
	END_SIMDATA_XML_INTERFACE

	Objective();

	virtual ~Objective();

};



#endif // __THEATER_OBJECTIVE_H__


