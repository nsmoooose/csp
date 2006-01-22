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
 * @file F16HUD.h
 *
 **/


#ifndef __CSPSIM_F16_F16HUD_H__
#define __CSPSIM_F16_F16HUD_H__

#include <csp/cspsim/f16/NavigationSystem.h>
#include <csp/cspsim/f16/AlphaNumericDisplay.h>
#include <csp/cspsim/f16/MasterModes.h>
#include <csp/cspsim/hud/HUD.h>
#include <csp/cspsim/CockpitInterface.h>
#include <csp/cspsim/Filters.h>
#include <csp/cspsim/System.h>

#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Vector3.h>

CSP_NAMESPACE

class DisplayFont;
class HUDTape;
class PitchLadder;
class DEDReadout;
class PullupAnticipationCue;
class RadarAltitudeScale;
class GhostHorizon;
class ImpactPointCue;
class BombSteeringCue;

class F16HUD: public System {
public:
	CSP_DECLARE_OBJECT(F16HUD)

	F16HUD();
	virtual ~F16HUD();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

protected:
	DataChannel<double>::CRefT b_GearExtension;
	DataChannel<double>::CRefT b_Airspeed;
	DataChannel<double>::CRefT b_Roll;
	DataChannel<double>::CRefT b_Pitch;
	DataChannel<double>::CRefT b_Heading;
	DataChannel<double>::CRefT b_Alpha;
	DataChannel<double>::CRefT b_CAS;
	DataChannel<double>::CRefT b_Mach;
	DataChannel<double>::CRefT b_G;
	DataChannel<double>::CRefT b_GroundZ;
	DataChannel<double>::CRefT b_CaraAlow;
	DataChannel<bool>::CRefT b_LeftMainLandingGearWOW;
	DataChannel<bool>::CRefT b_GearHandleUp;
	DataChannel<Quat>::CRefT b_Attitude;
	DataChannel<Vector3>::CRefT b_Position;
	DataChannel<Vector3>::CRefT b_Velocity;
	DataChannel<Ref<NavigationSystem> >::CRefT b_NavigationSystem;
	DataChannel<Ref<AlphaNumericDisplay> >::CRefT b_DEDReadout;
	DataChannel<bool>::CRefT b_AltitudeAdvisory;
	DataChannel<bool>::CRefT b_DescentWarningAfterTakeoff;
	DataChannel<double>::CRefT b_PullupAnticipation;

	DataChannel<EnumLink>::CRefT b_DataSwitch;
	DataChannel<EnumLink>::CRefT b_FlightPathMarkerSwitch;
	DataChannel<EnumLink>::CRefT b_ScalesSwitch;
	DataChannel<EnumLink>::CRefT b_VelocitySwitch;
	DataChannel<EnumLink>::CRefT b_AltitudeSwitch;
	f16::MasterModeSelection::CRefT b_MasterMode;
	CockpitInterface m_HudPanel;
	DataChannel<EnumLink>::RefT b_AOAIndexer;

	virtual double onUpdate(double dt);
	virtual void updateReadouts();
	virtual void updateSwitches();
	virtual void placeVerticalFrames();
	virtual void buildHUD();
	virtual void addFlightPathMarker();
	virtual void addGunCross();
	virtual void addRollTape();
	virtual void addBreakX();
	virtual void addHeadingTape();
	virtual void addAirspeedTape();
	virtual void addAltitudeTape();
	virtual void addTextElements();
	virtual void addPitchLadder();
	virtual void addExtraSymbols();
	virtual void addDEDReadout();
	virtual void newVerticalText(osg::ref_ptr<display::ElementText> &element, float x, float y, display::ElementText::AlignmentType alignment=display::ElementText::BASE_LINE);

	virtual double getSpeedCaret(double ground_speed) const;
	virtual double getSpeed() const;

	Vector3 m_Color;

	osg::ref_ptr<display::DirectionElement> m_Tadpole;
	osg::ref_ptr<display::DirectionElement> m_Steerpoint;
	osg::ref_ptr<display::DirectionElement> m_FlightPathMarker;
	osg::ref_ptr<display::DirectionElement> m_BankAngleIndicator;
	osg::ref_ptr<GhostHorizon> m_GhostHorizon;
	osg::ref_ptr<PullupAnticipationCue> m_PullupAnticipationCue;
	osg::ref_ptr<RadarAltitudeScale> m_RadarAltitudeScale;
	osg::ref_ptr<ImpactPointCue> m_ImpactPointCue;
	osg::ref_ptr<BombSteeringCue> m_BombSteeringCue;
	osg::ref_ptr<display::Element> m_GunCross;
	osg::ref_ptr<display::Element> m_RollTape;
	osg::ref_ptr<display::Element> m_BreakX;
	osg::ref_ptr<display::MoveableElement> m_RollMarker;
	osg::ref_ptr<display::ElementText> m_GForceMeter;
	osg::ref_ptr<display::ElementText> m_MaxGForce;
	osg::ref_ptr<display::ElementText> m_MasterMode;
	osg::ref_ptr<display::ElementText> m_MasterArm;
	osg::ref_ptr<display::ElementText> m_MachMeter;
	osg::ref_ptr<display::ElementText> m_AirspeedUnits;
	osg::ref_ptr<display::ElementText> m_RadarAltimeter;
	osg::ref_ptr<display::ElementText> m_Alow;
	osg::ref_ptr<display::ElementText> m_SlantRange;
	osg::ref_ptr<display::ElementText> m_SteerpointTime;
	osg::ref_ptr<display::ElementText> m_SteerpointDistance;
	osg::ref_ptr<PitchLadder> m_PitchLadder;
	osg::ref_ptr<display::Element> m_GlideSlopeBar;
	osg::ref_ptr<HUDTape> m_AirspeedTape;
	osg::ref_ptr<display::Element> m_AirspeedBox;
	osg::ref_ptr<display::ElementText> m_AirspeedText;
	osg::ref_ptr<HUDTape> m_AltitudeTape;
	osg::ref_ptr<display::Element> m_AltitudeBox;
	osg::ref_ptr<display::ElementText> m_AltitudeText;
	osg::ref_ptr<HUDTape> m_HeadingTape;
	osg::ref_ptr<display::Element> m_HeadingBox;
	osg::ref_ptr<display::ElementText> m_HeadingText;
	osg::ref_ptr<display::DirectionElement> m_LandingAngleOfAttackRange;
	osg::ref_ptr<display::FloatingFrame> m_VerticalFrame;
	osg::ref_ptr<display::FloatingFrame> m_HeadingTapeFrame;
	osg::ref_ptr<DEDReadout> m_DEDReadout;

	LagFilter m_AlphaFilter;

	int m_G;
	int m_MaxG;
	int m_Heading;
	int m_Mach;
	int m_LastSlantRange;
	int m_LastSteerpointTime;
	int m_LastSteerpointDistance;

	osg::ref_ptr<DisplayFont> m_StandardFont;
	HUD m_HUD;

	double m_ElapsedTime;
	double m_UpdateTime;

	bool m_ShowVerticalVelocity;
	bool m_ShowScales;
	bool m_ShowData;
	bool m_ShowAttitude;
	bool m_ShowFPM;
	std::string m_VelocityLabel;
	Vector3 m_CueVelocity;

	EnumLink m_LastMasterMode;
	void updateMasterMode();

	typedef enum { CAS, TAS, GND } VelocityUnits;
	VelocityUnits m_VelocityUnits;

	display::SymbolMaker m_CaretSymbol;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_F16_F16HUD_H__

