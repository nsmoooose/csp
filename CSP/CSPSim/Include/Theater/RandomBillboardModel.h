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
 * @file RandomBillboardModel.h
 *
 **/


#ifndef __THEATER_RANDOMBILLBOARDMODEL_H__
#define __THEATER_RANDOMBILLBOARDMODEL_H__


#include "Theater/FeatureGroupModel.h"

#include <SimData/Link.h>

#include <vector>

class FeatureQuad;
class FeatureSceneGroup;
class LayoutTransform;
class ElevationCorrection;
class IsoContour;

/**
 * class RandomBillboardModel (STATIC)
 *
 * A feature layout that places a set of features repeatedly
 * in random locations within a given area.
 */
class RandomBillboardModel: public FeatureGroupModel {
	simdata::Link<FeatureQuad>::vector m_Models;
	std::vector<float> m_Density;
	float m_MinimumSpacing;
	int m_Seed;
	std::vector<std::vector<simdata::Vector3> > m_Offsets;
	simdata::Link<IsoContour> m_IsoContour;
	
public:
	SIMDATA_DECLARE_STATIC_OBJECT(RandomBillboardModel)

	/**
	 * Return the total number of features below this group.
	 */
	virtual int getFeatureCount() const;

	/**
	 * Make Feature instances for all features below this group.
	 */
	void makeFeatures(std::vector<Feature> &features, int value) const;

	RandomBillboardModel();

	virtual ~RandomBillboardModel();

	/**
	 * Generate the layout.
	 */
	virtual void postCreate();

	/**
	 * Add all the features below this group to the scene graph of a FeatureGroup.
	 */
	void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction);

};


#endif // __THEATER_RANDOMBILLBOARDMODEL_H__


