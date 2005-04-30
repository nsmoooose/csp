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
 * @file ObjectModel.cpp
 *
 **/
#include <algorithm>
#include <vector>
#include <utility>

#include "ObjectModel.h"
#include "Animation.h"
#include "Config.h"
#include "HUD/HUD.h"
#include "SmokeEffects.h"

#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgFX/SpecularHighlights>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/GLObjectsVisitor>
#include <osgUtil/Optimizer>
#include <osg/CullFace>
#include <osg/NodeVisitor>
#include <osg/Geometry>
#include <osg/Texture>
#include <osg/Geode>
#include <osg/Depth>
#include <osgText/Text>
#include <osg/PolygonOffset>
#include <osg/Node>
#include <osg/Switch>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include <SimCore/Util/Log.h>
#include <SimData/FileUtility.h>
#include <SimData/HashUtility.h>
#include <SimData/osg.h>

/*
	TODO

		o adjust contact markers and view point for model
		  transform

		o implement ModelProcessor

 */


const simdata::Enumeration ObjectModel::EffectItems("None SpecularHighlights");

SIMDATA_REGISTER_INTERFACE(ObjectModel)


/**
 * Animation binding helper that can be attached to model
 * nodes (using osg::Object user data).  When the model
 * prototype is cloned, these bindings help to connect the
 * new animation transform nodes to the correct animation
 * callback.
 */
class AnimationBinding: public osg::Referenced {
	simdata::Ref<Animation const> m_Animation;
	simdata::Ref<Animation const> m_NestedAnimation;
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
		return m_NestedAnimation->newCallback(node->getUpdateCallback());
	}
};


/**
 * A visitor for model prototypes to attach animation
 * bindings to the appropriate nodes.  Each prototype
 * only needs to be processed once, immediately after
 * loading the model.  The bindings are used when the
 * model is later cloned by SceneModel to attach
 * animation callbacks.
 */
class ModelProcessor: public osg::NodeVisitor {
	osg::ref_ptr<osg::Node> m_Root;
	typedef std::multimap<simdata::Key,simdata::Link<Animation> > AnimationsMap;
	//typedef HASH_MULTIMAP<simdata::Key,simdata::Link<Animation> > AnimationsMap_;
	AnimationsMap m_AnimationsMap;

	void fillMap(simdata::Link<Animation>::vector const *animations) {
		simdata::Link<Animation>::vector::const_iterator i = animations->begin();
		simdata::Link<Animation>::vector::const_iterator i_end = animations->end();
		for (; i != i_end; ++i)
			m_AnimationsMap.insert(std::make_pair((*i)->getModelID(),*i));
	}

	AnimationBinding* installAnimation(osg::Node& node, const simdata::Ref<Animation>& anim) const {
		// Flag node as dynamic to enable the callback.
		node.setDataVariance(osg::Object::DYNAMIC);
		AnimationBinding* animation_binding = new AnimationBinding(anim.get());
		node.setUserData(animation_binding);
		return animation_binding;
	}

	void breakNameInComponents(const std::string& name, std::string &animation_name, 
							   std::string& node_name, const char token =':') const {
		node_name = name;
		animation_name = "";
		std::string::size_type pos = name.find(token);
		if (pos != std::string::npos) {
			animation_name = name.substr(0,pos);
			node_name = name.substr(pos+2,name.length() - (pos + 2));
		} 
	}

