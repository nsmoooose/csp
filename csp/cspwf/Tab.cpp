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

#include <csp/cspwf/ControlGeometryBuilder.h>
#include <csp/cspwf/StyleBuilder.h>
#include <csp/cspwf/StringResourceManager.h>
#include <csp/cspwf/Tab.h>
#include <csp/cspwf/Window.h>
#include <osg/Group>
#include <osg/NodeCallback>

namespace csp {
namespace wf {
	
Tab::Tab() : Container("Tab"), m_CurrentPage(NULL) {
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

void Tab::performLayout() {
	Container::performLayout();
	
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
	m_CurrentPage->performLayout();
	getNode()->addChild(m_CurrentPage->getNode());
	
	TabHeaderVector::iterator header = m_Headers.begin();
	for(;header != m_Headers.end();++header) {
		// Add the selected state so we can style the header.
		if(m_CurrentPage.get() == (*header)->getPage().get()) {
			(*header)->addState("selected");
		}
		
		(*header)->performLayout();
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
	
	Ref<Style> headerStyle = StyleBuilder::buildStyle(m_Headers[0].get());
	ControlGeometryBuilder builder;
	
	float headerHeight = 0;
	float headerXLocation = 0.0f;
	
	for(TabPageVector::size_type i=0;i<m_Pages.size();++i) {
		Ref<TabPage> page = m_Pages[i];
		Ref<TabHeader> header = m_Headers[i];

		Ref<Style> style = header->getStyle();
		
		if(headerStyle->getFontFamily() && headerStyle->getFontSize()) {
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
			Size textSize = builder.getSizeOfText(parsedText, *headerStyle->getFontFamily(), *headerStyle->getFontSize());
			
			// TODO: These hardcoded values shouldn't be here. We should
			// set a margin property in the style object. But this change
			// works for now.
			textSize.height += 15;
			textSize.width += 20;
			headerHeight = std::max(headerHeight, (float)textSize.height);

			style->setWidth(Style::UnitValue(Style::Pixels, textSize.width));
			style->setHeight(Style::UnitValue(Style::Pixels, textSize.height));
		}
		else {
			// This happens when the user has forgotten to set
			// mandatory style settings.
			style->setWidth(Style::UnitValue(Style::Pixels, 100));
			style->setHeight(Style::UnitValue(Style::Pixels, 200));
		}
			
		// Position header control after one and the other according
		// to the order the pages are in.
		style->setLeft(Style::UnitValue(Style::Pixels, headerXLocation));
		style->setTop(Style::UnitValue(Style::Pixels, 0));
		headerXLocation += header->getSize().width;
	}

	// Adjust header height to the highest header. 
	for(TabHeaderVector::size_type i=0;i<m_Headers.size();++i) {
		Ref<TabHeader> header = m_Headers[i];
		Ref<Style> style = header->getStyle();
		style->setHeight(Style::UnitValue(Style::Pixels, headerHeight));
	}

	// Adjust the size of the tab pages now when we know the 
	// largest header control.
	for(TabPageVector::size_type i=0;i<m_Pages.size();++i) {
		Ref<TabPage> page = m_Pages[i];

		Ref<Style> style = page->getStyle();

		Size size = getSize();

		// Position the content of a page.
		style->setLeft(Style::UnitValue(Style::Pixels, 0));
		style->setTop(Style::UnitValue(Style::Pixels, headerHeight));
		style->setWidth(Style::UnitValue(Style::Pixels, size.width));
		style->setHeight(Style::UnitValue(Style::Pixels, size.height - headerHeight));
		
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
	performLayout();
}


// ===================================================================

TabPage::TabPage() : SingleControlContainer("TabPage") {
}

TabPage::~TabPage() {
}

void TabPage::performLayout() {
	// Make sure that all our child controls onInit() is called.
	SingleControlContainer::performLayout();	

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

TabHeader::TabHeader() : Control("TabHeader") {
}

TabHeader::~TabHeader() {
}

void TabHeader::performLayout() {
	Control::performLayout();
	
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
} // namespace csp
