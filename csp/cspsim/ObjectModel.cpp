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
 * @file ObjectModel.cpp
 *
 **/

#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/Animation.h>
#include <csp/cspsim/Bus.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/Station.h>

#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/HashUtility.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/osg.h>
#include <csp/csplib/util/StringTools.h>
#include <csp/csplib/util/Timing.h>
#include <csp/csplib/data/Enum.h>
#include <csp/csplib/data/Key.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Quat.h>

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
#include <osg/PolygonOffset>
#include <osg/Switch>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include <algorithm>
#include <vector>
#include <utility>


CSP_NAMESPACE

const Enumeration ObjectModel::EffectItems("None SpecularHighlights");

CSP_XML_BEGIN(ObjectModel)
	CSP_DEF("label", m_Label, false)
	CSP_DEF("model_path", m_ModelPath, true)
	CSP_DEF("ground_shadowpath", m_GroundShadowPath, false)
	CSP_DEF("axis_0", m_Axis0, false)
	CSP_DEF("axis_1", m_Axis1, false)
	CSP_DEF("view_point", m_ViewPoint, false)
	CSP_DEF("hud_placement", m_HudPlacement, false)
	CSP_DEF("hud_width", m_HudWidth, false)
	CSP_DEF("hud_height", m_HudHeight, false)
	CSP_DEF("offset", m_Offset, false)
	CSP_DEF("scale", m_Scale, false)
	CSP_DEF("smooth", m_Smooth, false)
	CSP_DEF("filter", m_Filter, false)
	CSP_DEF("filter_value", m_FilterValue, false)
	CSP_DEF("effect", m_Effect, false)
	CSP_DEF("contacts", m_Contacts, false)
	CSP_DEF("debug_points", m_DebugPoints, false)
	CSP_DEF("elevation_correction", m_ElevationCorrection, false)
	CSP_DEF("polygon_offset", m_PolygonOffset, false)
	CSP_DEF("cull_face", m_CullFace, false)
	CSP_DEF("lighting", m_Lighting, false)
	CSP_DEF("animations", m_Animations, false)
	CSP_DEF("stations", m_Stations, false)
CSP_XML_END


/**
 * Animation binding helper that can be attached to model
 * nodes (using osg::Object user data).  When the model
 * prototype is cloned, these bindings help to connect the
 * new animation transform nodes to the correct animation
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
				CSPLOG(WARNING, OBJECT) << "AnimationBinder: failed to bind animation in " << m_Label;
			}
		} else {
			CSPLOG(WARNING, OBJECT) << "AnimationBinder: AnimationCallback not valid in " << m_Label << "; skipping";
		}
	}
};


/** A visitor for model prototypes to attach animation bindings to the
 *  appropriate nodes.  Each prototype only needs to be processed once,
 *  immediately after loading the model.  The bindings are used when the
 *  model is later cloned by SceneModel to attach animation callbacks.
 */
class ModelProcessor: public osg::NodeVisitor {
	osg::ref_ptr<osg::Node> m_Root;
	typedef std::multimap<Key, Link<Animation> > AnimationsMap;
	AnimationsMap m_AnimationsMap;
	std::map<std::string, unsigned> m_InteriorMap;

	void fillMap(Link<Animation>::vector const *animations) {
		Link<Animation>::vector::const_iterator i = animations->begin();
		Link<Animation>::vector::const_iterator i_end = animations->end();
		for (; i != i_end; ++i) {
			m_AnimationsMap.insert(std::make_pair(Key((*i)->getNodeLabel()),*i));
		}
	}

	AnimationBinding* installAnimation(osg::Node& node, const Ref<Animation>& anim) const {
		// Flag node as dynamic to enable the callback.
		node.setDataVariance(osg::Object::DYNAMIC);
		AnimationBinding* animation_binding = new AnimationBinding(anim.get());
		node.setUserData(animation_binding);
		return animation_binding;
	}

	void breakNameInComponents(const std::string& name, std::string &animation_name, std::string& node_name, const char token =':') const {
		node_name = TrimString(name);
		animation_name = "";
		std::string::size_type pos = name.find(token);
		if (pos != std::string::npos) {
			animation_name = TrimString(name.substr(0, pos));
			node_name = TrimString(name.substr(pos+1));
		}
	}

