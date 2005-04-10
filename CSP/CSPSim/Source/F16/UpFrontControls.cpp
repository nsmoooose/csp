// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file UpFrontControls.cpp
 *
 **/


#include "UpFrontControls.h"
#include "PageCNI.h"
#include "PageALOW.h"
#include "PageLIST.h"
#include "PageSTPT.h"

#include <iostream>

SIMDATA_REGISTER_INTERFACE(UpFrontControls)
DEFINE_INPUT_INTERFACE(UpFrontControls);


UpFrontControls::UpFrontControls(): m_AA(false), m_AG(false), m_ElapsedTime(0) {
	m_PageMap["CNI"] = new PageCNI;
	m_PageMap["ALOW"] = new PageALOW;
	m_PageMap["LIST"] = new PageLIST;
	m_PageMap["STPT"] = new PageSTPT;
}

void UpFrontControls::registerChannels(Bus *bus) {
	m_Display =  bus->registerLocalDataChannel<AlphaNumericDisplay::Ref>("DED", new AlphaNumericDisplay(26, 5));
}

void UpFrontControls::importChannels(Bus* bus) {
	for (PageMap::iterator i = m_PageMap.begin(); i != m_PageMap.end(); ++i) {
		i->second->importChannels(bus);
	}
	transition("CNI");
}

// event handlers
void UpFrontControls::ICP_0() { transition(getActivePage()->ICP_0()); }
void UpFrontControls::ICP_1() { transition(getActivePage()->ICP_1()); }
void UpFrontControls::ICP_2() { transition(getActivePage()->ICP_2()); }
void UpFrontControls::ICP_3() { transition(getActivePage()->ICP_3()); }
void UpFrontControls::ICP_4() { transition(getActivePage()->ICP_4()); }
void UpFrontControls::ICP_5() { transition(getActivePage()->ICP_5()); }
void UpFrontControls::ICP_6() { transition(getActivePage()->ICP_6()); }
void UpFrontControls::ICP_7() { transition(getActivePage()->ICP_7()); }
void UpFrontControls::ICP_8() { transition(getActivePage()->ICP_8()); }
void UpFrontControls::ICP_9() { transition(getActivePage()->ICP_9()); }
void UpFrontControls::ICP_RCL() { transition(getActivePage()->ICP_RCL()); }
void UpFrontControls::ICP_ENTR() { transition(getActivePage()->ICP_ENTR()); }
void UpFrontControls::ICP_SEQ() { transition(getActivePage()->ICP_SEQ()); }
void UpFrontControls::ICP_UP() { transition(getActivePage()->ICP_UP()); }
void UpFrontControls::ICP_DN() { transition(getActivePage()->ICP_DN()); }
void UpFrontControls::ICP_INC() { transition(getActivePage()->ICP_INC()); }
void UpFrontControls::ICP_DEC() { transition(getActivePage()->ICP_DEC()); }


void UpFrontControls::ICP_RTN() {
	m_Override = 0;
	transition("CNI");
}

void UpFrontControls::ICP_COM1() {
	override("COM1");
}

void UpFrontControls::ICP_COM2() {
	override("COM2");
}

void UpFrontControls::ICP_LIST() {
	override("LIST");
}

void UpFrontControls::ICP_IFF() {
	override("IFF");
}

void UpFrontControls::ICP_AA() {
	//b_MasterMode->push(m_AA ? "NAV" : "AA");
	m_AA = !m_AA;
	m_AG = false;
	updateActivePage();
}

void UpFrontControls::ICP_AG() {
	//b_MasterMode->push(m_AG ? "NAV" : "AG");
	m_AG = !m_AG;
	m_AA = false;
	updateActivePage();
}

double UpFrontControls::onUpdate(double dt) {
	m_ElapsedTime += dt;
	if (m_ElapsedTime > m_UpdateTime) {
		updateActivePage();
	}
	return 0.0;
}

DataEntryPage::Ref UpFrontControls::getActivePage() {
	assert(m_ActivePage.valid());
	if (!m_Override) return m_ActivePage;
	return m_Override;
}

void UpFrontControls::addPages(Bus*) {
}

void UpFrontControls::getInfo(InfoList &info) const {
	m_Display->value()->dump(info);
}

void UpFrontControls::transition(std::string const &page) {
	if (!page.empty()) {
		PageMap::const_iterator iter = m_PageMap.find(page);
		if (iter != m_PageMap.end() && m_ActivePage != iter->second) {
			m_ActivePage = iter->second;
		}
	}
	updateActivePage();
}

void UpFrontControls::override(std::string const &page) {
	if (!page.empty()) {
		PageMap::const_iterator iter = m_PageMap.find(page);
		if (iter != m_PageMap.end()) {
			m_Override = (m_Override == iter->second) ? 0 : iter->second;
			if (m_Override.valid()) m_Override->reset();
		}
	} else {
		m_Override = 0;
	}
	updateActivePage();
}

void UpFrontControls::updateActivePage() {
	m_ElapsedTime = 0;
	m_UpdateTime = 1.0;
	DataEntryPage::Ref page = getActivePage();
	if (page.valid()) {
		m_UpdateTime = page->update();
		// TODO display needs to keep a double buffer to allow
		// dirty checks.
		m_Display->value()->clear();
		page->render(*(m_Display->value()));  // XXX fixme
		if (m_Display->value()->isDirty()) {
			//m_Display->value().dump();
		}
	}
}
