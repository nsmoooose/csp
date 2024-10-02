#pragma once
// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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

#include <csp/csplib/data/Vector3.h>

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Ref.h>

#include <osg/ref_ptr>
#include <vector>

namespace osg { class Node; }
namespace osg { class Group; }
namespace osg { class Switch; }
namespace osg { class PositionAttitudeTransform; }
namespace osgText { class Text; }

namespace csp {

class AnimationCallback;
class Bus;
class ObjectModel;
class Quat;

namespace hud { class HUD; }
namespace fx { class SmokeTrailSystem; }


/** A representation of an ObjectModel that can be added to the scene
 *  as a child of a SceneModel.
 */
class SceneModelChild: public Referenced {
	osg::ref_ptr<osg::Node> m_ModelCopy;
	Ref<ObjectModel> m_Model;
	std::vector< osg::ref_ptr<AnimationCallback> > m_AnimationCallbacks;
protected:
	virtual ~SceneModelChild();
public:
	SceneModelChild(Ref<ObjectModel> const &model);
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
class SceneModel: public Referenced {
private:
	osg::ref_ptr<osg::PositionAttitudeTransform> m_PositionTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_AttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_CenterOfMassOffset;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_GroundShadow;
	osg::ref_ptr<osg::Group> m_DynamicGroup;
	osg::ref_ptr<osg::Group> m_Children;
	osg::ref_ptr<osg::Node> m_ModelCopy;
	osg::ref_ptr<osgText::Text> m_Label;
	Ref<ObjectModel> m_Model;
	bool m_Smoke;
	osg::ref_ptr<fx::SmokeTrailSystem> m_SmokeTrails;
	std::vector<Vector3> m_SmokeEmitterLocation;
	std::vector< osg::ref_ptr<AnimationCallback> > m_AnimationCallbacks;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_HudModel;
	osg::ref_ptr<osg::Switch> m_PitSwitch;
	int m_Station;

	osg::Node *getModelCopy() { return m_ModelCopy.get(); }

protected:
	virtual ~SceneModel();

public:
	SceneModel(Ref<ObjectModel> const & model);

	Ref<ObjectModel> getModel();
	osg::Group* getRoot();
	osg::Group* getDynamicGroup();

	bool hasGroundShadow() const { return m_GroundShadow.valid(); }
	void updateGroundShadow(Vector3 const &height, Vector3 const &ground_normal);

	void setPositionAttitude(Vector3 const &position, Quat const &attitude, Vector3 const &cm_offset);

	void addChild(Ref<SceneModelChild> const &child);
	void removeChild(Ref<SceneModelChild> const &child);
	void removeAllChildren();

	/** Connect the scene model animations to the bus.  Channels on the bus
	 *  are used to drive the animations.  This method can be called with
	 *  a NULL pointer to unbind the animations.
	 */
	void bindAnimationChannels(Bus*);
	void bindHud(hud::HUD* hud);

	void setSmokeEmitterLocation(std::vector<Vector3> const &sel);
	bool addSmoke();
	bool isSmoke();
	void disableSmoke();
	void enableSmoke();
	void updateSmoke(double dt, Vector3 const & global_position, Quat const &attitude);

	virtual void onViewMode(bool internal);
	void setLabel(std::string const &);
	void setStation(int index);
	void pick(int x, int y);
};

} // namespace csp
