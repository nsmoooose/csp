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
 * @file SceneModel.cpp
 *
 **/
#include <csp/cspsim/SceneModel.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/Animation.h>
#include <csp/cspsim/SceneConstants.h>
#include <csp/cspsim/SmokeEffects.h>
#include <csp/cspsim/Station.h>
#include <csp/cspsim/hud/HUD.h>

//#include <csp/csplib/util/HashUtility.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/osg.h>
#include <csp/csplib/util/Timing.h>
#include <csp/csplib/data/Quat.h>

#include <osg/Depth>
#include <osg/Geode>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Switch>
#include <osgText/Text>

#include <algorithm>
#include <vector>
#include <utility>


namespace csp {


/**
 * Animation binding helper that can be attached to model nodes (using
 * osg::Object user data).  When the model prototype is cloned, these bindings
 * help to connect the new animation transform nodes to the correct animation
 * callback.
 */
class AnimationBinding: public osg::Referenced {
	Ref<Animation const> m_Animation;
	Ref<Animation const> m_NestedAnimation;
public:
	AnimationBinding(Animation const* animation): m_Animation(animation) {}
	AnimationCallback *bind(osg::Node *node) const {
		return m_Animation->newCallback(node);
	}
	void setNestedAnimation(Animation const* animation) {
		m_NestedAnimation = animation;
	}
	bool hasNestedAnimation() const {
		return m_NestedAnimation.valid();
	}
	AnimationCallback *bindNested(osg::Node* node) const {
		return m_NestedAnimation->newCallback(node);
	}
	bool needsSwitch() const {
		return m_Animation->needsSwitch();
	}
};


/** A helper class for connecting cloned animations to the bus.
 */
class AnimationBinder {
	Bus *m_Bus;
	std::string m_Label;
public:
	AnimationBinder(Bus *bus, std::string const &label): m_Bus(bus), m_Label(label) { }
	void operator()(osg::ref_ptr<AnimationCallback> &cb) {
		if (cb.valid()) {
			if (!cb->bindChannels(m_Bus)) {
				CSPLOG(Prio_WARNING, Cat_OBJECT) << "AnimationBinder: failed to bind animation in " << m_Label;
			}
		} else {
			CSPLOG(Prio_WARNING, Cat_OBJECT) << "AnimationBinder: AnimationCallback not valid in " << m_Label << "; skipping";
		}
	}
};


/**
 * Copy class for cloning model prototypes for use in the scene graph.  Each
 * new SceneModel uses this class to create a copy of the associated
 * ObjectModel prototype.
 */
class ModelCopy: public osg::CopyOp {
public:
	typedef std::vector<osg::ref_ptr<AnimationCallback> > AnimationCallbackVector;
	inline AnimationCallbackVector const &getAnimationCallbacks() const {
		return m_AnimationCallbacks;
	}

	virtual osg::Node* operator() (const osg::Node* node) const {
		assert(node);
		osg::Referenced const *data = node->getUserData();
		// user data bound to nodes is used to modify the copy operations
		if (data) {
			AnimationBinding const *binding = dynamic_cast<AnimationBinding const *>(data);
			// nodes with animation bindings need a callback
			if (binding) {
				assert(node->asGroup());  // can't attach ANIM label to leaf nodes!
				osg::Node *new_node = 0;
				// The switch insert logic (see cloneSwitch) requires a somewhat ugly hack to
				// bind the animation callback to the right node.  Note that needsSwitch()
				// only refers to the primary callback; nested switch callbacks aren't currently
				// supported.
				osg::Node *bind_node = 0;
				if (binding->needsSwitch()) {
					osg::Switch *select_node = 0;;
					new_node = cloneSwitch(node, select_node);
					bind_node = select_node;
				} else if (node->asTransform()) {
					new_node = static_cast<osg::Node*>(node->clone(*this));
				} else {
					CSPLOG(Prio_ERROR, Cat_OBJECT) << "Unknown node type for animation binding";
				}
				assert(new_node);
				if (!bind_node) bind_node = new_node;
				AnimationCallback *cb = binding->bind(bind_node);
				if (cb) {
					m_AnimationCallbacks.push_back(cb);
					CSPLOG(Prio_INFO, Cat_OBJECT) << "ADDED CALLBACK (" << m_AnimationCallbacks.size() << ") ON " << node->getName().substr(6);
				} else {
					CSPLOG(Prio_WARNING, Cat_OBJECT) << "Failed to add animation callback to " << node->getName().substr(6);
				}
				if (binding->hasNestedAnimation()) {
					AnimationCallback *cb = binding->bindNested(new_node);
					if (cb) {
						m_AnimationCallbacks.push_back(cb);
						CSPLOG(Prio_INFO, Cat_OBJECT) << "ADDED NESTED CALLBACK (" << m_AnimationCallbacks.size() << ") ON " << node->getName().substr(6);
					} else {
						CSPLOG(Prio_WARNING, Cat_OBJECT) << "Failed to add nested animation callback to " << node->getName().substr(6);
					}
				}
				return new_node;
			}
		}
		if (node->asGroup()) {
			if (node->getName() == "__PITS__") {
				CSPLOG(Prio_INFO, Cat_OBJECT) << "Copying __PITS__ node";
				assert(!m_PitSwitch.valid());
				// clone the __PITS__ node as a switch and save a reference.
				osg::Switch *select_node = 0;
				osg::Node *new_node = cloneSwitch(node, select_node);
				m_PitSwitch = select_node;
				return new_node;
			} else {
				// clone groups
				return dynamic_cast<osg::Node*>(node->clone(*this));
			}
		} else {
			// copy other leaf nodes by reference
			return const_cast<osg::Node*>(node);
		}
	}

