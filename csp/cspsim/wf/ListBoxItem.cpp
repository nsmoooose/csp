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

ListBoxItem::ListBoxItem(Theme* theme) : SingleControlContainer(theme), m_ChildControl(new Label(theme)) {
	Label* label = (Label*)m_ChildControl.get();
	label->setAlignment(osgText::Text::CENTER_CENTER);
	label->setZPos(-0.1f);
	setSize(Size(30.0f, 7.0f));
}

ListBoxItem::ListBoxItem(Theme* theme, const std::string text) : SingleControlContainer(theme), m_ChildControl(new Label(theme, text)) {
	Label* label = (Label*)m_ChildControl.get();
	label->setAlignment(osgText::Text::CENTER_CENTER);
	label->setZPos(-0.1f);
	setSize(Size(30.0f, 7.0f));
}

ListBoxItem::~ListBoxItem() {
}

void ListBoxItem::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	SingleControlContainer::buildGeometry();
	
	osg::ref_ptr<osg::Switch> item = getTheme()->buildListBoxItem(this);
	getNode()->addChild(item.get());
	
	if(m_ChildControl.valid()) {
		m_ChildControl->buildGeometry();
		getNode()->addChild(m_ChildControl->getNode());
		/*
		osg::ref_ptr<osg::Group> childControl = m_ChildControl->getNode();	
		osg::Group* itemGroup = dynamic_cast<osg::Group*>(item->getChild(0));
		if(itemGroup != NULL) {
			itemGroup->addChild(childControl.get());
		}
		*/
	}
}

const std::string ListBoxItem::getText() const {
	Label* label = dynamic_cast<Label*>(m_ChildControl.get());
	if(label != NULL) {
		return label->getText();
	}
	return std::string("");
}

void ListBoxItem::setText(const std::string& text) {
	Label* label = dynamic_cast<Label*>(m_ChildControl.get());
	if(label != NULL) {
		label->setText(text);
	}
}

Control* ListBoxItem::getControl() {
	return m_ChildControl.get();
}

void ListBoxItem::setControl(Control* control) {
	m_ChildControl = control;
}

} // namespace wf

CSP_NAMESPACE_END
