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
 * @file FeatureGroup.h
 *
 **/


#ifndef __THEATER_FEATUREGROUP_H__
#define __THEATER_FEATUREGROUP_H__


#include "Feature.h"
#include "FeatureModel.h"

#include <SimData/Ref.h>
#include <SimData/Path.h>
#include <SimData/Object.h>
#include <SimData/Vector3.h>
#include <SimData/GeoPos.h>
#include <SimData/InterfaceRegistry.h>

#include <osg/Vec3>
#include <osg/ref_ptr>

#include "SimObject.h"

#include <vector>

class FeatureSceneGroup;
class TerrainObject;
class Projection;


/**
 * class FeatureGroup
 *
 * This is the basic unit of static objects in a Theater.  FeatureGroups
 * contain multiple individual FeaturesModels laid out with respect to a 
 * common origin.  Each FeatureGroup instance shares a common (static) 
 * layout of individual features, but can be placed and oriented 
 * independently anywhere in the Theater.  FeatureGroup also stores 
 * additional state data unique to each instance, and ultimately will be
 * the basis for constructing complex fixed installations that incorporate
 * AI and other program logic (such as airbases with air traffic 
 * controllers and aircraft maintenance facilities).
 */
class FeatureGroup: public SimObject {
protected:
	simdata::Link<FeatureModel> m_Model;
	simdata::LLA m_Position;
	float m_Orientation;
	float m_X, m_Y;

	osg::ref_ptr<FeatureSceneGroup> m_SceneGroup;
	std::vector<Feature> m_Features;

public:

	typedef simdata::Ref<FeatureGroup> Ref;

	SIMDATA_OBJECT(FeatureGroup, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(FeatureGroup)
		SIMDATA_XML("model", FeatureGroup::m_Model, true)
		SIMDATA_XML("position", FeatureGroup::m_Position, true)
		SIMDATA_XML("orientation", FeatureGroup::m_Orientation, true)
	END_SIMDATA_XML_INTERFACE
		//SIMDATA_XML("x", FeatureGroup::m_X, true)
		//SIMDATA_XML("y", FeatureGroup::m_Y, true)

	/**
	 * Return the absolute position of this FeatureGroup in the Theater.
	 */
	virtual simdata::Vector3 getGlobalPosition() const;

	/**
	 * Return the scene graph for this FeatureGroup.  The scene graph, if 
	 * it exists, contains all the 3D models of the child FeatureModels and 
	 * is positioned relative to the origin of the Battlefield cell that 
	 * contains it.
	 */
	virtual FeatureSceneGroup* getSceneGroup();

	/**
	 * Notification that the FeatureGroup has moved into a visual
	 * bubble in the battlefield.
	 */
	virtual void onEnterScene();

	/**
	 * Notification that the FeatureGroup has moved out of any visual
	 * bubbles in the battlefield.
	 */
	virtual void onLeaveScene();
	
	/**
	 * Construct a scene graph containing all the child FeatureModels.
	 * 
	 * The scene graph will be positioned relative to the supplied origin (which 
	 * is usually the origin of the Battlefield cell that contains the 
	 * FeatureGroup.  Elevation corrections are computed for each individual
	 * feature using the supplied terrain model.
	 */
	virtual FeatureSceneGroup* makeSceneGroup(simdata::Vector3 const &origin, TerrainObject *terrain);

	/**
	 * Aggregate the FeatureGroup. 
	 */
	virtual void onAggregate();

	/**
	 * Deaggregate the FeatureGroup. 
	 */
	virtual void onDeaggregate();

	FeatureGroup();

	virtual ~FeatureGroup();

	virtual void postCreate();

	virtual void project(Projection const &);
};



#endif // __THEATER_FEATUREGROUP_H__


