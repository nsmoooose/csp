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
 * @file Theater.h
 *
 * A theater is a large geographic area it which military operations are
 * conducted (dictionary.com).  In CSPSim this include the terrain,
 * natural features, and static man-made objects.  The classes here are
 * used to orginize and manage this data.
 *
 **/



/*
 * TODO The class and member variable names are not set in stone,
 *      especially if we can find a clearer set of terms.
 */


#ifndef __THEATER_H__
#define __THEATER_H__

#include <list>
#include <string>
#include <algorithm>

#include <SimData/Ref.h>
#include <SimData/Path.h>
#include <SimData/Object.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/osg.h>

#include "ObjectModel.h"
#include "TerrainObject.h"


/**
 * clss DamageModifier (STATIC)
 *
 * Damage modifiers reflect the resistance of an object to 
 * various types of weapons.
 *
 */
class DamageModifier: public simdata::Object { 
public:
	char m_Incendiary;
	char m_HighExplosive;
	char m_Penetrating;
	char m_ArmorPiercing;
	char m_SmallArms;
	// etc
	
	SIMDATA_OBJECT(DamageModifier, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(DamageModifier)
		SIMDATA_XML("incediary", DamageModifier::m_Incendiary, false)
		SIMDATA_XML("high_explosive", DamageModifier::m_HighExplosive, false)
		SIMDATA_XML("penetrating", DamageModifier::m_Penetrating, false)
		SIMDATA_XML("armor_piercing", DamageModifier::m_ArmorPiercing, false)
		SIMDATA_XML("small_arms", DamageModifier::m_SmallArms, false)
	END_SIMDATA_XML_INTERFACE

	DamageModifier() {}
	virtual ~DamageModifier() {}

	virtual void pack(simdata::Packer& p) const {
		Object::pack(p);
		p.pack(m_Incendiary);
		p.pack(m_HighExplosive);
		p.pack(m_Penetrating);
		p.pack(m_ArmorPiercing);
		p.pack(m_SmallArms);
	}
	virtual void unpack(simdata::UnPacker& p) {
		Object::unpack(p);
		p.unpack(m_Incendiary);
		p.unpack(m_HighExplosive);
		p.unpack(m_Penetrating);
		p.unpack(m_ArmorPiercing);
		p.unpack(m_SmallArms);
	}
	virtual void postCreate() {}
};


/**
 * Scene graph class to encapsulate and position and orientation
 * of one 3D model contained in a FeatureGroup.  The origin and 
 * orientation is relative to the parent FeatureGroup.
 */
class FeatureSceneModel: public osg::PositionAttitudeTransform {
};


/**
 * Scene graph class to encapsulate and position and orientation
 * of a FeatureGroup.  Ultimately this will probably include an
 * osg::Imposter node.
 */
class FeatureSceneGroup: public osg::PositionAttitudeTransform {
};



/**
 * class Feature
 *
 * A class representing an individual fixed object in the theater.  Examples
 * of Features are buildings, signs, runway segments, bridge segments, etc.
 * Groups of Features form FeatureGroups that are the primary building blocks
 * for constructing a theater.  Each type of feature shares many common properties
 * (such as the 3D model).  This class has links to this static data, but also
 * contains state data specific to individual features (such as the damage state
 * and total strategic value).
 */
class Feature {
protected:
	char damage;
	char value;
public:
	Feature(char value_ = 0): damage(0), value(value_) {}
};



/**
 * class ElevationCorrection
 *
 * A helper class for correcting the elevation of static objects to match
 * the local terrain.
 */
class ElevationCorrection {
	TerrainObject *m_Terrain;
	double m_X, m_Y;
	ElevationCorrection() {}
public:
	ElevationCorrection(TerrainObject *terrain, double x=0.0, double y=0.0) {
		assert(terrain);
		m_Terrain = terrain;
		m_X = x;
		m_Y = y;
	}
	void apply(simdata::Vector3 &position) const {
		position.z = m_Terrain->getElevation(position.x + m_X, position.y + m_Y);
	}
};



/**
 * class LayoutTransform
 *
 * A helper class for accumulationg and applying 2D displacements and rotations.
 */
class LayoutTransform: simdata::Matrix3 {
	double m_X, m_Y, m_Angle;
	double c, s;
	LayoutTransform() {}
public:
	LayoutTransform(double x, double y, double angle) {
		m_X = x;
		m_Y = y;
		m_Angle = angle;
		c = cos(angle);
		s = sin(angle);
	}
	LayoutTransform operator *(LayoutTransform const &t) const {
		double angle = m_Angle + t.m_Angle;
		double x = m_X + t.m_X * c + t.m_Y * s;
		double y = m_Y - t.m_X * s + t.m_Y * c;
		return LayoutTransform(x, y, angle);
	}
	void operator()(FeatureSceneModel *model, ElevationCorrection const &correction) const {
		assert(model);
		simdata::Vector3 position(m_X, m_Y, 0.0);
		correction.apply(position);
		model->setPosition(simdata::toOSG(position));
		osg::Quat q;
		q.makeRotate(m_Angle, 0.0, 0.0, 1.0);
		model->setAttitude(q);
	}
};



/**
 * class FeatureModel (STATIC, ABSTRACT)
 *
 * Base class for constructing FeatureGroups.  FeatureModels can
 * form trees, with branch nodes (FeatureGroupModel) and leaf
 * nodes (FeatureObjectModel).  This allow hierarchical construction
 * of FeatureGroups out both primitive and complex components.
 *
 */
class FeatureModel: public simdata::Object {
public:
	/**
	 * Add all children to the scene graph of the FeatureGroup.
	 *
	 * @param group The root of the FeatureGroup scene graph.
	 * @param transform The cummulative transform for this model.
	 * @param correction A helper for adjusting the Z-coordinate of 3D objects to match the local terrain elevation.
	 */
	virtual void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {};
	
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
	virtual void makeFeatures(std::vector<Feature> &features, int value) const {};
};


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
	void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {
		assert(group != 0);
		FeatureSceneModel *model = new FeatureSceneModel;
		transform(model, correction);
		model->addChild(m_ObjectModel->getModel().get());
		group->addChild(model);
	}

