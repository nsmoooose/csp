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
 * @file HUDTape.h
 *
 **/


#ifndef __HUDTAPE_H__
#define __HUDTAPE_H__

#include "HUD.h"
#include "Text.h"


// TODO
// add major ticks
// generalize the tape and provide a specialized subclass for the f16 if necessary.


/** A class for implementing a scrolling scale bar.   The behavior is somewhat
 *  customized to the F-16 HUD scales.
 */
class HUDTape: public osg::Referenced {
public:
	typedef enum { HORIZONTAL, VERTICAL } Orientation;

	/** Create a HUD tape scale.
	 *
	 *  @param orientation The direction of the scale motion.  Left/right and top/bottom
	 *    are controlled by the sign of tick_width.
	 *  @param tick_count The number of visibile ticks.  The tick count must be odd and
	 *    will be incremented by one if necessary.
	 *  @param tick_spacing The distance between successive ticks.
	 *  @param tick_width The width of each tick.  If negative the scale is a left or top
	 *    scale depending on the orientation.
	 *  @param offest_x The x position of the center point of the scale.
	 *  @param offest_y The y position of the center point of the scale.
	 */
	HUDTape(Orientation orientation, int tick_count, float tick_spacing, float tick_width, float offset_x, float offset_y);

	/** Set the scale and offset of the input value relative to the tape scale.
	 *
	 *  The tape position (in units of minor tape ticks) is (value + offset) / scale.
	 *  Thus if one tape tick corresponds to 100 m/s, the scale should be set to
	 *  100.0 so that the input can be specified in m/s.  Changing the sign of
	 *  the scale changes the direction of motion of the tape.
	 *
	 *  @param scale Scale factor from tape ticks to input value units.
	 *  @param offset Offset in value units.  This is the effective value when the
	 *    input value is zero.
	 */
	void setValueScale(float scale, float offset=0.0);

	/** Add numeric labels to the tape scale.
	 *
	 *  @param increment The increment between successive labels.
	 *  @param value_offset The offset between the value passed to update and the
	 *    corresponding label text (usually zero).
	 *  @param position_offset The offset of the labels perpendicular to the tape
	 *    direction.
	 *  @param minor_ticks The number of unlabelled ticks between successive labels.
	 *  @param formatter A LabelFormatter that converts values into label strings.
	 *  @param font The font for the labels.
	 */
	void addNumericLabels(float increment, float value_offset, float position_offset, int minor_ticks, LabelFormatter *formatter, HUDFont *font);

	/** Show or hide negative labels.  By default negative labels are displayed.
	 */
	void showNegativeLabels(bool show);

	/** Set a region in which the labels will be hidden.  The positions are absolute
	 *  coordinates along the direction of the tape.
	 */
	void setLabelHidingWindow(float window_min, float window_max);

	/** Set label wrapping parameters.  Label wrapping is useful for cyclic variables,
	 *  such as heading.  If the period is greater than zero, the displayed label will
	 *  be ((value + offset) MOD period) - offset.
	 *  @param offset Negative the minimum tape value.  For example, if the tape value
	 *    ranges from -10 to 10 the offset should be 10.  If the tape value ranges from
	 *    10 to 30 the offset should be -10.
	 *  @param period The wrapping period.  For example, if the values range from
	 *    -10 to 10 the period is 20.
	 */
	void setLabelWrapping(float offset, float period);

	/** Set the caret symbol.  The canonical symbol direction is "<" for any tape
	 *  orientation, taking the sign of the tick width into account --- the symbol
	 *  will be rotated appropriately.
	 *
	 *  See setCaret() for setting the caret position.
	 */
	void setCaretSymbol(HUD::SymbolMaker &symbol);

	/** Set the caret position.  The value parameter uses the same scale as the
	 *  update() method.
	 */
	void setCaret(float value);

	/** Show or hide the caret.
	 */
	void enableCaret(bool enabled);

	/** Add a line extending perpendicularly from the center of the tape.
	 *
	 *  @param The length of the line.
	 *  @param The gap between the line and the tape scale.
	 */
	void addCenterLine(float length, float gap);

	/** Add the tape symbology to a HUD frame.  This method must be called after all
	 *  configuration methods (e.g. setCaretSymbol).
	 */
	void addToHudFrame(HUD::Frame *hud_frame);

	/** Update the tape scale and readout to the current value.
	 *  FIXME value is specified in tick increments but should be absolute.
	 */
	void update(double value);

	/** Show or hide the tape.
	 */
	void show(bool visible);

	/** Test if the tape is hidden.
	 */
	bool isHidden() const { return m_Hidden; }

private:

	/** Convert from value units to minor tick units.
	 */
	float valueToTicks(double value);

	/** Convert the tape offset to the correct 2D displacement for the tape orientation.
	 */
	void getMotion(float offset, float &dx, float &dy);

	/** Internal method to update the tape labels; called by update().
	 */
	void updateLabels(float value, float left_tick, float right_tick);

	osg::ref_ptr<HUD::MoveableElement> m_Ticks;
	osg::ref_ptr<HUD::MoveableElement> m_ExtraTick;
	osg::ref_ptr<HUD::Element> m_CenterLine;

	Orientation m_Orientation;
	int m_TickCount;
	float m_TickSpacing;
	float m_TickWidth;
	float m_OffsetX;
	float m_OffsetY;
	float m_TickScale;
	float m_WrapPeriod;
	float m_WrapOffset;
	int m_MinorTicks;
	float m_HideLabelMinimum;
	float m_HideLabelMaximum;
	bool m_ShowNegativeLabels;
	float m_Caret;
	bool m_HasCaret;
	bool m_CaretEnabled;
	int m_LabelBase;
	float m_LabelOffset;
	float m_LabelIncrement;
	float m_ValueScale;
	float m_ValueOffset;
	bool m_Hidden;
	osg::ref_ptr<HUD::MoveableElement> m_CaretSymbol;
	osg::ref_ptr<LabelFormatter> m_LabelFormatter;
	std::vector<osg::ref_ptr<HUD::LabelElement> > m_Labels;
};


#endif // __HUDTAPE_H__

