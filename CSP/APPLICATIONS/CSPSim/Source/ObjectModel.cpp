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


#include "ObjectModel.h"
#include "Log.h"
#include "Config.h"

#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/DisplayListVisitor>
#include <osgUtil/Optimizer>
#include <osg/NodeVisitor>
#include <osg/Geometry>
#include <osg/Texture>
#include <osg/Geode>

#include <SimData/FileUtility.h>


SIMDATA_REGISTER_INTERFACE(ObjectModel)


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
}

ObjectModel::~ObjectModel() {
}

void ObjectModel::pack(simdata::Packer& p) const {
	Object::pack(p);
	p.pack(m_ModelPath);
	p.pack(m_Axis0);
	p.pack(m_Axis1);
	p.pack(m_ViewPoint);
	p.pack(m_Offset);
	p.pack(m_Scale);
	p.pack(m_Smooth);
	p.pack(m_Filter);
	p.pack(m_Contacts);
}

void ObjectModel::unpack(simdata::UnPacker& p) {
	Object::unpack(p);
	p.unpack(m_ModelPath);
	p.unpack(m_Axis0);
	p.unpack(m_Axis1);
	p.unpack(m_ViewPoint);
	p.unpack(m_Offset);
	p.unpack(m_Scale);
	p.unpack(m_Smooth);
	p.unpack(m_Filter);
	p.unpack(m_Contacts);
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
        v[i][0] = vv[i][0] + pos.x;
        v[i][1] = vv[i][1] + pos.y;
        v[i][2] = vv[i][2] + pos.z;
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
	if (g_ModelPath == "") {
		g_ModelPath = g_Config.getPath("Paths", "ModelPath", ".", true);
	}

	std::string source = simdata::ospath::filter(simdata::ospath::join(g_ModelPath, m_ModelPath.getSource()));

	CSP_LOG(APP, DEBUG, "ObjectModel::loadModel: " << source);

	osg::Node *pNode = osgDB::readNodeFile(source);

	if (pNode) {
		CSP_LOG(APP, DEBUG, "ObjectModel::loadModel: readNodeFile() succeeded");
	} else {
		CSP_LOG(APP, DEBUG, "ObjectModel::loadModel: readNodeFile() failed.");
	}

	assert(pNode);

	m_Node = pNode;
	m_Node->setName(m_ModelPath.getSource());
	
	// XXX should do this after scaling, no?
	osg::BoundingSphere s = m_Node->getBound();
	m_BoundingSphereRadius = s.radius();
    
	m_Transform = new osg::MatrixTransform;
	m_Transform->setName("MODEL TRANSFORM");
	m_Transform->addChild(m_Node.get());

	assert(m_Axis0.Length() > 0.0);
	m_Axis0.Normalize();
	// orthogonalize
	m_Axis1 = m_Axis1 - m_Axis0 * simdata::Dot(m_Axis0, m_Axis1);
	assert(m_Axis1.Length() > 0.0);
	m_Axis1.Normalize();
	// find third axis
	simdata::Vector3 axis2 = simdata::Cross(m_Axis0, m_Axis1);
	
	simdata::Matrix3 o(m_Axis0.x, m_Axis0.y, m_Axis0.z, m_Axis1.x, m_Axis1.y, m_Axis1.z, axis2.x, axis2.y, axis2.z);
	o = o.Inverse() * m_Scale;
	simdata::Matrix3::M_t (&R)[3][3] = o.rowcol;
	osg::Matrix model_orientation;
	model_orientation.set(
		R[0][0], R[1][0], R[2][0], 0.0,
		R[0][1], R[1][1], R[2][1], 0.0,
		R[0][2], R[1][2], R[2][2], 0.0,
		m_Offset.x, m_Offset.y, m_Offset.z, 1.0
	);
	m_Transform->setMatrix(model_orientation);

	if (m_Scale != 1.0) {
		m_Transform->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	}

	//osg::StateSet * stateSet = m_rpNode->getStateSet();
	//stateSet->setGlobalDefaults();
	//m_rpNode->setStateSet(stateSet);
     
	if (m_Smooth) {
		osgUtil::SmoothingVisitor sv;
		m_Transform->accept(sv);
	}

	if (m_Filter) {
		// FIXME: level should come from global graphics settings
		TrilinearFilterVisitor tfv(16.0);
		m_Transform->accept(tfv);
	}

	// TODO: run an optimizer to flatten the model transform (if it differs from identity)

	m_DebugMarkers = new osg::Switch;
	m_Transform->addChild(m_DebugMarkers.get());

	// create visible markers for each contact point
	addContactMarkers();

	osgUtil::Optimizer opt;
	opt.optimize(m_Transform.get());

	osg::ref_ptr<osg::State> state = new osg::State;
	osgUtil::DisplayListVisitor dlv(osgUtil::DisplayListVisitor::COMPILE_DISPLAY_LISTS);
	dlv.setState(state.get());
	dlv.setNodeMaskOverride(0xffffffff);
	m_Transform->accept(dlv);
}