	/**
	 * Construct a new Feature instance for this feature.
	 */
	virtual void makeFeatures(std::vector<Feature> &features, int value) const {
		features.push_back(Feature(value + int(m_Value)));
	}

	SIMDATA_OBJECT(FeatureObjectModel, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(FeatureObjectModel)
		SIMDATA_XML("model", FeatureObjectModel::m_ObjectModel, false)
		SIMDATA_XML("hit_points", FeatureObjectModel::m_HitPoints, false)
		SIMDATA_XML("value_modifier", FeatureObjectModel::m_Value, false)
	END_SIMDATA_XML_INTERFACE

	FeatureObjectModel(): m_HitPoints(10), m_Value(0) {}
	virtual ~FeatureObjectModel() {}

	virtual void pack(simdata::Packer& p) const {
		Object::pack(p);
		p.pack(m_HitPoints);
		p.pack(m_Value);
		p.pack(m_ObjectModel);
	}
	virtual void unpack(simdata::UnPacker& p) {
		Object::unpack(p);
		p.unpack(m_HitPoints);
		p.unpack(m_Value);
		p.unpack(m_ObjectModel);
	}
	virtual void postCreate() {}
};



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

	FeatureLayout() { 
		m_X = 0.0;
		m_Y = 0.0;
		m_Orientation = 0.0;
		m_ValueModifier = char(0);
	}
	virtual ~FeatureLayout() {}

	/**
	 * Get the FeatureModel.
	 */
	simdata::Ref<FeatureModel const> getFeatureModel() const { return m_FeatureModel; } 

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
	inline void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {
		LayoutTransform newTransform = transform * LayoutTransform(m_X, m_Y, m_Orientation); 
		m_FeatureModel->addSceneModel(group, newTransform, correction);
	}

	/**
	 * Make Feature instances for all features in the FeatureModel,
	 * adding in the local value modifier.
	 */
	void makeFeatures(std::vector<Feature> &features, int value) const {
		m_FeatureModel->makeFeatures(features, value + int(m_ValueModifier));
	}

