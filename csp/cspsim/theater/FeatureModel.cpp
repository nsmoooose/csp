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
 * @file FeatureModel.cpp
 *
 **/


#include <csp/cspsim/theater/FeatureModel.h>

namespace csp {

CSP_XML_BEGIN(FeatureModel)
CSP_XML_END

void FeatureModel::addSceneModel(FeatureSceneGroup *, LayoutTransform const &, ElevationCorrection const &) {
}

void FeatureModel::makeFeatures(std::vector<Feature> &, int) const {
}

} // namespace csp