void ObjectModel::addContactMarkers() {
	m_ContactMarkers = new osg::Group;
	for (unsigned i = 0; i < m_Contacts.size(); i++) {
		osg::Geode *diamond = new osg::Geode;
		diamond->addDrawable(makeDiamond(m_Contacts[i], 0.2));
		m_ContactMarkers->addChild(diamond);
	}
	m_DebugMarkers->addChild(m_ContactMarkers.get());
}

void ObjectModel::showContactMarkers(bool on) {
	m_DebugMarkers->setChildValue(m_ContactMarkers.get(), on);
}


SceneModel::SceneModel(simdata::Ref<ObjectModel> const & model) {
	m_Model = model;
	assert(m_Model.valid());
	CSP_LOG(APP, INFO, "create SceneModel for " << m_Model->getModelPath());
	osg::Node *model_node = m_Model->getModel().get();
	assert(model_node);
	// to switch between various representations of the same object (depending on views for example)
	m_Switch = new osg::Switch;
	m_Switch->addChild(model_node);
	m_Switch->setAllChildrenOn();
	m_Transform = new osg::PositionAttitudeTransform;
	m_Transform->addChild(m_Switch.get());
	//show();
}

SceneModel::~SceneModel() {
	osg::Node *model_node = m_Model->getModel().get();
	assert(model_node);
	m_Switch->removeChild(model_node);
	m_Transform->removeChild(m_Switch.get()); 
}


// FIXME: from SimObject.... needs to be incorparated:
/*
void SimObject::initModel()
{ 
	CSP_LOG(APP, DEBUG, "SimObject::initModel() - ID: " << m_iObjectID);

	assert(m_rpNode == NULL && m_rpSwitch == NULL && m_rpTransform == NULL);
	assert(m_Model.valid());

	std::cout << "INIT MODEL\n";

	m_rpNode = m_Model->getModel();
    
	//osg::StateSet * stateSet = m_rpNode->getStateSet();
	//stateSet->setGlobalDefaults();
	//m_rpNode->setStateSet(stateSet);
    
	// to switch between various representants of same object (depending on views for example)
	m_rpSwitch = new osg::Switch;
	m_rpSwitch->setName("MODEL SWITCH");
	m_rpSwitch->addChild(m_rpNode.get());

	// master object to which all others ones are linked
	m_rpTransform = new osg::MatrixTransform;
	m_rpTransform->setName("MODEL TRANSFORM");

	m_rpTransform->addChild( m_rpSwitch.get() );
	//m_rpSwitch->setAllChildrenOn();
}

	if (m_rpTransform != NULL && m_rpSwitch != NULL) {
		m_rpTransform->removeChild( m_rpSwitch.get() ); 
	}

void SimObject::ShowRepresentant(unsigned short const p_usflag)
{
	m_rpSwitch->setAllChildrenOff();
	m_rpSwitch->setValue(p_usflag, true);
}

	osg::Matrix worldMat;
	simdata::Matrix3::M_t (&R)[3][3] = m_Orientation.rowcol;
	worldMat.set(R[0][0], R[1][0], R[2][0], 0.0,
	             R[0][1], R[1][1], R[2][1], 0.0,
		     R[0][2], R[1][2], R[2][2], 0.0,
		     m_LocalPosition.x, m_LocalPosition.y, m_LocalPosition.z, 1.0);

	//m_rpTransform->setReferenceFrame(osg::Transform::RELATIVE_TO_PARENTS);

	m_rpTransform->setMatrix(worldMat);
	
	// FIXME: call specific versions of this from derived classes:
	//scene->addNodeToScene(m_rpTransform.get());

	setCullingActive(true);

	//CSP_LOG(APP, DEBUG, "NodeName: " << m_rpNode->getName() <<
	//	", BoundingPos: " << sphere.center() << ", BoundingRadius: " << 
	//	sphere.radius() );


void SimObject::setCullingActive(bool flag)
{
	if (m_rpTransform.valid()) {
		m_rpTransform->setCullingActive(flag);
	}
}

*/