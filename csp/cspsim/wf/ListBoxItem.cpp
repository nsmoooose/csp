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
 * @file ListBoxItem.cpp
 *
 **/

#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/ListBoxItem.h>
#include <csp/cspsim/wf/Theme.h>
#include <csp/cspsim/wf/WindowManager.h>

#include <osg/Switch>

CSP_NAMESPACE

namespace wf {

ListBoxItem::ListBoxItem(Theme* theme) : SingleControlContainer(theme) {
	Ref<Label> label = new Label(theme);
	label->setAlignment(osgText::Text::CENTER_CENTER);
	label->setZPos(-0.1f);
	setControl(label.get());
	setSize(Size(30.0f, 7.0f));
}

ListBoxItem::ListBoxItem(Theme* theme, const std::string text) : SingleControlContainer(theme), m_Text(text) {
	Ref<Label> label = new Label(theme, text);
	label->setAlignment(osgText::Text::CENTER_CENTER);
	label->setZPos(-0.1f);
	setControl(label.get());
	setSize(Size(30.0f, 7.0f));
}

ListBoxItem::~ListBoxItem() {
}

void ListBoxItem::buildGeometry() {
	Label* label = dynamic_cast<Label*>(getControl());
	if(label != NULL) {
		label->setText(m_Text);
	}

	// Make sure that all our child controls onInit() is called.
	SingleControlContainer::buildGeometry();
	
	osg::ref_ptr<osg::Switch> item = getTheme()->buildListBoxItem((ListBox*)getParent(), this);
	getNode()->addChild(item.get());	
}

const std::string ListBoxItem::getText() const {
	return m_Text;
}

void ListBoxItem::setText(const std::string& text) {
	m_Text = text;
	buildGeometry();
}

} // namespace wf

CSP_NAMESPACE_END
