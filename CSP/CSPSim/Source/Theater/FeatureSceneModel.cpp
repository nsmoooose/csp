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
 * @file FeatureSceneModel.cpp
 *
 **/


#include "Theater/FeatureSceneModel.h"
#include "Theater/LayoutTransform.h"
#include "Theater/ElevationCorrection.h"

#include <osg/Quat>


FeatureSceneModel::FeatureSceneModel(LayoutTransform const &transform) {
	setPosition(transform());
	osg::Quat q;
	q.makeRotate(transform.getAngle(), 0.0, 0.0, 1.0);
	setAttitude(q);
}

FeatureSceneModel::FeatureSceneModel(LayoutTransform const &transform, ElevationCorrection const &correction) {
	setPosition(correction(transform()));
	osg::Quat q;
	q.makeRotate(transform.getAngle(), 0.0, 0.0, 1.0);
	setAttitude(q);
}
