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
 * @file HUDTape.cpp
 *
 **/


#include <HUD/Tape.h>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgText/Text>

#include <cassert>
#include <cmath>


HUDTape::HUDTape(Orientation orientation, int tick_count, float tick_spacing, float tick_width, float offset_x, float offset_y):
	m_Orientation(orientation),
	m_TickCount(tick_count),
	m_TickSpacing(tick_spacing),
	m_TickWidth(tick_width),
	m_OffsetX(offset_x),
	m_OffsetY(offset_y),
	m_TickScale(1.0),
	m_WrapPeriod(0.0),
	m_WrapOffset(0.0),
	m_MinorTicks(0),
	m_HideLabelMinimum(0.0),
	m_HideLabelMaximum(0.0),
	m_ShowNegativeLabels(true),
	m_Caret(0.0),
	m_HasCaret(false),
	m_CaretEnabled(false),
	m_ValueScale(1.0),
	m_ValueOffset(0.0),
	m_Hidden(false)
{
	m_Ticks = new HUD::MoveableElement;
	m_ExtraTick = new HUD::MoveableElement;
	HUD::SymbolMaker ticks;
	ticks.beginDrawLines();

	// always an odd count with an extra tick.  the extra tick is visible through
	// part of the range and helps prevent the sawtooth motion of the main tape from
	// appearing like a discontinuous jump.
	m_TickCount |= 1;
	float center_offset = 0.5 * (m_TickCount - 1);
	for (int i = 0; i < m_TickCount; ++i) {
		ticks.drawLine(0.0, (i - center_offset)  * tick_spacing, tick_width, (i - center_offset) * tick_spacing);
	}
	ticks.endDrawLines();
	m_Ticks->addSymbol(ticks);

	HUD::SymbolMaker extra_tick;
	extra_tick.addLine(0.0, (-1 - center_offset) * tick_spacing, tick_width, (-1 - center_offset) * tick_spacing);
	m_ExtraTick->addSymbol(extra_tick);
	m_ExtraTick->show(false);

	float tick_direction = (tick_width < 0) ? -1.0 : 1.0;

	m_CaretSymbol = new HUD::MoveableElement;
	m_CaretSymbol->show(false);

	m_CenterLine = new HUD::Element;
	m_CenterLine->show(false);

	if (orientation == HORIZONTAL) {
		m_Ticks->setOrientation(osg::PI_2);
		m_ExtraTick->setOrientation(osg::PI_2);
		m_CaretSymbol->setOrientation(-tick_direction * osg::PI_2);
	} else {
		m_CaretSymbol->setOrientation((1.0 + tick_direction) * osg::PI_2);
	}
}

void HUDTape::setValueScale(float scale, float offset) {
	m_ValueScale = scale;
	m_ValueOffset = offset;
}

void HUDTape::addNumericLabels(float increment, float value_offset, float position_offset, int minor_ticks, LabelFormatter *formatter, HUDFont *font) {
	assert(formatter);
	assert(font);
	const int label_count = m_TickCount / (minor_ticks + 1) + 1;
	m_Labels.reserve(label_count);
	m_LabelFormatter = formatter;
	m_MinorTicks = minor_ticks;
	m_LabelBase = 999999999;  // force relabelling
	m_LabelOffset = value_offset;
	m_LabelIncrement = increment;
	const float center_offset = 0.0; // put label_0 at center
	for (int i=0; i < label_count; ++i) {
		HUD::LabelElement *element = new HUD::LabelElement;
		osgText::Text *text = element->text();
		m_Labels.push_back(element);
		font->apply(text);
		if (m_Orientation == VERTICAL) {
			text->setAlignment((m_TickWidth < 0) ? osgText::Text::RIGHT_CENTER : osgText::Text::LEFT_CENTER);
			text->setPosition(osg::Vec3(m_TickWidth + position_offset,  0, m_TickSpacing * (i * (1 + minor_ticks) - center_offset)));
		} else {
			text->setAlignment((m_TickWidth < 0) ? osgText::Text::CENTER_BOTTOM : osgText::Text::CENTER_TOP);
			text->setPosition(osg::Vec3(m_TickSpacing * (i * (1 + minor_ticks) - center_offset), 0, -m_TickWidth + position_offset));
		}
	}
}

