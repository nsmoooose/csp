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

#include <osg/ShapeDrawable>

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
#include <osg/PolygonOffset>
#include <osg/CullFace>

#include <SimData/FileUtility.h>
#include <SimData/osg.h>


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
	m_ElevationCorrection = true;
	m_PolygonOffset = 0.0;
	m_CullFace = -1;
	m_MarkersVisible = true;
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
	p.pack(m_ElevationCorrection);
	p.pack(m_PolygonOffset);
	p.pack(m_CullFace);
	p.pack(m_LandingGear);
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
	p.unpack(m_ElevationCorrection);
	p.unpack(m_PolygonOffset);
	p.unpack(m_CullFace);
	p.unpack(m_LandingGear);
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
/*
	if (g_ModelPath == "") {
		g_ModelPath = getDataPath("ModelPath");
	}

	std::string source = simdata::ospath::filter(simdata::ospath::join(g_ModelPath, m_ModelPath.getSource()));
	*/
	std::string source = m_ModelPath.getSource();

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

	if (m_PolygonOffset != 0.0) {
		osg::StateSet *ss = m_Node->getOrCreateStateSet();
		osg::PolygonOffset *po = new osg::PolygonOffset;
		po->setFactor(-1.0);
		po->setUnits(m_PolygonOffset);
		ss->setAttributeAndModes(po, osg::StateAttribute::ON);
	}

	if (m_CullFace != 0) {
		// XXX should reuse a single static CullFace instance.
		osg::StateSet *ss = m_Node->getOrCreateStateSet();
		osg::CullFace *cf = new osg::CullFace;
		cf->setMode(m_CullFace < 0 ? osg::CullFace::BACK : osg::CullFace::FRONT);
		ss->setAttributeAndModes(cf, osg::StateAttribute::ON);
	}

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

	m_DebugMarkers = new osg::Switch;
	m_Transform->addChild(m_DebugMarkers.get());

	// create visible markers for each contact point
	addContactMarkers();

	// create landing gear wheels
	addGearSprites();

	// TODO: run an optimizer to flatten the model transform (if it differs from identity)
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
		makeFrontGear(m_LandingGear[0]);
		makeLeftGear(m_LandingGear[1]);
		makeRightGear(m_LandingGear[2]);
		m_Transform->addChild(m_GearSprites.get());
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
			if (pos) {
				osg::Vec3 p = osg::Vec3(m_LandingGear[i].x,m_LandingGear[i].y,move[i].z);
				pos->setPosition(p);
				++i;
			}
		}
	}
}

void ObjectModel::showContactMarkers(bool on) {
	if (on) 
		m_DebugMarkers->setAllChildrenOn();
	else
		m_DebugMarkers->setAllChildrenOff();
	m_MarkersVisible = on;
}

//FIXME: to be moved in AircraftModel?
void ObjectModel::showGearSprites(bool on) {
	if (on)
		m_GearSprites->setNodeMask(0x1);
	else
		m_GearSprites->setNodeMask(0x0);
}

bool ObjectModel::getMarkersVisible() const {
	return m_MarkersVisible;
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
	m_Smoke = false;
	//show();
}

SceneModel::~SceneModel() {
	osg::Node *model_node = m_Model->getModel().get();
	assert(model_node);
	m_Switch->removeChild(model_node);
	m_Transform->removeChild(m_Switch.get()); 
}

void SceneModel::updateSmoke(double dt, simdata::Vector3 const &global_position, simdata::Quaternion const &attitude) {
	m_SmokeTrails->update(dt, global_position, attitude);
}

void SceneModel::setSmokeEmitterLocation(std::vector<simdata::Vector3> const &sel) {
	m_SmokeEmitterLocation = sel;
}

bool SceneModel::addSmoke() {
	if (m_SmokeTrails.valid()) 
		return true;
	else {
		size_t n = m_SmokeEmitterLocation.size();
		if (n>0) {
			for (size_t i = 0; i <n; ++i) {
				fx::SmokeTrail *trail = new fx::SmokeTrail();
				trail->setEnabled(false);
				trail->setTexture("Images/white-smoke-hilite.rgb");
				trail->setColorRange(osg::Vec4(0.9, 0.9, 1.0, 1.0), osg::Vec4(1.0, 1.0, 1.0, 0.5));
				trail->setSizeRange(0.2, 4.0);
				trail->setLight(false);
				trail->setLifeTime(5.5);
				trail->setExpansion(1.2);
				trail->addOperator(new fx::SmokeThinner);

				trail->setOffset(m_SmokeEmitterLocation[i]);

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
	if (!m_Smoke) {
		if (!addSmoke()) return;
		m_SmokeTrails->setEnabled(true);
		m_Smoke = true;
	}
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
