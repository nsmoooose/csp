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
 * @file FeatureObjectModel.h
 *
 **/


#ifndef __THEATER_FEATUREOBJECTMODEL_H__
#define __THEATER_FEATUREOBJECTMODEL_H__


#include "Theater/FeatureModel.h"
#include "DamageModifier.h"
#include "Feature.h"

#include <vector>


class ObjectModel;
class FeatureSceneGroup;
class LayoutTransform;
class ElevationCorrection;


/**
 * class FeatureModel (STATIC)
 *
 * Static data representing a single Feature that is shared by all 
 * instances of that feature.
 */
class FeatureObjectModel: public FeatureModel {
protected:
	char m_HitPoints;
	char m_Value;
	simdata::Link<ObjectModel> m_ObjectModel;
	
public:

	/**
	 * Get the damage modifiers for this feature type.
	 */
	virtual simdata::Ref<DamageModifier const> getDamageModifier() const { return simdata::Ref<DamageModifier const>(); }

	/**
	 * Get the damage resitance of this feature type.
	 */
	char getHitPoints() const { return m_HitPoints; }

	/**
	 * Get the base value for this feature type.
	 */
	char getValue() const { return m_Value; }

	/**
	 * Add this feature to the scene graph of a FeatureGroup.
	 *
	 * @param group The root of the FeatureGroup scene graph.
	 * @param transform The cummulative transform for this model.
	 * @param correction A helper for adjusting the Z-coordinate of 3D objects to match the local terrain elevation.
	 */
	void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction);

	/**
	 * Construct a new Feature instance for this feature.
	 */
	virtual void makeFeatures(std::vector<Feature> &features, int value) const;

	SIMDATA_STATIC_OBJECT(FeatureObjectModel, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(FeatureObjectModel)
		SIMDATA_XML("model", FeatureObjectModel::m_ObjectModel, false)
		SIMDATA_XML("hit_points", FeatureObjectModel::m_HitPoints, false)
		SIMDATA_XML("value_modifier", FeatureObjectModel::m_Value, false)
	END_SIMDATA_XML_INTERFACE

	FeatureObjectModel();

	virtual ~FeatureObjectModel();

	virtual void serialize(simdata::Archive &archive) {
		Object::serialize(archive);
		archive(m_HitPoints);
		archive(m_Value);
		archive(m_ObjectModel);
	}

};



#endif // __THEATER_FEATUREOBJECTMODEL_H__


