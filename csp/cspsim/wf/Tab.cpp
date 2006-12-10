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
 * @file Tab.cpp
 *
 **/

#include <csp/cspsim/Animation.h>
#include <csp/cspsim/wf/Tab.h>
#include <csp/cspsim/wf/TabPage.h>
#include <csp/cspsim/wf/Theme.h>
#include <osg/Switch>

CSP_NAMESPACE

namespace wf {

class Tab::TabClickedCallback : public AnimationCallback {
public:
	TabClickedCallback(Tab* tab, TabPage* page) : m_Tab(tab), m_Page(page) {}
	virtual ~TabClickedCallback() {}
	virtual bool pick(int /*flags*/) {
		// Well. The user has clicked on the tab control. We need to switch the page
		// that is displayed.
		m_Tab->setCurrentPage(m_Page);
		return true;
	}
	
private:
	Tab* m_Tab;
	TabPage* m_Page;
};

Tab::Tab(Theme* theme) : Container(theme), m_CurrentPage(NULL) {
}

Tab::~Tab() {
}

ControlVector Tab::getChildControls() {
	ControlVector childControls;
	TabPageVector::iterator tabPage = m_Pages.begin();
	for(;tabPage != m_Pages.end();++tabPage) {
		childControls.push_back(tabPage->first);
	}
	return childControls;
}

void Tab::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	Container::buildGeometry();	
	
	// Get the geometry fot the tab control. This is only the default background of a page.
	getNode()->addChild(getTheme()->buildTab(this));
	
	TabPageVector::iterator page = m_Pages.begin();
	int index = 0;
	for(;page != m_Pages.end();++page,++index) {
		// Not that setCurrentPage() method is dependent on the order of function calls here.
		
		// Create a button for the page and add it to our control tree.
		osg::ref_ptr<osg::Switch> button = getTheme()->buildTabButton(this, page->first.get(), index);
		osg::ref_ptr<TabClickedCallback> callback = new TabClickedCallback(this, page->first.get());
		button->getChild(1)->setUpdateCallback(callback.get());
		page->second = button;

		// Well we add the page content to the button. This will make the page visible when the button
		// is visible. 
		page->first->buildGeometry();
		button->addChild(page->first->getNode(), (getCurrentPage() == page->first.get() ? true : false));
		
		// Add button and page to this tab control.
		getNode()->addChild(button.get());
	}
}

void Tab::layoutChildControls() {
	TabPageVector::iterator page = m_Pages.begin();
	for(;page != m_Pages.end();++page) {
		page->first->setSize(getTheme()->getTabPageClientAreaSize(this));
		page->first->setLocation(getTheme()->getTabPageClientAreaLocation(this));
	}
}

void Tab::addPage(TabPage* page) {
	// If there is no pages in the vector then assign the first page as the current page.
	if(m_Pages.size() == 0) {
		m_CurrentPage = page;
	}
	m_Pages.push_back(PageAndSwitch(page, NULL));
}

TabPage* Tab::getCurrentPage() const {
	return m_CurrentPage;
}

TabPage* Tab::getCurrentPage() {
	return m_CurrentPage;
}

void Tab::setCurrentPage(TabPage* page) {
	m_CurrentPage = page;
	
	// Index 0 == Current page button.
	// Index 1 == Not current page button.
	// Index 2 == Page content.

	TabPageVector::iterator iteratedPage = m_Pages.begin();
	for(;iteratedPage != m_Pages.end();++iteratedPage) {
		if(iteratedPage->first.get() == page) {
			iteratedPage->second->setSingleChildOn(0);
			iteratedPage->second->setValue(2, true);
		}
		else {
			iteratedPage->second->setSingleChildOn(1);
		}
	}
}

} // namespace wf

CSP_NAMESPACE_END