	ModelCopy(): CopyOp(osg::CopyOp::SHALLOW_COPY), m_ShallowCopy(osg::CopyOp::SHALLOW_COPY) { }

	osg::Switch *getPitSwitch() { return m_PitSwitch.get(); }

private:
	mutable AnimationCallbackVector m_AnimationCallbacks;
	mutable osg::ref_ptr<osg::Switch> m_PitSwitch;
	osg::CopyOp m_ShallowCopy;

	// Dance for cloning switch nodes that may be masquerading as transforms.
	// Most modelling software does not export osg::Switch; the standard grouping
	// node is a MatrixTransform.  To create a switch in that case we need to
	// move the groups children to a new Switch node and add the Switch to the
	// transform.
	osg::Node *cloneSwitch(const osg::Node *node, osg::Switch* &select_node) const {
		osg::Group *clone = 0;
		const osg::Group *group = node->asGroup();
		assert(group);
		const unsigned num_children = group->getNumChildren();
		osg::Switch *select = new osg::Switch;
		for (unsigned i = 0; i < num_children; ++i) {
			select->addChild(static_cast<osg::Node*>(group->getChild(i)->clone(*this)));
		}
		if (node->asTransform()) {
			clone = static_cast<osg::Group*>(node->clone(m_ShallowCopy));
			assert(clone);
			clone->removeChild(0U, num_children);
			clone->addChild(select);
		} else {
			clone = select;
		}
		select_node = select;
		return clone;
	}
};


SceneModel::SceneModel(Ref<ObjectModel> const & model) {
	m_Model = model;
	assert(m_Model.valid());
	CSPLOG(Prio_INFO, Cat_APP) << "create SceneModel for " << m_Model->getModelPath();

	// get the prototype model scene graph
	osg::Node *model_node = m_Model->getModel().get();
	assert(model_node);

	// create a working copy
	ModelCopy model_copy;

	Timer timer;
	timer.start();
	m_ModelCopy = model_copy(model_node);
	m_PitSwitch = model_copy.getPitSwitch();
	timer.stop();

	CSPLOG(Prio_INFO, Cat_APP) << "Copied model, animation count = " << model_copy.getAnimationCallbacks().size();
	if (timer.elapsed() > 0.01) {
		CSPLOG(Prio_WARNING, Cat_APP) << "Model copy took " << (timer.elapsed() * 1e+3) << " ms";
	}

	m_AnimationCallbacks.resize(model_copy.getAnimationCallbacks().size());

	// store all the animation update callbacks
	std::copy(model_copy.getAnimationCallbacks().begin(), model_copy.getAnimationCallbacks().end(), m_AnimationCallbacks.begin());

	m_Label = new osgText::Text();
	m_Label->setFont("screeninfo.ttf");
	m_Label->setFontResolution(30, 30);
	m_Label->setColor(osg::Vec4(1.0f, 0.4f, 0.2f, 1.0f));
	m_Label->setCharacterSize(20.0);
	m_Label->setPosition(osg::Vec3(0, 0, 0));
	m_Label->setAxisAlignment(osgText::Text::SCREEN);
	m_Label->setAlignment(osgText::Text::CENTER_BOTTOM);
	m_Label->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
	m_Label->setText(m_Model->getLabel());
	osg::Geode *label = new osg::Geode;
	osg::Depth *depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0, 1.0);
	label->getOrCreateStateSet()->setAttributeAndModes(depth, osg::StateAttribute::OFF);
	label->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	label->addDrawable(m_Label.get());
	label->setNodeMask(SCENEMASK_LABELS);

