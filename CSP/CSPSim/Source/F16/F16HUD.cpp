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
 * @file F16HUD.cpp
 *
 **/


#include <F16/F16HUD.h>
#include <F16/F16Channels.h>
#include <F16/SpecialFonts.h>
#include <Atmosphere.h>
#include <ConditionsChannels.h>
#include <CSPSim.h>
#include <FlightDynamicsChannels.h>
#include <HUD/HUD.h>
#include <HUD/Tape.h>
#include <HUD/Text.h>
#include <HUD/PitchLadder.h>
#include <KineticsChannels.h>
#include <LandingGearChannels.h>
#include <SimData/Conversions.h>
#include <SimData/osg.h>
#include <SimData/Math.h>
#include <SimData/Vector3.h>
#include <SimData/Timing.h>


// TODO
// true airspeed and ground speed
// ils symbology
// 10,000 other things!
// more efficient calculations, push channel callbacks

SIMDATA_REGISTER_INTERFACE(F16HUD)


class F16HUDFont: public HUDFont {
	float m_Height;
	osgText::Font *m_Font;
public:
	F16HUDFont(float height): m_Height(height) {
		//m_Font = new ReverseAltFont(osgText::readFontFile("hud.ttf")); // FIXME LEAKS!
		m_Font = new ScaledAltFont(osgText::readFontFile("hud.ttf"), 1.2); // FIXME LEAKS!
	}
	virtual void apply(osgText::Text *text) {
		text->setFont(m_Font);
		text->setFontResolution(30, 30);
		text->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
		text->setCharacterSize(m_Height);
		text->setKerningType(osgText::KERNING_NONE);
		text->setAxisAlignment(osgText::Text::XZ_PLANE);
	}
};


class F16AltitudeFormatter: public LabelFormatter {
	virtual void format(char *buffer, int len, float value) {
		int clicks = static_cast<int>(value * 0.1);
		snprintf(buffer, len, "%02d\037%d", clicks, static_cast<int>(value - 10.0 * clicks + 0.5));
	}
};


class RadarAltitudeScale: public HUD::Element {
public:
	RadarAltitudeScale(double x0, double y0, double wide, double narrow, double spacing, double label_offset, osg::ref_ptr<HUDFont> font):
		m_X0(x0),
		m_Y0(y0),
		m_Wide(wide),
		m_Narrow(narrow),
		m_Spacing(spacing),
		m_LabelOffset(label_offset),
		m_Alow(-1),
		m_AltitudeAgl(0)
	{
		HUD::SymbolMaker scale;
		scale.beginDrawLines();
		for (int i = 0; i <= 24; i++) {
			if (i != 21 && i != 23) {
				double y = y0 + i * spacing;
				scale.drawLine(x0, y, x0 + ((i & 1) ? narrow : wide), y);
			}
		}
		scale.endDrawLines();
		addSymbol(scale);
		const char *labels[] = {"2", "4", "6", "8", "10", "15"};
		for (int i = 1; i <= 6; ++i) {
			double y = y0 + i * 4 * spacing;
			osgText::Text *label = addText();
			font->apply(label);
			label->setText(labels[i-1]);
			label->setAlignment(osgText::Text::LEFT_CENTER);
			label->setPosition(osg::Vec3(x0 + wide + label_offset, 0, y));
		}
		addSymbol(m_Indicator);
		addSymbol(m_AlowCaret);
	}

	void update(double altitude_agl_ft, double alow_ft) {
		int int_alow = static_cast<int>(alow_ft);
		if (int_alow != m_Alow) {
			if (int_alow >= 0 && int_alow <= 1500) {
				double y = m_Y0 + m_Spacing * ((int_alow <= 1000) ? int_alow * 0.02 : 20.0 + (int_alow - 1000) * 0.008);
				double x = m_X0 - 2.0 * m_Spacing;
				m_AlowCaret.erase();
				m_AlowCaret.beginDrawLines();
				m_AlowCaret.drawLine(x, y, m_X0, y);
				m_AlowCaret.drawLine(x, y + m_Spacing, x, y - m_Spacing);
				m_AlowCaret.endDrawLines();
			} else {
				m_AlowCaret.erase();
			}
			m_Alow = int_alow;
		}
		int agl = static_cast<int>(altitude_agl_ft);
		if (agl != m_AltitudeAgl) {
			m_AltitudeAgl = agl;
			double y = m_Y0 + m_Spacing * ((agl <= 1000) ? agl * 0.02 : 20.0 + (agl - 1000.0) * 0.008);
			m_Indicator.erase();
			m_Indicator.beginDrawLines();
			m_Indicator.moveTo(m_X0 + m_Wide, m_Y0);
			m_Indicator.lineTo(m_X0 - m_LabelOffset, m_Y0);
			m_Indicator.lineTo(m_X0 - m_LabelOffset, y);
			m_Indicator.lineTo(m_X0 + m_Wide, y);
			m_Indicator.endDrawLines();
		}
	}

private:
	double m_X0;
	double m_Y0;
	double m_Wide;
	double m_Narrow;
	double m_Spacing;
	double m_LabelOffset;
	int m_Alow;
	int m_AltitudeAgl;
	HUD::SymbolMaker m_Indicator;
	HUD::SymbolMaker m_AlowCaret;
};


/** A vertical scale bar that serves as an aid for maintaining the optimal
 *  angle of attack during landing approach.  This is a direction element
 *  aligned with the pitch ladder, positioned 13 degrees below the nose.
 *  The bar extends +/- 2 degrees.  When the flight path marker is within
 *  the bar the angle of attack is between +11 to +15 degrees.  The symbol
 *  automatically resizes itself if the view point changes to maintain the
 *  correct angular extent.
 */
class LandingAngleOfAttackRange: public HUD::DirectionElement {
public:
	virtual void updateView(osg::Vec3 const &origin, osg::Vec3 const &view_point) {
		HUD::DirectionElement::updateView(origin, view_point);
		float d = (origin - view_point).length();
		// the dash one says this marker extends from 11 to 15 degrees AoA, but
		// in a HUD video (post MLU?) it is closer to 2 degrees in extent.
		float h = 0.5 * d * osg::inDegrees(2.0);
		HUD::SymbolMaker bar;
		bar.beginDrawLines();
		bar.drawLine(-0.0065, h, -0.005, h);
		bar.drawLine(-0.0065, h, -0.0065, -h);
		bar.drawLine(-0.0065, -h, -0.005, -h);
		bar.endDrawLines();
		clear();
		bar.addToGeode(geode());
	}
	static double getCenterAngle() {
		return simdata::toRadians(13.0);
	}
};


