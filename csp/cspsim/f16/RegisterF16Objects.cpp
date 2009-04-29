#ifndef __CSPSIM_F16_REGISTERF16OBJECTS_H__
#define __CSPSIM_F16_REGISTERF16OBJECTS_H__

#include <csp/cspsim/f16/DataEntryDisplay.h>
#include <csp/cspsim/f16/F16Engine.h>
#include <csp/cspsim/f16/F16FlightModel.h>
#include <csp/cspsim/f16/F16GearDynamics.h>
#include <csp/cspsim/f16/F16HUD.h>
// Is included in compilation
// #include <csp/cspsim/f16/F16InputSystem.h>
#include <csp/cspsim/f16/F16INS.h>
#include <csp/cspsim/f16/F16System.h>
#include <csp/cspsim/f16/FLCS.h>
#include <csp/cspsim/f16/FuelSystem.h>
#include <csp/cspsim/f16/GroundAvoidanceAdvisory.h>
#include <csp/cspsim/f16/JetFuelStarter.h>
#include <csp/cspsim/f16/MultiFunctionDisplay.h>
#include <csp/cspsim/f16/PanelHUD.h>
#include <csp/cspsim/f16/Radio.h>
#include <csp/cspsim/f16/UpFrontControls.h>
#include <csp/cspsim/f16/VoiceMessageUnit.h>

namespace csp {

void registerF16Objects() {
	{ static DataEntryDisplay::__csp_interface_proxy instance; }
	{ static F16Engine::__csp_interface_proxy instance; }
	{ static F16FlightModel::__csp_interface_proxy instance; }
	{ static F16GearDynamics::__csp_interface_proxy instance; }
	{ static F16HUD::__csp_interface_proxy instance; }
	// Isn't included in compilation.
	// { static F16InputSystem::__csp_interface_proxy instance; }
	{ static F16INS::__csp_interface_proxy instance; }
	{ static F16System::__csp_interface_proxy instance; }
	registerFLCSObjects();
	{ static f16::FuelSystem::__csp_interface_proxy instance; }
	{ static GroundAvoidanceAdvisory::__csp_interface_proxy instance; }
	{ static JetFuelStarter::__csp_interface_proxy instance; }
	{ static MultiFunctionDisplay::__csp_interface_proxy instance; }
	{ static PanelHUD::__csp_interface_proxy instance; }
	{ static F16Radio::__csp_interface_proxy instance; }
	{ static UpFrontControls::__csp_interface_proxy instance; }
	{ static VoiceMessageUnit::__csp_interface_proxy instance; }
}

} // namespace csp

#endif // __CSPSIM_F16_REGISTERF16OBJECTS_H__
