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
 * @file ListBox.cpp
 *
 **/

#include <csp/cspsim/Animation.h>
#include <csp/cspsim/wf/ControlGeometryBuilder.h>
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/SignalData.h>
#include <csp/cspsim/wf/WindowManager.h>

#include <osg/Switch>

CSP_NAMESPACE

namespace wf {

ListBox::ListBox() : m_SelectedItemChanged(new Signal) {
}

ListBox::~ListBox() {
}

ControlVector ListBox::getChildControls() {
	ControlVector childControls;
	ListBoxItemVector::iterator item = m_Items.begin();
	for(;item != m_Items.end();++item) {
		childControls.push_back(*item);
	}
	return childControls;
}

std::string ListBox::getName() const {
	return "ListBox";
}

void ListBox::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	Container::buildGeometry();	
	
	// Build the geometry for the current control.
	ControlGeometryBuilder geometryBuilder;
	getNode()->addChild(geometryBuilder.buildListBox(this));
	
	ListBoxItemVector::iterator item = m_Items.begin();
	for(;item != m_Items.end();++item) {
		(*item)->buildGeometry();
		getNode()->addChild((*item)->getNode());
	}
}

void ListBox::layoutChildControls() {
	Size listBoxSize = getSize();
	Rectangle clientRect = getClientRect();

	double yPosition =  clientRect.y0;

	ListBoxItemVector::iterator item = m_Items.begin();
	for(int index=0;item != m_Items.end();++item, ++index) {	
		// Set the width and location of the list box item.	
		Ref<Style> style = (*item)->getStyle();
		style->setWidth(Style::UnitValue(Style::Pixels, clientRect.width()));
		style->setLeft(Style::UnitValue(Style::Pixels, clientRect.x0));
		style->setTop(Style::UnitValue(Style::Pixels, yPosition));

		(*item)->setZPos(2.0f);
		(*item)->layoutChildControls();

		// Prepare the position for the next listbox item.
		Size size = (*item)->getSize();
		yPosition += (size.height);
	}
}

void ListBox::addItem(ListBoxItem* item) {
	m_Items.push_back(item);
	item->setParent(this);
}

ListBoxItemVector ListBox::getItems() {
	return m_Items;
}

ListBoxItem* ListBox::getSelectedItem() {
	return m_SelectedItem.get();
}

ListBoxItem* ListBox::getSelectedItem() const {
	return m_SelectedItem.get();
}

void ListBox::setSelectedItem(ListBoxItem* newItem) {
	// Do nothing if we select the same object again.
	if(m_SelectedItem.get() == newItem) {
		return;
	}
	
	// Remove the old selected state from the item.
	if(m_SelectedItem.valid()) {
		m_SelectedItem->removeState("selected");
	}
	
	m_SelectedItem = newItem;
	
	// Add the selected state to the new listbox item.
	if(m_SelectedItem.valid()) {
		m_SelectedItem->addState("selected");
	}
	
	// Fire event to any listeners.
	Ref<SignalData> data = new SignalData();
	m_SelectedItemChanged->emit(data.get());
	 
	buildGeometry();
}

bool ListBox::setSelectedItemByText(const std::string& text) {
	ListBoxItemVector::iterator item = m_Items.begin();
	for(;item != m_Items.end();++item) {
		if(text == (*item)->getText()) {
			setSelectedItem(item->get());
			return true;
		}
	}
	return false;
}

Signal* ListBox::getSelectedItemChangedSignal() {
	return m_SelectedItemChanged.get();
}

} // namespace wf

CSP_NAMESPACE_END
