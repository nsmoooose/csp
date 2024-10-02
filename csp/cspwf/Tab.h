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
 * @file Tab.h
 *
 **/

#include <csp/cspwf/SingleControlContainer.h>

namespace csp {
namespace wf {

class Tab;
class TabPage;

/** A class that represents the page header.
 * The header of a tab control can be by styled using this class.
 * Simply create a style named TabHeader for the unselected style.
 * For a selected TabPage you must create a style named TabHeader:selected.
 * 
 */ 
class CSPWF_EXPORT TabHeader : public Control {
public:
	TabHeader();
	virtual ~TabHeader();
	
	virtual void performLayout();
	
	// Returns the text that is displayed in header of a tab page.
	virtual const std::string getText() const;
	virtual void setText(const std::string& text);
	
	virtual void setPage(TabPage* page);
	virtual Ref<TabPage> getPage();
	
	// Override the click behaviour in order to handle tab page change.
	virtual void onClick(ClickEventArgs& event);

private:
	std::string m_Text;
	Ref<TabPage> m_Page;
};

typedef std::vector<Ref<TabHeader> > TabHeaderVector;
	

/** A class that represents a page in a Tab control.
 *
 *  The page is the control that holds new child controls like
 *  buttons, labels listbox etc. Each page is hidden or visible depending
 *  on the currently selected page in the Tab control.
 *  
 */
class CSPWF_EXPORT TabPage : public SingleControlContainer {
public:
	TabPage();
	virtual ~TabPage();

	virtual void performLayout();
	
	// Returns the text that is displayed in header of a tab page.
	virtual const std::string getText() const;
	virtual void setText(const std::string& text);
	
	template<class Archive>
	void serialize(Archive & ar) {
		SingleControlContainer::serialize(ar);
		ar & make_nvp("@Text", m_Text);
	}
private:
	std::string m_Text;
};

typedef std::vector<Ref<TabPage> > TabPageVector;


/** A class that represents a tab control.
 *
 *  This class contains a vector with one or more tab pages.
 *  Each page can hold new controls that are hidden or displayed
 *  depending on what the current selected page is.
 *  
 */
class CSPWF_EXPORT Tab : public Container {
public:
	Tab();
	virtual ~Tab();

	virtual ControlVector getChildControls();

	virtual void performLayout();
	virtual void layoutChildControls();
	
	virtual void addPage(TabPage* page);
	
	virtual Ref<TabPage> getCurrentPage() const;
	virtual Ref<TabPage> getCurrentPage();
	virtual void setCurrentPage(TabPage* page);

	template<class Archive>
	void serialize(Archive & ar) {
		Container::serialize(ar);
		ar & make_nvp("Pages", m_Pages);
	}	

private:
	TabPageVector m_Pages;
	TabHeaderVector m_Headers;
	
	Ref<TabPage> m_CurrentPage;
	
	void rebuildHeaders();
};

} // namespace wf
} // namespace csp
