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
 * @file SimObject.h
 *
 **/

#ifndef __SIMOBJECT_H__
#define __SIMOBJECT_H__


#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Switch>
#include <osgParticle/SegmentPlacer>

#include "ObjectModel.h"

#include <SimData/Object.h>
#include <SimData/Types.h>
#include <SimData/InterfaceRegistry.h>


class VirtualBattlefield;


/**
 * class SimObject - Base class for all objects in the simulation.
 *
 */

class SimObject: public simdata::Object
{
public:

	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(SimObject)
		SIMDATA_XML("model", SimObject::m_Model, true)
		SIMDATA_XML("army", SimObject::m_Army, true)
	END_SIMDATA_XML_INTERFACE


	SimObject();
	virtual ~SimObject();

	virtual void dump() = 0;

	virtual void onUpdate(double dt) = 0;

	virtual void initialize() = 0;
	virtual unsigned int onRender() = 0;

	unsigned int getObjectID() const { return m_iObjectID; }
	unsigned int getObjectType() const { return m_iObjectType; }
	
	void setFreezeState(bool flag) { m_bFreezeFlag = flag; }
	bool getFreezeState() const { return m_bFreezeFlag; }

	void setDeleteFlag(bool flag) { m_bDeleteFlag = flag; }
	bool getDeleteFlag() const { return m_bDeleteFlag; }

	virtual void getStats(std::vector<std::string> &stats) {}

	// bind the osg model
	osg::Node *getNode();
	void setCullingActive(bool flag);
	
	void initModel();
	void addToScene(VirtualBattlefield *);
	void removeFromScene();
	virtual int updateScene();

	void ShowRepresentant(unsigned short const p_usflag);

	// position accessor methods
	simdata::Vector3 const & getLocalPosition() const { return m_LocalPosition; }
	simdata::Vector3 const & getGlobalPosition() const { return m_GlobalPosition; }
	void getLatticePosition(int & x, int & y) const;

	// These are "incomplete" position updates which (probably) should never
	// be done (especially as public methods).
	// ---------------------------------------------------------------------
	//void setLocalPosition(simdata::Vector3 const & position);
	//void setLocalPosition(double x, double y, double z);
	//void setLatticePosition(int x, int y);
	
	void setGlobalPosition(simdata::Vector3 const & position);
	void setGlobalPosition(double x, double y, double z);

	simdata::Matrix3 & getOrientation() { return m_Orientation; }
	simdata::Matrix3 const & getOrientation() const { return m_Orientation; }
	simdata::Quaternion & getOrientationQ() { return m_qOrientation; }
	simdata::Quaternion const & getOrientationQ() const { return m_qOrientation; }
	void setOrientation(simdata::Matrix3 const & mOrientation);
	void setOrientation(simdata::Quaternion const & qOrientation);

	void setArmy(int army) { m_Army = army; }
	int getArmy() const { return m_Army; }

	double getBoundingSphereRadius() const { if (m_Model.isNull()) return 0.0; return m_Model->getBoundingSphereRadius(); }

	void setObjectName(const std::string name) { m_sObjectName = name; }
	std::string getObjectName() const { return m_sObjectName; }

	simdata::Vector3 const & getDirection() const { return m_Direction; }
	simdata::Vector3 const & getUpDirection() const { return m_NormalDirection; }
	
protected:

	int m_Army;
	simdata::PathPointer<ObjectModel> m_Model;

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);

	simdata::Matrix3 m_Orientation;
	simdata::Matrix3 m_OrientationInverse;
	simdata::Quaternion m_qOrientation;

	simdata::Vector3 m_Direction;
	simdata::Vector3 m_NormalDirection;

	simdata::Vector3 m_GlobalPosition;
	simdata::Vector3 m_LocalPosition;
	int m_XLatticePos;
	int m_YLatticePos;

	VirtualBattlefield *m_Battlefield;

	osg::ref_ptr<osg::Node> m_rpNode;
	osg::ref_ptr<osg::MatrixTransform> m_rpTransform;
	osg::ref_ptr<osg::Switch> m_rpSwitch;

	unsigned int m_iObjectID;
	unsigned int m_iObjectType;

	bool m_bDeleteFlag;
	bool m_bFreezeFlag;
	bool m_bOnGround;
	bool m_ModelInit;

	// The object name holds an identifier string for
	// in-game display.  It is not intended for unique
	// object identification.
	// e.g. "Callsign", Cowboy11, Static
	std::string m_sObjectName;
};


#endif // __SIMOBJECT_H__