	struct KeyToCompare: std::unary_function<simdata::Key,bool> {
		const simdata::Key m_KeyToCompare;
	public:
		KeyToCompare(const simdata::Key& key): m_KeyToCompare(key) {}
		bool operator()(const std::pair<simdata::Key,simdata::Link<Animation> >& e) const {
			return m_KeyToCompare == e.first;
		}
	};

public:
	ModelProcessor(): NodeVisitor(TRAVERSE_ALL_CHILDREN) { }
	void setAnimations(simdata::Link<Animation>::vector const *animations) {
		fillMap(animations);
	}
	virtual void apply(osg::Transform &node) {
		if (m_AnimationsMap.empty()) return;
		std::string name = node.getName();
		CSP_LOG(APP, DEBUG, "MODEL TRANSFORM: " << name);
		if (name.substr(0,6) == "ANIM: ") {
			// In case of a TimedAnimationPath, the node name is not the relevant
			// string to look for.
			name = name.substr(6);
			AnimationBinding* animation_binding = 0;
			
			// Extract the animation name (if any) and set the node name if different
			// from 'name'.
			std::string animation_name;
			std::string node_name;
			breakNameInComponents(name,animation_name,node_name);

			// Define the key associated to this (possible) node name.
			simdata::Key node_id = node_name;
			CSP_LOG(APP, DEBUG, "SEARCHING FOR " << name << " (" << node_id.asString() << ")");

			// Find the first (if any) bound animation mapped by node_id.
			AnimationsMap::iterator i = m_AnimationsMap.find(node_id);
			AnimationsMap::const_iterator i_end = m_AnimationsMap.end();

			bool found_first_animation = i != i_end;
			if (found_first_animation) {
				// Bind the animation; this will install an osg::UpdateCallback.
				CSP_LOG(APP, DEBUG, "FOUND ANIMATION");
				animation_binding = installAnimation(node, i->second);
			}

			// second pass
			bool found_second_animation = false;
			if (animation_name.empty()) {
				// regular node name
				if (found_first_animation) {
					i = std::find_if(++i, m_AnimationsMap.end(), KeyToCompare(node_id));
					found_second_animation = i != i_end;
				}
			} else { // animation name to find by animation_id
				if (found_first_animation) std::cout << "Looking for: " << animation_name << "\n";
				simdata::Key animation_id = animation_name;
				i = m_AnimationsMap.find(animation_id);
				if (i != i_end) {
					if (!found_first_animation) {
						installAnimation(node,i->second);
					} else {
						found_second_animation = true;
					}
				}
			}
			if (found_second_animation) {
				// Install as a nested callback.
				CSP_LOG(APP, DEBUG, "FOUND 2nd ANIMATION");
				if (animation_binding) animation_binding->setNestedAnimation(i->second.get());
			}
		}
		traverse(node);
	}
};

/**
 * Visit nodes, applying anisotropic filtering to textures.
 */
class TrilinearFilterVisitor: public osg::NodeVisitor
{
	float m_MaxAnisotropy;
public:
	TrilinearFilterVisitor(float MaxAnisotropy=16.0f):
		osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
		m_MaxAnisotropy(MaxAnisotropy) {
	}

	virtual void apply(osg::Node& node) {
	    osg::StateSet* ss = node.getStateSet();
		filter(ss);
		traverse(node);
	}

	virtual void apply(osg::Geode& geode) {
	    osg::StateSet* ss = geode.getStateSet();
	    filter(ss);
	    for(unsigned int i=0;i<geode.getNumDrawables();++i) {
		osg::Drawable* drawable = geode.getDrawable(i);
		if (drawable) {
		    ss = drawable->getStateSet();
		    filter(ss);
		}
	    }
	}

	void filter(osg::StateSet *set) {
		if (!set) return;
		osg::StateSet::TextureAttributeList& attr = set->getTextureAttributeList();
		osg::StateSet::TextureAttributeList::iterator i;
		for (i = attr.begin(); i != attr.end(); i++) {
			// TODO don't we need to consider other members within the TEXTURE group?
			osg::StateSet::AttributeList::iterator tex = i->find(osg::StateAttribute::TypeMemberPair(osg::StateAttribute::TEXTURE, 0));
			if (tex != i->end()) {
				osg::Texture* texture = dynamic_cast<osg::Texture*>(tex->second.first.get());
				if (texture) {
					texture->setMaxAnisotropy(m_MaxAnisotropy);
				}
			}
		}
	}
};


std::string g_ModelPath = "";

