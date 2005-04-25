// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file GearAnimation.cpp
 *
 **/


#include "GearAnimation.h"

SIMDATA_REGISTER_INTERFACE(GearSequenceAnimation)
SIMDATA_REGISTER_INTERFACE(M2kGearAnimation)


void GearSequenceAnimation::setGearName(std::string const &name) {
	GearAnimation::setGearName(name);
	const std::string control_channel = prefix() + ".RetractCompress";
	if (m_RetractSequence.valid()) m_RetractSequence->setSequenceChannel(control_channel);
	if (m_CompressionSequence.valid()) m_CompressionSequence->setSequenceChannel(control_channel);
}

void GearSequenceAnimation::forceExtend() {
	if (m_RetractSequence.valid()) m_RetractSequence->jumpToEnd();
	if (m_CompressionSequence.valid()) m_CompressionSequence->enable();
}

void GearSequenceAnimation::registerChannels(Bus* bus) {
	if (m_RetractSequence.valid()) m_RetractSequence->registerChannels(bus);
	if (m_CompressionSequence.valid()) m_CompressionSequence->registerChannels(bus);
	b_TireRotation = bus->registerLocalDataChannel<double>(prefix() + ".TireRotation", 0.0);
	b_SteeringAngle = bus->registerLocalDataChannel<double>(prefix() + ".SteeringAngle", 0.0);
}


void M2kGearAnimation::setCompression(double compression) {
	const double compression_distance = compression * getCompressionScale();
	b_Absorber02Angle->value() = asin((compression_distance-m_Offset)/(2*m_Absorber02Length));
	b_Absorber03Angle->value() = asin((compression_distance-m_Offset)/(2*m_Absorber03Length));
	b_Compression->value() = compression_distance * getCompressionAxis();
}

void M2kGearAnimation::registerChannels(Bus* bus) {
	GearSequenceAnimation::registerChannels(bus);
	b_Absorber02Angle = bus->registerLocalDataChannel<double>(prefix() + ".Absorber02", 0.0);
	b_Absorber03Angle = bus->registerLocalDataChannel<double>(prefix() + ".Absorber03", 0.0);
	b_Compression = bus->registerLocalDataChannel<simdata::Vector3>(prefix() + ".Compression", simdata::Vector3::ZERO);
}