class FlightPathMarker: public HUD::DirectionElement {
public:
	FlightPathMarker() {
		HUD::SymbolMaker fpm;
		fpm.addCircle(0.0, 0.0, 0.002, 10);
		fpm.beginDrawLines();
		fpm.drawLine(0.002, 0.0, 0.006, 0.0);
		fpm.drawLine(-0.002, 0.0, -0.006, 0.0);
		fpm.drawLine(0.0, 0.002, 0.0, 0.004);
		fpm.endDrawLines();
		addSymbol(fpm);
	}
};


class BankAngleIndicator: public HUD::DirectionElement {
public:
	BankAngleIndicator() {
		const int ticks = 9;
		const float angles[ticks] = {-60.0, -30.0, -20.0, -10.0, 0.0, 10.0, 20.0, 30.0, 60.0 };
		const float length[ticks] = {2.0, 2.0, 1.0, 1.0, 2.0, 1.0, 1.0, 2.0, 2.0 };
		HUD::SymbolMaker bai;
		bai.beginDrawLines();
		for (int i = 0; i < ticks; i++) {
			float angle = osg::inDegrees(angles[i]);
			float x = sin(angle);
			float y = cos(angle);
			float r0 = 0.006;
			float r1 = r0 + 0.001 * length[i];
			bai.drawLine(x * r0, y * r0, x * r1, y * r1);
		}
		bai.endDrawLines();
		addSymbol(bai);
	}
};


class Tadpole: public HUD::DirectionElement {
public:
	Tadpole() {
		HUD::SymbolMaker tadpole;
		tadpole.addCircle(0.0, 0.0, 0.001, 8);
		tadpole.addLine(0.0, 0.001, 0.0, 0.006);
		addSymbol(tadpole);
		show(false);
	}
};


class SteerpointMarker: public HUD::DirectionElement {
public:
	SteerpointMarker() {
		const float size = 0.002;
		HUD::SymbolMaker diamond;
		diamond.beginDrawLines();
		diamond.moveTo(0.0, size);
		diamond.lineTo(size, 0.0);
		diamond.lineTo(0.0, -size);
		diamond.lineTo(-size, 0.0);
		diamond.lineTo(0.0, size);
		diamond.endDrawLines();
		addSymbol(diamond);
		show(false);
	}
};


class OffsetAimpointMarker: public HUD::DirectionElement {
public:
	OffsetAimpointMarker() {
		const float size = 0.002;
		HUD::SymbolMaker symbol;
		symbol.beginDrawLines();
		symbol.moveTo(0.0, size * 1.2);
		symbol.lineTo(size * 0.5, -size * 0.8);
		symbol.lineTo(-size * 0.5, -size * 0.8);
		symbol.lineTo(0.0, size * 1.2);
		symbol.endDrawLines();
		addSymbol(symbol);
		show(false);
	}
};


class PullupAnticipationCue: public HUD::DirectionElement {
public:
	PullupAnticipationCue() {
		HUD::SymbolMaker symbol;
		symbol.beginDrawLines();
		symbol.moveTo(-0.0044, 0.0028);
		symbol.lineTo(-0.0044, 0.0);
		symbol.lineTo(+0.0044, 0.0);
		symbol.lineTo(+0.0044, 0.0028);
		symbol.endDrawLines();
		addSymbol(symbol);
		show(false);
	}
	void update(double anticipation, osg::Vec3 const &fpm) {
		if (anticipation > 0.0) {
			show(true);
			double angle = std::max(0.0, 1.0 - anticipation) * simdata::toRadians(3.0);
			setDirection(fpm * osg::Matrixf::rotate(-angle, osg::Vec3(1.0, 0.0, 0.0)));
		} else {
			show(false);
		}
	}
};

class DEDReadout: public HUD::MoveableElement {
	simdata::Ref<const AlphaNumericDisplay> m_Display;
	osgText::Text** m_Lines;
public:
	DEDReadout(simdata::Ref<const AlphaNumericDisplay> display, osg::ref_ptr<HUDFont> font): m_Display(display) {
		assert(font.valid());
		unsigned lines = m_Display->height();
		CSP_LOG(APP, DEBUG, "DEDReadout: " << lines << " lines");
		m_Lines = new osgText::Text*[lines];
		for (unsigned i = 0; i < lines; ++i) {
			m_Lines[i] = addText();
			//m_Lines[i]->setAlignment(osgText::Text::LEFT_TOP);
			font->apply(m_Lines[i]);
			m_Lines[i]->setPosition(osg::Vec3(0, 0, (i + 1) * (-0.0035)));
		}
	}
	~DEDReadout() {
		delete[] m_Lines;
	}
	void update() {
		unsigned lines = m_Display->height();
		for (unsigned i = 0; i < lines; ++i) {
			m_Lines[i]->setText(m_Display->getLine(i));
		}
	}
};


F16HUD::F16HUD(): m_AlphaFilter(1.0), m_G(-1), m_MaxG(-100), m_Heading(-1), m_Mach(-1), m_ElapsedTime(0.0), m_UpdateTime(0.0) { }
F16HUD::~F16HUD() { }

void F16HUD::registerChannels(Bus *bus) {
	// XXX slightly dangerous... need to be sure to set the channel to NULL in
	// our dtor, and users need to check for a null hud pointer.
	bus->registerLocalDataChannel<HUD*>("HUD", &m_HUD);
}

