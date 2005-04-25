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

#include <SimData/Object.h>
#include <SimData/Vector3.h>
#include <SimData/Matrix3.h>
#include <SimData/Enum.h>
#include <SimData/Quat.h>
#include <SimData/External.h>
#include <SimData/InterfaceRegistry.h>

#include <osg/ref_ptr>

#include <Export.h>

class Animation;
class AnimationCallback;
class AnimationChannel;
class Bus;

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

	SIMDATA_STATIC_OBJECT(ObjectModel, 6, 0);
	
	BEGIN_SIMDATA_XML_INTERFACE(ObjectModel)
		SIMDATA_XML("model_path", ObjectModel::m_ModelPath, true)
		SIMDATA_XML("axis_0", ObjectModel::m_Axis0, false)
		SIMDATA_XML("axis_1", ObjectModel::m_Axis1, false)
		SIMDATA_XML("view_point", ObjectModel::m_ViewPoint, false)
		SIMDATA_XML("hud_placement", ObjectModel::m_HudPlacement, false)
		SIMDATA_XML("hud_width", ObjectModel::m_HudWidth, false)
		SIMDATA_XML("hud_height", ObjectModel::m_HudHeight, false)
		SIMDATA_XML("offset", ObjectModel::m_Offset, false)
		SIMDATA_XML("scale", ObjectModel::m_Scale, false)
		SIMDATA_XML("smooth", ObjectModel::m_Smooth, false)
		SIMDATA_XML("filter", ObjectModel::m_Filter, false)
		SIMDATA_XML("filter_value", ObjectModel::m_FilterValue, false)
		SIMDATA_XML("effect", ObjectModel::m_Effect, false)
		SIMDATA_XML("contacts", ObjectModel::m_Contacts, false)
		SIMDATA_XML("debug_points", ObjectModel::m_DebugPoints, false)
		SIMDATA_XML("elevation_correction", ObjectModel::m_ElevationCorrection, false)
		SIMDATA_XML("polygon_offset", ObjectModel::m_PolygonOffset, false)
		SIMDATA_XML("cull_face", ObjectModel::m_CullFace, false)
		SIMDATA_XML("lighting", ObjectModel::m_Lighting, false)
		SIMDATA_XML("animations", ObjectModel::m_Animations, false)
	END_SIMDATA_XML_INTERFACE

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

	double getBoundingSphereRadius() const { return m_BoundingSphereRadius; }
	PointList const &getContacts() const { return m_Contacts; }

	bool getElevationCorrection() const { return m_ElevationCorrection; }

	void showDebugMarkers(bool on);

	bool getDebugMarkersVisible() const;

	static void setDataFilePathList(std::string search_path);

protected:

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

	virtual void postCreate();
	virtual void loadModel();
	void addDebugMarkers();

	double m_BoundingSphereRadius;

	enum { DEBUG_MARKERS };
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
	osg::ref_ptr<osgText::Text> m_Label;
	simdata::Ref<ObjectModel> m_Model;
	bool m_Smoke;
	osg::ref_ptr<fx::SmokeTrailSystem> m_SmokeTrails;
	std::vector<simdata::Vector3> m_SmokeEmitterLocation;
	std::vector< osg::ref_ptr<AnimationCallback> > m_AnimationCallbacks;
protected:
	virtual ~SceneModel();
public:
	SceneModel(simdata::Ref<ObjectModel> const & model);
	
	simdata::Ref<ObjectModel> getModel() { return m_Model; }
	osg::Group* getRoot();

	void setPositionAttitude(simdata::Vector3 const &position, simdata::Quat const &attitude, simdata::Vector3 const &cm_offset);

	void bindAnimationChannels(simdata::Ref<Bus>);

	void setSmokeEmitterLocation(std::vector<simdata::Vector3> const &sel);
	bool addSmoke();
	bool isSmoke();
	void disableSmoke();
	void enableSmoke();
	void updateSmoke(double dt, simdata::Vector3 const & global_position, simdata::Quat const &attitude);
	void setLabel(std::string const &);
};


#endif // __OBJECTMODEL_H__

