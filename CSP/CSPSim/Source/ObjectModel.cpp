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

#include <vector>

#include "ObjectModel.h"
#include "Animation.h"
#include "Log.h"
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
#include <osg/ShapeDrawable>
#include <osgText/Text>
#include <osg/PolygonOffset>


#include <SimData/FileUtility.h>
#include <SimData/osg.h>

/*
	TODO

		o adjust contact markers and view point for model
		  transform

		o implement ModelProcessor

 */

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
public:
	AnimationBinding(Animation const* animation): m_Animation(animation) {}
	inline AnimationCallback *bind(osg::Node *node) const {
		return m_Animation->newCallback(node);
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
	simdata::Link<Animation>::vector const *m_Animations;
public:
	ModelProcessor(): NodeVisitor(TRAVERSE_ALL_CHILDREN), m_Animations(0) { }
	void setAnimations(simdata::Link<Animation>::vector const *animations) {
		m_Animations = animations;
	}
	virtual void apply(osg::Transform &node) {
		if (!m_Animations) return;
		std::string name = node.getName();
		std::cout << "MODEL TRANSFORM: " << name << "\n";
		if (name.substr(0,6) == "ANIM: ") {
			simdata::Key id = name.substr(6);
			simdata::Link<Animation>::vector::const_iterator i = m_Animations->begin();
			std::cout << "SEARCHING FOR " << name.substr(6) << " (" << id.asString() << ")\n";
			for (; i != m_Animations->end(); ++i) {
				std::cout << "COMPARING TO " << (*i)->getModelID().asString() << "\n";
				if ((*i)->getModelID() == id) {
					node.setUserData(new AnimationBinding(i->get()));
					std::cout << "FOUND\n";
					break;
				}
			}
		}
	}
};



/** 
 * Visit nodes, applying anisotropic filtering to textures.
 */
class TrilinearFilterVisitor: public osg::NodeVisitor
{
	float m_MaxAnisotropy;
public:
	TrilinearFilterVisitor(float MaxAnisotropy=16.0): 
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
	m_ElevationCorrection = true;
	m_PolygonOffset = 0.0;
	m_CullFace = -1;
	m_Lighting = true;
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
		po->setFactor(-1.0);
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
		// FIXME: level should come from global graphics settings
		TrilinearFilterVisitor tfv(16.0);
		m_Model->accept(tfv);
	}

	osgUtil::Optimizer opt;
	opt.optimize(m_Model.get());

	// add animation hooks to user data field of animation
	// transform nodes
	ModelProcessor processor;
	std::cout << "ANIMATIONS AVAILABLE: " << m_Animations.size() << "\n";
	processor.setAnimations(&m_Animations);
	std::cout << "PROCESSING MODEL\n";
	m_Model->accept(processor);
	std::cout << "PROCESSING MODEL DONE\n";

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
     
	m_DebugMarkers = new osg::Switch;
	// XXX should reuse a single static stateset?
	m_DebugMarkers->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	m_DebugMarkers->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace, osg::StateAttribute::ON);

	// create visible markers for each contact point
	addContactMarkers();

	// create landing gear wheels
	addGearSprites();

	osg::ref_ptr<osg::State> state = new osg::State;
	osgUtil::DisplayListVisitor dlv(osgUtil::DisplayListVisitor::COMPILE_DISPLAY_LISTS);
	dlv.setState(state.get());
	dlv.setNodeMaskOverride(0xffffffff);
	m_Model->accept(dlv);
	m_DebugMarkers->accept(dlv);
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

// temporary hack to deal with gear objects
void ObjectModel::makeFrontGear(simdata::Vector3 const &point) {
	//FIXME: harcoded  gravity center height
	double const offset = -0.75;
	double const radius1 = 0.22;
	double const e1 = 0.11;

	osg::Vec3 p = simdata::toOSG(point);

	osg::Quat q;
	q.makeRotate(osg::DegreesToRadians(90.0),osg::Vec3(0.0,1.0,0.0)); 
	osg::Cylinder *cyl1 = new osg::Cylinder(osg::Vec3(e1,0.0,p.z()+radius1), radius1, e1);
	cyl1->setRotation(q);
	osg::ShapeDrawable *wheel_front1 = new osg::ShapeDrawable(cyl1);
	wheel_front1->setColor(osg::Vec4(0.2,0.2,0.2,1.0));

	osg::Cylinder *cyl2 = new osg::Cylinder(osg::Vec3(-e1,0.0,p.z()+radius1), radius1, e1);
	cyl2->setRotation(q);
	osg::ShapeDrawable *wheel_front2 = new osg::ShapeDrawable(cyl2);
	wheel_front2->setColor(osg::Vec4(0.2,0.2,0.2,1.0));

	osg::Geode *geode = new osg::Geode;
	geode->addDrawable(wheel_front1);
	geode->addDrawable(wheel_front2);


	double l = (- p.z() + offset - radius1) / 2.0;
	osg::ShapeDrawable *tube_mobile = 
		new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0,0.0,offset - l), e1/4.0, 2.0 * l));
	tube_mobile->setColor(osg::Vec4(0.4,0.4,0.4,1.0));
	geode->addDrawable(tube_mobile);

	osg::PositionAttitudeTransform *mobile_group = new osg::PositionAttitudeTransform; 
	mobile_group->addChild(geode);

	osg::ShapeDrawable *tube_static = 
		new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(p.x(),p.y(),offset), e1/2.0, l));
	tube_static->setColor(osg::Vec4(0.4,0.4,0.4,1.0));
	osg::Geode *geode2 = new osg::Geode;
	geode2->addDrawable(tube_static);
	osg::Group *static_group = new osg::Group;
	static_group->addChild(geode2);

	m_GearSprites->addChild(mobile_group);
	m_GearSprites->addChild(static_group);
}

