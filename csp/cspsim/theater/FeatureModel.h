#pragma once
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
 * @file FeatureModel.h
 *
 **/

#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/cspsim/Export.h>

namespace csp {

class Feature;
class LayoutTransform;
class ElevationCorrection;
class FeatureSceneGroup;


/**
 * class FeatureModel (STATIC, ABSTRACT)
 *
 * Base class for constructing FeatureGroups.  FeatureModels can
 * form trees, with branch nodes (FeatureGroupModel) and leaf
 * nodes (FeatureObjectModel).  This allow hierarchical construction
 * of FeatureGroups out both primitive and complex components.
 *
 */
class CSPSIM_EXPORT FeatureModel: public Object {
public:
	CSP_DECLARE_ABSTRACT_OBJECT(FeatureModel)

	/**
	 * Add all children to the scene graph of the FeatureGroup.
	 *
	 * @param group The root of the FeatureGroup scene graph.
	 * @param transform The cummulative transform for this model.
	 * @param correction A helper for adjusting the Z-coordinate of 3D objects to match the local terrain elevation.
	 */
	virtual void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction);

	/**
	 * Return the number of features in this model.
	 */
	virtual int getFeatureCount() const { return 1; }

	/**
	 * Construct Feature instances for each feature in this model.
	 *
	 * @param features Collects the new Feature instances.
	 * @param value the cummulative value modifier from all parents.
	 */
	virtual void makeFeatures(std::vector<Feature> &features, int value) const;
};

} // namespace csp