	m_PositionTransform = new osg::PositionAttitudeTransform;
	m_AttitudeTransform = new osg::PositionAttitudeTransform;
	m_PositionTransform->setName("position");
	m_AttitudeTransform->setName("attitude");
	m_CenterOfMassOffset = new osg::PositionAttitudeTransform;
	m_CenterOfMassOffset->setName("cm_offset");
	m_PositionTransform->addChild(m_AttitudeTransform.get());
	m_AttitudeTransform->addChild(m_CenterOfMassOffset.get());
	m_CenterOfMassOffset->addChild(m_ModelCopy.get());
	m_CenterOfMassOffset->addChild(m_Model->getDebugMarkers().get());
	m_CenterOfMassOffset->addChild(label);
	m_Station = -1;
	m_Smoke = false;

	osg::ref_ptr<osg::Node> ground_shadow = m_Model->getGroundShadow();
	if (ground_shadow.valid()) {
		m_GroundShadow = new osg::PositionAttitudeTransform;
		m_GroundShadow->addChild(ground_shadow.get());
		m_PositionTransform->addChild(m_GroundShadow.get());
	}
}

SceneModel::~SceneModel() {
	// FIXME shouldn't we be removing the copy?
	osg::Node *model_node = m_Model->getModel().get();
	assert(model_node);
	// FIXME why?
	m_CenterOfMassOffset->removeChild(model_node);
}

void SceneModel::setStation(int index) {
	if (m_Station != index && m_PitSwitch.valid()) {
		m_Station = index;
		const unsigned mask = (index >= 0) ? m_Model->station(index)->getMask() : 0;
		const unsigned n = m_PitSwitch->getNumChildren();
		for (unsigned i = 0; i < n; ++i) {
			m_PitSwitch->setValue(i, (mask & (1 << i)) == 0);
		}
	}
}

void SceneModel::setLabel(std::string const &label) {
	m_Label->setText(label);
}

void SceneModel::updateSmoke(double dt, Vector3 const &global_position, Quat const &attitude) {
	m_SmokeTrails->update(dt, global_position, attitude);
}

void SceneModel::setSmokeEmitterLocation(std::vector<Vector3> const &sel) {
	m_SmokeEmitterLocation = sel;
}

bool SceneModel::addSmoke() {
	if (m_SmokeTrails.valid())
		return true;
	else {
		if (!m_SmokeEmitterLocation.empty()) {
			std::vector<Vector3>::const_iterator iEnd = m_SmokeEmitterLocation.end();
			for (std::vector<Vector3>::iterator i = m_SmokeEmitterLocation.begin(); i != iEnd; ++i) {
				fx::SmokeTrail *trail = new fx::SmokeTrail();
				trail->setEnabled(false);
				trail->setTexture("Smoke/white-smoke-hilite.rgb");

				// short blue trail slowly evasing
				trail->setColorRange(osg::Vec4(0.9, 0.9, 1.0, 0.2), osg::Vec4(0.97, 0.97, 0.99, 0.08));
				trail->setSizeRange(0.7, 1.5);
				trail->setLifeTime(0.2);

				// short red trail
				//trail->setColorRange(osg::Vec4(0.9, 0.1, 0.1, 0.3), osg::Vec4(0.99, 0.97, 0.97, 0.1));
				//trail->setSizeRange(0.8, 0.6);
				//trail->setLifeTime(0.1);

				trail->setLight(false);
				trail->setExpansion(1.0);

				// Author: please, document this SmokeThinner operator.
				//trail->addOperator(new fx::SmokeThinner);

				trail->setOffset(*i);

				m_SmokeTrails = new fx::SmokeTrailSystem;
				m_SmokeTrails->addSmokeTrail(trail);
			}
			m_Smoke = false;
			return true;
		}
	}
	return false;
}

bool SceneModel::isSmoke() {
	return m_Smoke;
}

void SceneModel::disableSmoke() {
	if (m_Smoke) {
		m_SmokeTrails->setEnabled(false);
		m_Smoke = false;
	}
}

void SceneModel::enableSmoke() {
	CSPLOG(Prio_DEBUG, Cat_OBJECT) << "SceneModel::enableSmoke()...";
	if (!m_Smoke) {
		if (!addSmoke()) return;
		CSPLOG(Prio_DEBUG, Cat_OBJECT) << "SceneModel::enableSmoke()";
		m_SmokeTrails->setEnabled(true);
		m_Smoke = true;
	}
}

void SceneModel::bindAnimationChannels(Bus* bus) {
	AnimationBinder binder(bus, m_Model->getModelPath());
	std::for_each(m_AnimationCallbacks.begin(), m_AnimationCallbacks.end(), binder);
}

