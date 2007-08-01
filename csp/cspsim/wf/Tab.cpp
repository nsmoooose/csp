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
#include <csp/cspsim/wf/StyleBuilder.h>
#include <csp/cspsim/wf/StringResourceManager.h>
#include <csp/cspsim/wf/Tab.h>
#include <csp/cspsim/wf/Window.h>
#include <osg/Group>

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
	TabHeaderVector::iterator tabHeader = m_Headers.begin();
	for(;tabHeader != m_Headers.end();++tabHeader) {
		childControls.push_back((*tabHeader));
	}
	return childControls;
}

std::string Tab::getName() const {
	return "Tab";
}

void Tab::buildGeometry() {
	Container::buildGeometry();
	
	// If there is no pages then there is nothing to display. Lets just return in
	// that case.
	if(m_Pages.size() == 0) {
		return;
	}	
	
	// This fix must be here bequase of serialization. We must always have an
	// active page.
	if(!m_CurrentPage.valid()) {
		m_CurrentPage = m_Pages.begin()->get();
	}
	
	// Get the geometry for the tab control. This is only the default background of a page.
	ControlGeometryBuilder geometryBuilder;
	getNode()->addChild(geometryBuilder.buildTab(this));
	
	// Build the geometry for the current tab page. All other pages won't
	// be built bequase they aren't visible.
	m_CurrentPage->buildGeometry();
	getNode()->addChild(m_CurrentPage->getNode());
	
	TabHeaderVector::iterator header = m_Headers.begin();
	for(;header != m_Headers.end();++header) {
		// Add the selected state so we can style the header.
		if(m_CurrentPage.get() == (*header)->getPage().get()) {
			(*header)->addState("selected");
		}
		
		(*header)->buildGeometry();
		getNode()->addChild((*header)->getNode());
	}
}

void Tab::layoutChildControls() {
	if(m_Pages.size() == 0) {
		return;
	}
	
	// Due to serialization this vector can be empty. If it is then we
	// recreates the vector from all pages we got.
	if(m_Headers.size() != m_Pages.size()) {
		rebuildHeaders();
	}
	
	Style headerStyle = StyleBuilder::buildStyle(m_Headers[0].get());
	ControlGeometryBuilder builder;
	
	float headerHeight = 0;
	float headerXLocation = 0.0f;
	
	for(TabPageVector::size_type i=0;i<m_Pages.size();++i) {
		Ref<TabPage> page = m_Pages[i];
		Ref<TabHeader> header = m_Headers[i];

		if(headerStyle.fontFamily && headerStyle.fontSize) {
			// First we must handle if the string is a resource string.
			// All strings that includes ${my_string} is a reference from
			// a resource declared in an other file. The correct string
			// can be retreived from the StringResourceManager that the
			// window holds.
			std::string text = header->getText();
			std::string parsedText = text;
			Ref<const Window> window = Window::getWindow(this);
			if (window.valid()) {
				Ref<const StringResourceManager> stringResources = window->getStringResourceManager();
				parsedText = stringResources->parseAndReplace(text);
			}
			
			// Retreive the size of the text.
			Size textSize = builder.getSizeOfText(parsedText, *headerStyle.fontFamily, *headerStyle.fontSize);
			
			// TODO: These hardcoded values shouldn't be here. We should
			// set a margin property in the style object. But this change
			// works for now.
			textSize.height += 15;
			textSize.width += 20;
			headerHeight = std::max(headerHeight, (float)textSize.height);
			header->setSize(textSize);
		}
		else {
			// This happens when the user has forgotten to set
			// mandatory style settings.
			header->setSize(Size(100, 20));
		}
			
		// Position header control after one and the other according
		// to the order the pages are in.
		header->setLocation(Point(headerXLocation, 0));
		headerXLocation += header->getSize().width;
	}

	// Adjust header height to the highest header. 
	for(TabHeaderVector::size_type i=0;i<m_Headers.size();++i) {
		Ref<TabHeader> header = m_Headers[i];
		Size headerSize = header->getSize();
		headerSize.height = headerHeight;
		header->setSize(headerSize);
	}

	// Adjust the size of the tab pages now when we know the 
	// largest header control.
	for(TabPageVector::size_type i=0;i<m_Pages.size();++i) {
		Ref<TabPage> page = m_Pages[i];

		// Position the content of a page.
		page->setLocation(Point(0.0f, headerHeight));
		page->setSize(Size(getSize().width, getSize().height - headerHeight));
		
		// Layout all child controls.
		page->layoutChildControls();
	}
}

void Tab::rebuildHeaders() {
	m_Headers.clear();
	
	TabPageVector::iterator page = m_Pages.begin();
	for(;page != m_Pages.end();++page) {
		Ref<TabHeader> header = new TabHeader;
		header->setText((*page)->getText());
		header->setParent(this);
		header->setPage(page->get());
		m_Headers.push_back(header);
	}
	
	// Since we have removed all our header controls and created new ones
	// we are forced to recalculate all positions on child controls.
	layoutChildControls();
}

void Tab::addPage(TabPage* page) {
	// If there is no pages in the vector then assign the first page as the current page.
	if(m_Pages.size() == 0) {
		m_CurrentPage = page;
	}
	
	m_Pages.push_back(page);
	rebuildHeaders();
}

Ref<TabPage> Tab::getCurrentPage() const {
	return m_CurrentPage;
}

Ref<TabPage> Tab::getCurrentPage() {
	return m_CurrentPage;
}

void Tab::setCurrentPage(TabPage* page) {
	m_CurrentPage = page;
	rebuildHeaders();
	buildGeometry();
}


// ===================================================================

TabPage::TabPage() {
}

TabPage::~TabPage() {
}

std::string TabPage::getName() const {
	return "TabPage";
}

void TabPage::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	SingleControlContainer::buildGeometry();	

	ControlGeometryBuilder geometryBuilder;
	getNode()->addChild(geometryBuilder.buildTabPage(this));
}

const std::string TabPage::getText() const {
	return m_Text;
}

void TabPage::setText(const std::string& text) {
	m_Text = text;
}

// ===================================================================

TabHeader::TabHeader() {
	// TODO: Remove when we can set size and position with style object.
	setSize(Size(150, 30));
}

TabHeader::~TabHeader() {
}

std::string TabHeader::getName() const {
	return "TabHeader";
}

void TabHeader::buildGeometry() {
	Control::buildGeometry();
	
	ControlGeometryBuilder builder;
	getNode()->addChild(builder.buildTabHeader(this));
}  

const std::string TabHeader::getText() const {
	return m_Text;
}

void TabHeader::setText(const std::string& text) {
	m_Text = text;
}

void TabHeader::setPage(TabPage* page) {
	m_Page = page;
}

Ref<TabPage> TabHeader::getPage() {
	return m_Page;
}

void TabHeader::onClick(ClickEventArgs& event) {
	// Retreive the parent control if we have one.
	Ref<Container> parent = getParent();
	if(!parent.valid()) {
		return;
	}
	
	Tab* tab = dynamic_cast<Tab*>(parent.get());
	if(tab == NULL) {
		return;
	}
	
	// Now we have the tab control that holds the TabHeader. Lets check 
	// if we want to change the current tab page or not.
	if(tab->getCurrentPage() != m_Page.get()) {
		tab->setCurrentPage(m_Page.get());
	}

	// Set the event to handled to prevent it to bubble up to the parent control.
	event.handled = true;
	Control::onClick(event);
}

} // namespace wf

CSP_NAMESPACE_END