void F16HUD::importChannels(Bus* bus) {
	b_Airspeed = bus->getChannel(bus::FlightDynamics::Airspeed);
	b_Attitude = bus->getChannel(bus::F16::INSAttitude, false);
	if (!b_Attitude) {
		b_Attitude = bus->getChannel(bus::Kinetics::Attitude);
	}
	b_Velocity = bus->getChannel(bus::Kinetics::Velocity);
	b_Position = bus->getChannel(bus::Kinetics::Position);
	b_Roll = bus->getChannel(bus::Kinetics::Roll);
	b_Pitch = bus->getChannel(bus::Kinetics::Pitch);
	b_Heading = bus->getChannel(bus::Kinetics::Heading);
	b_Alpha = bus->getChannel(bus::FlightDynamics::Alpha);
	b_CAS = bus->getChannel(bus::Conditions::CAS);
	b_Mach = bus->getChannel(bus::Conditions::Mach);
	b_G = bus->getChannel(bus::FlightDynamics::GForce);
	b_GroundZ = bus->getChannel(bus::Kinetics::GroundZ);
	b_CaraAlow = bus->getChannel("F16.CaraAlow", false);
	b_LeftMainLandingGearWOW = bus->getChannel(bus::LandingGear::selectWOW("LeftGear"));
	b_GearHandleUp = bus->getChannel(bus::F16::GearHandleUp);
	b_NavigationSystem = bus->getChannel("Navigation", false);
	b_DEDReadout = bus->getChannel("DED", false);

	b_AltitudeAdvisory = bus->getChannel("F16.GroundAvoidance.AltitudeAdvisory", false);
	b_PullupAnticipation = bus->getChannel("F16.GroundAvoidance.PullupAnticipation", false);
	b_DescentWarningAfterTakeoff = bus->getChannel("F16.GroundAvoidance.DescentWarningAfterTakeoff", false);

	b_DataSwitch = bus->getChannel("HUD.DataSwitch", false);
	b_FlightPathMarkerSwitch = bus->getChannel("HUD.FlightPathMarkerSwitch", false);
	b_ScalesSwitch = bus->getChannel("HUD.ScalesSwitch", false);
	b_VelocitySwitch = bus->getChannel("HUD.VelocitySwitch", false);
	b_AltitudeSwitch = bus->getChannel("HUD.AltitudeSwitch", false);
	buildHUD();
}