void HUDTape::showNegativeLabels(bool show) {
	m_ShowNegativeLabels = show;
}

void HUDTape::setLabelHidingWindow(float window_min, float window_max) {
	m_HideLabelMinimum = window_min;
	m_HideLabelMaximum = window_max;
}

float HUDTape::valueToTicks(double value) {
	// negative scale factor so that positive values displace the ticks to the left/down
	// (center marker appears to move right/up relative to the tape).
	return - (value + m_ValueOffset) / m_ValueScale;
}

void HUDTape::setLabelWrapping(float offset, float period) {
	m_WrapOffset = offset;
	m_WrapPeriod = period;
}

void HUDTape::setCaretSymbol(HUD::SymbolMaker &symbol) {
	assert(!m_HasCaret);
	m_CaretSymbol->addSymbol(symbol);
	m_HasCaret = true;
	m_CaretEnabled = true;
}

void HUDTape::setCaret(float value) {
	m_Caret = valueToTicks(value);
}

void HUDTape::enableCaret(bool enabled) {
	m_CaretEnabled = enabled;
}

void HUDTape::addCenterLine(float length, float gap) {
	float sign = m_TickWidth < 0.0 ? -1.0 : 1.0;
	HUD::SymbolMaker line;
	if (m_Orientation == HORIZONTAL) {
		line.addLine(m_OffsetX, m_OffsetY + sign * gap, m_OffsetX, m_OffsetY + sign * (gap + length));
	} else {
		line.addLine(m_OffsetX - sign * gap, m_OffsetY, m_OffsetX - sign * (gap + length), m_OffsetY);
	}
	m_CenterLine->clear();
	m_CenterLine->addSymbol(line);
	m_CenterLine->show(true);
}

void HUDTape::addToHudFrame(HUD::Frame *hud_frame) {
	hud_frame->addElement(m_Ticks);
	hud_frame->addElement(m_ExtraTick);
	for (unsigned i=0; i < m_Labels.size(); ++i) {
		hud_frame->addElement(m_Labels[i]);
	}
	hud_frame->addElement(m_CenterLine);
	if (m_HasCaret) {
		hud_frame->addElement(m_CaretSymbol);
	}
}

void HUDTape::update(double value) {
	if (m_Hidden) return;

	value = valueToTicks(value);

	float delta = value - static_cast<int>(value);
	bool show_extra = std::abs(delta) > 0.25f && std::abs(delta) < 0.75f;

	if (delta > 0.75f) delta -= 1.0f;
	if (delta < -0.25f) delta += 1.0f;

	float dx, dy;
	getMotion(m_TickSpacing * delta, dx, dy);

	m_Ticks->setPosition(m_OffsetX + dx, m_OffsetY + dy);

	if (show_extra) {
		m_ExtraTick->setPosition(m_OffsetX + dx, m_OffsetY + dy);
		m_ExtraTick->show(true);
	} else {
		m_ExtraTick->show(false);
	}
	
	float left_tick = -0.5 * (m_TickCount - 1) + (show_extra ? -1.0 : 0.0) + delta;
	float right_tick = 0.5 * (m_TickCount - 1) + delta;

	if (!m_Labels.empty()) {
		updateLabels(value, left_tick, right_tick);
	}

	if (m_HasCaret) {
		// prevent caret flicker at the extreme ends of the tape
		float hysteresis = m_CaretSymbol->isHidden() ? 0.05 : -0.05;
		bool visible = m_CaretEnabled && ((m_Caret >= -value - 0.5 * (m_TickCount + 1) + hysteresis) && (m_Caret <= -value + 0.5 * (m_TickCount + 1) - hysteresis));
		m_CaretSymbol->show(visible);
		if (visible) {
			getMotion(m_TickSpacing * (value + m_Caret), dx, dy);
			m_CaretSymbol->setPosition(m_OffsetX + dx, m_OffsetY + dy);
		}
	}
}

