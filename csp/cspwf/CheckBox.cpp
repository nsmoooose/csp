// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file CheckBox.cpp
 *
 **/

#include <csp/cspwf/Check.h>
#include <csp/cspwf/CheckBox.h>
#include <csp/cspwf/ControlGeometryBuilder.h>
#include <csp/cspwf/Label.h>
#include <csp/cspwf/SignalData.h>
#include <csp/cspwf/WindowManager.h>

#include <osg/Group>

namespace csp {
namespace wf {

CheckBox::CheckBox() : SingleControlContainer("CheckBox"), 
	m_Checked(false), m_CheckedChanged(new Signal) {
	Ref<Check> check = new Check;
	Ref<Style> style = check->getStyle();
	style->setWidth(Style::UnitValue(Style::Pixels, 20));
	style->setHeight(Style::UnitValue(Style::Pixels, 20));
	setControl(check.get());
}

CheckBox::~CheckBox() {
}

void CheckBox::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	SingleControlContainer::buildGeometry();

	// Build our own button control and add it to the group.
	ControlGeometryBuilder geometryBuilder;
	osg::ref_ptr<osg::Group> checkBox = geometryBuilder.buildCheckBox(this);
	getNode()->addChild(checkBox.get());
}

void CheckBox::layoutChildControls() {
	// Override the layout of child control. The check control is placed
	// to the left at position 0,0. The width and heigth of the check control
	// is the same as the height of the checkbox control.
	Control* childControl = getControl();
	if(childControl != NULL) {
		float height = getSize().height;
		Ref<Style> style = childControl->getStyle();
		style->setLeft(Style::UnitValue(Style::Pixels, 0));
		style->setTop(Style::UnitValue(Style::Pixels, 0));
		style->setWidth(Style::UnitValue(Style::Pixels, height));
		style->setHeight(Style::UnitValue(Style::Pixels, height));
		childControl->setZPos(0.5f);
	}
}

const std::string CheckBox::getText() const {
	return m_Text;
}

void CheckBox::setText(const std::string& text) {
	m_Text = text;
	buildGeometry();
}

bool CheckBox::getChecked() const {
	return m_Checked;
}

void CheckBox::setChecked(bool checked) {
	// Test to see if there is a change in status.
	if(m_Checked == checked) {
		return;
	}
	Ref<Control> control = getControl();
	if(control.valid())
	{
		if(checked) {
			control->addState("checked");
		}
		else {
			control->removeState("checked");
		}
	}
	m_Checked = checked;
	buildGeometry();

	Ref<SignalData> data = new SignalData;
	m_CheckedChanged->emit(data.get());
}

Signal* CheckBox::getCheckedChangedSignal() {
	return m_CheckedChanged.get();
}

void CheckBox::onClick(ClickEventArgs& event) {
	// Modify the geometry according to the click event.
	setChecked(!getChecked());

	// Set the event to handled to prevent it to bubble up to the parent control.
	event.handled = true;
	Control::onClick(event);
}

} // namespace wf
} // namespace csp