	struct KeyToCompare: std::unary_function<Key,bool> {
		const Key m_KeyToCompare;
	public:
		KeyToCompare(const Key& key): m_KeyToCompare(key) {}
		bool operator()(const std::pair<Key,Link<Animation> >& e) const {
			return m_KeyToCompare == e.first;
		}
	};

public:

	ModelProcessor(): NodeVisitor(TRAVERSE_ALL_CHILDREN) { }

	void setAnimations(Link<Animation>::vector const *animations) {
		fillMap(animations);
	}

	std::map<std::string, unsigned> const &getInteriorMap() const { return m_InteriorMap; }

	virtual void apply(osg::Group &node) {
		const std::string label = node.getName();
		if (label.substr(0, 5) == "ANIM:") {
			const std::string name = TrimString(label.substr(5));
			AnimationBinding* animation_binding = 0;

			// Extract the animation and node names from the node label, which is of
			// one of the following two forms:
			//   ANIM: animation_name : node_name
			//   ANIM: node_name
			std::string animation_name;
			std::string node_name;
			breakNameInComponents(name, animation_name, node_name);

			// Find the first animation binding, if any, based on the node_name.
			const Key node_id = node_name;
			AnimationsMap::iterator i = m_AnimationsMap.find(node_id);
			AnimationsMap::const_iterator i_end = m_AnimationsMap.end();

			bool found_first_animation = (i != i_end);
			if (found_first_animation) {
				// Bind the animation; this will install an osg::UpdateCallback.
				animation_binding = installAnimation(node, i->second);
			}

			// Second pass: if this node has an animation_name, check to see if it has a binding.
			// Otherwise, check to see if there may be two different bindings for node_name.  Note
			// that we do not allow two node_name bindings if animation_name is set.
			bool found_second_animation = false;
			if (!animation_name.empty()) {
				// animation_name is set; check for an explicit binding.
				const Key animation_id = animation_name;
				i = m_AnimationsMap.find(animation_id);
				if (i != i_end) {
					if (!found_first_animation) {
						installAnimation(node, i->second);
						found_first_animation = true;
					} else {
						found_second_animation = true;
					}
				}
			} else {
				// only node_name is set, check if it has a second binding.
				if (found_first_animation) {
					i = std::find_if(++i, m_AnimationsMap.end(), KeyToCompare(node_id));
					found_second_animation = (i != i_end);
				}
			}
			if (found_first_animation) {
				CSPLOG(DEBUG, OBJECT) << "Found primary animation for " << name;
			}
			if (found_second_animation) {
				// Install as a nested callback.
				CSPLOG(DEBUG, OBJECT) << "Found secondary animation for " << name;
				if (animation_binding) animation_binding->setNestedAnimation(i->second.get());
			}
			if (!found_first_animation && !found_second_animation) {
				CSPLOG(WARNING, OBJECT) << "Found no animations for node " << name;
			}
		} else if (label.substr(0, 8) == "__PITS__") {
			CSPLOG(INFO, OBJECT) << "Found __PITS__, " << node.getNumChildren() << " children";
			assert(m_InteriorMap.empty());
			for (unsigned i = 0; i < node.getNumChildren(); ++i) {
				std::string pitname = node.getChild(i)->getName();
				if (m_InteriorMap.find(pitname) == m_InteriorMap.end()) {
					CSPLOG(INFO, OBJECT) << "Found pit " << pitname;
					m_InteriorMap[pitname] = i;
				} else {
					CSPLOG(ERROR, OBJECT) << "Duplicate interior label " << pitname;
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

ObjectModel::ObjectModel(): Object() {
	m_Label = "OBJECT";
	m_Axis0 = Vector3::XAXIS;
	m_Axis1 = Vector3::ZAXIS;
	m_Offset = Vector3::ZERO;
	m_ViewPoint = Vector3::ZERO;
	m_HudPlacement = Vector3::ZERO;
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

osg::Geometry *makeDiamond(Vector3 const &pos, float s, osg::Vec4 const &color) {
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
	effect->setName("specular_highlights");
	effect->setTextureUnit(1);
	effect->addChild(model_node);

	/*osgFX::BumpMapping* effect = new osgFX::BumpMapping;
	effect->setUpDemo();
	effect->setNormalMapTextureUnit(2);
	effect->addChild(model_node);
	effect->prepareChildren();*/
	
	return effect;
}

void ObjectModel::generateStationMasks(std::map<std::string, unsigned> const &interior_map) const {
	for (unsigned i = 0; i < m_Stations.size(); ++i) {
		unsigned mask = 0;
		std::vector<std::string> const &names = m_Stations[i]->getMaskNames();
		for (unsigned j = 0; j < names.size(); ++j) {
			std::map<std::string, unsigned>::const_iterator iter = interior_map.find(names[j]);
			if (iter != interior_map.end()) {
				mask |= (1 << iter->second);
			} else {
				CSPLOG(WARNING, OBJECT) << "Pit " << names[j] << " not found while processing station " << m_Stations[i]->getName();
			}
		}
		assert(m_Stations[i]->getMask() == 0U);
		m_Stations[i]->setMask(mask);
	}
}

void ObjectModel::loadModel() {
	Timer timer;
	const std::string source = m_ModelPath.getSource();

	CSPLOG(INFO, OBJECT) << "ObjectModel::loadModel: " << source;

	timer.start();
	osg::Node *pNode = osgDB::readNodeFile(source);
	timer.stop();

	if (pNode) {
		CSPLOG(INFO, OBJECT) << "ObjectModel::loadModel: readNodeFile(" << source << ") succeeded in " << (timer.elapsed() * 1e3) << " ms";
	} else {
		CSPLOG(ERROR, OBJECT) << "ObjectModel::loadModel: readNodeFile(" << source << ") failed.";
	}

	assert(pNode);

	m_Model = pNode;
	m_Model->setName(source);

	if (m_PolygonOffset != 0.0) {
		osg::StateSet *ss = m_Model->getOrCreateStateSet();
		// polygon offset is used for co-planar overlays, like the runway.  there should
		// be no need to write to the depth buffer since it will already be set by the
		// base layer (the terrain is drawn in bin -3) and setting a nearer value (due to
		// polygon offset) can mask 3d objects on top of this object when the distance to
		// the camera is large.
		ss->setAttributeAndModes(new osg::PolygonOffset(-5, m_PolygonOffset), osg::StateAttribute::ON);
		ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 0.0, 1.0, false), osg::StateAttribute::ON);
		ss->setRenderBinDetails(-2, "RenderBin");
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

	CSPLOG(INFO, OBJECT) << "Animations available: " << m_Animations.size();
	CSPLOG(INFO, OBJECT) << "Processing model";
	timer.start();

	// add animation hooks to user data field of animation
	// transform nodes
	{
		ModelProcessor processor;
		processor.setAnimations(&m_Animations);
		m_Model->accept(processor);
		generateStationMasks(processor.getInteriorMap());
	}

	timer.stop();
	CSPLOG(INFO, OBJECT) << "Processing model finished in " << (timer.elapsed() * 1e+6) << " us";

	// normalize and orthogonalize the model axes.
	assert(m_Axis0.length() > 0.0);
	m_Axis0.normalize();
	m_Axis1 = m_Axis1 - m_Axis0 * dot(m_Axis0, m_Axis1);
	assert(m_Axis1.length() > 0.0);
	m_Axis1.normalize();

	// insert an adjustment matrix at the head of the model only if necessary.
	if (m_Axis0 != Vector3::XAXIS || m_Axis1 != Vector3::YAXIS || m_Scale != 1.0 || m_Offset != Vector3::ZERO) {
		CSPLOG(WARNING, OBJECT) << "Adding model adjustment matrix";
		// find third axis and make the transform matrix
		Vector3 axis2 = m_Axis0 ^ m_Axis1;
		Matrix3 o(m_Axis0.x(), m_Axis0.y(), m_Axis0.z(),
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
		adjust->setName("xmladjustment");
		//adjust->setDataVariance(osg::Object::STATIC);
		adjust->setDataVariance(osg::Object::DYNAMIC);
		adjust->setMatrix(model_orientation);
		adjust->addChild(m_Model.get());
		m_Model = adjust;
		Matrix3 sd_adjust = fromOSG(model_orientation); //.getInverse();
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

	// add an osgFX::effect
	if (m_Effect == "SpecularHighlights") {
		m_Model = addSpecularHighlights(m_Model.get());
	};

	m_DebugMarkers = new osg::Switch;
	m_DebugMarkers->setName("debug");
	// XXX should reuse a single static stateset?
	m_DebugMarkers->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	m_DebugMarkers->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace, osg::StateAttribute::ON);

	// create visible markers for each contact and debug point
	CSPLOG(DEBUG, OBJECT) << "Adding debug markers";
	addDebugMarkers();

	/*
	// FIXME Segfaults when creating objects using the CSP theater layout tool.
	// Need to figure out why, but for now just disable.
	
	osg::ref_ptr<osg::State> state = new osg::State;

	CSPLOG(DEBUG, OBJECT) << "Compiling display lists";
	osgUtil::GLObjectsVisitor ov;
	ov.setState(state.get());
	ov.setNodeMaskOverride(0xffffffff);
	m_Model->accept(ov);
	CSPLOG(DEBUG, OBJECT) << "Compiling display lists for debug markers";
	m_DebugMarkers->accept(ov);
	*/

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

	//CSPLOG(DEBUG, OBJECT) << "LoadModel: Optimizer run";
	//osgUtil::Optimizer opt;
	//opt.optimize(m_Model.get());
	//CSPLOG(DEBUG, OBJECT) << "LoadModel: Optimizer done";

	if (!m_GroundShadowPath.asString().empty()) {
		CSPLOG(DEBUG, OBJECT) << "Loading ground shadow " << m_GroundShadowPath.asString();
		m_GroundShadow = osgDB::readNodeFile(m_GroundShadowPath.asString());
		if (m_GroundShadow.valid()) {
			// ground shadows are drawn after the terrain and flat objects, but before
			// normal objects.  they do not modify the depth buffer, and are offset away
			// from the ground to prevent z fighting.
			osg::StateSet *ss = m_GroundShadow->getOrCreateStateSet();
			ss->setRenderBinDetails(-1, "RenderBin");
			ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 0.0, 1.0, false), osg::StateAttribute::ON);
			ss->setAttributeAndModes(new osg::PolygonOffset(-5, -10), osg::StateAttribute::ON);
		} else {
			CSPLOG(WARNING, OBJECT) << "Failed to load ground shadow " << m_GroundShadowPath.asString();
		}
	}

	CSPLOG(DEBUG, OBJECT) << "Done loading model " << source;

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

std::string ObjectModel::getModelPath() const {
	return m_ModelPath.getSource();
}

osg::ref_ptr<osg::Node> ObjectModel::getGroundShadow() {
	return m_GroundShadow.get();
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
					CSPLOG(ERROR, OBJECT) << "Unknown node type for animation binding";
				}
				assert(new_node);
				if (!bind_node) bind_node = new_node;
				AnimationCallback *cb = binding->bind(bind_node);
				if (cb) {
					m_AnimationCallbacks.push_back(cb);
					CSPLOG(INFO, OBJECT) << "ADDED CALLBACK (" << m_AnimationCallbacks.size() << ") ON " << node->getName().substr(6);
				} else {
					CSPLOG(WARNING, OBJECT) << "Failed to add animation callback to " << node->getName().substr(6);
				}
				if (binding->hasNestedAnimation()) {
					AnimationCallback *cb = binding->bindNested(new_node);
					if (cb) {
						m_AnimationCallbacks.push_back(cb);
						CSPLOG(INFO, OBJECT) << "ADDED NESTED CALLBACK (" << m_AnimationCallbacks.size() << ") ON " << node->getName().substr(6);
					} else {
						CSPLOG(WARNING, OBJECT) << "Failed to add nested animation callback to " << node->getName().substr(6);
					}
				}
				return new_node;
			}
		}
		if (node->asGroup()) {
			if (node->getName() == "__PITS__") {
				CSPLOG(INFO, OBJECT) << "Copying __PITS__ node";
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

CSP_NAMESPACE_END

