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

#include <osg/Node>
#include <osg/Switch>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include <SimData/Object.h>
#include <SimData/Vector3.h>
#include <SimData/Matrix3.h>
#include <SimData/Quaternion.h>
#include <SimData/External.h>
#include <SimData/InterfaceRegistry.h>

#include "SmokeEffects.h"


/**
 * class ObjectModel - Static representation of 3D models in the simulation.
 *
 * One ObjectModel instance is created for each type of model, and shared by
 * many SceneModel instances.
 * 
 * TODO: add LOD support here.
 *
 */
class ObjectModel: public simdata::Object
{
public:
	typedef std::vector<simdata::Vector3> ContactList;

	SIMDATA_OBJECT(ObjectModel, 3, 0);
	
	BEGIN_SIMDATA_XML_INTERFACE(ObjectModel)
		SIMDATA_XML("model_path", ObjectModel::m_ModelPath, true)
		SIMDATA_XML("axis_0", ObjectModel::m_Axis0, false)
		SIMDATA_XML("axis_1", ObjectModel::m_Axis1, false)
		SIMDATA_XML("view_point", ObjectModel::m_ViewPoint, false)
		SIMDATA_XML("offset", ObjectModel::m_Offset, false)
		SIMDATA_XML("scale", ObjectModel::m_Scale, false)
		SIMDATA_XML("smooth", ObjectModel::m_Smooth, false)
		SIMDATA_XML("filter", ObjectModel::m_Filter, false)
		SIMDATA_XML("contacts", ObjectModel::m_Contacts, false)
		SIMDATA_XML("landing_gear", ObjectModel::m_LandingGear, false)
	END_SIMDATA_XML_INTERFACE

	ObjectModel();
	virtual ~ObjectModel();

	osg::ref_ptr<osg::Node> getModel() { return m_Transform.get(); }
	osg::ref_ptr<osg::Node> getRawModel() { return m_Node.get(); }
	std::string getModelPath() const { return m_ModelPath.getSource(); }

	const simdata::Vector3 &getAxis0() const { return m_Axis0; }
	const simdata::Vector3 &getAxis1() const { return m_Axis1; }
	const simdata::Vector3 &getViewPoint() const { return m_ViewPoint; }

	double getBoundingSphereRadius() const { return m_BoundingSphereRadius; }
	ContactList const &getContacts() const { return m_Contacts; }

	void showContactMarkers(bool on);
	void showGearSprites(bool on);
	void updateGearSprites(std::vector<simdata::Vector3> const &landing_gear);

	bool getMarkersVisible() const;

protected:

	simdata::External m_ModelPath;
	simdata::Vector3 m_Axis0, m_Axis1;
	simdata::Vector3 m_Offset;
	simdata::Vector3 m_ViewPoint;
	double m_Scale;
	bool m_Smooth;
	bool m_Filter;
	ContactList m_Contacts;
	std::vector<simdata::Vector3> m_LandingGear;
	
	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);
	virtual void postCreate();
	virtual void loadModel();
	void addContactMarkers();
	void addGearSprites();
	void makeFrontGear(simdata::Vector3 const &point);
	void makeLeftGear(simdata::Vector3 const &point);
	void makeRightGear(simdata::Vector3 const &point);

	double m_BoundingSphereRadius;

	enum { CONTACT_MARKERS };

	bool m_MarkersVisible;

private:
	osg::ref_ptr<osg::Group> m_GearSprites;
	osg::ref_ptr<osg::Node> m_Node;
	osg::ref_ptr<osg::MatrixTransform> m_Transform;
	osg::ref_ptr<osg::Switch> m_DebugMarkers;
	osg::ref_ptr<osg::Group> m_ContactMarkers;
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
	osg::ref_ptr<osg::Switch> m_Switch;
	simdata::Ref<ObjectModel> m_Model;
	bool m_Smoke;
	osg::ref_ptr<fx::SmokeTrailSystem> m_SmokeTrails;
	std::vector<simdata::Vector3> m_SmokeEmitterLocation;
protected:
	virtual ~SceneModel();
public:
	SceneModel(simdata::Ref<ObjectModel> const & model);
	
	
	simdata::Ref<ObjectModel> getModel() { return m_Model; }
	osg::Group* getRoot() { return m_Transform.get(); }

	void setPositionAttitude(simdata::Vector3 const &position, simdata::Quaternion const &attitude) {
		m_Transform->setAttitude(osg::Quat(attitude.x, attitude.y, attitude.z, attitude.w));
		m_Transform->setPosition(osg::Vec3(position.x, position.y, position.z));
	}

	// XXX 
	// these two methods are temporary, and will probably change when
	// animation code becomes available.
	void show() { 
		m_Switch->setAllChildrenOn();
	}
	void hide() {
		m_Switch->setAllChildrenOff();
	}

	void setSmokeEmitterLocation(std::vector<simdata::Vector3> const &sel);
	bool addSmoke();
	bool isSmoke();
	void disableSmoke();
	void enableSmoke();
	void updateSmoke(double dt, simdata::Vector3 const & global_position, simdata::Quaternion const &attitude);
};


#endif // __OBJECTMODEL_H__

