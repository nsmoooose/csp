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
 * @file DynamicObject.h
 *
 **/

#ifndef __DYNAMICOBJECT_H__
#define __DYNAMICOBJECT_H__


#include "SimObject.h"
#include "InputInterface.h"
#include "BaseController.h"
#include "SmokeEffects.h"


namespace osgParticle {
	class Geode;
	class ParticleSystemUpdater;
	class ModularEmitter;
}
//#include <osg/Geode>
//#include <osgParticle/ParticleSystemUpdater>
//#include <osgParticle/ModularEmitter>

/**
 * class DynamicObject - Base class for all mobile objects in the simulation.
 *
 */
class DynamicObject: public SimObject, public InputInterface
{
public:
	EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(DynamicObject, SimObject)
		SIMDATA_XML("mass", DynamicObject::m_Mass, true)
		SIMDATA_XML("inertia", DynamicObject::m_Inertia, false)
	END_SIMDATA_XML_INTERFACE

	DynamicObject();
	virtual ~DynamicObject();

	void setController(BaseController * pController) { 
		m_pController = pController;
		m_iControllerID = pController->controller_id;
	}
	unsigned int getControllerID() const { return m_iControllerID; }

	void setVelocity(simdata::Vector3 const & velocity);
	void setVelocity(double Vx, double Vy, double Vz);
	simdata::Vector3 const & getVelocity() const { return m_LinearVelocity; }
	double getSpeed() const { return m_Speed; }

	bool isLocal() const { return m_Local; }
	bool isHuman() const { return m_Human; }
	void setLocal(bool local=true) { m_Local = local; }
	void setHuman(bool human=true) { m_Human = human; }

	void updateLocalPosition();
	void updateGlobalPosition();
	void updateGroundPosition();
	void updateOrientation();
	
	virtual void onUpdate(double dt);
	
	bool AddSmoke();
	bool isSmoke();
	void DisableSmoke();
	void EnableSmoke();

protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);

	virtual void doMovement(double dt);
	virtual void postMotion(double dt);

	BaseController * m_pController;
	unsigned int m_iControllerID;
	
	bool m_Smoke;
	fx::smoke::SmokeSegments *m_SmokeSegments;
	osg::ref_ptr<osg::Geode> m_SmokeGeode;
	osg::ref_ptr<osgParticle::ParticleSystemUpdater> m_SmokeUpdater;
	osg::ref_ptr<osgParticle::ModularEmitter> m_SmokeEmitter;


	// dynamic properties
	
	simdata::Vector3 m_PrevPosition;
	
	double m_Mass;
	double m_Speed;

	bool m_Local;
	bool m_Human;

	simdata::Matrix3 m_Inertia;
	simdata::Matrix3 m_InertiaInv;

	simdata::Vector3 m_AngularVelocity;
	simdata::Vector3 m_LinearVelocity;
	simdata::Vector3 m_LinearVelocityDirection;
};


#endif // __DYNAMICOBJECT_H__

