// Combat Simulator Project - CSPSim
// Copyright (C) 2002, 2003 The Combat Simulator Project
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
 * @file AircraftDynamics.h
 *
 **/


#ifndef __AIRCRAFTDYNAMICS_H__
#define __AIRCRAFTDYNAMICS_H__

#include <DynamicObject.h>
#include <Engine.h>
#include <LandingGear.h>
#include <FlightDynamics.h>
#include <FlightModel.h>
#include <System.h>
#include <KineticsChannels.h>


class VehiclePhysics: public System, public PhysicsModel {
protected:
	void importChannels(Bus *) {
		b_Position = bus->getChannel(Kinetics::Position);
		b_Mass = bus->getChannel(Kinetic::Mass);
		b_GroundZ = bus->getChannel(Kinetic::GroundZ);
		b_GroundN = bus->getChannel(Kinetic::GroundN);
		b_NearGround = bus->getChannel(Kinetic::NearGround);
		b_Inertia = bus->getChannel(Kinetic::Inertia);
		b_InertiaInv = bus->getChannel(Kinetic::InertiaInverse);
		b_AngularVelocity = bus->getChannel(Kinetic::AngularVelocity);
		b_LinearVelocity = bus->getChannel(Kinetic::Velocity);
		b_Attitude = bus->getChannel(Kinetic::Attitude);
	}

	virtual bool addChild(Node *node) {
		simdata::Ref<BaseDynamics> dynamics = node;
		if (dynamics.valid()) addDynamics(dynamics.get());
		return System::addChild(node);
	}

	DataChannel<simdata::Vector3>::CRef b_Position;
	DataChannel<simdata::Vector3>::CRef b_Velocity;
	DataChannel<simdata::Vector3>::CRef b_AngularVelocity;
	DataChannel<simdata::Quat>::CRef b_Attitude;

	DataChannel<simdata::double>::CRef b_Mass;
	DataChannel<simdata::Matrix3>::CRef b_Inertia;
	DataChannel<simdata::Matrix3>::CRef b_InertiaInv;

	DataChannel<simdata::Vector3>::CRef b_GroundN;
	DataChannel<double>::CRef b_GroundZ;
	DataChannel<bool>::Ref b_NearGround;
};


class AircraftPhysics: public VehiclePhysics {
	simdata::Ref<FlightDynamics> m_FlightDynamics;
	simdata::Link<FlightModel> m_FlightModel;
	simdata::Link<GearDynamics> m_GearDynamics;
	simdata::Link<EngineDynamics> m_EngineDynamics;
public:

	SIMDATA_OBJECT(AircraftDynamics, 4, 0)

	BEGIN_SIMDATA_XML_INTERFACE(AircraftDynamics)
		SIMDATA_XML("flight_model", AircraftDynamics::m_FlightModel, true)
		SIMDATA_XML("gear_dynamics", AircraftDynamics::m_GearDynamics, true) 
		SIMDATA_XML("engine_dynamics", AircraftDynamics::m_EngineDynamics, true)
	END_SIMDATA_XML_INTERFACE

	simdata::Ref<FlightDynamics> getFlightDynamics() const {
		return m_FlightDynamics;
	}
	simdata::Ref<GearDynamics> getGearDynamics() const {
		return m_GearDynamics;
	}
	simdata::Ref<EngineDynamics> getEngineDynamics() const {
		return m_EngineDynamics;
	}

protected:
	void postCreate();

	void registerChannels(Bus::Ref) {
		b_WindVelocity = bus->registerChannel(DataChannel<simdata::Vector3>::newLocal("Conditions.WindVelocity", simdata::Vector3::ZERO));
		b_CAS = bus->registerChannel(DataChannel<double>::newLocal("Conditions.CalibratedAirspeed", 0.0));
		b_Pressure = bus->registerChannel(DataChannel<double>::newLocal("Conditions.Pressure", 1.0));
		b_Temperature = bus->registerChannel(DataChannel<double>::newLocal("Conditions.Temperature", 1.0));
		b_qBar = bus->registerChannel(DataChannel<double>::newLocal("Conditions.DynamicPressure", 1.0));
	}
	void importChannels(Bus *bus) { VehicleDynamics::importChannels(bus); }

	double onUpdate(double dt) {
	}

	DataChannel<simdata::Vector3>::Ref b_WindVelocity;
	DataChannel<double>::Ref b_qBar;
	DataChannel<double>::Ref b_CAS;
	DataChannel<double>::Ref b_Temperature;
	DataChannel<double>::Ref b_Pressure;
};


class AircraftFlightSensors: public System {
public:

protected:
	double onUpdate(double dt) {
		simdata::Vector3 pos = b_Position->value();
		Atmosphere const *atmosphere = CSPSim::theSim->getAtmosphere();
		if (atmosphere)	{
			b_Density->value() = atmosphere->getDensity(pos.z());
			b_Density->value() = atmosphere->getDensity(pos.z());
			simdata::Vector3 wind = atmosphere->getWind(pos);
			wind += atmosphere->getTurbulence(pos, m_Distance);
			b_WindVelocityBody->value() = wind;
			m_Distance += (wind - b_Velocity.value()).length() * dt;
		} else {
			b_Density->value() = 1.25; // nominal sea-level air density
			b_WindVelocityBody->value() = simdata::Vector3::ZERO;
		}
		return 0.101;
	}

	void importChannels(Bus *bus) {
		b_Position = bus->getChannel("Kinetics.Position");
		b_Velocity = bus->getChannel("Kinetics.LinearVelocity");
	}

	void registerChannel(Bus::Ref bus) {
		b_WindVelocityBody = bus->registerLocalDataChannel("Conditions.WindVelocity", 0.0);
		b_Pressure = bus->registerLocalDataChannel("Conditions.Pressure", 1.25);
		b_Density = bus->registerLocalDataChannel("Conditions.Density", 100000.0);
		b_Temperature = bus->registerLocalDataChannel("Conditions.Temperature", 300);
	}

protected:
	DataChannel<simdata::Vector3>::Ref b_Position;
	DataChannel<simdata::Vector3>::Ref b_Velocity;
	DataChannel<double>::Ref b_Temperature;
	DataChannel<double>::Ref b_Pressure;
	DataChannel<double>::Ref b_Density;

private:
	double m_Distance;
};

#endif // __AIRCRAFTDYNAMICS_H__


