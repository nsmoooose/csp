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
#include "SmokeEffects.h"
#include "Config.h"

#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgFX/SpecularHighlights>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/DisplayListVisitor>
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
	inline AnimationCallback *bind(osg::Node *node) const {
		return m_Animation->newCallback(node);
	}
	void setNestedAnimation(Animation const* animation) {
		m_NestedAnimation = animation;
	}
	bool hasNestedAnimation() const {
		return m_NestedAnimation.valid();
	}
	AnimationCallback* bindNested(osg::Node* node) const {
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
			simdata::Key id = name.substr(6);
			CSP_LOG(APP, DEBUG, "SEARCHING FOR " << name.substr(6) << " (" << id.asString() << ")");
			AnimationsMap::iterator i = m_AnimationsMap.find(id);
			AnimationsMap::const_iterator i_end = m_AnimationsMap.end();
			if (i != i_end) {
				CSP_LOG(APP, DEBUG, "FOUND");
				node.setDataVariance(osg::Object::DYNAMIC);
				AnimationBinding* animation_binding = new AnimationBinding(i->second.get());
				node.setUserData(animation_binding);
				i = std::find_if(++i, m_AnimationsMap.end(), KeyToCompare(id));
				if (i != i_end) {
					CSP_LOG(APP, DEBUG, "FOUND 2nd");
					animation_binding->setNestedAnimation(i->second.get());
				}
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
			osg::StateSet::AttributeList::iterator tex = i->find(osg::StateAttribute::TEXTURE);
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

osg::Geometry *makeDiamond(simdata::Vector3 const &pos, float s) {
    float vv[][3] =
    {
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
    osg::Vec4Array& l = *(new osg::Vec4Array(1));

    int   i;

    l[0][0] = 1;
    l[0][1] = 0;
    l[0][2] = 0;
    l[0][3] = 1;

    for( i = 0; i < 12; i++ )
    {
        v[i][0] = vv[i][0] + pos.x();
        v[i][1] = vv[i][1] + pos.y();
        v[i][2] = vv[i][2] + pos.z();
    }

    osg::Geometry *geom = new osg::Geometry;
    geom->setVertexArray(&v);
    geom->setColorArray(&l);
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);
    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,0,6) );
    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,6,6) );

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
	if (m_Axis0 != simdata::Vector3::XAXIS ||
	    m_Axis1 != simdata::Vector3::YAXIS ||
	    m_Scale != 1.0 ||
	    m_Offset != simdata::Vector3::ZERO) {
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
		m_ViewPoint = sd_adjust * m_ViewPoint  + m_Offset;
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

	// create visible markers for each contact point
	CSP_LOG(APP, DEBUG, "LoadModel: add contact markers");
	addContactMarkers();

	osg::ref_ptr<osg::State> state = new osg::State;
	osgUtil::DisplayListVisitor dlv(osgUtil::DisplayListVisitor::COMPILE_DISPLAY_LISTS);
	dlv.setState(state.get());
	dlv.setNodeMaskOverride(0xffffffff);
	m_Model->accept(dlv);
	m_DebugMarkers->accept(dlv);

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
	CSP_LOG(APP, DEBUG, "LoadModel: Optimizer run");
	osgUtil::Optimizer opt;
	opt.optimize(m_Model.get());
	CSP_LOG(APP, DEBUG, "LoadModel: Optimizer done");
}

void ObjectModel::addContactMarkers() {
	m_ContactMarkers = new osg::Group;
	osg::CullFace *cf = new osg::CullFace;
	cf->setMode(osg::CullFace::BACK);
	m_ContactMarkers->getOrCreateStateSet()->setAttributeAndModes(cf, osg::StateAttribute::ON);
	for (unsigned i = 0; i < m_Contacts.size(); i++) {
		osg::Geode *diamond = new osg::Geode;
		diamond->addDrawable(makeDiamond(m_Contacts[i], 0.2));
		m_ContactMarkers->addChild(diamond);
	}
	// set markers not visible by default
	m_ContactMarkers->setNodeMask(0x0);
	m_DebugMarkers->addChild(m_ContactMarkers.get());
}

void ObjectModel::showContactMarkers(bool on) {
	if (on)
		m_ContactMarkers->setNodeMask(0xffffffff);
	else
		m_ContactMarkers->setNodeMask(0x0);
}

bool ObjectModel::getMarkersVisible() const {
	return (m_ContactMarkers->getNodeMask() != 0x0);
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
				if (binding->hasNestedAnimation())
					m_AnimationCallbacks.push_back(binding->bindNested(new_node));
				CSP_LOG(APP, INFO, "ADDED CALLBACK");
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

	CSP_LOG(APP, INFO, "MODEL animation count = " << model_copy.getAnimationCallbacks().size());

	m_AnimationCallbacks.resize(model_copy.getAnimationCallbacks().size());

	// store all the animation update callbacks
	std::copy(model_copy.getAnimationCallbacks().begin(),
	          model_copy.getAnimationCallbacks().end(),
	          m_AnimationCallbacks.begin());

	CSP_LOG(APP, INFO, "MODEL animation count = " << m_AnimationCallbacks.size());

	m_Label = new osgText::Text();
	m_Label->setFont("screeninfo.ttf");
	m_Label->setFontSize(16, 16);
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
	m_modelview_abs->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);
	m_modelview_abs->setMatrix(osg::Matrix::identity());
	m_modelview_abs->addChild(label);

	m_Transform = new osg::PositionAttitudeTransform;
	m_Transform->addChild(model_node);
	m_Transform->addChild(m_Model->getDebugMarkers().get());
	m_Transform->addChild(m_modelview_abs);
	m_Smoke = false;
}

SceneModel::~SceneModel() {
	osg::Node *model_node = m_Model->getModel().get();
	assert(model_node);
	m_Transform->removeChild(model_node);
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
}

void SceneModel::setPositionAttitude(simdata::Vector3 const &position, simdata::Quat const &attitude) {
	m_Transform->setAttitude(simdata::toOSG(attitude));
	m_Transform->setPosition(simdata::toOSG(position));
}

osg::Group* SceneModel::getRoot() {
	return m_Transform.get();
}

