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

#include <vector>

class FeatureQuad;
class FeatureSceneGroup;
class LayoutTransform;
class ElevationCorrection;


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
	
public:
	SIMDATA_OBJECT(RandomBillboardModel, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(RandomBillboardModel)
		SIMDATA_XML("models", RandomBillboardModel::m_Models, true)
		SIMDATA_XML("density", RandomBillboardModel::m_Density, true)
		SIMDATA_XML("minimum_spacing", RandomBillboardModel::m_MinimumSpacing, true)
		SIMDATA_XML("seed", RandomBillboardModel::m_Seed, false)
	END_SIMDATA_XML_INTERFACE


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

	virtual void pack(simdata::Packer& p) const {
		FeatureGroupModel::pack(p);
		p.pack(m_Models);
		p.pack(m_Density);
		p.pack(m_MinimumSpacing);
		p.pack(m_Seed);
	}

	virtual void unpack(simdata::UnPacker& p) {
		FeatureGroupModel::unpack(p);
		p.unpack(m_Models);
		p.unpack(m_Density);
		p.unpack(m_MinimumSpacing);
		p.unpack(m_Seed);
	}

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


