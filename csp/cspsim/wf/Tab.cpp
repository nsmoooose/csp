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
#include <csp/cspsim/wf/ControlGeometryBuilder.h>
#include <csp/cspsim/wf/Tab.h>
#include <csp/cspsim/wf/TabPage.h>
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

Tab::Tab() : m_CurrentPage(NULL) {
}

Tab::~Tab() {
}

ControlVector Tab::getChildControls() {
	ControlVector childControls;
	TabPageVector::iterator tabPage = m_Pages.begin();
	for(;tabPage != m_Pages.end();++tabPage) {
		childControls.push_back((*tabPage));
	}
	return childControls;
}

std::string Tab::getName() const {
	return "Tab";
}

void Tab::buildGeometry() {
	Container::buildGeometry();	
	
	// This fix must be here bequase of serialization. We must always have an
	// active page.
	if(m_CurrentPage == NULL && m_Pages.size() > 0) {
		m_CurrentPage = m_Pages.begin()->get();
	}
	
	// Get the geometry fot the tab control. This is only the default background of a page.
	ControlGeometryBuilder geometryBuilder;
	getNode()->addChild(geometryBuilder.buildTab(this));
	
	TabPageVector::iterator page = m_Pages.begin();
	int index = 0;
	for(;page != m_Pages.end();++page,++index) {	
		// Well we add the page content to the button. This will make the page visible when the button
		// is visible. 
		(*page)->buildGeometry();
		
		// Add button and page to this tab control.
//		getNode()->addChild(button.get());
	}
}

void Tab::layoutChildControls() {
	TabPageVector::iterator page = m_Pages.begin();
	for(;page != m_Pages.end();++page) {
		ControlGeometryBuilder geometryBuilder;
		/*
		(*page)->setSize(geometryBuilder.getTabPageClientAreaSize(this));
		(*page)->setLocation(geometryBuilder.getTabPageClientAreaLocation(this));
		(*page)->layoutChildControls();
		*/
	}
}

void Tab::addPage(TabPage* page) {
	// If there is no pages in the vector then assign the first page as the current page.
	if(m_Pages.size() == 0) {
		m_CurrentPage = page;
	}
	m_Pages.push_back(page);
}

TabPage* Tab::getCurrentPage() const {
	return m_CurrentPage;
}

TabPage* Tab::getCurrentPage() {
	return m_CurrentPage;
}

void Tab::setCurrentPage(TabPage* page) {
	m_CurrentPage = page;
	buildGeometry();
}

} // namespace wf

CSP_NAMESPACE_END
