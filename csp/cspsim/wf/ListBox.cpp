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
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/Theme.h>
#include <csp/cspsim/wf/WindowManager.h>

#include <osg/Switch>

CSP_NAMESPACE

namespace wf {

class ListBox::ListBoxItemClickedCallback : public AnimationCallback {
public:
	ListBoxItemClickedCallback(ListBox* listBox, ListBoxItem* item) : m_ListBox(listBox), m_Item(item) {}
	virtual ~ListBoxItemClickedCallback() {}
	virtual bool pick(int /*flags*/) {
		m_ListBox->setSelectedItem(m_Item);
		return true;
	}
	
private:
	ListBox* m_ListBox;
	ListBoxItem* m_Item;
};

ListBox::ListBox(Theme* theme) : Container(theme) {
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

void ListBox::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	Container::buildGeometry();	
	
	// Build the geometry for the current control.
	getNode()->addChild(getTheme()->buildListBox(this));
	
	ListBoxItemVector::iterator item = m_Items.begin();
	for(;item != m_Items.end();++item) {
		(*item)->buildGeometry();
		getNode()->addChild((*item)->getNode());

		osg::ref_ptr<ListBoxItemClickedCallback> callback = new ListBoxItemClickedCallback(this, item->get());
		(*item)->getNode()->setUpdateCallback(callback.get());
	}
}

void ListBox::layoutChildControls() {
	Size listBoxSize = getSize();

	double yPosition = listBoxSize.m_H / 2.0f;

	ListBoxItemVector::iterator item = m_Items.begin();
	for(int index=0;item != m_Items.end();++item, ++index) {
		// Set the width of the list box item.	
		Size size = (*item)->getSize();
		size.m_W = listBoxSize.m_W;
		(*item)->setSize(size);
		(*item)->setZPos(-0.2f);
		(*item)->layoutChildControls();

		// Set the location of the listbox item.
		yPosition -= (size.m_H / 2.0f);
		(*item)->setLocation(Point(0.0f, yPosition));
		yPosition -= (size.m_H / 2.0f);
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
	m_SelectedItem = newItem;
	buildGeometry();
}

} // namespace wf

CSP_NAMESPACE_END
