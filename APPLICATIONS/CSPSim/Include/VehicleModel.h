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
 * @file VehicleModel.h
 *
 **/


#ifndef __VEHICLEMODEL_H__
#define __VEHICLEMODEL_H__

#include <SimData/Object.h>
#include <SimData/Types.h>
#include <SimData/InterfaceRegistry.h>

#include <cassert>
#include <cstdio>
#include <iostream>


using simdata::Pointer;
using simdata::PathPointer;
using simdata::Vector3;
using simdata::Matrix3;

namespace vehicle {

class Model: public simdata::Object
{
public:
	Model() {}
	virtual ~Model() {}
	virtual void onUpdate(double) {}
	virtual void detach() {}
};


class System: public simdata::Object
{
public:
	System() {}
	virtual void bind(Model *model) { assert(model); m_Model = model; }
	virtual ~System() {}
	virtual void onUpdate(double, bool) {}
protected:
	Pointer<Model> m_Model;
};

}



/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////


class Hud: public vehicle::System
{
public:
	SIMDATA_OBJECT(Hud, 0, 0)
	
	BEGIN_SIMDATA_XML_INTERFACE(Hud)
		SIMDATA_XML("center", Hud::m_Center, true)
		SIMDATA_XML("width", Hud::m_Width, true)
		SIMDATA_XML("height", Hud::m_Height, true)
	END_SIMDATA_XML_INTERFACE

	void pack(simdata::Packer &p) const {
		vehicle::System::pack(p);
		p.pack(m_Center);
		p.pack(m_Width);
		p.pack(m_Height);
	}

	void unpack(simdata::UnPacker &p) {
		vehicle::System::unpack(p);
		p.unpack(m_Center);
		p.unpack(m_Width);
		p.unpack(m_Height);
		double d = m_Center.Length();
		double w = atan2(m_Width, d);
		double h = atan2(m_Height, d);
		assert(d > 0.0);
		assert(w > 0.0);
		assert(h > 0.0);
		m_ScaleTheta = 1.0 / w;
		m_ScalePhi = 1.0 / h;
		m_CenterTheta = atan2(m_Center.z, m_Center.y);
		m_CenterPhi = atan2(m_Center.x, m_Center.y);
	}
		
	void positionFPM(Vector3 &body_velocity, double &x, double &y)
	{
		double theta = atan2(body_velocity.z, body_velocity.y);
		y = (theta - m_CenterTheta) * m_ScaleTheta;
		double phi = atan2(body_velocity.x, body_velocity.y);
		x = (phi - m_CenterPhi) * m_ScalePhi;
	}

protected:
	double m_CenterTheta, m_CenterPhi;
	double m_ScaleTheta, m_ScalePhi;
	Vector3 m_Center;
	double m_Width, m_Height;
};


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////

class GenericHud: public Hud
{
public:
	SIMDATA_OBJECT(GenericHud, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(GenericHud, Hud)
	//	SIMDATA_XML("", ::, true)
	END_SIMDATA_XML_INTERFACE

};

/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////


class F16Model;

class F16Hud: public Hud
{
public:
	SIMDATA_OBJECT(F16Hud, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(F16Hud, Hud)
	//	SIMDATA_XML("", ::, true)
	END_SIMDATA_XML_INTERFACE

	void update(double, bool);
	virtual void bind(vehicle::Model* model);

protected:
	int m_Scales;
	int m_FPM;
	int m_DED;
	int m_DepressibleReticle;
	int m_Velocity;
	int m_Altitude;
	int m_Brightness;
	int m_Test;
	F16Model * m_F16Model;
};


class F16Model: public vehicle::Model
{
public:
	SIMDATA_OBJECT(F16Model, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(F16Model)
		SIMDATA_XML("HUD", F16Model::m_HUD, true)
	END_SIMDATA_XML_INTERFACE
	
	F16Model() { }
	~F16Model() {
	}
	
	void detach() {
		m_HUD.setNull();
	}
	
	void pack(simdata::Packer &p) const {
		vehicle::Model::pack(p);
		p.pack(m_HUD);
	}

	void unpack(simdata::UnPacker &p) {
		vehicle::Model::unpack(p);
		p.unpack(m_HUD);
		m_HUD->bind(this);
	}

	int getMasterMode() { return m_MasterMode; }
	void onUpdate(double dt) {
		m_HUD->onUpdate(dt, false);
		m_HUD->onUpdate(dt, true);
	}

	PathPointer<F16Hud> m_HUD;

protected:
	
	int m_MasterMode;
	int m_Override;
	int m_EJ;
	int m_SJ;
	int m_MasterArm;
	int m_CAT;
	int m_LandingGear;
};


void F16Hud::bind(vehicle::Model* model) { 
	Hud::bind(model);
	m_F16Model = dynamic_cast<F16Model*>(model);
	assert(m_F16Model); //.valid());
}

void F16Hud::update(double, bool) {
	std::cout << "hud: master mode = " << m_F16Model->getMasterMode() << std::endl;
}

#endif // __VEHICLEMODEL_H__