double F16HUD::onUpdate(double dt) {
	//simdata::Timer timer;
	//timer.start();
	simdata::Quat const &attitude = b_Attitude->value();
	simdata::Vector3 velocity = b_Velocity->value();

	// if velocity is too small, the hud symbols that are cued by V will jitter wildly.
	// we fix V at the forward horizon in that case.  this appears to be what the real
	// jet does.  we only do this on the ground; otherwise useful information would be
	// lost in a severe stall.
	if (velocity.length2() < 2500.0 && b_LeftMainLandingGearWOW->value()) {
		velocity = attitude.rotate(simdata::Vector3::YAXIS);  // forward vector in world coordinates
		velocity.z() = 0.0;
	}

	double speed = simdata::convert::mps_kts(getSpeed());

	bool gear_down = !b_GearHandleUp->value();
	bool ILS_mode = false;

	simdata::Vector3 horizon = velocity; horizon.z() = 0.0;
	simdata::Vector3 horizon_right(horizon.y(), -horizon.x(), horizon.z()); // 90 deg rotation around z
	osg::Vec3 velocity_body = simdata::toOSG(attitude.invrotate(velocity));
	osg::Vec3 velocity_right_body(-velocity_body.z(), 0.0, velocity_body.x());
	m_FlightPathMarker->show(m_ShowFPM);
	m_FlightPathMarker->setDirection(velocity_body);

	const bool show_roll = !m_ShowData && !m_ShowVerticalVelocity;
	m_DEDReadout->show(m_ShowData);
	m_RollMarker->show(show_roll);
	m_RollTape->show(show_roll);
	m_BankAngleIndicator->show(m_ShowVerticalVelocity && !m_FlightPathMarker->isHidden());

	if (!m_RollMarker->isHidden()) {
		m_RollMarker->setOrientation(-b_Roll->value());
	}

	if (!m_BankAngleIndicator->isHidden()) {
		m_BankAngleIndicator->setDirection(velocity_body);
		m_BankAngleIndicator->setOrientation(-b_Roll->value());
	}

	osg::Vec3 horizon_body = simdata::toOSG(attitude.invrotate(horizon));
	osg::Vec3 horizon_right_body = simdata::toOSG(attitude.invrotate(horizon_right));

	m_PitchLadder->show(m_ShowAttitude || gear_down);
	m_PitchLadder->update(horizon_body, horizon_right_body, b_Pitch->value());

	if (gear_down) {
		// on the ground at low aoa the aoa bracket is pegged to the fpm
		if (b_LeftMainLandingGearWOW->value() && (b_Alpha->value() < simdata::toRadians(10.0))) {
			m_AlphaFilter.update(0.0, dt);
		} else {
			// FIXME getCenterAngle should be an instance method, and m_LandingAngleOfAttackRange should
			// be of type LandingAngleOfAttackRange.
			double da = b_Alpha->value() - LandingAngleOfAttackRange::getCenterAngle();
			da = simdata::clampTo(da, simdata::toRadians(-9.0), simdata::toRadians(9.0));
			m_AlphaFilter.update(da, dt);
		}
		float landing_aoa_angle = static_cast<float>(m_AlphaFilter.value());
		osg::Vec3 landing_aoa_dir = velocity_body * osg::Matrixf::rotate(landing_aoa_angle, osg::Vec3(1.0, 0.0, 0.0));
		m_LandingAngleOfAttackRange->setDirection(landing_aoa_dir);
		m_LandingAngleOfAttackRange->show(true);
		m_GlideSlopeBar->show(true);
	} else {
		m_LandingAngleOfAttackRange->show(false);
		m_GlideSlopeBar->show(false);
	}

	const float vertical_frame_fpm_offset = 0.051;  // offset from fpm when locked together
	const float vertical_frame_top_position = 0.0;  // highest possible vertical frame position
	const float vertical_frame_bottom_position = -0.04;  // lowest possible vertical frame position
	const float heading_tape_top_position = 0.05;  // when at the top of the hud
	const float heading_tape_vertical_offset = 0.030;  // when moving together

	if (gear_down) {
		// when the landing gear is down the heading tape, vertical tapes, and other symbology
		// move vertically with the fpm.  the heading tape moves independently from the top of the
		// hud down to the midpoint of the vertical tapes.  from there it moves with the vertical
		// tapes and other symbology down to the base of the hud.  further downward motion of the
		// fpm does not affect the symbology (i.e. the heading tape and altitude/airspeed readouts
		// will not move off the bottom of the hud).  when the heading top is moving it is fixed
		// just above the fpm.
		float gun_cross_y = m_HUD.getForwardFrameOrigin().z();

		// get fpm y coordinate in the gun cross frame.  the origin of the gun cross frame is near
		// the top of the hud, while free elements like the fpm move with respect to the center of
		// the hud.
		float fpm_y = m_FlightPathMarker->position().z() - gun_cross_y;

		// lock the position of the vertical tapes and other symbology to the fpm through part of
		// it's range.
		float vertical_frame_y = std::max(vertical_frame_bottom_position, std::min(vertical_frame_top_position, fpm_y + vertical_frame_fpm_offset));
		m_VerticalFrame->setPosition(0.0, vertical_frame_y);

		// the fpm begins to pull the heading tape down when it is below the heading tape by the
		// same distance as when the heading tape and vertical scale bars are moving together.
		float heading_tape_frame_y = std::max(vertical_frame_y + heading_tape_vertical_offset, std::min(heading_tape_top_position, fpm_y + heading_tape_vertical_offset + vertical_frame_fpm_offset));
		m_HeadingTapeFrame->setPosition(0.0, heading_tape_frame_y);
	} else {
		// TODO readjust the vertical frame elements to eliminate the -1 cm displacement
		m_VerticalFrame->setPosition(0.0, vertical_frame_top_position - 0.01);
		m_HeadingTapeFrame->setPosition(0.0, 0.0);
	}

	m_AirspeedTape->show(m_ShowScales);
	m_AirspeedTape->update(speed >= 50.0 ? speed : 0.0);

	double altitude = b_Position->value().z();
	double altitude_agl_ft = simdata::convert::m_ft(altitude - b_GroundZ->value());
	bool baro_alt = b_AltitudeSwitch.valid() && b_AltitudeSwitch->value() == "BARO";
	if ((altitude_agl_ft <= 1500) && !baro_alt) {
		double alow_ft = b_CaraAlow.valid() ? b_CaraAlow->value() : 0.0;
		m_RadarAltitudeScale->show(m_ShowScales);
		m_RadarAltitudeScale->update(altitude_agl_ft, alow_ft);
		m_AltitudeTape->show(false);
		m_AltitudeBox->show(false);
	} else {
		m_RadarAltitudeScale->show(false);
		m_AltitudeTape->show(m_ShowScales);
		m_AltitudeTape->update(simdata::convert::m_ft(altitude));
		m_AltitudeBox->show(true);
	}

	m_HeadingTape->show(m_ShowScales);
	m_HeadingTape->update(simdata::toDegrees(b_Heading->value()));

	Steerpoint::Ref active_steerpoint = b_NavigationSystem.valid() ? b_NavigationSystem->value()->activeSteerpoint() : 0;
	if (active_steerpoint.valid()) {
		simdata::Vector3 steerpoint = active_steerpoint->position();
		simdata::Vector3 steerpoint_offset = steerpoint - b_Position->value();
		float steering_angle = atan2(steerpoint_offset.x(), steerpoint_offset.y());

		osg::Vec3 steerpoint_offset_body = simdata::toOSG(attitude.invrotate(steerpoint_offset));
		m_Steerpoint->setDirection(steerpoint_offset_body);
		m_Steerpoint->setOrientation(-b_Roll->value());
		m_Steerpoint->show(steerpoint_offset_body.y() > 0);

		float velocity_angle = atan2(velocity.x(), velocity.y());
		float tadpole_angle = steering_angle - velocity_angle;
		if (tadpole_angle > osg::PI) {
			tadpole_angle -= 2.0 * osg::PI;
		} else if (tadpole_angle < -osg::PI) {
			tadpole_angle += 2.0 * osg::PI;
		}
		float constrained_tadpole_angle = osg::clampTo(tadpole_angle, -osg::inDegrees(2.5f), osg::inDegrees(2.5f));
		osg::Vec3 up_body = simdata::toOSG(attitude.invrotate(simdata::Vector3::ZAXIS));
		osg::Vec3 tadpole_cue = velocity_body * osg::Matrix::rotate(-constrained_tadpole_angle, up_body);
		m_Tadpole->setDirection(tadpole_cue);
		m_Tadpole->setOrientation(tadpole_angle);
		m_Tadpole->show(tadpole_cue.y() > 0);

		m_AltitudeTape->enableCaret(true);
		m_AltitudeTape->setCaret(simdata::convert::m_ft(steerpoint.z()));

		double distance = steerpoint_offset.length();
		// TODO move these calculations to a navigation system; update sp cas less often but low pass
		// filter it.
		double time = active_steerpoint->time().getRelativeTime(CSPSim::theSim->getCurrentTime());
		m_AirspeedTape->enableCaret(time > 0);
		if (time > 0) {
			double ground_speed = distance / time;
			double speed_caret = getSpeedCaret(ground_speed);
			m_AirspeedTape->setCaret(speed_caret);
		}
	} else {
		m_Tadpole->show(false);
		m_Steerpoint->show(false);
		m_AltitudeTape->enableCaret(false);
		m_AirspeedTape->enableCaret(false);
	}

	if (ILS_mode) {
		// FIXME need ils model to generate this angle
		double ils_steering_angle = -10;  /* radians, off scale */
		m_HeadingTape->setCaret(osg::RadiansToDegrees(ils_steering_angle));
		m_HeadingTape->enableCaret(true);
	} else {
		m_HeadingTape->enableCaret(false);
	}

	// Flash at 3 Hz when enabled.
	m_BreakX->show(b_AltitudeAdvisory.valid() && b_AltitudeAdvisory->value() && (static_cast<int>(m_ElapsedTime * 6.0) & 1));
	// TODO only display in A-G mode
	if (b_PullupAnticipation.valid()) {
		m_PullupAnticipationCue->update(b_PullupAnticipation->value(), velocity_body);
	}

	/* TODO
	 * Davor "Bowman" Perkovac:
	 * You can engage it when you press T-ILS, DCS down to move cursor to CMD STRG field on DED and press 0-Misc
	 * to toggle it on/off. ILS signal must be present for it to work.
	 *
	 * When CMDSTR is ON, your waypoint steering cue (the small circle in the HUD with a line extending from it
	 * in the direction of the waypoint) will not show you the location of the currently selected waypoint, but
	 * will direct you towards a perfect ILS landing.  If you're outside a 3 degree horizontal cone from the
	 * runway treshold it will guide you in the horizontal plane towards an aproach at 45 degrees intercept
	 * course. When you come within a 3 degree cone, it will switch to terminal phase guidance, showing you
	 * where you need to fly to smootly follow (both horizontally and vertically) ILS glidepath.
	 */

	// update the numeric readouts at most 10 times per second to prevent thrashing
	m_ElapsedTime += dt;
	if (m_ElapsedTime >= m_UpdateTime) {
		m_UpdateTime = m_ElapsedTime + 0.1;
		updateReadouts();
		updateSwitches();
	}
	//double debug_dt = timer.stop();
	//CSP_LOG(APP, DEBUG, "HUD UPDATE took " << static_cast<int>(1e+6*debug_dt) << " microseconds");
	return 0.0;
}

