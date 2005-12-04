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


#ifndef __CSPSIM_THEATER_FEATUREOBJECTMODEL_H__
#define __CSPSIM_THEATER_FEATUREOBJECTMODEL_H__

#include <csp/cspsim/theater/Feature.h>
#include <csp/cspsim/theater/FeatureModel.h>
#include <csp/cspsim/DamageModifier.h>
#include <csp/csplib/data/Link.h>

#include <vector>

CSP_NAMESPACE

class FeatureSceneGroup;
class LayoutTransform;
class ElevationCorrection;
class ObjectModel;


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
	Link<ObjectModel> m_ObjectModel;

public:
	CSP_DECLARE_STATIC_OBJECT(FeatureObjectModel)

	FeatureObjectModel();

	virtual ~FeatureObjectModel();

	/** Get the damage modifiers for this feature type.
	 */
	virtual Ref<DamageModifier const> getDamageModifier() const { return Ref<DamageModifier const>(); }

	/** Get the damage resitance of this feature type.
	 */
	char getHitPoints() const { return m_HitPoints; }

	/** Get the base value for this feature type.
	 */
	char getValue() const { return m_Value; }

	/** Add this feature to the scene graph of a FeatureGroup.
	 *
	 * @param group The root of the FeatureGroup scene graph.
	 * @param transform The cummulative transform for this model.
	 * @param correction A helper for adjusting the Z-coordinate of 3D objects to match the local terrain elevation.
	 */
	void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction);

	/** Construct a new Feature instance for this feature.
	 */
	virtual void makeFeatures(std::vector<Feature> &features, int value) const;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_THEATER_FEATUREOBJECTMODEL_H__