	virtual void pack(simdata::Packer& p) const {
		Object::pack(p);
		p.pack(m_FeatureModel);
		p.pack(m_X);
		p.pack(m_Y);
		p.pack(m_Orientation);
		p.pack(m_ValueModifier);
	}
	virtual void unpack(simdata::UnPacker& p) {
		Object::unpack(p);
		p.unpack(m_FeatureModel);
		p.unpack(m_X);
		p.unpack(m_Y);
		p.unpack(m_Orientation);
		p.unpack(m_ValueModifier);
	}
	virtual void postCreate() {}
};



/**
 * class FeatureGroupModel (STATIC, ABSTRACT)
 *
 * A static collection of multiple FeatureModels that form part of 
 * a FeatureGroup. 
 */
class FeatureGroupModel: public FeatureModel {
public:
	FeatureGroupModel() {}
	virtual ~FeatureGroupModel() {}
};


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
	SIMDATA_OBJECT(CustomLayoutModel, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(CustomLayoutModel)
		SIMDATA_XML("layout", CustomLayoutModel::m_FeatureLayout, true)
	END_SIMDATA_XML_INTERFACE

	/**
	 * Return the total number of features below this group.
	 */
	virtual int getFeatureCount() const { 
		if (m_FeatureCount == 0) {
			simdata::Link<FeatureLayout>::vector::const_iterator i = m_FeatureLayout.begin();
			simdata::Link<FeatureLayout>::vector::const_iterator j = m_FeatureLayout.end();
			for (; i != j; i++) m_FeatureCount += (*i)->getFeatureModel()->getFeatureCount();
		}
		return m_FeatureCount;
	}

	/**
	 * Make Feature instances for all features below this group.
	 */
	void makeFeatures(std::vector<Feature> &features, int value) const {
		simdata::Link<FeatureLayout>::vector::const_iterator i = m_FeatureLayout.begin();
		simdata::Link<FeatureLayout>::vector::const_iterator j = m_FeatureLayout.end();
		for (; i != j; i++) {
			(*i)->makeFeatures(features, value);
		}
	}

	/**
	 * Add all the features below this group to the scene graph of a FeatureGroup.
	 */
	void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {
		simdata::Link<FeatureLayout>::vector::iterator i = m_FeatureLayout.begin();
		simdata::Link<FeatureLayout>::vector::iterator j = m_FeatureLayout.end();
		for (; i != j; i++) {
			(*i)->addSceneModel(group, transform, correction);
		}	
	}

	CustomLayoutModel() { m_FeatureCount = 0; }
	virtual ~CustomLayoutModel() {}

	virtual void pack(simdata::Packer& p) const {
		FeatureGroupModel::pack(p);
		p.pack(m_FeatureLayout);
	}
	virtual void unpack(simdata::UnPacker& p) {
		FeatureGroupModel::unpack(p);
		p.unpack(m_FeatureLayout);
	}
};


/**
 * class RandomLayoutModel (STATIC)
 *
 * A feature layout that places a set of features repeatedly
 * in random locations within a given area.
 *
 * XXX not yet implemented.
 */
class RandomLayoutModel: public FeatureGroupModel {
	simdata::Link<FeatureModel>::vector m_FeatureTypes;
	std::vector<float> m_Ratio;
	float m_Density;
	float m_MinimumSpacing;
	float m_Seed;
	
public:
	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(RandomLayoutModel)
		SIMDATA_XML("features", RandomLayoutModel::m_FeatureTypes, true)
		SIMDATA_XML("ratio", RandomLayoutModel::m_Ratio, true)
		SIMDATA_XML("density", RandomLayoutModel::m_Density, true)
		SIMDATA_XML("minimum_spacing", RandomLayoutModel::m_MinimumSpacing, true)
		SIMDATA_XML("seed", RandomLayoutModel::m_Seed, false)
	END_SIMDATA_XML_INTERFACE


