// Combat Simulator Project - FlightSim Demo
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
 * @file ObjectModel.h
 *
 **/

#ifndef __OBJECTMODEL_H__
#define __OBJECTMODEL_H__

#include <vector>

#include <SimData/Enum.h>
#include <SimData/External.h>
#include <SimData/Link.h>
#include <SimData/Object.h>
#include <SimData/Vector3.h>

#include <osg/ref_ptr>

#include <Export.h>
#include <Bus.h>

class Animation;
class AnimationCallback;
class AnimationChannel;
class Bus;
class HUD;
class Station;

namespace simdata {
	class Quat;
}

namespace osg {
	class Node;
	class Group;
	class Switch;
	class MatrixTransform;
	class PositionAttitudeTransform;
}

namespace osgText {
	class Text;
}

namespace fx {
	class SmokeTrailSystem;
}


/**
 * class ObjectModel - Static representation of 3D models in the simulation.
 *
 * One ObjectModel instance is created for each type of model, and shared by
 * many SceneModel instances.
 *
 * TODO: add LOD support here.
 *
 */
class CSPSIM_EXPORT ObjectModel: public simdata::Object {
	static const simdata::Enumeration EffectItems;

private:

	osg::ref_ptr<osg::MatrixTransform> m_Transform;
	osg::ref_ptr<osg::Node> m_Model;
	osg::ref_ptr<osg::Switch> m_DebugMarkers;
	osg::ref_ptr<osg::Switch> m_ContactMarkers;
public:
	typedef std::vector<simdata::Vector3> PointList;

	SIMDATA_DECLARE_STATIC_OBJECT(ObjectModel)

	ObjectModel();
	virtual ~ObjectModel();

	osg::ref_ptr<osg::Node> getModel();
	osg::ref_ptr<osg::Node> getDebugMarkers();
	std::string getModelPath() const { return m_ModelPath.getSource(); }

	const simdata::Vector3 &getAxis0() const { return m_Axis0; }
	const simdata::Vector3 &getAxis1() const { return m_Axis1; }
	const simdata::Vector3 &getViewPoint() const { return m_ViewPoint; }
	const simdata::Vector3 &getHudPlacement() const { return m_HudPlacement; }
	double getHudWidth() const { return m_HudWidth; }
	double getHudHeight() const { return m_HudHeight; }

	unsigned numAnimations() const { return m_Animations.size(); }
	const Animation &animation(unsigned i) const {
		assert(i < numAnimations());
		return *(m_Animations[i]);
	}

	double getBoundingSphereRadius() const { return m_BoundingSphereRadius; }
	PointList const &getContacts() const { return m_Contacts; }
	std::string const &getLabel() const { return m_Label; }

	bool getElevationCorrection() const { return m_ElevationCorrection; }

	void showDebugMarkers(bool on);
	bool getDebugMarkersVisible() const;

	static void setDataFilePathList(std::string search_path);

	unsigned numStations() const { return m_Stations.size(); }
	const Station *station(unsigned i) const {
		assert(i < numStations());
		return m_Stations[i].get();
	}

protected:

	std::string m_Label;
	simdata::External m_ModelPath;
	simdata::Vector3 m_Axis0, m_Axis1;
	simdata::Vector3 m_Offset;
	simdata::Vector3 m_ViewPoint;
	simdata::Vector3 m_HudPlacement;
	double m_HudWidth;
	double m_HudHeight;
	double m_Scale;

	bool m_Smooth;
	bool m_Filter;
	float m_FilterValue;
	simdata::Enum<EffectItems> m_Effect;

	bool m_ElevationCorrection;
	float m_PolygonOffset;
	int m_CullFace;
	bool m_Lighting;

	PointList m_Contacts;
	PointList m_DebugPoints;

	simdata::Link<Animation>::vector m_Animations;
	simdata::Link<Station>::vector m_Stations;

	virtual void postCreate();
	virtual void loadModel();
	void addDebugMarkers();
	void generateStationMasks(std::map<std::string, unsigned> const &interior_map) const;

	double m_BoundingSphereRadius;

	enum { DEBUG_MARKERS };
};


/** A representation of an ObjectModel that can be added to the scene
 *  as a child of a SceneModel.
 */
class SceneModelChild: public simdata::Referenced {
	osg::ref_ptr<osg::Node> m_ModelCopy;
	simdata::Ref<ObjectModel> m_Model;
	std::vector< osg::ref_ptr<AnimationCallback> > m_AnimationCallbacks;
protected:
	virtual ~SceneModelChild();
public:
	SceneModelChild(simdata::Ref<ObjectModel> const &model);
	void bindAnimationChannels(Bus*);
	osg::Node *getRoot();
};


/**
 * SceneModel - Represents an object in the scene graph.
 *
 * The base object classes are independent of the scene graph.  SimObject
 * contains a pointer to a SceneModel that can be instantiated to create
 * a visual representation of the object.
 */
class SceneModel: public simdata::Referenced {
private:
	osg::ref_ptr<osg::PositionAttitudeTransform> m_Transform;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_CenterOfMassOffset;
	osg::ref_ptr<osg::Group> m_DynamicGroup;
	osg::ref_ptr<osg::Group> m_Children;
	osg::ref_ptr<osg::Node> m_ModelCopy;
	osg::ref_ptr<osgText::Text> m_Label;
	simdata::Ref<ObjectModel> m_Model;
	bool m_Smoke;
	osg::ref_ptr<fx::SmokeTrailSystem> m_SmokeTrails;
	std::vector<simdata::Vector3> m_SmokeEmitterLocation;
	std::vector< osg::ref_ptr<AnimationCallback> > m_AnimationCallbacks;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_HudModel;
	osg::ref_ptr<osg::Switch> m_PitSwitch;

	osg::Node *getModelCopy() { return m_ModelCopy.get(); }

protected:
	virtual ~SceneModel();

public:
	SceneModel(simdata::Ref<ObjectModel> const & model);

	simdata::Ref<ObjectModel> getModel() { return m_Model; }
	osg::Group* getRoot();
	osg::Group* getDynamicGroup();

	void setPositionAttitude(simdata::Vector3 const &position, simdata::Quat const &attitude, simdata::Vector3 const &cm_offset);

	void addChild(simdata::Ref<SceneModelChild> const &child);
	void removeChild(simdata::Ref<SceneModelChild> const &child);
	void removeAllChildren();

	void bindAnimationChannels(Bus*);
	void bindHud(HUD* hud);

	void setSmokeEmitterLocation(std::vector<simdata::Vector3> const &sel);
	bool addSmoke();
	bool isSmoke();
	void disableSmoke();
	void enableSmoke();
	virtual void onViewMode(bool internal);
	void updateSmoke(double dt, simdata::Vector3 const & global_position, simdata::Quat const &attitude);
	void setLabel(std::string const &);
	void setPitMask(unsigned mask);
	void pick(int x, int y);
};


#endif // __OBJECTMODEL_H__

