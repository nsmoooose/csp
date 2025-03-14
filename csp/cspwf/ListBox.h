#pragma once
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
 * @file ListBox.h
 *
 **/

#include <csp/cspwf/Container.h>
#include <csp/cspwf/ListBoxItem.h>

namespace csp {
namespace wf {

/** A Control that holds one or more ListBoxItem objects. The purpose with this 
 * Control is to make it possible to display a list of texts that can be selected.
 * For example a list of all availible missions. A text can then be selected
 * using the mouse.
 */
class CSPWF_EXPORT ListBox : public Container {
public:
	ListBox();
	virtual ~ListBox();

	virtual ControlVector getChildControls();

	virtual void performLayout();
	virtual void layoutChildControls();
	
	virtual void addItem(ListBoxItem* item);
	virtual ListBoxItemVector getItems();
	
	virtual ListBoxItem* getSelectedItem() const;
	virtual ListBoxItem* getSelectedItem();
	virtual void setSelectedItem(ListBoxItem* item);
	virtual bool setSelectedItemByText(const std::string& text);
	
	virtual Signal* getSelectedItemChangedSignal();

	template<class Archive>
	void serialize(Archive & ar) {
		Container::serialize(ar);
		ar & make_nvp("Items", m_Items);
	}	

private:

protected:
	ListBoxItemVector m_Items;
	Ref<ListBoxItem> m_SelectedItem;
	Ref<Signal> m_SelectedItemChanged;
};

} // namespace wf
} // namespace csp
