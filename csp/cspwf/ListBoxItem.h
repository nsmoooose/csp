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
 * @file ListBoxItem.h
 *
 **/

#ifndef __CSPSIM_WF_LISTBOXITEM_H__
#define __CSPSIM_WF_LISTBOXITEM_H__

#include <csp/cspwf/SingleControlContainer.h>

namespace csp {
namespace wf {

/** A Control that is display within the ListBox Control. This control
 * can have the selected state. This means that you can have a different
 * visual appearence when the item is selected. For example: different 
 * background color.
 */
class CSPWF_EXPORT ListBoxItem : public SingleControlContainer {
public:
	ListBoxItem();
	/** Creates a new list box item with the text as content. */
	ListBoxItem(const std::string& text);
	virtual ~ListBoxItem();

	virtual void buildGeometry();
	
	/** Retreives the text associated with this list box item. */
	virtual const std::string getText() const;
	virtual void setText(const std::string& text);
		
	virtual const Ref<Style> getSelectedStyle() const;

	virtual void onClick(ClickEventArgs& event);
		
	template<class Archive>
	void serialize(Archive & ar) {
		Container::serialize(ar);
		ar & make_nvp("@Text", m_Text);
		ar & make_nvp("SelectedStyle", m_SelectedStyle);
	}	

private:

protected:
	std::string m_Text;
	Ref<Style> m_SelectedStyle;
};

typedef std::vector<Ref<ListBoxItem> > ListBoxItemVector;

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_LISTBOXITEM_H__
