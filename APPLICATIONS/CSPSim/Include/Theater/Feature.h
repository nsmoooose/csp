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
 * @file Feature.h
 *
 **/


#ifndef __THEATER_FEATURE_H__
#define __THEATER_FEATURE_H__


#include <SimData/Ref.h>

class FeatureObjectModel;


/**
 * class Feature
 *
 * A class representing an individual fixed object in the theater.  Examples
 * of Features are buildings, signs, runway segments, bridge segments, etc.
 * Groups of Features form FeatureGroups that are the primary building blocks
 * for constructing a theater.  Each type of feature shares many common properties
 * (such as the 3D model).  This class has links to this static data, but also
 * contains state data specific to individual features (such as the damage state
 * and total strategic value).
 */
class Feature {
protected:
	char m_Damage;
	char m_Value;
	simdata::Ref<FeatureObjectModel const> m_Model;
public:
	Feature(FeatureObjectModel const *model, char value_ = 0);
};


#endif // __THEATER_FEATURE_H__


