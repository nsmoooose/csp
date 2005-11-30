// Combat Simulator Project
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
 * @file PanelHUD.h
 *
 **/


#ifndef __CSPSIM_F16_PANEL_HUD_H__
#define __CSPSIM_F16_PANEL_HUD_H__

#include <csp/cspsim/System.h>
#include <csp/csplib/data/Enum.h>

CSP_NAMESPACE

class PanelHUD: public System {
public:
	CSP_DECLARE_OBJECT(PanelHUD)

	DECLARE_INPUT_INTERFACE(PanelHUD, System)
		BIND_ACTION("CYCLE_HUD_DATA_SWITCH", onCycleDataSwitch);
		BIND_ACTION("CYCLE_HUD_FLIGHT_PATH_MARKER_SWITCH", onCycleFlightPathMarkerSwitch);
		BIND_ACTION("CYCLE_HUD_SCALES_SWITCH", onCycleScalesSwitch);
		BIND_ACTION("CYCLE_HUD_VELOCITY_SWITCH", onCycleVelocitySwitch);
		BIND_ACTION("CYCLE_HUD_ALTITUDE_SWITCH", onCycleAltitudeSwitch);
	END_INPUT_INTERFACE  // protected:

public:
	PanelHUD();
	virtual ~PanelHUD();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

	void onCycleDataSwitch();
	void onCycleFlightPathMarkerSwitch();
	void onCycleScalesSwitch();
	void onCycleVelocitySwitch();
	void onCycleAltitudeSwitch();

protected:
	DataChannel<EnumLink>::RefT b_DataSwitch;
	DataChannel<EnumLink>::RefT b_FlightPathMarkerSwitch;
	DataChannel<EnumLink>::RefT b_ScalesSwitch;
	DataChannel<EnumLink>::RefT b_VelocitySwitch;
	DataChannel<EnumLink>::RefT b_AltitudeSwitch;

	static const Enumeration DataSwitchSettings;
	static const Enumeration FlightPathMarkerSettings;
	static const Enumeration ScalesSwitchSettings;
	static const Enumeration VelocitySwitchSettings;
	static const Enumeration AltitudeSwitchSettings;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_F16_PANEL_HUD_H__