void F16HUD::updateSwitches() {
	bool gear_down = !b_GearHandleUp->value();  // CAS forced when gear is down
	m_ShowVerticalVelocity = !b_ScalesSwitch || (b_ScalesSwitch->value() == "VV/VAH");
	m_ShowScales = !b_ScalesSwitch || (b_ScalesSwitch->value() != "OFF") || gear_down;
	m_ShowData = !b_DataSwitch || (b_DataSwitch->value() == "DATA");
	m_ShowAttitude = !b_FlightPathMarkerSwitch || (b_FlightPathMarkerSwitch->value() == "ATT/FPM");
	m_ShowFPM = !b_FlightPathMarkerSwitch || (b_FlightPathMarkerSwitch->value() != "OFF") || gear_down;
	if (b_VelocitySwitch.valid() && !gear_down) {
		if (b_VelocitySwitch->value() == "CAS") {
			m_VelocityUnits = CAS;
			m_VelocityLabel = "C";
		} else if (b_VelocitySwitch->value() == "TAS") {
			m_VelocityUnits = TAS;
			m_VelocityLabel = "T";
		} else {
			m_VelocityUnits = GND;
			m_VelocityLabel = "G";
		}
	} else {
		m_VelocityUnits = CAS;
		m_VelocityLabel = "C";
	}
}

void F16HUD::updateReadouts() {
	const int dG = static_cast<int>(b_G->value() * 10.0 + 0.5);
	if (dG != m_G) {
		m_G = dG;
		m_GForceMeter->print("%4.1f", dG * 0.1);
	}
	if (dG > m_MaxG) {
		m_MaxG = dG;
		m_MaxGForce->print("%3.1f", dG * 0.1);
	}
	const int heading = static_cast<int>(simdata::toDegrees(b_Heading->value()) + 0.5);
	if (heading != m_Heading) {
		m_Heading = heading;
		m_HeadingText->print("%03d", (heading < 0) ? (heading + 360) : heading);
	}
	const int speed = static_cast<int>(simdata::convert::mps_kts(getSpeed()) + 0.5);
	m_AirspeedText->print("%4d", speed < 50 ? 0 : speed);

	bool radar_alt = b_AltitudeSwitch.valid() && b_AltitudeSwitch->value() == "RADAR";
	double altitude_value = b_Position->value().z();
	if (radar_alt) altitude_value -= b_GroundZ->value();
	const int altitude = static_cast<int>(simdata::convert::m_ft(altitude_value) + 0.5);
	const int clicks = altitude / 1000;
	if (altitude >= 1000) {
		m_AltitudeText->print("%2d\037%03d", clicks, altitude - clicks * 1000);
	} else {
		m_AltitudeText->print("%6d", altitude);
	}
	const int radar_altitude = 10 * static_cast<int>(0.1 * simdata::convert::m_ft(b_Position->value().z() - b_GroundZ->value()) + 0.5);
	const int radar_clicks = radar_altitude / 1000;
	if (radar_altitude >= 1000) {
		m_RadarAltimeter->print("R %2d\037%03d", radar_clicks, radar_altitude - radar_clicks * 1000);
	} else {
		m_RadarAltimeter->print("R %6d", radar_altitude);
	}
	const int dMach = static_cast<int>(b_Mach->value() * 100.0 + 0.5);
	if (dMach != m_Mach) {
		m_Mach = dMach;
		m_MachMeter->print("%4.2f", b_Mach->value());
	}

	if (b_CaraAlow.valid()) {
		m_Alow->print("AL%5d", static_cast<int>(b_CaraAlow->value() + 0.5));
	}

	// TODO use projection to compute geodesic distances and headings
	Steerpoint::Ref active_steerpoint = b_NavigationSystem.valid() ? b_NavigationSystem->value()->activeSteerpoint() : 0;
	if (active_steerpoint.valid()) {
		simdata::Vector3 steerpoint = active_steerpoint->position();
		simdata::Vector3 steerpoint_offset = steerpoint - b_Position->value();
		const double slant_range = steerpoint_offset.length();
		const int dslant_range = static_cast<int>(simdata::convert::m_nm(slant_range) * 10.0 + 0.5);
		steerpoint_offset.z() = 0.0;
		const double steerpoint_range = steerpoint_offset.length();
		simdata::Vector3 ground_velocity = b_Velocity->value();
		ground_velocity.z() = 0.0;
		const double ground_speed = ground_velocity.length();
		const int dsteerpoint_time = static_cast<int>(steerpoint_range / ground_speed + 0.5);
		const int dsteerpoint_distance = static_cast<int>(simdata::convert::m_nm(steerpoint_offset.length()) + 0.5);
		if (dslant_range != m_LastSlantRange) {
			m_LastSlantRange = dslant_range;
			m_SlantRange->print("B%05.1f", dslant_range * 0.1);
		}
		if (dsteerpoint_time != m_LastSteerpointTime) {
			m_LastSteerpointTime = dsteerpoint_time;
			int minutes = std::min(999, dsteerpoint_time / 60);
			m_SteerpointTime->print("%03d:%02d", minutes, dsteerpoint_time%60);
		}
		if (dsteerpoint_distance != m_LastSteerpointDistance) {
			m_LastSteerpointDistance = dsteerpoint_distance;
			m_SteerpointDistance->print("%03d>%02d", dsteerpoint_distance, active_steerpoint->index());
		}
	} else {
		CSP_LOG(APP, DEBUG, "no active steerpoint");
	}

	m_AirspeedUnits->setText(m_VelocityLabel);

	if (!m_DEDReadout->isHidden()) {
		m_DEDReadout->update();
	}
}

