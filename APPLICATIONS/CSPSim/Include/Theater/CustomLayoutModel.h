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
 * @file CustomLayoutModel.h
 *
 **/


#ifndef __THEATER_CUSTOMLAYOUTMODEL_H__
#define __THEATER_CUSTOMLAYOUTMODEL_H__


#include "Theater/FeatureGroupModel.h"

class FeatureLayout;

/**
 * class CustomLayoutModel (STATIC)
 *
 * A static collection of multiple FeatureModels that form part of
 * a FeatureGroup, where the position and orientation of each model
 * is specified in XML using a FeatureLayout.
 *
 */
class CustomLayoutModel: public FeatureGroupModel { 
protected:
	simdata::Link<FeatureLayout>::vector m_FeatureLayout;
	mutable int m_FeatureCount;

public:
	SIMDATA_STATIC_OBJECT(CustomLayoutModel, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(CustomLayoutModel)
		SIMDATA_XML("layout", CustomLayoutModel::m_FeatureLayout, true)
	END_SIMDATA_XML_INTERFACE

	/**
	 * Return the total number of features below this group.
	 */
	virtual int getFeatureCount() const;

	/**
	 * Make Feature instances for all features below this group.
	 */
	void makeFeatures(std::vector<Feature> &features, int value) const;

	/**
	 * Add all the features below this group to the scene graph of a FeatureGroup.
	 */
	void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction);

	CustomLayoutModel();

	virtual ~CustomLayoutModel();

	virtual void serialize(simdata::Archive &archive) {
		FeatureGroupModel::serialize(archive);
		archive(m_FeatureLayout);
	}

};


#endif // __THEATER_CUSTOMLAYOUTMODEL_H__


