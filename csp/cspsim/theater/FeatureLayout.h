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
 * @file FeatureLayout.h
 *
 **/

#include <csp/cspsim/theater/Feature.h>

#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Vector3.h>

#include <vector>

namespace csp {

class FeatureModel;
class FeatureSceneGroup;
class LayoutTransform;
class ElevationCorrection;


/**
 * class FeatureLayout (ANONYMOUS)
 *
 * Positions (X, Y) and orients a FeatureModel.
 */
class FeatureLayout: public Object {

protected:

	Link<FeatureModel> m_FeatureModel;
	float m_X, m_Y;
	float m_Orientation;
	char m_ValueModifier;
public:

	CSP_DECLARE_OBJECT(FeatureLayout)

	FeatureLayout();

	virtual ~FeatureLayout();

	/**
	 * Get the FeatureModel.
	 */
	Ref<FeatureModel const> getFeatureModel() const;

	/**
	 * Get the relative position of the FeatureModel.
	 */
	Vector3 getPosition() const { return Vector3(m_X, m_Y, 0.0); }

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

};

} // namespace csp