	/**
	 * Add all the features below this group to the scene graph of a FeatureGroup.
	 */
	void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction) {
	}

	RandomLayoutModel() {
		m_Seed = 0;
	}
	virtual ~RandomLayoutModel() {}

	virtual void pack(simdata::Packer& p) const {
		FeatureGroupModel::pack(p);
		p.pack(m_FeatureTypes);
		p.pack(m_Ratio);
		p.pack(m_Density);
		p.pack(m_MinimumSpacing);
		p.pack(m_Seed);
	}
	virtual void unpack(simdata::UnPacker& p) {
		FeatureGroupModel::unpack(p);
		p.unpack(m_FeatureTypes);
		p.unpack(m_Ratio);
		p.unpack(m_Density);
		p.unpack(m_MinimumSpacing);
		p.unpack(m_Seed);
	}
};



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
class FeatureGroup: public simdata::Object {
protected:
	simdata::Link<FeatureModel> m_Model;
	float m_X, m_Y;
	float m_Orientation;

	osg::ref_ptr<FeatureSceneGroup> m_SceneGroup;
	std::vector<Feature> m_Features;

public:
	SIMDATA_OBJECT(FeatureGroup, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(FeatureGroup)
		SIMDATA_XML("model", FeatureGroup::m_Model, true)
		SIMDATA_XML("x", FeatureGroup::m_X, true)
		SIMDATA_XML("y", FeatureGroup::m_Y, true)
		SIMDATA_XML("orientation", FeatureGroup::m_Orientation, true)
	END_SIMDATA_XML_INTERFACE

	/**
	 * Return the absolute position of this FeatureGroup in the Theater.
	 */
	simdata::Vector3 getPosition() {
		return simdata::Vector3(m_X, m_Y, 0.0);
	}

	/**
	 * Return the scene graph for this FeatureGroup.  The scene graph, if 
	 * it exists, contains all the 3D models of the child FeatureModels and 
	 * is positioned relative to the origin of the Battlefield cell that 
	 * contains it.
	 */
	FeatureSceneGroup* getSceneGroup() {
		return m_SceneGroup.get();
	}

	/**
	 * Notification that the FeatureGroup has moved into or out of any visual
	 * bubbles in the battlefield.
	 */
	virtual void setVisible(bool visible) {
		if (!visible) {
			std::cout << "FeatureGroup @ " << m_SceneGroup->getPosition() << ": removing " << m_Model->getFeatureCount() << " feature(s) from the scene\n";
			// our scene graph is no longer needed
			m_SceneGroup = NULL;
		}
	}
	
	/**
	 * Construct a scene graph containing all the child FeatureModels.
	 * 
	 * The scene graph will be positioned relative to the supplied origin (which 
	 * is usually the origin of the Battlefield cell that contains the 
	 * FeatureGroup.  Elevation corrections are computed for each individual
	 * feature using the supplied terrain model.
	 */
	virtual FeatureSceneGroup* makeSceneGroup(simdata::Vector3 const &origin, TerrainObject *terrain) {
		assert(!m_SceneGroup);
		m_SceneGroup = new FeatureSceneGroup();
		m_Model->addSceneModel(m_SceneGroup.get(), LayoutTransform(0.0, 0.0, m_Orientation), ElevationCorrection(terrain, m_X, m_Y));
		m_SceneGroup->setPosition(osg::Vec3(m_X-origin.x, m_Y-origin.y, -origin.z));
		m_SceneGroup->setAttitude(osg::Quat(m_Orientation, osg::Z_AXIS));
		std::cout << "FeatureGroup @ " << m_SceneGroup->getPosition() << ": adding " << m_Model->getFeatureCount() << " feature(s) to the scene\n";
		return m_SceneGroup.get();
	}

	/**
	 * Aggregate the FeatureGroup. 
	 */
	virtual void aggregate() {
	}

	/**
	 * Deaggregate the FeatureGroup. 
	 */
	virtual void deaggregate() {
	}

	FeatureGroup() {
		m_X = 0.0;
		m_Y = 0.0;
		m_Orientation = 0.0;
		m_SceneGroup = NULL;
	}
	virtual ~FeatureGroup() {}

	virtual void pack(simdata::Packer& p) const {
		Object::pack(p);
		p.pack(m_Model);
		p.pack(m_X);
		p.pack(m_Y);
		p.pack(m_Orientation);
	}
	virtual void unpack(simdata::UnPacker& p) {
		Object::unpack(p);
		p.unpack(m_Model);
		p.unpack(m_X);
		p.unpack(m_Y);
		p.unpack(m_Orientation);
	}
	virtual void postCreate() {
		m_Features.reserve(m_Model->getFeatureCount());
		m_Model->makeFeatures(m_Features, 0);
	}
};


/**
 * class Objective.
 *
 * An Objective is a special kind of FeatureGroup that has special
 * strategic importance in a campaign.  
 */
class Objective: public FeatureGroup { 
public:
	char m_StrategicValueModifier;

