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

#include <csp/cspwf/ControlGeometryBuilder.h>
#include <csp/cspwf/Label.h>
#include <csp/cspwf/ListBox.h>
#include <csp/cspwf/ListBoxItem.h>
#include <csp/cspwf/WindowManager.h>

#include <osg/Switch>

namespace csp {

namespace wf {

ListBoxItem::ListBoxItem() : SingleControlContainer("ListBoxItem") {
	Ref<Label> label = new Label();
	Ref<Style> style = label->getStyle();
	style->setTextHorizontalAlign(std::string("center"));
	label->setZPos(-0.1f);
	setControl(label.get());
}

ListBoxItem::ListBoxItem(const std::string& text) : 
	SingleControlContainer("ListBoxItem"), m_Text(text) {
	Ref<Label> label = new Label();
	Ref<Style> style = label->getStyle();
	style->setTextHorizontalAlign(std::string("center"));
	label->setZPos(-0.1f);
	label->setText(text);
	setControl(label.get());
}

ListBoxItem::~ListBoxItem() {
}

void ListBoxItem::performLayout() {
	// Without a parent we cannot build geometry.
	if(getParent() == NULL) {
		return;
	}

	Label* label = dynamic_cast<Label*>(getControl());
	if(label != NULL) {
		label->setText(m_Text);
	}

	// Make sure that all our child controls onInit() is called.
	SingleControlContainer::performLayout();
	
	ControlGeometryBuilder geometryBuilder;
	osg::ref_ptr<osg::Group> item = geometryBuilder.buildListBoxItem(this);
	getNode()->addChild(item.get());	
}

const std::string ListBoxItem::getText() const {
	return m_Text;
}

void ListBoxItem::setText(const std::string& text) {
	m_Text = text;
	performLayout();
}

const Ref<Style> ListBoxItem::getSelectedStyle() const {
	return m_SelectedStyle;
}

void ListBoxItem::onClick(ClickEventArgs& event) {
	ListBox* parent = dynamic_cast<ListBox*>(getParent());
	if(parent != NULL) {
		parent->setSelectedItem(this);
	}
	
	// Set that the event has been handled to prevent it to
	// bubble up to the parent control.
	event.handled = true;
	SingleControlContainer::onClick(event);
}

} // namespace wf

} // namespace csp
