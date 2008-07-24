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


#ifndef __CSPSIM_THEATER_OBJECTIVE_H__
#define __CSPSIM_THEATER_OBJECTIVE_H__

#include <csp/cspsim/theater/FeatureGroup.h>

namespace csp {

/**
 * class Objective.
 *
 * An Objective is a special kind of FeatureGroup that has special
 * strategic importance in a campaign.
 */
class Objective: public FeatureGroup {
public:
	char m_StrategicValueModifier;

	CSP_DECLARE_OBJECT(Objective)

	Objective();

	virtual ~Objective();

};

} // namespace csp

#endif // __CSPSIM_THEATER_OBJECTIVE_H__


