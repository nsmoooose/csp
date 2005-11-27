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
 * @file FeatureSceneGroup.h
 *
 **/


#ifndef __CSPSIM_THEATER_FEATURESCENEGROUP_H__
#define __CSPSIM_THEATER_FEATURESCENEGROUP_H__

#include <csp/csplib/util/Namespace.h>
#include <osg/PositionAttitudeTransform>

CSP_NAMESPACE

/**
 * Scene graph class to encapsulate and position and orientation
 * of one 3D model contained in a FeatureGroup.  The origin and
 * orientation is relative to the parent FeatureGroup.
 */
class FeatureSceneGroup: public osg::PositionAttitudeTransform {
public:
};

CSP_NAMESPACE_END

#endif // __CSPSIM_THEATER_FEATURESCENEGROUP_H__