void ObjectModel::makeLeftGear(simdata::Vector3 const &point) {
	//FIXME: harcoded  gravity center height
	double const offset = -0.67;
	double const radius = 0.44;
	double const e = 0.22;

	osg::Vec3 p = simdata::toOSG(point);

	osg::Quat q;
	q.makeRotate(osg::DegreesToRadians(90.0),osg::Vec3(0.0,1.0,0.0)); 
	osg::Cylinder *cyl = new osg::Cylinder(osg::Vec3(-0.75 * e,0.0,p.z()+radius), radius, e);
	cyl->setRotation(q);
	osg::ShapeDrawable *wheel = new osg::ShapeDrawable(cyl);
	wheel->setColor(osg::Vec4(0.2,0.2,0.2,1.0));

	osg::Geode *geode = new osg::Geode;
	geode->addDrawable(wheel);

	double l1 = (- p.z() + offset - radius) / 2.0;
	double l = l1 - l1 / 4.0;
	osg::ShapeDrawable *tube_mobile = 
		new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0,0.0,offset - l1 /4.0 - l), e/4.0, 2.0 * l));
	tube_mobile->setColor(osg::Vec4(0.4,0.4,0.4,1.0));
	geode->addDrawable(tube_mobile);

	osg::PositionAttitudeTransform *mobile_group = new osg::PositionAttitudeTransform; 
	mobile_group->addChild(geode);

	osg::ShapeDrawable *tube_static = 
		new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(p.x(),p.y(),offset), e/2.0, l1/2.0));
	tube_static->setColor(osg::Vec4(0.4,0.4,0.4,1.0));
	osg::Geode *geode2 = new osg::Geode;
	geode2->addDrawable(tube_static);
	osg::Group *static_group = new osg::Group;
	static_group->addChild(geode2);

	m_GearSprites->addChild(mobile_group);
	m_GearSprites->addChild(static_group);
}

void ObjectModel::makeRightGear(simdata::Vector3 const &point) {
	//FIXME: harcoded  gravity center height
	double const offset = -0.67;
	double const radius = 0.44;
	double const e = 0.22;

	osg::Vec3 p = simdata::toOSG(point);

	osg::Quat q;
	q.makeRotate(osg::DegreesToRadians(90.0),osg::Vec3(0.0,1.0,0.0)); 
	osg::Cylinder *cyl = new osg::Cylinder(osg::Vec3(0.75 * e,0.0,p.z()+radius), radius, e);
	cyl->setRotation(q);
	osg::ShapeDrawable *wheel = new osg::ShapeDrawable(cyl);
	wheel->setColor(osg::Vec4(0.2,0.2,0.2,1.0));

	osg::Geode *geode = new osg::Geode;
	geode->addDrawable(wheel);

	double l1 = (- p.z() + offset - radius) / 2.0;
	double l = l1 - l1 / 4.0;
	osg::ShapeDrawable *tube_mobile = 
		new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0,0.0,offset - l1 /4.0 - l), e/4.0, 2.0 * l));
	tube_mobile->setColor(osg::Vec4(0.4,0.4,0.4,1.0));
	geode->addDrawable(tube_mobile);

	osg::PositionAttitudeTransform *mobile_group = new osg::PositionAttitudeTransform; 
	mobile_group->addChild(geode);

	osg::ShapeDrawable *tube_static = 
		new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(p.x(),p.y(),offset), e/2.0, l1/2.0));
	tube_static->setColor(osg::Vec4(0.4,0.4,0.4,1.0));
	osg::Geode *geode2 = new osg::Geode;
	geode2->addDrawable(tube_static);
	osg::Group *static_group = new osg::Group;
	static_group->addChild(geode2);

	m_GearSprites->addChild(mobile_group);
	m_GearSprites->addChild(static_group);
}

