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
 * @file FeatureLayout.h
 *
 **/


#ifndef __THEATER_FEATURELAYOUT_H__
#define __THEATER_FEATURELAYOUT_H__

#include "Theater/Feature.h"

#include <SimData/Path.h>
#include <SimData/Object.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/Vector3.h>

#include <vector>

class FeatureModel;
class FeatureSceneGroup;
class LayoutTransform;
class ElevationCorrection;


/**
 * class FeatureLayout (ANONYMOUS)
 *
 * Positions and orients a FeatureModel.
 */
class FeatureLayout: public simdata::Object {

protected:

	simdata::Link<FeatureModel> m_FeatureModel;
	float m_X, m_Y;
	float m_Orientation;
	char m_ValueModifier;
public:

	SIMDATA_OBJECT(FeatureLayout, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(FeatureLayout)
		SIMDATA_XML("model", FeatureLayout::m_FeatureModel, true)
		SIMDATA_XML("x", FeatureLayout::m_X, true)
		SIMDATA_XML("y", FeatureLayout::m_Y, true)
		SIMDATA_XML("orientation", FeatureLayout::m_Orientation, true)
		SIMDATA_XML("value", FeatureLayout::m_ValueModifier, false)
	END_SIMDATA_XML_INTERFACE

	FeatureLayout();

	virtual ~FeatureLayout();

	/**
	 * Get the FeatureModel.
	 */
	simdata::Ref<FeatureModel const> getFeatureModel() const;

	/** 
	 * Get the relative position of the FeatureModel.
	 */
	simdata::Vector3 getPosition() const { return simdata::Vector3(m_X, m_Y, 0.0); }

	/**
	 * Get the relative orientation of the FeatureModel.
	 */
	float getOrientation() const { return m_Orientation; }

	/**
	 * Get the value modifier of the FeatureModel.
	 *
	 * Value modifiers are cummulative, and are used to adjust
	 * the value of child models to suit their significance as 
	 * part of a larger model.  For example, the same building
	 * that is insignificant as part of a town might be highly
	 * significant as part of a chemical weapons factory.
	 */
	char getValueModifier() const { return m_ValueModifier; }

	/**
	 * Add the FeatureModel to the scene graph of a FeatureGroup,
	 * adding the layout parameters to the cummulative transform.
	 */
	void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction);

	/**
	 * Make Feature instances for all features in the FeatureModel,
	 * adding in the local value modifier.
	 */
	void makeFeatures(std::vector<Feature> &features, int value) const;

	virtual void serialize(simdata::Archive &archive) {
		Object::serialize(archive);
		archive(m_FeatureModel);
		archive(m_X);
		archive(m_Y);
		archive(m_Orientation);
		archive(m_ValueModifier);
	}

};



#endif // __THEATER_FEATURELAYOUT_H__