ObjectModel::ObjectModel(): simdata::Object() {
	m_Axis0 = simdata::Vector3::XAXIS;
	m_Axis1 = simdata::Vector3::ZAXIS;
	m_Offset = simdata::Vector3::ZERO;
	m_ViewPoint = simdata::Vector3::ZERO;
	m_HudPlacement = simdata::Vector3::ZERO;
	m_HudWidth = 0.1;
	m_HudHeight = 0.1;
	m_Scale = 1.0;
	m_Smooth = true;
	m_Filter = true;
	m_FilterValue = 16.0f;
	m_ElevationCorrection = true;
	m_PolygonOffset = 0.0;
	m_CullFace = -1;
	m_Lighting = true;
	m_Effect = "None";
}

ObjectModel::~ObjectModel() {
}

void ObjectModel::postCreate() {
	Object::postCreate();
	loadModel();
}

osg::Geometry *makeDiamond(simdata::Vector3 const &pos, float s, osg::Vec4 const &color) {
	float vv[][3] = {
		{ 0.0, 0.0,   s },
		{   s, 0.0, 0.0 },
		{ 0.0,   s, 0.0 },
		{  -s, 0.0, 0.0 },
		{ 0.0,  -s, 0.0 },
		{   s, 0.0, 0.0 },
		{ 0.0, 0.0,  -s },
		{   s, 0.0, 0.0 },
		{ 0.0,  -s, 0.0 },
		{  -s, 0.0, 0.0 },
		{ 0.0,   s, 0.0 },
		{   s, 0.0, 0.0 },
	};

	osg::Vec3Array& v = *(new osg::Vec3Array(12));
	osg::Vec4Array& c = *(new osg::Vec4Array(1));
	c[0] = color;

	for(int i = 0; i < 12; i++ ) {
		v[i][0] = vv[i][0] + pos.x();
		v[i][1] = vv[i][1] + pos.y();
		v[i][2] = vv[i][2] + pos.z();
	}

	osg::Geometry *geom = new osg::Geometry;
	geom->setVertexArray(&v);
	geom->setColorArray(&c);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,0,6));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,6,6));
	return geom;
}

//#include <osgFX/BumpMapping>
osg::Node* addSpecularHighlights(osg::Node* model_node) {
	// add an osgFX effect
	osgFX::SpecularHighlights *effect = new osgFX::SpecularHighlights;	
	effect->setTextureUnit(1);
	effect->addChild(model_node);

	/*osgFX::BumpMapping* effect = new osgFX::BumpMapping;
	effect->setUpDemo();
	effect->setNormalMapTextureUnit(2);
	effect->addChild(model_node);
	effect->prepareChildren();*/
	
	return effect;
}	