void ObjectModel::addGearSprites() {
	size_t n = m_LandingGear.size();
	if (n>0) {
		m_GearSprites = new osg::Group;
		m_GearSprites->setName("GearSprites");
		makeFrontGear(m_LandingGear[0]);
		makeLeftGear(m_LandingGear[1]);
		makeRightGear(m_LandingGear[2]);
		m_DebugMarkers->addChild(m_GearSprites.get());
		simdata::Vector3 move[3] = {simdata::Vector3::ZERO,simdata::Vector3::ZERO,simdata::Vector3::ZERO};
		updateGearSprites(std::vector<simdata::Vector3>(move,move+3));
	}
}

//FIXME: to be moved in AircraftModel?
void ObjectModel::updateGearSprites(std::vector<simdata::Vector3> const &move) {
	if (m_GearSprites.valid()) {
		size_t m =  m_GearSprites->getNumChildren();
		size_t i = 0;
		for (size_t j = 0; j < m; ++j) {
			osg::Node *node = m_GearSprites->getChild(j);
			osg::PositionAttitudeTransform *pos = dynamic_cast<osg::PositionAttitudeTransform*>(node); 
			if (pos && i < move.size()) {
				osg::Vec3 p(m_LandingGear[i].x(),m_LandingGear[i].y(),move[i].z());
				pos->setPosition(p);
				++i;
			}
		}
	}
}

void ObjectModel::showContactMarkers(bool on) {
	if (on) 
		m_ContactMarkers->setNodeMask(0xffffffff);
	else
		m_ContactMarkers->setNodeMask(0x0);
}

//FIXME: to be moved in AircraftModel?
void ObjectModel::showGearSprites(bool on) {
	if (on)
		m_GearSprites->setNodeMask(0xffffffff);
	else
		m_GearSprites->setNodeMask(0x0);
}

bool ObjectModel::getMarkersVisible() const {
	return (m_ContactMarkers->getNodeMask() != 0x0);
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
				std::cout << "ADDED CALLBACK\n";
				return new_node;
			}
		}
		if (dynamic_cast<osg::Group const *>(node)) {
			// clone groups
			return dynamic_cast<osg::Node*>(node->clone(*this));
		} else {
			// copy other leaf nodes by reference
			return const_cast<osg::Node*>(node);
		}
	}
private:
	mutable AnimationCallbackVector m_AnimationCallbacks;
};


osg::Node *addEffect(osg::Node *model_node) {
	// add an osgFX effect
	osgFX::SpecularHighlights *effect = new osgFX::SpecularHighlights;	
	effect->setTextureUnit(1);
	effect->addChild(model_node);
	return effect;
}	

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
	// XXX add effect or not
	model_node = addEffect(model_node);

	std::cout << "MODEL COPIED\n";

	std::cout << "MODEL animation count = " << model_copy.getAnimationCallbacks().size() << "\n";

	m_AnimationCallbacks.resize(model_copy.getAnimationCallbacks().size());

	// store all the animation update callbacks
	std::copy(model_copy.getAnimationCallbacks().begin(), 
	          model_copy.getAnimationCallbacks().end(), 
		  m_AnimationCallbacks.begin());

	std::cout << "MODEL animation count = " << m_AnimationCallbacks.size() << "\n";

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

	// XXX the switch node is probably not necessary in most cases and should be removed
	// to switch between various representations of the same object (depending on views for example)
	m_Switch = new osg::Switch;
	m_Switch->addChild(model_node);
	m_Switch->setAllChildrenOn();
	m_Transform = new osg::PositionAttitudeTransform;
	m_Transform->addChild(m_Switch.get());
	m_Transform->addChild(m_Model->getDebugMarkers().get());
	m_Transform->addChild(m_modelview_abs);
	m_Smoke = false;
	//show();
}

SceneModel::~SceneModel() {
	osg::Node *model_node = m_Model->getModel().get();
	assert(model_node);
	m_Switch->removeChild(model_node);
	m_Transform->removeChild(m_Switch.get()); 
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
				trail->setTexture("Images/white-smoke-hilite.rgb");
				trail->setColorRange(osg::Vec4(0.9, 0.9, 1.0, 1.0), osg::Vec4(1.0, 1.0, 1.0, 0.5));
				trail->setSizeRange(0.2, 4.0);
				trail->setLight(false);
				trail->setLifeTime(5.5);
				trail->setExpansion(1.2);
				trail->addOperator(new fx::SmokeThinner);

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
	int index, n = m_AnimationCallbacks.size();
	for (index = 0; index < n; ++index) {
		DataChannel<double>::CRef channel;
		if (bus.valid()) {
			std::string name = m_AnimationCallbacks[index]->getChannelName();
			channel = bus->getChannel(name, false);
		}
		m_AnimationCallbacks[index]->bindChannel(channel);
	}
}


