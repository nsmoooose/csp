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
 * @file Channels.cpp
 *
 * Catchall file for defining the channel strings declared in
 * the various CSPSim/Include/...Channels.h files.  Channels
 * declared in headers in other directories should be defined
 * elsewhere.
 *
 */

#include <csp/cspsim/ConditionsChannels.h>
#include <csp/cspsim/ControlInputsChannels.h>
#include <csp/cspsim/ControlSurfacesChannels.h>
#include <csp/cspsim/FlightDynamicsChannels.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/LandingGearChannels.h>
#include <csp/cspsim/NavigationChannels.h>

namespace csp {

namespace bus {

	const char *Kinetics::Attitude = "Kinetics.Attitude";
	const char *Kinetics::AngularVelocity = "Kinetics.AngularVelocity";
	const char *Kinetics::AngularVelocityBody = "Kinetics.AngularVelocityBody";
	const char *Kinetics::AccelerationBody = "Kinetics.AccelerationBody";
	const char *Kinetics::Velocity = "Kinetics.Velocity";
	const char *Kinetics::Position = "Kinetics.Position";
	const char *Kinetics::ModelPosition = "Kinetics.ModelPosition";
	const char *Kinetics::NearGround = "Kinetics.NearGround";
	const char *Kinetics::Mass = "Kinetics.Mass";
	const char *Kinetics::Inertia= "Kinetics.Inertia";
	const char *Kinetics::InertiaInverse = "Kinetics.InertiaInverse";
	const char *Kinetics::GroundZ = "Kinetics.GroundZ";
	const char *Kinetics::GroundN = "Kinetics.GroundN";
	const char *Kinetics::Heading = "Kinetics.Heading";
	const char *Kinetics::Roll = "Kinetics.Roll";
	const char *Kinetics::Pitch = "Kinetics.Pitch";
	const char *Kinetics::CenterOfMassOffset = "Kinetics.CenterOfMassOffset";
	const char *Kinetics::StoresDynamics = "Kinetics.StoresDynamics";

	const char *FlightDynamics::Alpha = "FlightDynamics.Alpha";
	const char *FlightDynamics::Beta = "FlightDynamics.Beta";
	const char *FlightDynamics::Airspeed = "FlightDynamics.Airspeed";
	const char *FlightDynamics::QBar = "FlightDynamics.QBar";
	const char *FlightDynamics::GForce = "FlightDynamics.G";  // DEPRECATED
	const char *FlightDynamics::G = "FlightDynamics.G";
	const char *FlightDynamics::LateralG = "FlightDynamics.LateralG";
	const char *FlightDynamics::PressureAltitude = "FlightDynamics.PressureAltitude";
	const char *FlightDynamics::VerticalVelocity = "FlightDynamics.VerticalVelocity";

	const char *Conditions::WindVelocity = "Conditions.WindVelocity";
	const char *Conditions::Pressure = "Conditions.Pressure";
	const char *Conditions::Density = "Conditions.Density";
	const char *Conditions::Temperature = "Conditions.Temperature";
	const char *Conditions::Mach = "Conditions.Mach";
	const char *Conditions::CAS = "Conditions.CAS";

	const char *ControlSurfaces::AileronDeflection = "ControlSurfaces.AileronDeflection";
	const char *ControlSurfaces::ElevatorDeflection = "ControlSurfaces.ElevatorDeflection";
	const char *ControlSurfaces::RudderDeflection = "ControlSurfaces.RudderDeflection";
	const char *ControlSurfaces::AirbrakeDeflection = "ControlSurfaces.AirbrakeDeflection";
	const char *ControlSurfaces::TrailingEdgeFlapDeflection = "ControlSurfaces.TrailingEdgeFlapDeflection";
	const char *ControlSurfaces::LeadingEdgeFlapDeflection = "ControlSurfaces.LeadingEdgeFlapDeflection";

	const char *ControlInputs::PitchInput = "ControlInputs.PitchInput";
	const char *ControlInputs::RollInput = "ControlInputs.RollInput";
	const char *ControlInputs::RudderInput = "ControlInputs.RudderInput";
	const char *ControlInputs::LeftBrakeInput = "ControlInputs.LeftBrakeInput";
	const char *ControlInputs::RightBrakeInput = "ControlInputs.RightBrakeInput";
	const char *ControlInputs::ThrottleInput = "ControlInputs.ThrottleInput";
	const char *ControlInputs::AirbrakeInput = "ControlInputs.AirbrakeInput";
	
	const char *ControlInputs::BrakeInput = "ControlInputs.BrakeInput";
	const char *ControlInputs::SteeringInput = "ControlInputs.SteeringInput";

	const char *Navigation::ActiveSteerpoint = "Navigation.ActiveSteerpoint";

	const char *LandingGear::WOW = "LandingGear.WOW";
	const char *LandingGear::FullyExtended = "LandingGear.FullyExtended";
	const char *LandingGear::FullyRetracted = "LandingGear.FullyRetracted";
	const char *LandingGear::GearExtendSelected = "LandingGear.GearExtendSelected";
	const char *LandingGear::GearCommand = "LandingGear.GearCommand";
	std::string LandingGear::selectWOW(std::string const &gearname) { return "LandingGear." + gearname + ".WOW"; }
	std::string LandingGear::selectFullyExtended(std::string const &gearname) { return "LandingGear." + gearname + ".FullyExtended"; }
	std::string LandingGear::selectFullyRetracted(std::string const &gearname) { return "LandingGear." + gearname + ".FullyRetracted"; }
	std::string LandingGear::selectAntilockBrakingActive(std::string const &gearname) { return "LandingGear." + gearname + ".AntilockBrakingActive"; }
	std::string LandingGear::selectBrakeCommand(std::string const &gearname) { return "LandingGear." + gearname + ".BrakeCommand"; }
	std::string LandingGear::selectSteeringCommand(std::string const &gearname) { return "LandingGear." + gearname + ".SteeringCommand"; }
}

} // namespace csp