void HUDTape::getMotion(float offset, float &dx, float &dy) {
	dx = (m_Orientation == HORIZONTAL) ? offset : 0.0;
	dy = (m_Orientation == HORIZONTAL) ? 0.0 : offset;
}


// like the tick marks, the labels move continuously through one interval and then
// ratchet up/down discontinuously by one interval.  the left and rightmost labels
// appear/disappear with the corresponding tick mark.  we have to do a little more
// work with each jump, since the text of each label must be regenerated (although
// in principle this could be avoided, for a small number of labels it is the best
// approach).
void HUDTape::updateLabels(float value, float left_tick, float right_tick) {
	assert(!m_Labels.empty());
	int interval = 1 + m_MinorTicks;

	int base = static_cast<int>(value / interval);
	float label_delta = value - base * interval;

	// very ugly :-(
	while (label_delta - interval >= left_tick - 0.1) {
		label_delta -= interval;
		base += 1;
	}
	const bool show_back = (label_delta + (m_Labels.size() - 1) * interval <= right_tick + 0.1);

	float dx, dy;
	getMotion(m_TickSpacing * label_delta, dx, dy);

	for (unsigned i = 0; i < m_Labels.size(); ++i) {
		m_Labels[i]->setPosition(m_OffsetX + dx, m_OffsetY + dy);
	}

	// if the label elements have ratched down/up we need to relabel them.
	if (base != m_LabelBase) {
		const bool wrap = (m_WrapPeriod > 0.0);
		for (unsigned i = 0; i < m_Labels.size(); ++i) {
			char label[80];
			float value = m_LabelOffset + m_LabelIncrement * (static_cast<int>(i) - base);
			if (wrap) {
				value = fmod(value + m_WrapOffset, m_WrapPeriod);
				if (value < 0) value += m_WrapPeriod;
				value -= m_WrapOffset;
			}
			if (!m_ShowNegativeLabels && value < 0.0) {
				*label = 0;
			} else {
				m_LabelFormatter->format(label, sizeof(label), value);
			}
			m_Labels[i]->text()->setText(label);
		}
		m_LabelBase = base;
	}

	// if there is a hiding window, test each label against it and show/hide them
	// as needed.
	const bool check_hide = (m_HideLabelMinimum < m_HideLabelMaximum);
	if (check_hide) {
		for (unsigned i = 0; i < m_Labels.size(); ++i) {
			osg::BoundingSphere bound = m_Labels[i]->node()->getBound();
			if (m_Orientation == HORIZONTAL) {
				float left = bound.center().x() - bound.radius();
				float right = bound.center().x() + bound.radius();
				m_Labels[i]->show(left > m_HideLabelMaximum || right < m_HideLabelMinimum);
			} else {
				float top = bound.center().z() + bound.radius();
				float bottom = bound.center().z() - bound.radius();
				m_Labels[i]->show(bottom > m_HideLabelMaximum || top < m_HideLabelMinimum);
			}
		}
	}

	// defer to the end since the hiding window may have made this label visible.
	m_Labels.back()->show(show_back);
}

void HUDTape::show(bool visible) {
	if (m_Hidden == visible) {
		m_CenterLine->show(visible);
		if (m_HasCaret) m_CaretSymbol->show(visible);
		m_Ticks->show(visible);
		m_ExtraTick->show(visible);
		for (unsigned i = 0; i < m_Labels.size(); ++i) {
			m_Labels[i]->show(visible);
		}
	}
	m_Hidden = !visible;
}