void F16HUD::buildHUD() {
	m_StandardFont = new F16HUDFont(0.003);
	m_CaretSymbol.beginDrawLines();
	m_CaretSymbol.drawLine(0.001, 0.0, 0.00372,  0.00127);
	m_CaretSymbol.drawLine(0.001, 0.0, 0.00372, -0.00127);
	m_CaretSymbol.endDrawLines();

	m_VerticalFrame = new HUD::FloatingFrame;
	m_HUD.addFrameElement(m_VerticalFrame);
	addFlightPathMarker();
	addGunCross();
	addRollTape();
	addBreakX();
	addAirspeedTape();
	addAltitudeTape();
	addHeadingTape();
	addTextElements();
	addPitchLadder();
	addExtraSymbols();
	addDEDReadout();
}

void F16HUD::addExtraSymbols() {
	m_Tadpole = new Tadpole;
	m_HUD.addFloatingElement(m_Tadpole);
	m_Steerpoint = new SteerpointMarker;
	m_HUD.addFloatingElement(m_Steerpoint);
	m_PullupAnticipationCue = new PullupAnticipationCue;
	m_HUD.addFloatingElement(m_PullupAnticipationCue);
	m_LandingAngleOfAttackRange = new LandingAngleOfAttackRange;
	m_HUD.addFloatingElement(m_LandingAngleOfAttackRange);
}

void F16HUD::addFlightPathMarker() {
	m_FlightPathMarker = new FlightPathMarker;
	m_BankAngleIndicator = new BankAngleIndicator;
	m_HUD.addFloatingElement(m_FlightPathMarker);
	m_HUD.addFloatingElement(m_BankAngleIndicator);
}

void F16HUD::addGunCross() {
	m_GunCross = new HUD::Element;
	HUD::SymbolMaker cross;
	cross.beginDrawLines();
	cross.drawLine(0.0010, 0.0, 0.0035, 0.0);
	cross.drawLine(-0.0010, 0.0, -0.0035, 0.0);
	cross.drawLine(0.0, 0.0010, 0.0, 0.0035);
	cross.drawLine(0.0, -0.0010, 0.0, -0.0035);
	cross.endDrawLines();
	m_GunCross->addSymbol(cross);
	m_HUD.addFrameElement(m_GunCross);
}

void F16HUD::addBreakX() {
	m_BreakX = new HUD::Element;
	HUD::SymbolMaker x;
	x.beginDrawLines();
	x.drawLine(-0.0175, -0.010, 0.0175, -0.036);
	x.drawLine(0.0175, -0.010, -0.0175, -0.036);
	x.endDrawLines();
	m_BreakX->addSymbol(x);
	m_HUD.addFrameElement(m_BreakX);
}

void F16HUD::addRollTape() {
	m_RollTape = new HUD::Element;
	HUD::SymbolMaker rolltape;
	rolltape.beginDrawLines();
	float y0 = -0.067;
	float r0 = 0.02;
	for (int i=-4; i <= 4; ++i) {
		float angle = i * simdata::toRadians(10.0);
		float r1 = r0 - 0.0015;
		if ((i == 4) || (i == -4)) {
			angle *= 4.5/4.0;
		}
		if ((i == 0) || (std::abs(i) > 2)) {
			r1 = r0 - 0.003;
		}
		rolltape.drawLine(r1 * sin(angle), y0 - r1 * cos(angle), r0 * sin(angle), y0 - r0 * cos(angle));
	}
	rolltape.endDrawLines();
	m_RollTape->addSymbol(rolltape);
	m_HUD.addFrameElement(m_RollTape);

	m_RollMarker = new HUD::MoveableElement;
	HUD::SymbolMaker rollmarker;
	rollmarker.beginDrawLines();
	rollmarker.drawLine(0.0, - r0, 0.0015, - r0 - 0.0026);
	rollmarker.drawLine(0.0, - r0, -0.0015, - r0 - 0.0026);
	rollmarker.drawLine(0.0015, - r0 - 0.0026, -0.0015, - r0 - 0.0026);
	rollmarker.endDrawLines();
	m_RollMarker->addSymbol(rollmarker);
	m_RollMarker->setPosition(0.0, y0);
	m_HUD.addFrameElement(m_RollMarker);
}

void F16HUD::addAirspeedTape() {
	const float offset_x = -0.042;
	const float offset_y = -0.03;
	m_AirspeedTape = new HUDTape(HUDTape::VERTICAL, 17, 0.002, -0.0025, offset_x, offset_y);
	m_AirspeedTape->setValueScale(10.0 /*kts per tick*/);
	// FIXME formatter should show "000" for speed < 50
	m_AirspeedTape->addNumericLabels(5.0, 0.0, -0.0005, 4, new StandardFormatter("%02.0f"), m_StandardFont.get());
	m_AirspeedTape->showNegativeLabels(false);
	m_AirspeedTape->setLabelHidingWindow(offset_y - 0.0010, offset_y + 0.0010);
	m_AirspeedTape->addCenterLine(0.005, 0.0005);
	m_AirspeedTape->setCaretSymbol(m_CaretSymbol);
	m_AirspeedTape->addToHudFrame(m_VerticalFrame.get());

	const float box_origin = offset_x - 0.0025 - 0.0025;
	m_AirspeedBox = new HUD::Element;
	m_AirspeedText = new HUD::ElementText(m_AirspeedBox.get());
	HUD::SymbolMaker box;
	box.beginDrawLines();
	box.drawLine(-0.010 + box_origin, -0.0018 + offset_y, -0.010 + box_origin, 0.0018 + offset_y);
	box.drawLine(-0.010 + box_origin, 0.0018 + offset_y, 0.000 + box_origin, 0.0018 + offset_y);
	box.drawLine(-0.010 + box_origin, -0.0018 + offset_y, 0.000 + box_origin, -0.0018 + offset_y);
	box.drawLine(0.000 + box_origin, 0.0018 + offset_y, 0.0018 + box_origin, 0.000 + offset_y);
	box.drawLine(0.000 + box_origin, -0.0018 + offset_y, 0.0018 + box_origin, 0.000 + offset_y);
	box.endDrawLines();
	m_AirspeedBox->addSymbol(box);
	m_VerticalFrame->addElement(m_AirspeedBox);
	m_StandardFont->apply(m_AirspeedText);
	m_AirspeedText->setPosition(box_origin - 0.009, offset_y);
	m_AirspeedText->setAlignment(osgText::Text::LEFT_CENTER);
}

