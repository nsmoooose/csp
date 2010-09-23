// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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

#include <csp/cspsim/AircraftEngine.h>
#include <csp/cspsim/AircraftObject.h>
#include <csp/cspsim/AnimationSequence.h>
#include <csp/cspsim/DataRecorder.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/Collision.h>
#include <csp/cspsim/FlightModel.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/SceneModel.h>
#include <csp/cspsim/SystemsModel.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <sstream>
#include <iomanip>

namespace csp {

CSP_XML_BEGIN(AircraftObject)
CSP_XML_END

DEFINE_INPUT_INTERFACE(AircraftObject)


AircraftObject::AircraftObject(): DynamicObject(TYPE_AIR_UNIT) {
	CSPLOG(DEBUG, OBJECT) << "AircraftObject::AircraftObject() ...";
	m_ObjectName = "AIRCRAFT";

	b_Heading = DataChannel<double>::newLocal(bus::Kinetics::Heading, 0.0);
	b_Roll = DataChannel<double>::newLocal(bus::Kinetics::Roll, 0.0);
	b_Pitch = DataChannel<double>::newLocal(bus::Kinetics::Pitch, 0.0);

	CSPLOG(DEBUG, OBJECT) << "... AircraftObject::AircraftObject()";
}

AircraftObject::~AircraftObject() {
}

void AircraftObject::postCreate() {
	DynamicObject::postCreate();
}

void AircraftObject::registerChannels(Bus* bus) {
	DynamicObject::registerChannels(bus);
	if (bus) {
		bus->registerChannel(b_Heading.get());
		bus->registerChannel(b_Pitch.get());
		bus->registerChannel(b_Roll.get());
	}
}

void AircraftObject::bindChannels(Bus* bus) {
	DynamicObject::bindChannels(bus);
}

void AircraftObject::dump() {
}

void AircraftObject::initialize() {
}

double AircraftObject::onUpdate(double dt) {
	return DynamicObject::onUpdate(dt);
}

void AircraftObject::onRender() {
	DynamicObject::onRender();
	if (m_SceneModel.valid()) {
		/** 
		 * @warning XXX TEMPORARY HACK 
		 * @todo Remove need for hack.
		 */
		if (getSystemsModel().valid()) {
			Ref<AircraftEngineDynamics> ed = getSystemsModel()->getSystem("EngineDynamics",false);
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

void AircraftObject::setAttitude(double pitch, double roll, double heading) {
	b_Pitch->value() = pitch;
	b_Roll->value() = roll;
	b_Heading->value() = heading;
	
	Quat attitude;
	/** use standard Euler convension (X axis is roll, Y is pitch, Z is yaw). */
	attitude.makeRotate(roll, pitch, heading);
	/** convert to CSP coordinate system (X axis is pitch, Y axis is roll, -Z is yaw) */
	Quat modified(attitude.y(), attitude.x(), -attitude.z(), attitude.w());
	DynamicObject::setAttitude(modified);
}

void AircraftObject::postUpdate(double dt) {
	DynamicObject::postUpdate(dt);

	/** convert from CSP frame to standard Euler (X is roll, Y is pitch, Z is yaw) */
	Quat modified = b_Attitude->value();
	modified = Quat(modified.y(), modified.x(), -modified.z(), modified.w());
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
	line << ", Pitch: " << std::setw(4) << pitch << ", Roll: " << std::setw(4) << roll;
	info.push_back(line.str());
}

void AircraftObject::setDataRecorder(DataRecorder *recorder) {
	DynamicObject::setDataRecorder(recorder);
	if (!recorder) return;
	recorder->addSource(b_Pitch);
	recorder->addSource(b_Roll);
	recorder->addSource(b_Heading);
}

} // namespace csp

