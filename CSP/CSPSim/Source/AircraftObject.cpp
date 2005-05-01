// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002, 2004 The Combat Simulator Project
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
 * @file AircraftObject.cpp
 *
 **/

#include <SimData/Math.h>

#include "AircraftObject.h"
#include "Animation.h"
#include "AnimationSequence.h"
#include "KineticsChannels.h"
#include "Collision.h"
#include "Engine.h"
#include "FlightModel.h"
#include "ObjectModel.h"
#include "SystemsModel.h"

#include <SimCore/Util/Log.h>

#include <sstream>
#include <iomanip>

using simdata::toRadians;
using simdata::toDegrees;


SIMDATA_REGISTER_INTERFACE(AircraftObject)
DEFINE_INPUT_INTERFACE(AircraftObject)

AircraftObject::AircraftObject(): DynamicObject(TYPE_AIR_UNIT) {

	CSP_LOG(OBJECT, DEBUG, "AircraftObject::AircraftObject() ...");
	m_ObjectName = "AIRCRAFT";

	b_Heading = DataChannel<double>::newLocal(bus::Kinetics::Heading, 0.0);
	b_Roll = DataChannel<double>::newLocal(bus::Kinetics::Roll, 0.0);
	b_Pitch = DataChannel<double>::newLocal(bus::Kinetics::Pitch, 0.0);
	
	CSP_LOG(OBJECT, DEBUG, "... AircraftObject::AircraftObject()");
}


AircraftObject::~AircraftObject() {
}

void AircraftObject::convertXML() {
}

void AircraftObject::postCreate() {
	DynamicObject::postCreate();
}


void AircraftObject::registerChannels(Bus::Ref bus) {
	DynamicObject::registerChannels(bus);
	if (bus.valid()) {
		bus->registerChannel(b_Heading.get());
		bus->registerChannel(b_Pitch.get());
		bus->registerChannel(b_Roll.get());
		if (m_FuelDoorSequence.valid()) {
			m_FuelDoorSequence->registerChannels(bus.get());
		}
		if (m_CanopySequence.valid()) {
			m_CanopySequence->registerChannels(bus.get());
		}
	}
}

void AircraftObject::bindChannels(Bus::Ref bus) {
	DynamicObject::bindChannels(bus);
	/* TODO REMOVE ME!
	if (m_FuelDoorSequence.valid()) {
		m_FuelDoorSequence->bindChannels(bus);
	}
	if (m_CanopySequence.valid()) {
		m_CanopySequence->bindChannels(bus);
	}
	*/
}

void AircraftObject::dump() {
}

void AircraftObject::initialize() {
}

double AircraftObject::onUpdate(double dt)
{
	if (m_FuelDoorSequence.valid()) {
		m_FuelDoorSequence->update(dt);
	}
	if (m_CanopySequence.valid()) {
		m_CanopySequence->update(dt);
	}
	return DynamicObject::onUpdate(dt);
}


void AircraftObject::onRender() {
	DynamicObject::onRender();
	if (m_SceneModel.valid()) {
		// XXX TEMPORARY HACK
		if (getSystemsModel().valid()) {
			simdata::Ref<EngineDynamics> ed = getSystemsModel()->getSystem("EngineDynamics",false);
			if (ed.valid())
				m_SceneModel->setSmokeEmitterLocation(ed->getSmokeEmitterLocation());
		}
	}
}


void AircraftObject::SmokeOn() {
	enableSmoke();
}

void AircraftObject::SmokeOff() {
	disableSmoke();
}

void AircraftObject::SmokeToggle() {
	if (isSmoke()) {
		disableSmoke();
	} else {
		enableSmoke();
	}
}

void AircraftObject::MarkersToggle() {
	m_Model->showDebugMarkers(!m_Model->getDebugMarkersVisible());
}

void AircraftObject::FuelDoorToggle() {
	if (m_FuelDoorSequence.valid()) {
		m_FuelDoorSequence->play();
	}
}

void AircraftObject::CanopyToggle() {
	if (m_CanopySequence.valid()) {
		m_CanopySequence->play();
	}
}

void AircraftObject::setAttitude(double pitch, double roll, double heading)
{
	b_Pitch->value() = pitch;
	b_Roll->value() = roll;
	b_Heading->value() = heading;
	
	simdata::Quat attitude;
	// use standard Euler convension (X axis is roll, Y is pitch, Z is yaw).
	attitude.makeRotate(roll, pitch, heading);
	// convert to CSP coordinate system (X axis is pitch, Y axis is roll, -Z is yaw)
	simdata::Quat modified(attitude.y(), attitude.x(), -attitude.z(), attitude.w());
	DynamicObject::setAttitude(modified);
}

void AircraftObject::postUpdate(double dt)
{
	DynamicObject::postUpdate(dt);

	// convert from CSP frame to standard Euler (X is roll, Y is pitch, Z is yaw)
	simdata::Quat modified = b_Attitude->value();
	modified = simdata::Quat(modified.y(), modified.x(), -modified.z(), modified.w());
	modified.getEulerAngles(b_Roll->value(), b_Pitch->value(), b_Heading->value());
}


void AircraftObject::getInfo(std::vector<std::string> &info) const {
	DynamicObject::getInfo(info);
	std::stringstream line;
	line.precision(0);
	float heading = static_cast<float>(toDegrees(b_Heading->value()));
	float pitch = static_cast<float>(toDegrees(b_Pitch->value()));
	float roll = static_cast<float>(toDegrees(b_Roll->value()));
	if (heading < 0.0) heading += 360.0;
	line.setf(std::ios::fixed);
	line << "Heading: " << std::setw(3) << std::setfill('0') << heading << std::setfill(' ');
	line.setf(std::ios::fixed | std::ios::showpos);
	line << ", Pitch: " << std::setw(4) << pitch
	     << ", Roll: " << std::setw(4) << roll;
	info.push_back(line.str());
}

void AircraftObject::setDataRecorder(DataRecorder *recorder) {
	DynamicObject::setDataRecorder(recorder);
	if (!recorder) return;
	recorder->addSource(b_Pitch);
	recorder->addSource(b_Roll);
	recorder->addSource(b_Heading);
}

