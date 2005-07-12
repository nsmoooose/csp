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


#ifndef __F16_F16HUD_H__
#define __F16_F16HUD_H__

#include <CockpitInterface.h>
#include <F16/NavigationSystem.h>
#include <F16/AlphaNumericDisplay.h>
#include <Filters.h>
#include <HUD/HUD.h>
#include <System.h>
#include <SimData/Quat.h>
#include <SimData/Vector3.h>


class HUDFont;
class HUDTape;
class PitchLadder;
class DEDReadout;
class PullupAnticipationCue;
class RadarAltitudeScale;

class F16HUD: public System {
public:
	SIMDATA_DECLARE_OBJECT(F16HUD)

	F16HUD();
	virtual ~F16HUD();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

protected:
	DataChannel<double>::CRef b_GearExtension;
	DataChannel<double>::CRef b_Airspeed;
	DataChannel<double>::CRef b_Roll;
	DataChannel<double>::CRef b_Pitch;
	DataChannel<double>::CRef b_Heading;
	DataChannel<double>::CRef b_Alpha;
	DataChannel<double>::CRef b_CAS;
	DataChannel<double>::CRef b_Mach;
	DataChannel<double>::CRef b_G;
	DataChannel<double>::CRef b_GroundZ;
	DataChannel<double>::CRef b_CaraAlow;
	DataChannel<bool>::CRef b_LeftMainLandingGearWOW;
	DataChannel<bool>::CRef b_GearHandleUp;
	DataChannel<simdata::Quat>::CRef b_Attitude;
	DataChannel<simdata::Vector3>::CRef b_Position;
	DataChannel<simdata::Vector3>::CRef b_Velocity;
	DataChannel<NavigationSystem::Ref>::CRef b_NavigationSystem;
	DataChannel<AlphaNumericDisplay::Ref>::CRef b_DEDReadout;
	DataChannel<bool>::CRef b_AltitudeAdvisory;
	DataChannel<bool>::CRef b_DescentWarningAfterTakeoff;
	DataChannel<double>::CRef b_PullupAnticipation;

	DataChannel<simdata::EnumLink>::CRef b_DataSwitch;
	DataChannel<simdata::EnumLink>::CRef b_FlightPathMarkerSwitch;
	DataChannel<simdata::EnumLink>::CRef b_ScalesSwitch;
	DataChannel<simdata::EnumLink>::CRef b_VelocitySwitch;
	DataChannel<simdata::EnumLink>::CRef b_AltitudeSwitch;
	CockpitInterface m_HudPanel;
	DataChannel<simdata::EnumLink>::Ref b_AOAIndexer;

	virtual double onUpdate(double dt);
	virtual void updateReadouts();
	virtual void updateSwitches();
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
	virtual void newVerticalText(osg::ref_ptr<HUD::ElementText> &element, float x, float y, HUD::ElementText::AlignmentType alignment=HUD::ElementText::BASE_LINE);

	virtual double getSpeedCaret(double ground_speed) const;
	virtual double getSpeed() const;

	osg::ref_ptr<HUD::DirectionElement> m_Tadpole;
	osg::ref_ptr<HUD::DirectionElement> m_Steerpoint;
	osg::ref_ptr<HUD::DirectionElement> m_FlightPathMarker;
	osg::ref_ptr<HUD::DirectionElement> m_BankAngleIndicator;
	osg::ref_ptr<PullupAnticipationCue> m_PullupAnticipationCue;
	osg::ref_ptr<RadarAltitudeScale> m_RadarAltitudeScale;
	osg::ref_ptr<HUD::Element> m_GunCross;
	osg::ref_ptr<HUD::Element> m_RollTape;
	osg::ref_ptr<HUD::Element> m_BreakX;
	osg::ref_ptr<HUD::MoveableElement> m_RollMarker;
	osg::ref_ptr<HUD::ElementText> m_GForceMeter;
	osg::ref_ptr<HUD::ElementText> m_MaxGForce;
	osg::ref_ptr<HUD::ElementText> m_MasterMode;
	osg::ref_ptr<HUD::ElementText> m_MasterArm;
	osg::ref_ptr<HUD::ElementText> m_MachMeter;
	osg::ref_ptr<HUD::ElementText> m_AirspeedUnits;
	osg::ref_ptr<HUD::ElementText> m_RadarAltimeter;
	osg::ref_ptr<HUD::ElementText> m_Alow;
	osg::ref_ptr<HUD::ElementText> m_SlantRange;
	osg::ref_ptr<HUD::ElementText> m_SteerpointTime;
	osg::ref_ptr<HUD::ElementText> m_SteerpointDistance;
	osg::ref_ptr<PitchLadder> m_PitchLadder;
	osg::ref_ptr<HUD::Element> m_GlideSlopeBar;
	osg::ref_ptr<HUDTape> m_AirspeedTape;
	osg::ref_ptr<HUD::Element> m_AirspeedBox;
	osg::ref_ptr<HUD::ElementText> m_AirspeedText;
	osg::ref_ptr<HUDTape> m_AltitudeTape;
	osg::ref_ptr<HUD::Element> m_AltitudeBox;
	osg::ref_ptr<HUD::ElementText> m_AltitudeText;
	osg::ref_ptr<HUDTape> m_HeadingTape;
	osg::ref_ptr<HUD::Element> m_HeadingBox;
	osg::ref_ptr<HUD::ElementText> m_HeadingText;
	osg::ref_ptr<HUD::DirectionElement> m_LandingAngleOfAttackRange;
	osg::ref_ptr<HUD::FloatingFrame> m_VerticalFrame;
	osg::ref_ptr<HUD::FloatingFrame> m_HeadingTapeFrame;
	osg::ref_ptr<DEDReadout> m_DEDReadout;

	LagFilter m_AlphaFilter;

	int m_G;
	int m_MaxG;
	int m_Heading;
	int m_Mach;
	int m_LastSlantRange;
	int m_LastSteerpointTime;
	int m_LastSteerpointDistance;

	osg::ref_ptr<HUDFont> m_StandardFont;
	HUD m_HUD;

	double m_ElapsedTime;
	double m_UpdateTime;

	bool m_ShowVerticalVelocity;
	bool m_ShowScales;
	bool m_ShowData;
	bool m_ShowAttitude;
	bool m_ShowFPM;
	std::string m_VelocityLabel;

	typedef enum { CAS, TAS, GND } VelocityUnits;
	VelocityUnits m_VelocityUnits;

	HUD::SymbolMaker m_CaretSymbol;
};


#endif // __F16_F16HUD_H__

