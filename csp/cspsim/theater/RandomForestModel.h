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
 * @file RandomForest.h
 *
 **/

#include <csp/cspsim/theater/Feature.h>
#include <csp/cspsim/theater/FeatureQuad.h>
#include <csp/cspsim/theater/FeatureGroupModel.h>

#include <csp/csplib/data/Link.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Vector3.h>

#include <osg/Vec3>

#include <vector>

namespace osg {
	class Geometry;
}

namespace csp {

class FeatureSceneGroup;
class LayoutTransform;
class ElevationCorrection;
class IsoContour;


/**
 * class RandomForestModel
 *
 * A feature model that randomly distributes copies of several
 * X shaped geometric primitives within a give area (currently
 * a circle).   Intended for constructing forests.
 */
class RandomForestModel: public FeatureGroupModel {
	Link<FeatureQuad>::vector m_Models;
	std::vector<float> m_Density;
	float m_MinimumSpacing;
	int m_Seed;
	Link<IsoContour> m_IsoContour;

	std::vector<std::vector<Vector3> > m_Offsets;
	
	osg::Geometry *construct(Ref<FeatureQuad> quad, std::vector<osg::Vec3> const &position) const;

public:
	CSP_DECLARE_OBJECT(RandomForestModel)

	/**
	 * Return the total number of features below this group.
	 */
	virtual int getFeatureCount() const;

	/**
	 * Make Feature instances for all features below this group.
	 */
	void makeFeatures(std::vector<Feature> &features, int value) const;

	RandomForestModel();

	virtual ~RandomForestModel();

	virtual void postCreate();

	/**
	 * Add all the features below this group to the scene graph of a FeatureGroup.
	 */
	void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction);

};

} // namespace csp
