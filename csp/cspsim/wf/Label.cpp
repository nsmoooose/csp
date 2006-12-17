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
 * @file Label.cpp
 *
 **/

#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/Theme.h>
#include <osg/Group>

CSP_NAMESPACE

namespace wf {

Label::Label(Theme* theme) : Control(theme), m_Alignment(osgText::Text::LEFT_CENTER) {
}

Label::Label(Theme* theme, const std::string text) : Control(theme), m_Text(text), m_Alignment(osgText::Text::LEFT_CENTER) {
}

Label::~Label() {
}

void Label::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	Control::buildGeometry();	
	
	osg::ref_ptr<osg::Group> label = getTheme()->buildLabel(this);
	getNode()->addChild(label.get());
}

const std::string& Label::getText() const {
	return m_Text;
}

void Label::setText(const std::string& text) {
	m_Text = text;
	buildGeometry();
}

osgText::Text::AlignmentType Label::getAlignment() const {
	return m_Alignment;
}

void Label::setAlignment(osgText::Text::AlignmentType align) {
	m_Alignment = align;
}

} // namespace wf

CSP_NAMESPACE_END