void SceneModel::bindHud(hud::HUD *hud) {
	CSPLOG(Prio_DEBUG, Cat_OBJECT) << "adding HUD to model";
	assert(hud);
	m_HudModel = hud->hud();
	m_HudModel->setPosition(toOSG(m_Model->getHudPlacement()));
	m_CenterOfMassOffset->addChild(m_HudModel.get());
	hud->setOrigin(toOSG(m_Model->getHudPlacement()));
	hud->setViewPoint(toOSG(m_Model->getViewPoint()));
	hud->setDimensions(m_Model->getHudWidth(), m_Model->getHudHeight());
}

void SceneModel::onViewMode(bool internal) {
	// show/hide hud (if any) when the view is internal/external
	if (m_HudModel.valid()) {
		if (internal) {
			m_HudModel->setNodeMask(0xff);
		} else {
			m_HudModel->setNodeMask(0x0);
		}
	}
}

void SceneModel::setPositionAttitude(Vector3 const &position, Quat const &attitude, Vector3 const &cm_offset) {
	m_AttitudeTransform->setAttitude(toOSG(attitude));
	m_PositionTransform->setPosition(toOSG(position));
	m_CenterOfMassOffset->setPosition(toOSG(-cm_offset));
}

osg::Group* SceneModel::getRoot() {
	return m_PositionTransform.get();
}

Ref<ObjectModel> SceneModel::getModel() {
	return m_Model;
}

osg::Group *SceneModel::getDynamicGroup() {
	if (!m_DynamicGroup) {
		m_DynamicGroup = new osg::Group;
		m_CenterOfMassOffset->addChild(m_DynamicGroup.get());
	}
	return m_DynamicGroup.get();
}

void SceneModel::updateGroundShadow(Vector3 const &height, Vector3 const &ground_normal) {
	if (m_GroundShadow.valid()) {
		if (height.z() < 1000.0) {
			// assume ground_normal is normalized!
			double s = sqrt(0.5 + 0.5 * ground_normal.z());
			osg::Quat ground_attitude(-ground_normal.y() / (2.0 * s), ground_normal.x() / (2.0 * s), 0.0, s);
			const osg::Quat &attitude = m_AttitudeTransform->getAttitude();
			double x = attitude.x();
			double y = attitude.y();
			double z = attitude.z();
			double w = attitude.w();
			double heading = atan2(y*y+w*w-x*x-z*z, 2.0*(x*y - w*z)) - PI_2;
			m_GroundShadow->setAttitude(osg::Quat(heading, osg::Vec3(0.0, 0.0, 1.0)) * ground_attitude);
			m_GroundShadow->setPosition(toOSG(-height));
			m_GroundShadow->setNodeMask(~0);
		} else {
			// hide shadow at high altitude and skip updates.
			m_GroundShadow->setNodeMask(0);
		}
	}
}

void SceneModel::addChild(Ref<SceneModelChild> const &child) {
	assert(child->getRoot());
	if (!m_Children) {
		m_Children = new osg::Group;
		m_CenterOfMassOffset->addChild(m_Children.get());
	}
	m_Children->addChild(child->getRoot());
}

void SceneModel::removeChild(Ref<SceneModelChild> const &child) {
	assert(child->getRoot());
	if (m_Children.valid()) {
		m_Children->removeChild(child->getRoot());
	}
}

void SceneModel::removeAllChildren() {
	if (m_Children.valid()) {
		m_Children->removeChild(0, m_Children->getNumChildren());
		m_CenterOfMassOffset->removeChild(m_Children.get());
		m_Children = 0;
	}
}


SceneModelChild::~SceneModelChild() {}

SceneModelChild::SceneModelChild(Ref<ObjectModel> const &model) {
	m_Model = model;
	assert(m_Model.valid());
	CSPLOG(Prio_INFO, Cat_APP) << "Create SceneModelChild for " << m_Model->getModelPath();

	// create a working copy of the prototype model
	ModelCopy model_copy;
	m_ModelCopy = model_copy(m_Model->getModel().get());
	CSPLOG(Prio_INFO, Cat_APP) << "Copied model, animation count = " << model_copy.getAnimationCallbacks().size();

	// store all the animation update callbacks
	m_AnimationCallbacks.resize(model_copy.getAnimationCallbacks().size());
	std::copy(model_copy.getAnimationCallbacks().begin(), model_copy.getAnimationCallbacks().end(), m_AnimationCallbacks.begin());
}

osg::Node *SceneModelChild::getRoot() {
	return m_ModelCopy.get();
}

void SceneModelChild::bindAnimationChannels(Bus* bus) {
	AnimationBinder binder(bus, m_Model->getModelPath());
	std::for_each(m_AnimationCallbacks.begin(), m_AnimationCallbacks.end(), binder);
}

} // namespace csp

