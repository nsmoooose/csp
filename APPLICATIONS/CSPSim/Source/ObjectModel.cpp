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


#include "LogStream.h"
#include "Platform.h"
#include "ObjectModel.h"
#include "Config.h"

#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/SmoothingVisitor>


SIMDATA_REGISTER_INTERFACE(ObjectModel)


std::string g_ModelPath = "";


ObjectModel::ObjectModel(): simdata::Object() {
	m_Rotation = simdata::Matrix3::IDENTITY;
	m_Axis0 = simdata::Vector3::XAXIS;
	m_Axis1 = simdata::Vector3::ZAXIS;
	m_Offset = simdata::Vector3::ZERO;
	m_Scale = 1.0;
}

ObjectModel::~ObjectModel() {
}

void ObjectModel::pack(simdata::Packer& p) const {
	Object::pack(p);
	p.pack(m_ModelPath);
	p.pack(m_Rotation);
	p.pack(m_Axis0);
	p.pack(m_Axis1);
	p.pack(m_Offset);
	p.pack(m_Scale);
}

void ObjectModel::unpack(simdata::UnPacker& p) {
	Object::unpack(p);
	p.unpack(m_ModelPath);
	p.unpack(m_Rotation);
	p.unpack(m_Axis0);
	p.unpack(m_Axis1);
	p.unpack(m_Offset);
	p.unpack(m_Scale);
	loadModel();
}

void ObjectModel::loadModel() {
	if (g_ModelPath == "") {
		g_ModelPath = g_Config.getPath("Paths", "ModelPath", ".", true);
	}

	std::string source = ospath::filter(ospath::join(g_ModelPath, m_ModelPath.getSource()));

	CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectModel::loadModel: " << source);

	osg::Node *pNode = osgDB::readNodeFile(source);

	if (pNode) {
		CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectModel::loadModel: readNodeFile() succeeded");
	} else {
		CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectModel::loadModel: readNodeFile() failed.");
	}

	assert(pNode);

	m_Node = pNode;
	m_Node->setName("3D MODEL"); // why?
	
	osg::BoundingSphere s = m_Node->getBound();
	m_BoundingSphereRadius = s.radius();
    
	m_Transform = osgNew osg::MatrixTransform;
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
	// TODO test for o approx equal to identity.  if so, skip transform and return model node directly.
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

	//osg::StateSet * stateSet = m_rpNode->getStateSet();
	//stateSet->setGlobalDefaults();
	//m_rpNode->setStateSet(stateSet);
    
    	// TODO: make optional from xml interface?
	osgUtil::SmoothingVisitor sv;
	m_Transform->accept(sv);

	// TODO: run an optimizer to flatten the model transform (if it differs from identity)
}


osg::ref_ptr<osg::Node> ObjectModel::getModel() {
	//return m_Node;
	return m_Transform.get(); 
}