void ObjectModel::loadModel() {
	std::string source = m_ModelPath.getSource();

	CSP_LOG(APP, DEBUG, "ObjectModel::loadModel: " << source);

	osg::Node *pNode = osgDB::readNodeFile(source);

	if (pNode) {
		CSP_LOG(APP, DEBUG, "ObjectModel::loadModel: readNodeFile() succeeded");
	} else {
		CSP_LOG(APP, DEBUG, "ObjectModel::loadModel: readNodeFile() failed.");
	}

	assert(pNode);

	m_Model = pNode;
	m_Model->setName(m_ModelPath.getSource());

	if (m_PolygonOffset != 0.0) {
		osg::StateSet *ss = m_Model->getOrCreateStateSet();
		osg::PolygonOffset *po = new osg::PolygonOffset;
		po->setFactor(-1);
		po->setUnits(m_PolygonOffset);
		ss->setAttributeAndModes(po, osg::StateAttribute::ON);
	}

	if (!m_Lighting) {
		osg::StateSet *ss = m_Model->getOrCreateStateSet();
		ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	}

	if (m_CullFace != 0) {
		// XXX should reuse a single static CullFace instance.
		osg::StateSet *ss = m_Model->getOrCreateStateSet();
		osg::CullFace *cf = new osg::CullFace;
		cf->setMode(m_CullFace < 0 ? osg::CullFace::BACK : osg::CullFace::FRONT);
		ss->setAttributeAndModes(cf, osg::StateAttribute::ON);
	}

	if (m_Smooth) {
		osgUtil::SmoothingVisitor sv;
		m_Model->accept(sv);
	}

	if (m_Filter) {
		TrilinearFilterVisitor tfv(m_FilterValue);
		m_Model->accept(tfv);
	}

	// add animation hooks to user data field of animation
	// transform nodes
	ModelProcessor processor;
	CSP_LOG(APP, INFO, "ANIMATIONS AVAILABLE: " << m_Animations.size());
	processor.setAnimations(&m_Animations);
	CSP_LOG(APP, INFO, "PROCESSING MODEL");
	m_Model->accept(processor);
	CSP_LOG(APP, INFO, "PROCESSING MODEL DONE");

	CSP_LOG(APP, INFO, "LoadModel: XML adjustements");
	assert(m_Axis0.length() > 0.0);
	m_Axis0.normalize();
	// orthogonalize
	m_Axis1 = m_Axis1 - m_Axis0 * simdata::dot(m_Axis0, m_Axis1);
	assert(m_Axis1.length() > 0.0);
	m_Axis1.normalize();

	// insert an adjustment matrix at the head of the model only
	// if necessary.
	if (m_Axis0 != simdata::Vector3::XAXIS || m_Axis1 != simdata::Vector3::YAXIS || m_Scale != 1.0 || m_Offset != simdata::Vector3::ZERO) {
		CSP_LOG(APP, WARNING, "Adding model adjustment matrix");
		// find third axis and make the transform matrix
		simdata::Vector3 axis2 = m_Axis0 ^ m_Axis1;
		simdata::Matrix3 o(m_Axis0.x(), m_Axis0.y(), m_Axis0.z(),
		                   m_Axis1.x(), m_Axis1.y(), m_Axis1.z(),
		                   axis2.x(), axis2.y(), axis2.z());
		o = o.getInverse() * m_Scale;
		osg::Matrix model_orientation;
		model_orientation.set(
			o(0, 0), o(1, 0), o(2, 0), 0.0,
			o(0, 1), o(1, 1), o(2, 1), 0.0,
			o(0, 2), o(1, 2), o(2, 2), 0.0,
			m_Offset.x(), m_Offset.y(), m_Offset.z(), 1.0
		);
		osg::MatrixTransform *adjust = new osg::MatrixTransform;
		if (m_Scale != 1.0) {
			adjust->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		}
		adjust->setName("XML_ADJUSTMENT");
		adjust->setDataVariance(osg::Object::STATIC);
		adjust->setMatrix(model_orientation);
		adjust->addChild(m_Model.get());
		m_Model = adjust;
		simdata::Matrix3 sd_adjust = simdata::fromOSG(model_orientation); //.getInverse();
		for (unsigned i = 0; i < m_Contacts.size(); i++) {
			m_Contacts[i] = sd_adjust * m_Contacts[i]  + m_Offset;
		}
		for (unsigned i = 0; i < m_DebugPoints.size(); i++) {
			m_DebugPoints[i] = sd_adjust * m_DebugPoints[i]  + m_Offset;
		}
		m_ViewPoint = sd_adjust * m_ViewPoint  + m_Offset;
		m_HudPlacement = sd_adjust * m_HudPlacement  + m_Offset;
	}

	osg::BoundingSphere s = m_Model->getBound();
	m_BoundingSphereRadius = s.radius();

	//osg::StateSet * stateSet = m_rpNode->getStateSet();
	//stateSet->setGlobalDefaults();
	//m_rpNode->setStateSet(stateSet);

	// add an osgFX::effect
	if (m_Effect == "SpecularHighlights") {
		m_Model = addSpecularHighlights(m_Model.get());
	};

	m_DebugMarkers = new osg::Switch;
	// XXX should reuse a single static stateset?
	m_DebugMarkers->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	m_DebugMarkers->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace, osg::StateAttribute::ON);

	// create visible markers for each contact and debug point
	CSP_LOG(APP, DEBUG, "LoadModel: add debug markers");
	addDebugMarkers();

	/*
	// FIXME Segfaults when creating objects using the CSP theater layout tool.
	// Need to figure out why, but for now just disable.
	
	osg::ref_ptr<osg::State> state = new osg::State;

	CSP_LOG(APP, DEBUG, "LoadModel: setting state");
	osgUtil::GLObjectsVisitor ov;
	ov.setState(state.get());
	ov.setNodeMaskOverride(0xffffffff);
	m_Model->accept(ov);
	CSP_LOG(APP, DEBUG, "LoadModel: setting state for debug markers");
	m_DebugMarkers->accept(ov);
	*/

	CSP_LOG(APP, DEBUG, "LoadModel: done");

	// XXX: there is a really weird bug on vs with the optimizer:
	// 1) it rarely appears in the release built (never when called from this exact line)
	// 2) it appears in the debug built systematically on the second times the
	//    program is run but only if a few seconds separate the 2 runs. Wait
	//    for 5 minutes or so (or run another process) and the bug will not occur
	//    (thread related?).
	// 3) I haven't noticed the bug in the debug built when the optimizer is run in this
	//    line (like for the release built) but called it 10 lines above and it segfaults.
	// 4) The bug only occurs when CSP is run from command line or clicking CSPSim.py;
	//    it never occurs when running csp in debug mode from the ide.
	// 5) I'm unable to trace it :)

	//CSP_LOG(APP, DEBUG, "LoadModel: Optimizer run");
	osgUtil::Optimizer opt;
	//opt.optimize(m_Model.get());
	//CSP_LOG(APP, DEBUG, "LoadModel: Optimizer done");
}