void F16HUD::addAltitudeTape() {
	const float offset_x = 0.042;
	const float offset_y = -0.03;
	m_AltitudeTape = new HUDTape(HUDTape::VERTICAL, 17, 0.002, 0.0025, offset_x, offset_y);
	m_AltitudeTape->setValueScale(100.0 /*ft per tick*/);
	m_AltitudeTape->addNumericLabels(5.0, 0.0, 0.0005, 4, new F16AltitudeFormatter(), m_StandardFont.get());
	m_AltitudeTape->showNegativeLabels(false);
	// the comma in the labels apparently enlarges the bbox significantly, so we have to
	// use a much smaller (almost non-existent) hiding window than for the airspeed tape.
	m_AltitudeTape->setLabelHidingWindow(offset_y - 0.0001, offset_y + 0.0001);
	m_AltitudeTape->addCenterLine(0.005, 0.0005);
	m_AltitudeTape->setCaretSymbol(m_CaretSymbol);
	m_AltitudeTape->addToHudFrame(m_VerticalFrame.get());

	const float box_origin = offset_x + 0.0025 + 0.0025;
	m_AltitudeBox = new HUD::Element;
	m_AltitudeText = new HUD::ElementText(m_AltitudeBox.get());
	HUD::SymbolMaker box;
	box.beginDrawLines();
	box.drawLine(0.0135 + box_origin, -0.0018 + offset_y, 0.0135 + box_origin, 0.0018 + offset_y);
	box.drawLine(0.0135 + box_origin, 0.0018 + offset_y, 0.000 + box_origin, 0.0018 + offset_y);
	box.drawLine(0.0135 + box_origin, -0.0018 + offset_y, 0.000 + box_origin, -0.0018 + offset_y);
	box.drawLine(0.000 + box_origin, 0.0018 + offset_y, -0.0018 + box_origin, 0.000 + offset_y);
	box.drawLine(0.000 + box_origin, -0.0018 + offset_y, -0.0018 + box_origin, 0.000 + offset_y);
	box.endDrawLines();
	m_AltitudeBox->addSymbol(box);
	m_VerticalFrame->addElement(m_AltitudeBox);
	m_StandardFont->apply(m_AltitudeText);
	m_AltitudeText->setPosition(box_origin, offset_y);
	m_AltitudeText->setAlignment(osgText::Text::LEFT_CENTER);

	m_RadarAltitudeScale = new RadarAltitudeScale(0.048, -0.048, 0.002, 0.0014, 0.0015, 0.0005, m_StandardFont);
	m_VerticalFrame->addElement(m_RadarAltitudeScale);
}

void F16HUD::addHeadingTape() {
	m_HeadingTapeFrame = new HUD::FloatingFrame;
	m_HUD.addFrameElement(m_HeadingTapeFrame);
	m_HeadingTape = new HUDTape(HUDTape::HORIZONTAL, 5, 0.010, 0.0025, 0.0, -0.06);
	m_HeadingTape->setValueScale(10.0 /* degrees per tick */, 360.0 /* force the range to be positive */);
	m_HeadingTape->addNumericLabels(2.0, 0.0, -0.0005, 1, new StandardFormatter("%02.0f"), m_StandardFont.get());
	m_HeadingTape->setLabelWrapping(0.0, 36.0);
	m_HeadingTape->setLabelHidingWindow(-0.0035, 0.0035);
	m_HeadingTape->addCenterLine(0.004, 0.0005);
	m_HeadingTape->setCaretSymbol(m_CaretSymbol);
	m_HeadingTape->addToHudFrame(m_HeadingTapeFrame.get());

	m_HeadingBox = new HUD::Element;
	m_HeadingText = new HUD::ElementText(m_HeadingBox.get());
	HUD::SymbolMaker box;
	box.addRectangle(-0.0035, -0.0630, 0.0035, -0.0670);
	m_HeadingBox->addSymbol(box);
	m_HeadingTapeFrame->addElement(m_HeadingBox);
	m_StandardFont->apply(m_HeadingText);
	m_HeadingText->setPosition(0.0, -0.0650);
	m_HeadingText->setAlignment(osgText::Text::CENTER_CENTER);
}

void F16HUD::newVerticalText(osg::ref_ptr<HUD::ElementText> &element, float x, float y, HUD::ElementText::AlignmentType alignment) {
	element = new HUD::ElementText(new HUD::Element);
	m_VerticalFrame->addElement(element->element());
	m_StandardFont->apply(element);
	element->setPosition(x, y);
	element->setAlignment(alignment);
}

void F16HUD::addTextElements() {
	newVerticalText(m_GForceMeter, -0.0445, -0.010);
	newVerticalText(m_MasterArm, -0.042, -0.0520);
	m_MasterArm->setText("SIM");
	newVerticalText(m_MachMeter, -0.042, -0.0555);
	newVerticalText(m_MaxGForce, -0.042, -0.0590, HUD::ElementText::RIGHT_BOTTOM);
	newVerticalText(m_MasterMode, -0.042, -0.0625, HUD::ElementText::RIGHT_BOTTOM);
	m_MasterMode->setText("NAV");

	newVerticalText(m_RadarAltimeter, 0.035, -0.0535);
	HUD::SymbolMaker box;
	box.addRectangle(0.038, -0.0505, 0.053, -0.0540);
	m_RadarAltimeter->element()->addSymbol(box);

	newVerticalText(m_Alow, 0.037, -0.0570);
	newVerticalText(m_SlantRange, 0.037, -0.0605);
	newVerticalText(m_SteerpointTime, 0.037, -0.0640);
	newVerticalText(m_SteerpointDistance, 0.037, -0.0675);

	newVerticalText(m_AirspeedUnits, -0.040, -0.028);
}

