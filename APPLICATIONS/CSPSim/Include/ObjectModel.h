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


#include <osg/Node>
#include <osg/MatrixTransform>

#include <SimData/Object.h>
#include <SimData/Types.h>
#include <SimData/InterfaceRegistry.h>


/**
 * class ObjectModel - Interface for 3D models in the simulation.
 *
 */
class ObjectModel: public simdata::Object
{
public:
	SIMDATA_OBJECT(ObjectModel, 0, 0);
	
	BEGIN_SIMDATA_XML_INTERFACE(ObjectModel)
		SIMDATA_XML("model_path", ObjectModel::m_ModelPath, true)
		SIMDATA_XML("rotation", ObjectModel::m_Rotation, true)
		SIMDATA_XML("axis_0", ObjectModel::m_Axis0, true)
		SIMDATA_XML("axis_1", ObjectModel::m_Axis1, true)
		SIMDATA_XML("offset", ObjectModel::m_Offset, false)
		SIMDATA_XML("scale", ObjectModel::m_Scale, false)
	END_SIMDATA_XML_INTERFACE

	ObjectModel();
	virtual ~ObjectModel();

	osg::ref_ptr<osg::Node> getModel();
	std::string getModelPath() const { return m_ModelPath.getSource(); }

	const simdata::Matrix3 &getRotation() const { return m_Rotation; }
	const simdata::Vector3 &getAxis0() const { return m_Axis0; }
	const simdata::Vector3 &getAxis1() const { return m_Axis1; }

	double getBoundingSphereRadius() const { return m_BoundingSphereRadius; }

protected:

	simdata::External m_ModelPath;
	simdata::Matrix3 m_Rotation;
	simdata::Vector3 m_Axis0, m_Axis1;
	simdata::Vector3 m_Offset;
	double m_Scale;
	
	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);
	virtual void loadModel();

	double m_BoundingSphereRadius;

private:
	osg::ref_ptr<osg::Node> m_Node;
	osg::ref_ptr<osg::MatrixTransform> m_Transform;
};


#endif // __OBJECTMODEL_H__

