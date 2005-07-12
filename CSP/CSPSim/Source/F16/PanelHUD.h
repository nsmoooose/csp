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


#ifndef __F16_PANEL_HUD_H__
#define __F16_PANEL_HUD_H__

#include <System.h>
#include <SimData/Enum.h>


class PanelHUD: public System {
public:
	SIMDATA_DECLARE_OBJECT(PanelHUD)

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
	DataChannel<simdata::EnumLink>::Ref b_DataSwitch;
	DataChannel<simdata::EnumLink>::Ref b_FlightPathMarkerSwitch;
	DataChannel<simdata::EnumLink>::Ref b_ScalesSwitch;
	DataChannel<simdata::EnumLink>::Ref b_VelocitySwitch;
	DataChannel<simdata::EnumLink>::Ref b_AltitudeSwitch;

	static const simdata::Enumeration DataSwitchSettings;
	static const simdata::Enumeration FlightPathMarkerSettings;
	static const simdata::Enumeration ScalesSwitchSettings;
	static const simdata::Enumeration VelocitySwitchSettings;
	static const simdata::Enumeration AltitudeSwitchSettings;
};


#endif // __F16_PANEL_HUD_H__