void F16HUD::addDEDReadout() {
	m_DEDReadout = new DEDReadout(b_DEDReadout->value(), m_StandardFont);
	m_DEDReadout->setPosition(-0.030, -0.0760);
	m_HUD.addFrameElement(m_DEDReadout);
}

void F16HUD::addPitchLadder() {
	m_PitchLadder = new PitchLadder;  // TODO should take m_HUD in ctor to add bars

	for (int i = -85/5; i < 90/5; ++i) {
		char buffer[8];
		float angle = 5.0 * i;
		float sign = (i < 0) ? -1.0 : 1.0;
		float slope = (i < 0) ? sin(0.25 * osg::inDegrees(angle)) : 0.0;
		osgText::Text *label;
		HUD::DirectionElement *element = new HUD::DirectionElement;
		HUD::SymbolMaker bar;
		bar.beginDrawLines();
		if (i > 0) {  // above the horizon, flat pitch bars
			bar.drawLine(0.020, -0.002 * sign, 0.020, 0.000);
			bar.drawLine(-0.020, -0.002 * sign, -0.020, 0.000);
			bar.drawLine(0.02, 0.000, 0.005, 0.000);
			bar.drawLine(-0.02, 0.000, -0.005, 0.000);
		} else if (i < 0) {  // below the horizon, articulated/dashed pitch bars
			bar.drawLine( 0.005, 0.002, 0.005, 0.000);
			bar.drawLine(0.16 * 0.015 + 0.005, 0.16 * 0.015 * slope, 0.005, 0.000);
			bar.drawLine(0.28 * 0.015 + 0.005, 0.28 * 0.015 * slope, 0.44 * 0.015 + 0.005, 0.44 * 0.015 * slope);
			bar.drawLine(0.56 * 0.015 + 0.005, 0.56 * 0.015 * slope, 0.72 * 0.015 + 0.005, 0.72 * 0.015 * slope);
			bar.drawLine(0.84 * 0.015 + 0.005, 0.84 * 0.015 * slope, 1.00 * 0.015 + 0.005, 1.00 * 0.015 * slope);
			bar.drawLine(-0.005, 0.002, -0.005, 0.000);
			bar.drawLine(-0.16 * 0.015 - 0.005, 0.16 * 0.015 * slope, -0.005, 0.000);
			bar.drawLine(-0.28 * 0.015 - 0.005, 0.28 * 0.015 * slope, -0.44 * 0.015 - 0.005, 0.44 * 0.015 * slope);
			bar.drawLine(-0.56 * 0.015 - 0.005, 0.56 * 0.015 * slope, -0.72 * 0.015 - 0.005, 0.72 * 0.015 * slope);
			bar.drawLine(-0.84 * 0.015 - 0.005, 0.84 * 0.015 * slope, -1.00 * 0.015 - 0.005, 1.00 * 0.015 * slope);
		} else {
			// horizon line
			bar.drawLine(1.0, 0.000, 0.005, 0.000);
			bar.drawLine(-1.0, 0.000, -0.005, 0.000);
		}
		bar.endDrawLines();
		element->addSymbol(bar);
		if (i != 0) {
			label = element->addText();
			m_StandardFont->apply(label);
			snprintf(buffer, sizeof(buffer), "%d", std::abs(i * 5));
			label->setText(buffer);
			label->setPosition(osg::Vec3(0.0205, 0.0, 0.015 * slope));
			label->setAlignment((i < 0) ? osgText::Text::LEFT_CENTER : osgText::Text::LEFT_CENTER);
			label = element->addText();
			m_StandardFont->apply(label);
			label->setText(buffer);
			label->setPosition(osg::Vec3(-0.0205, 0.0, 0.015 * slope));
			label->setAlignment((i < 0) ? osgText::Text::RIGHT_CENTER : osgText::Text::RIGHT_CENTER);
		}
		m_PitchLadder->add(element, angle);
		m_HUD.addFloatingElement(element);
	}

	{ // -2.5 degree glideslope bar
		HUD::DirectionElement *element = new HUD::DirectionElement;
		HUD::SymbolMaker bar;
		bar.beginDrawLines();
		bar.drawLine(0.16 * 0.015 + 0.005, 0.0, 0.005, 0.0);
		bar.drawLine(0.28 * 0.015 + 0.005, 0.0, 0.44 * 0.015 + 0.005, 0.0);
		bar.drawLine(0.56 * 0.015 + 0.005, 0.0, 0.72 * 0.015 + 0.005, 0.0);
		bar.drawLine(-0.16 * 0.015 - 0.005, 0.0, -0.005, 0.0);
		bar.drawLine(-0.28 * 0.015 - 0.005, 0.0, -0.44 * 0.015 - 0.005, 0.0);
		bar.drawLine(-0.56 * 0.015 - 0.005, 0.0, -0.72 * 0.015 - 0.005, 0.0);
		bar.endDrawLines();
		element->addSymbol(bar);
		m_PitchLadder->add(element, -2.5);
		m_HUD.addFloatingElement(element);
		m_GlideSlopeBar = element;
	}

}

// TODO move these calculations to a navigation system; update sp cas less often but low pass
// filter it.
double F16HUD::getSpeedCaret(double ground_speed) const {
	if (m_VelocityUnits == CAS) {
		double altitude = b_Position->value().z();
		double mach = CSPSim::theSim->getAtmosphere()->getMach(ground_speed, altitude);
		return CSPSim::theSim->getAtmosphere()->getCAS(mach, altitude);
	}
	if (m_VelocityUnits == TAS) {
		return ground_speed; // XXX
	}
	return ground_speed;
}

double F16HUD::getSpeed() const {
	if (m_VelocityUnits == CAS) {
		return b_CAS->value();
	}
	if (m_VelocityUnits == TAS) {
		return b_Velocity->value().length();  // XXX
	}
	return b_Velocity->value().length();  // GND
}