void ObjectModel::addDebugMarkers() {
	m_ContactMarkers = new osg::Switch;
	osg::CullFace *cf = new osg::CullFace;
	cf->setMode(osg::CullFace::BACK);
	m_ContactMarkers->getOrCreateStateSet()->setAttributeAndModes(cf, osg::StateAttribute::ON);
	for (unsigned i = 0; i < m_Contacts.size(); i++) {
		osg::Geode *diamond = new osg::Geode;
		diamond->addDrawable(makeDiamond(m_Contacts[i], 0.20, osg::Vec4(1, 0, 0, 1)));
		m_ContactMarkers->addChild(diamond);
	}
	for (unsigned i = 0; i < m_DebugPoints.size(); i++) {
		osg::Geode *diamond = new osg::Geode;
		diamond->addDrawable(makeDiamond(m_DebugPoints[i], 0.05, osg::Vec4(1, 1, 0, 0.5)));
		m_DebugMarkers->addChild(diamond);
	}
	// set debug markers not visible by default
	showDebugMarkers(false);
	m_DebugMarkers->addChild(m_ContactMarkers.get());
}

void ObjectModel::showDebugMarkers(bool on) {
	if (on) {
		m_DebugMarkers->setAllChildrenOn();
	} else {
		m_DebugMarkers->setAllChildrenOff();
	}
}

bool ObjectModel::getDebugMarkersVisible() const {
	return (m_DebugMarkers->getNumChildren() > 0 && m_DebugMarkers->getValue(0) != 0);
}

osg::ref_ptr<osg::Node> ObjectModel::getModel() {
	return m_Model.get();
}

osg::ref_ptr<osg::Node> ObjectModel::getDebugMarkers() {
	return m_DebugMarkers.get();
}

void ObjectModel::setDataFilePathList(std::string search_path) {
	osgDB::setDataFilePathList(search_path);
}

/**
 * Copy class for cloning model prototypes for use
 * in the scene graph.  Each new SceneModel uses this
 * class to create a copy of the associated ObjectModel
 * prototype.
 */