	SIMDATA_OBJECT(Objective, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(Objective, FeatureGroup)
		SIMDATA_XML("strategic_value_modifier", Objective::m_StrategicValueModifier, true)
	END_SIMDATA_XML_INTERFACE

	Objective() {}
	virtual ~Objective() {}

	virtual void pack(simdata::Packer& p) const {
		FeatureGroup::pack(p);
		p.pack(m_StrategicValueModifier);
	}
	virtual void unpack(simdata::UnPacker& p) {
		FeatureGroup::unpack(p);
		p.unpack(m_StrategicValueModifier);
	}
	virtual void postCreate() {}
};


/**
 * class ObjectiveList (STATIC)
 *
 * A collection of all Objectives in a Theater.
 */
class FeatureGroupList: public simdata::Object {
	simdata::Link<FeatureGroup>::vector m_FeatureGroups;
public:

	SIMDATA_OBJECT(FeatureGroupList, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(FeatureGroupList)
		SIMDATA_XML("feature_groups", FeatureGroupList::m_FeatureGroups, true)
	END_SIMDATA_XML_INTERFACE

	FeatureGroupList() {}
	virtual ~FeatureGroupList() {}

	virtual void pack(simdata::Packer& p) const {
		Object::pack(p);
		p.pack(m_FeatureGroups);
	}
	virtual void unpack(simdata::UnPacker& p) {
		Object::unpack(p);
		p.unpack(m_FeatureGroups);
	}
	virtual void postCreate() {}

	/**
	 * Get the list of FeatureGroups.
	 */
	simdata::Link<FeatureGroup>::vector const & getFeatureGroups() { return m_FeatureGroups; }
};


/**
 * class Theater
 *
 * A Theater represents an area of military operations.  It consists of static
 * data representing the terrain, natural features, and man-made structures.
 */
class Theater: public simdata::Object
{
	simdata::Link<FeatureGroupList> m_FeatureGroupList;
	simdata::Link<TerrainObject> m_Terrain;
public:

	SIMDATA_OBJECT(Theater, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(Theater)
		SIMDATA_XML("feature_group_list", Theater::m_FeatureGroupList, true)
		SIMDATA_XML("terrain", Theater::m_Terrain, true)
	END_SIMDATA_XML_INTERFACE

	/**
	 * Return a list of all FeatureGroups in the theater.  Currently only
	 * Objectives are returned.
	 */
	simdata::Ref<FeatureGroup>::vector getAllFeatureGroups() {
		simdata::Link<FeatureGroup>::vector const &groups = m_FeatureGroupList->getFeatureGroups();
		simdata::Ref<FeatureGroup>::vector result;
		// translate from Link<> list to Ref<> list.  XXX should be a simdata function?
		result.reserve(groups.size());
		simdata::Link<FeatureGroup>::vector::const_iterator i = groups.begin();
		for (; i != groups.end(); i++) {
			result.push_back(*i);
		}
		return result;
	}

	Theater() {}
	virtual ~Theater() {}

	virtual void pack(simdata::Packer& p) const {
		Object::pack(p);
		p.pack(m_FeatureGroupList);
		p.pack(m_Terrain);
	}
	virtual void unpack(simdata::UnPacker& p) {
		Object::unpack(p);
		p.unpack(m_FeatureGroupList);
		p.unpack(m_Terrain);
	}
	virtual void postCreate() {}

	/**
	 * Get the TerrainObject used by the theater.
	 */
	simdata::Ref<TerrainObject> getTerrain() { return m_Terrain; }

protected:
};

#endif // __THEATER_H__


