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
 * @file Collision.h
 *
 **/
#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <SimData/Matrix3.h>
#include <SimData/Ref.h>
#include <SimData/Vector3.h>

#include "BaseDynamics.h"

class ContactList;

// base class for collision/impulsion/contact response
class ContactData {
protected:
	simdata::Vector3 m_pb;					// position of contact point in body coordinates
	simdata::Vector3 m_vb, m_vtdirb, m_avb; // velocity, tangent velocity direction, angular velocity in body coordinates
	simdata::Vector3 m_normalBody;			// contact normal in body coordinates
	double m_deep;							// contact deep in collided object
	simdata::Vector3 m_Force, m_Moment;
	friend class ContactList;
public:
	ContactData(simdata::Vector3 const& pb, simdata::Vector3 const& vb, simdata::Vector3 const& avb, 
				  simdata::Vector3 const& nb, double deep);
	simdata::Vector3 getTangentVelocityDir() const;
	void setForce(simdata::Vector3 const &force);
	virtual ~ContactData(){}
};

class HardContactData: public ContactData {
	simdata::Vector3 m_LinearImpulsion, m_AngularImpulsion;
	friend class ContactList;
public:
	HardContactData(simdata::Vector3 const& pb, simdata::Vector3 const& vb, simdata::Vector3 const& avb, 
				    simdata::Vector3 const& nb, double deep);
	void setImpulsion(simdata::Vector3 const& linearImpulsion, simdata::Vector3 const& angularImpulsion);
};


class ContactList {
	typedef std::vector<ContactData *> ContactDataList;
	double m_Deep;
	simdata::Vector3 m_NormalMaxDeep;
	ContactDataList m_contactDataList;
	simdata::Vector3 m_ForceBody, m_MomentBody;
	unsigned short m_Nhcd;
	simdata::Vector3 m_LinearImpulsion, m_AngularImpulsion;
	void deleteElements();
public:
	ContactList();
	~ContactList();
	void addContact(ContactData *cd);
	void reset();
	unsigned short size() const;
	bool hasImpulsion() const;
	simdata::Vector3 zCorrection() const;
	simdata::Vector3 linearImpulsion() const;
	simdata::Vector3 angularImpulsion() const;
	simdata::Vector3 forceBody() const;
	simdata::Vector3 momentBody() const;
};


class GroundCollisionDynamics: public simdata::Referenced, public BaseDynamics {
	static const double TOL;
	ContactList m_ContactList;
	std::vector<simdata::Vector3> const m_Contacts;
	double m_MassInverse;
	simdata::Matrix3 m_InertiaInverse;
	double const m_Elastic, m_Friction;
	double const m_Vtol;
	simdata::Vector3 const *m_WeightBody;
public:
	GroundCollisionDynamics(double mass, simdata::Matrix3 const &inertia, std::vector<simdata::Vector3> const &contacts);
	void reset(double dt);
	void bindWeight(simdata::Vector3 const &weightBody);
	void update(double dt);
	simdata::Vector3 getLinearImpulsion() const;
	simdata::Vector3 getAngularImpulsion() const;
	simdata::Vector3 getZCorrection() const;
	bool hasImpulsion() const;
	bool hasContact() const;
};

#endif // __COLLISION_H__