class ModelCopy: public osg::CopyOp {
public:
	typedef std::vector<osg::ref_ptr<AnimationCallback> > AnimationCallbackVector;
	inline AnimationCallbackVector const &getAnimationCallbacks() const {
		return m_AnimationCallbacks;
	}
	virtual osg::Node* operator() (const osg::Node* node) const {
		osg::Referenced const *data = node->getUserData();
		// user data bound to nodes is used to modify the copy operations
		if (data) {
			AnimationBinding const *binding = dynamic_cast<AnimationBinding const *>(data);
			// nodes with animation bindings need a callback
			if (binding) {
				osg::Node *new_node = dynamic_cast<osg::Node*>(node->clone(*this));
				m_AnimationCallbacks.push_back(binding->bind(new_node));
				CSP_LOG(APP, INFO, "ADDED CALLBACK (" << m_AnimationCallbacks.size() << ") ON " << node->getName().substr(6));
				if (binding->hasNestedAnimation()) {
					m_AnimationCallbacks.push_back(binding->bindNested(new_node));
					CSP_LOG(APP, INFO, "ADDED NESTED CALLBACK (" << m_AnimationCallbacks.size() << ") ON " 
										<< node->getName().substr(6));
				}
				return new_node;
			}
		}
		if (dynamic_cast<osg::Group const *>(node)) {
			// clone groups
			return dynamic_cast<osg::Node*>(node->clone(*this));
		}	else
			// copy other leaf nodes by reference
			return const_cast<osg::Node*>(node);
	}
private:
	mutable AnimationCallbackVector m_AnimationCallbacks;
};

SceneModel::SceneModel(simdata::Ref<ObjectModel> const & model) {
	m_Model = model;
	assert(m_Model.valid());
	CSP_LOG(APP, INFO, "create SceneModel for " << m_Model->getModelPath());

	// get the prototype model scene graph
	osg::Node *model_node = m_Model->getModel().get();
	assert(model_node);

	// create a working copy
	ModelCopy model_copy;
	model_node = model_copy(model_node);

	CSP_LOG(APP, DEBUG, "MODEL COPIED");

	CSP_LOG(APP, INFO, "COPIED MODEL animation count = " << model_copy.getAnimationCallbacks().size());

	m_AnimationCallbacks.resize(model_copy.getAnimationCallbacks().size());

	// store all the animation update callbacks
	std::copy(model_copy.getAnimationCallbacks().begin(),
	          model_copy.getAnimationCallbacks().end(),
	          m_AnimationCallbacks.begin());

	CSP_LOG(APP, INFO, "MODEL animation count = " << m_AnimationCallbacks.size());

	m_Label = new osgText::Text();
	m_Label->setFont("screeninfo.ttf");
	m_Label->setFontResolution(16, 16);
	m_Label->setColor(osg::Vec4(0.3f, 0.4f, 1.0f, 1.0f));
	m_Label->setCharacterSize(100.0, 1.0);
	m_Label->setPosition(osg::Vec3(6, 0, 0));
	m_Label->setText("AIRCRAFT");
	osg::Geode *label = new osg::Geode;
	osg::Depth *depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0, 1.0);
	label->getOrCreateStateSet()->setAttributeAndModes(depth, osg::StateAttribute::OFF);
	label->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//setMatrix(osg::Matrix::ortho2D(0,ScreenWidth,0,ScreenHeight));
	osg::MatrixTransform *m_modelview_abs = new osg::MatrixTransform;
	m_modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_modelview_abs->setMatrix(osg::Matrix::identity());
	m_modelview_abs->addChild(label);

	m_Transform = new osg::PositionAttitudeTransform;
	m_CenterOfMassOffset = new osg::PositionAttitudeTransform;
	m_Transform->addChild(m_CenterOfMassOffset.get());
	m_CenterOfMassOffset->addChild(model_node);
	m_CenterOfMassOffset->addChild(m_Model->getDebugMarkers().get());
	m_CenterOfMassOffset->addChild(m_modelview_abs);
	m_Smoke = false;
}

SceneModel::~SceneModel() {
	// FIXME shouldn't we be removing the copy?
	osg::Node *model_node = m_Model->getModel().get();
	assert(model_node);
	// FIXME why?
	m_CenterOfMassOffset->removeChild(model_node);
}

void SceneModel::setLabel(std::string const &label) {
	m_Label->setText(label);
}

void SceneModel::updateSmoke(double dt, simdata::Vector3 const &global_position, simdata::Quat const &attitude) {
	m_SmokeTrails->update(dt, global_position, attitude);
}

void SceneModel::setSmokeEmitterLocation(std::vector<simdata::Vector3> const &sel) {
	m_SmokeEmitterLocation = sel;
}

