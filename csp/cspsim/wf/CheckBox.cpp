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

#include <csp/cspsim/Animation.h>
#include <csp/cspsim/wf/CheckBox.h>
#include <csp/cspsim/wf/ControlGeometryBuilder.h>
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/WindowManager.h>

#include <osg/Group>

CSP_NAMESPACE

namespace wf {

class CheckBox::CheckBoxClickedCallback : public AnimationCallback {
public:
	CheckBoxClickedCallback(CheckBox* checkBox) : m_CheckBox(checkBox) {}
	virtual ~CheckBoxClickedCallback() {}
	virtual bool pick(int /*flags*/) {
		m_CheckBox->setChecked(!m_CheckBox->getChecked());
		return true;
	}
	
private:
	CheckBox* m_CheckBox;
};

CheckBox::CheckBox() : m_Checked(false) {
}

CheckBox::CheckBox(const std::string text) : m_Text(text), m_Checked(false) {
}

CheckBox::~CheckBox() {
}

void CheckBox::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	Control::buildGeometry();
	
	// Build our own button control and add it to the group.
	ControlGeometryBuilder geometryBuilder;
	osg::ref_ptr<osg::Group> checkBox = geometryBuilder.buildCheckBox(this);
	osg::ref_ptr<CheckBoxClickedCallback> callback = new CheckBoxClickedCallback(this);
	checkBox->setUpdateCallback(callback.get());
	getNode()->addChild(checkBox.get());		
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
	m_Checked = checked;
	buildGeometry();
}

} // namespace wf

CSP_NAMESPACE_END