bool SceneModel::addSmoke() {
	if (m_SmokeTrails.valid())
		return true;
	else {
		if (!m_SmokeEmitterLocation.empty()) {
			std::vector<simdata::Vector3>::const_iterator iEnd = m_SmokeEmitterLocation.end();
			for (std::vector<simdata::Vector3>::iterator i = m_SmokeEmitterLocation.begin(); i != iEnd; ++i) {
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

void SceneModel::disableSmoke()
{
	if (m_Smoke) {
		m_SmokeTrails->setEnabled(false);
		m_Smoke = false;
	}
}

void SceneModel::enableSmoke()
{
	CSP_LOG(OBJECT, DEBUG, "SceneModel::enableSmoke()...");
	if (!m_Smoke) {
		if (!addSmoke()) return;
		CSP_LOG(OBJECT, DEBUG, "SceneModel::enableSmoke()");
		m_SmokeTrails->setEnabled(true);
		m_Smoke = true;
	}
}

void SceneModel::bindAnimationChannels(Bus::Ref bus) {
	// XXX: not sure how to handle this situation.
	if (bus.valid()) {
		int index, n = m_AnimationCallbacks.size();
		for (index = 0; index < n; ++index) {
			if (!m_AnimationCallbacks[index].valid()) {
				CSP_LOG(OBJECT, WARNING, "bindAnimationChannels: AnimationCallbacks '" << index << "' not valid; skipping");
				continue;
			}
			std::string name = m_AnimationCallbacks[index]->getChannelName();
			simdata::Ref<const DataChannelBase> channel = bus->getChannel(name, false);
			if (!channel.valid()) {
				CSP_LOG(OBJECT, ERROR, "bindAnimationChannels: animation channel '" << name << "' not found; skipping");
				continue;
			}
			bool compatible = (DataChannel<double>::CRef::compatible(channel) || \
			                   DataChannel<bool>::CRef::compatible(channel) || \
			                   DataChannel<simdata::Vector3>::CRef::compatible(channel));
			if (compatible) {
				m_AnimationCallbacks[index]->bindChannel(channel);
			} else {
				CSP_LOG(OBJECT, ERROR, "bindAnimationChannels: animation channel '" << name << "' type not supported; skipping");
			}
		}
	}
	// XXX Hack for testing
	CSP_LOG(OBJECT, DEBUG, "Trying to bind HUD");
	DataChannel<HUD*>::CRef hud_channel = bus->getChannel("HUD", false);
	if (hud_channel.valid()) {
		HUD *hud = hud_channel->value();
		CSP_LOG(OBJECT, DEBUG, "Found HUD");
		m_3dHud = hud->hud();
		m_3dHud->setPosition(simdata::toOSG(m_Model->getHudPlacement()));
		m_CenterOfMassOffset->addChild(hud->hud());
		hud->setOrigin(simdata::toOSG(m_Model->getHudPlacement()));
		hud->setViewPoint(simdata::toOSG(m_Model->getViewPoint()));
		hud->setDimensions(m_Model->getHudWidth(), m_Model->getHudHeight());
		CSP_LOG(OBJECT, DEBUG, "HUD added to model");
	}
	CSP_LOG(OBJECT, DEBUG, "bindAnimationChannels complete");
}

void SceneModel::onViewMode(bool internal) {

	// show/hide hud (if any) when the view is internal/external
	if (m_3dHud.valid()) {
		if (internal) {
			m_3dHud->setNodeMask(0xff);
		} else	{
			m_3dHud->setNodeMask(0x0);
		}
	}

}

void SceneModel::setPositionAttitude(simdata::Vector3 const &position, simdata::Quat const &attitude, simdata::Vector3 const &cm_offset) {
	m_Transform->setAttitude(simdata::toOSG(attitude));
	m_Transform->setPosition(simdata::toOSG(position));
	m_CenterOfMassOffset->setPosition(simdata::toOSG(-cm_offset));
}

osg::Group* SceneModel::getRoot() {
	return m_Transform.get();
}

